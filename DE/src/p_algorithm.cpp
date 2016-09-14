#include <cstdlib>
#include <cmath>
#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm.h>
#include <parameters.h>
#include <factory.h>
#include <randomizer.h>
#include <parallel.h>
#include <eventer.h>

using namespace std;

bool exchangable ()
{
	return P::exchange_part != 0 && P::exchange_iter != 0 && Processors::size() > 1;
}

// Parallel version:

parallel_DE::~parallel_DE ()
{
	delete mutation;
	delete crossover;
	delete selection;
	for (int i = 0; i < population.size(); ++i)
	{
		delete population[i];
		delete new_population[i];
	}
	delete best;
	if (P::stop_condition & Best_non_change)
	{
		delete prev_best;
	}
	delete global_best;
	if (exchangable())
		free(exchange_buf);
}

void parallel_DE::init ()
{
	mutation = Factory::produce_mutation();
	crossover = Factory::produce_crossover();
	selection = Factory::produce_selection();

	population.resize(P::population_size);
	new_population.resize(P::population_size);
	for (int i = 0; i < population.size(); ++i)
	{
		population[i] = Factory::produce_chromosome();
		population[i] -> randomize();
		new_population[i] = Factory::produce_chromosome();
	}
	best = Factory::produce_chromosome();
	find_best();
	global_best = Factory::produce_chromosome();
	gather_bests();
	broadcast_best();

	if (exchangable())
	{
		int exchange_num = P::exchange_part * P::population_size;
		exchange_buf = malloc(exchange_num * population[0] -> size());
		if (exchange_buf == NULL)
			Processors::abort();
	}

	iteration = 0;

	non_change_itrs = 0;
	if (P::stop_condition & Best_non_change)
	{
		prev_best = Factory::produce_chromosome();
		*prev_best = *global_best;
	}
}
void parallel_DE::reset ()
{
	for (int i = 0; i < population.size(); ++i)
	{
		population[i] -> randomize();
	}
	find_best();
	gather_bests();
	broadcast_best();

	iteration = 0;

	non_change_itrs = 0;
	if (P::stop_condition & Best_non_change)
	{
		*prev_best = *global_best;
	}
}

void parallel_DE::next_generation ()
{
	for (int i = 0; i < population.size(); ++i)
	{
		vector<Chromosome*> in(3);
		vector<Chromosome*> out(1);
		// Mutation:
		in[0] = population[Randomizer::integer_rand(0, P::population_size)];
		//in[0] = global_best;
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
		in[0] = population[i];
		in[1] = out[0];
		selection -> set_data(in);
		selection -> apply();
		selection -> get_data(out);

		*new_population[i] = *out[0];
	}
	for (int i = 0; i < population.size(); ++i)
		*population[i] = *new_population[i];

	find_best();
}

bool parallel_DE::stop ()
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
		if (*global_best == *prev_best)
		{
			if (non_change_itrs >= P::n_change_iter_num)
				ret = true;
			else
				non_change_itrs++;
		}
		else
		{
			*prev_best = *global_best;
			non_change_itrs = 0;
		}
	// Stop by exact solution reached (with enough precision)
	if (P::stop_condition & Exact_solution)
		if (abs(global_best -> fitness() - P::exact_solution) <= P::epsilon)
			ret = true;

	return ret;
}

void parallel_DE::print_population (ostream& out)
{
	out << "Population:\n";
	for (int i = 0; i < population.size(); ++i)
		population[i] -> print(out);
	out << endl;
}
Event_id fitness_e, exchange_e, gather_e;
double fitness_t = 0, iteration_t = 0, exchange_t = 0, gather_t = 0;
void parallel_DE::print_results (ostream& out)
{
	if (Processors::is_root())
	{
		global_best -> print(out);
		out <<  iteration << " iterations\n";
		out << "Evolution time " << Eventer::get_evolution_time() << ' ';
		out << "Fitness evaluation time: " << fitness_t << ' ';
		out << "Iteration time: " << iteration_t << ' ';
		out << "Exchange time: " << exchange_t << ' ';
		out << "Gather time: " << gather_t << endl;
		out << flush;
	}
}

