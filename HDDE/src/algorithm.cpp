#include <cstdlib>
#include <cmath>
#include <cassert>
#include <iostream>
#include <algorithm.h>
#include <parameters.h>
#include <factory.h>
#include <randomizer.h>
#include <parallel.h>
#include <eventer.h>

using namespace std;

// Left fitness better than right
bool better (Fitness_type l, Fitness_type r)
{
	return l < r;
}
// Left fitness worse than right
bool worse (Fitness_type l, Fitness_type r)
{
	return l > r;
}
// Fitness values are equal
bool equal (Fitness_type l, Fitness_type r)
{
	return l == r;
}
// Variables for profiling:
Event_id fitness_e;
double fitness_t = 0, iteration_t = 0;

// Parallel version:

HD_DE::~HD_DE ()
{
	delete mutation;
	delete crossover;
	for (int i = 0; i < population.size(); ++i)
	{
		delete population[i];
		delete new_population[i];
	}
}

void HD_DE::init ()
{
	mutation = Factory::produce_mutation();
	crossover = Factory::produce_crossover();

	population.resize(P::population_size);
	new_population.resize(P::population_size);
	fitnesses.resize(P::population_size);
	for (int i = 0; i < population.size(); ++i)
	{
		population[i] = Factory::produce_chromosome();
		new_population[i] = Factory::produce_chromosome();
		population[i] -> randomize();
		fitnesses[i] = population[i] -> distributed_fitness();
	}

	iteration = 0;
	non_change_itrs = 0;
	if (P::stop_condition & Best_non_change)
	{
		prev_best_fitness = best_fitness();
	}
}
void HD_DE::reset ()
{
	for (int i = 0; i < population.size(); ++i)
	{
		population[i] -> randomize();
		fitnesses[i] = population[i] -> distributed_fitness();
	}

	iteration = 0;
	non_change_itrs = 0;
	if (P::stop_condition & Best_non_change)
	{
		prev_best_fitness = best_fitness();
	}
}

void HD_DE::next_generation ()
{
	for (int i = 0; i < population.size(); ++i)
	{
		vector<Chromosome*> in(3);
		vector<Chromosome*> out(1);
		// Mutation:
		in[0] = population[Randomizer::integer_rand(0, P::population_size)];
		in[1] = population[Randomizer::integer_rand(0, P::population_size)];
		in[2] = population[Randomizer::integer_rand(0, P::population_size)];
		mutation -> set_data(in);
		mutation -> apply();
		mutation -> get_data(out);

		in.resize(2);
		// Crossover:
		in[0] = population[i];
		in[1] = out[0];
		crossover -> set_data(in);
		crossover -> apply();
		crossover -> get_data(out);

		// Selection:
		// Profiling fitness evaluation time:
		fitness_e = Eventer::start_event();
		Fitness_type probe_fitness =  out[0] -> distributed_fitness();
		double cur_time = Eventer::stop_event(fitness_e);
		Eventer::erase_event(fitness_e);
		fitness_t = (iteration * fitness_t + cur_time * 1) / (iteration + 1);
		if (better(probe_fitness, fitnesses[i]))
		{
			*new_population[i] = *out[0];
			fitnesses[i] = probe_fitness;
		}
		else
			*new_population[i] = *population[i];
	}
	for (int i = 0; i < population.size(); ++i)
		*population[i] = *new_population[i];
}

bool HD_DE::stop ()
{
	bool ret = false;

	// Stop by maximum iteration number reached
	if (P::stop_condition & Iter_num)
		if (iteration >= P::max_iter_num)
			ret = true;
	// Stop by maximum time reached
	if (P::stop_condition & Time_out)
		if (Eventer::get_evolution_time() >= P::max_evolution_time)
			ret = true;
	// Stop by non-changing best
	if (P::stop_condition & Best_non_change)
		if (equal(best_fitness(),prev_best_fitness))
		{
			if (non_change_itrs >= P::n_change_iter_num)
				ret = true;
			else
				non_change_itrs++;
		}
		else
		{
			prev_best_fitness = best_fitness();
			non_change_itrs = 0;
		}
	// Stop by exact solution reached (with enough precision)
	if (P::stop_condition & Exact_solution)
		if (abs(best_fitness() - P::exact_solution) <= P::epsilon)
			ret = true;

	return ret;
}

void HD_DE::print_population (ostream& out)
{
	out << "Population:\n";
	for (int i = 0; i < population.size(); ++i)
		population[i] -> print(out);
	out << endl;
}
void HD_DE::print_results (ostream& out)
{
	Chromosome* best = assemble_best();
	if (Processors::is_root())
	{
		best -> print(out);
		out <<  iteration << " iterations\n";
		out << "Evolution time " << Eventer::get_evolution_time() << ' ';
		out << "Fitness evaluation time: " << fitness_t << ' ';
		out << "Iteration time: " << iteration_t << endl;
	}
}

void HD_DE::print_parameters (ostream& out)
{
	if (Processors::is_root())
	{
		out << "High-Dimensional Differential Evolution\n";
		out << "Population size: " << population.size() << endl;
		out << "Problem dimension: " << P::problem_dimension << endl;
		mutation -> print(out);
		crossover -> print(out);
		if (P::stop_condition & Iter_num)
			out << "Algorithm stops when " << P::max_iter_num << " iterations proceed\n";
		if (P::stop_condition & Time_out)
			out << "Algorithm stops when " << P::max_evolution_time << " seconds of evolution passed\n";
		if (P::stop_condition & Best_non_change)
			out << "Algorithm stops when for " << P::n_change_iter_num << " iterations best value doesn't change\n";
		if (P::stop_condition & Exact_solution)
			out << "Algorithm stops when distance between exact solution (" << P::exact_solution << ") is less than " << P::epsilon << "\n";
		out << endl;
		population[0] -> print_info(out);
	}
}

Chromosome* HD_DE::assemble_chromosome (int idx, bool for_all)
{
	int max_part = Processors::partition(P::problem_dimension, 0);
	size_t message_size = max_part * sizeof(Gene_type);
	size_t total_size = message_size * Processors::size();
	void *gather_buf, *message_buf;
	if (for_all)
		gather_buf = malloc(total_size);
	else
		if (Processors::is_root())
			gather_buf = malloc(total_size);
	message_buf = malloc(message_size);
	population[idx] -> get_data(message_buf);

	Processors::gather_to_root(message_buf, gather_buf, message_size);
	if (for_all)
		Processors::bcast_from_root(gather_buf, total_size);

	Chromosome* out;
	if (for_all)
	{
		int ofs = 0, k = 0;
		out = Factory::produce_chromosome(P::problem_dimension);
		for (int i = 0; i < Processors::size(); ++i, ofs += max_part)
			for (int j = 0; j < Processors::partition(P::problem_dimension,i); ++j)
			{
				out -> set_gene(k, (static_cast<Gene_type*>(gather_buf))[ofs+j]);
				k++;
			}
		free(gather_buf);
	}
	else
		if (Processors::is_root())
		{
			int ofs = 0, k = 0;
			out = Factory::produce_chromosome(P::problem_dimension);
			for (int i = 0; i < Processors::size(); ++i, ofs += max_part)
				for (int j = 0; j < Processors::partition(P::problem_dimension,i); ++j)
				{
					out -> set_gene(k, (static_cast<Gene_type*>(gather_buf))[ofs+j]);
					k++;
				}
			free(gather_buf);
		}
	free(message_buf);
	return out;
}
int HD_DE::best_idx ()
{
	int idx_best = -1;
	Fitness_type best_fitness_ = 0;
	for (int i = 0; i < population.size(); ++i)
	{
		if (idx_best == -1 || better(fitnesses[i],best_fitness_))
		{
			idx_best = i;
			best_fitness_ = fitnesses[i];
		}
	}
	return idx_best;
}
int HD_DE::worst_idx ()
{
	int idx_worst = -1;
	Fitness_type worst_fitness_ = 0;
	for (int i = 0; i < population.size(); ++i)
	{
		if (idx_worst == -1 || worse(fitnesses[i],worst_fitness_))
		{
			idx_worst = i;
			worst_fitness_ = fitnesses[i];
		}
	}
	return idx_worst;
}
Fitness_type HD_DE::avg_fitness ()
{
	Fitness_type fitness = 0;
	for (int i = 0; i < population.size(); ++i)
	{
		fitness += fitnesses[i];
	}
	return fitness / population.size();
}

// Main method:
void HD_DE::execute ()
{
	Eventer::start_evolution();
	for (; !stop(); ++iteration)
	{
		next_generation();
		Chromosome* best = assemble_best();
		//if (Processors::is_root())
		//	cout << iteration << " : " << best_fitness() << " | " << avg_fitness() << " | " << worst_fitness() << endl;
	}
	Eventer::stop_evolution();
	iteration_t = Eventer::get_evolution_time() / iteration;
}