void parallel_DE::print_parameters (ostream& out)
{
	if (Processors::is_root())
	{
		out << "Differential Evolution Island Model\n";
		out << "Population size: " << population.size() << endl;
		out << "Problem dimension: " << P::problem_dimension << endl;
		mutation -> print(out);
		crossover -> print(out);
		selection -> print(out);
		if (P::stop_condition & Iter_num)
			out << "Algorithm stops when " << P::max_iter_num << " iterations proceed\n";
		if (P::stop_condition & Time_out)
			out << "Algorithm stops when " << P::max_evolution_time << " seconds of evolution passed\n";
		if (P::stop_condition & Best_non_change)
			out << "Algorithm stops when for " << P::n_change_iter_num << " iterations best value doesn't change\n";
		if (P::stop_condition & Exact_solution)
			out << "Algorithm stops when distance between exact solution (" << P::exact_solution << ") is less than " << P::epsilon << "\n";
		if (P::exchange_part != 0)
		{
			out << "Exchange occurs every " << P::exchange_iter << " step(s)\n";
			out << (P::exchange_part * P::population_size) << " chromosome(s) are exchanging\n";
			if (P::exchange_select_strategy == Bests)
				out << "They're selected from bests\n";
			if (P::exchange_select_strategy == Worsts)
				out << "They're selected from worsts\n";
			if (P::exchange_select_strategy == Random)
				out << "They're selected randomly\n";
			if (P::exchange_strategy == Rotation)
				out << "Exchange data flows as cyclical rotation\n";
			if (P::exchange_strategy == Pair)
				out << "Pair exchanging\n";
		}
		out << endl;
		population[0] -> print_info(out);
	}
}

Chromosome* parallel_DE::find_best ()
{
	int idx_best = 0;
	for (int i = 1; i < population.size(); ++i)
		if (*population[i] > *population[idx_best])
			idx_best = i;
	*best = *population[idx_best];
	return best;
}
Fitness_type parallel_DE::avg_fitness ()
{
	Fitness_type out = 0;
	for (int i = 1; i < population.size(); ++i)
		out += population[i] -> fitness();
	return out/population.size();
}
Chromosome* parallel_DE::find_worst ()
{
	int idx_worst = 0;
	for (int i = 1; i < population.size(); ++i)
		if (*population[i] < *population[idx_worst])
			idx_worst = i;
	return population[idx_worst];
}

// Parallel operations:
// Global best & global worst fitnesses work only if problem is to find minimum double:
Fitness_type parallel_DE::get_global_best_fitness ()
{
	Fitness_type min = 0;
	int idx_best = -1;
	for (int i = 0; i < population.size(); ++i)
	{
		Fitness_type tmp = population[i] -> fitness();
		if (idx_best == -1 || tmp < min)
		{
			idx_best = i;
			min = tmp;
		}
	}
	// Works only if Fitness_type is castable to double
	return Processors::minimum(min);
}
Fitness_type parallel_DE::get_global_avg_fitness ()
{
	Fitness_type out = 0;
	for (int i = 0; i < population.size(); ++i)
		out += population[i] -> fitness();
	out /= population.size();
	// Works only if Fitness_type is castable to double
	return Processors::average(out);
}
Fitness_type parallel_DE::get_global_worst_fitness ()
{
	Fitness_type max = 0;
	int idx_best = -1;
	for (int i = 0; i < population.size(); ++i)
	{
		Fitness_type tmp = population[i] -> fitness();
		if (idx_best == -1 || tmp > max)
		{
			idx_best = i;
			max = tmp;
		}
	}
	// Works only if Fitness_type is castable to double
	return Processors::maximum(max);
}
// Gather bests to root and choose the best among them
void parallel_DE::gather_bests ()
{
	size_t message_size = population[0] -> size();
	void *root_buf, *message_buf;
	if (Processors::is_root())
		root_buf = malloc(Processors::size() * message_size);
	message_buf = malloc(message_size);
	best -> get_data(message_buf);
	
	//MPI_Gather(message_buf, message_size, MPI_BYTE, root_buf, message_size, MPI_BYTE, Processors::root, MPI_COMM_WORLD);
	Processors::gather_to_root(message_buf, root_buf, message_size);

	if (Processors::is_root())
	{
		Chromosome* tmp = Factory::produce_chromosome();
		global_best -> set_data(root_buf);	
		for (size_t ofs = message_size; ofs < Processors::size() * message_size; ofs += message_size)
		{
			tmp -> set_data(static_cast<Byte*>(root_buf) + ofs);
			if (*tmp > *global_best)
				*global_best = *tmp;
		}
		delete tmp;
	}

	free(message_buf);
	if (Processors::is_root())
		free(root_buf);
}
// Broadcast global best chromosome from root to others
void parallel_DE::broadcast_best ()
{
	void* chromosome_buf = malloc(population[0] -> size());
	if (Processors::is_root())
		global_best -> get_data(chromosome_buf);
	//MPI_Bcast(chromosome_buf, population[0] -> size(), MPI_BYTE, Processors::root, MPI_COMM_WORLD);
	Processors::bcast_from_root(chromosome_buf, population[0] -> size());
	if (!Processors::is_root())
		global_best -> set_data(chromosome_buf);
	free(chromosome_buf);
}
// Selection for exchange strategies:
	// Exchange strategy is to take first <exchange_num> bests
	vector<int> parallel_DE::bests_idxs(int exchange_num)
	{
		vector<int> out(exchange_num, 0);
		for (int i = 0; i < population.size(); ++i)
		{
			for (int j = 0; j < out.size(); ++j)
			{
				if (*population[i] > *population[out[j]])
				{
					for (int k = out.size()-1; k > j; ++k)
						out[k] = out[k-1];
					out[j] = i;
					break;
				}
			}
		}
		return out;
	}
	// Exchange strategy is to take last <exchange_num> worsts
	vector<int> parallel_DE::worsts_idxs(int exchange_num)
	{
		vector<int> out(exchange_num, 0);
		for (int i = 0; i < population.size(); ++i)
		{
			for (int j = 0; j < out.size(); ++j)
			{
				if (*population[i] < *population[out[j]])
				{
					for (int k = out.size()-1; k > j; ++k)
						out[k] = out[k-1];
					out[j] = i;
					break;
				}
			}
		}
		return out;
	}
	// Exchange strategy is to take random <exchange_num> chromosomes
	vector<int> parallel_DE::random_idxs(int exchange_num)
	{
		vector<int> out(exchange_num, 0);
		for (int i = 0; i < out.size(); ++i)
			out[i] = Randomizer::integer_rand(0, P::population_size);
		return out;
	}
// Exchange populations
void parallel_DE::exchange (int iteration)
{
	vector<int> exchange_idxs;
	int exchange_num = P::exchange_part * P::population_size;
	if (P::exchange_select_strategy == Bests)
		exchange_idxs = bests_idxs(exchange_num);
	if (P::exchange_select_strategy == Worsts)
		exchange_idxs = worsts_idxs(exchange_num);
	if (P::exchange_select_strategy == Random)
		exchange_idxs = random_idxs(exchange_num);

	size_t ofs = 0;
	size_t chromosome_size = population[0] -> size();
	for (int i = 0; i < exchange_idxs.size(); ++i)
	{
		population[exchange_idxs[i]] -> get_data(static_cast<Byte*>(exchange_buf) + ofs);
		ofs += chromosome_size;
	}

	if (P::exchange_strategy == Rotation)
		Processors::rotation_exchange(iteration, exchange_buf, exchange_num * chromosome_size);
	if (P::exchange_strategy == Pair)
		Processors::neighbor_exchange(exchange_buf, exchange_num * chromosome_size);

	ofs = 0;
	for (int i = 0; i < exchange_idxs.size(); ++i)
	{
		population[exchange_idxs[i]] -> set_data(static_cast<Byte*>(exchange_buf) + ofs);
		ofs += chromosome_size;
	}
}

// Main method:
void parallel_DE::execute ()
{
	double cur_time = 0;
	//std::ofstream file("evolution.txt");
	Eventer::start_evolution();
	for (; !stop(); ++iteration)
	{
		next_generation();
		if (exchangable() && iteration % P::exchange_iter == 0)
		{
			exchange_e = Eventer::start_event();
			exchange(iteration);
			cur_time = Eventer::stop_event(exchange_e);
			Eventer::erase_event(exchange_e);
			exchange_t = (iteration * exchange_t + cur_time * 1) / (iteration + 1);
		}

		fitness_e = Eventer::start_event();
		population[0] -> fitness();
		cur_time = Eventer::stop_event(fitness_e);
		Eventer::erase_event(fitness_e);
		fitness_t = (iteration * fitness_t + cur_time * 1) / (iteration + 1);

		gather_e = Eventer::start_event();
		gather_bests();
		broadcast_best();
		cur_time = Eventer::stop_event(gather_e);
		Eventer::erase_event(gather_e);
		gather_t = (iteration * gather_t + cur_time * 1) / (iteration + 1);

		//print_population(cout);
		//Fitness_type bst = get_global_best_fitness();
		//Fitness_type avg = get_global_avg_fitness();
		//Fitness_type wrst = get_global_worst_fitness();
		//if (Processors::is_root())
		//	cout << iteration << " : " << bst << " | " << avg << " | " << wrst << endl;
	}
	Eventer::stop_evolution();
	//file.close();
	iteration_t = Eventer::get_evolution_time() / iteration;
}
