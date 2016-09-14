#include <iostream>
#include <algorithm.h>
#include <parameters.h>
#include <factory.h>
#include <randomizer.h>
#include <parallel.h>

using namespace std;

// Serial vesion:

DE::~DE ()
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
}

void DE::init ()
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

	iteration = 0;

	non_change_itrs = 0;
	if (P::stop_condition & Best_non_change)
	{
		prev_best = Factory::produce_chromosome();
		*prev_best = *best;
	}
}
void DE::reset ()
{
	delete mutation;
	delete crossover;
	delete selection;
	mutation = Factory::produce_mutation();
	crossover = Factory::produce_crossover();
	selection = Factory::produce_selection();

	for (int i = 0; i < population.size(); ++i)
	{
		population[i] -> randomize();
	}
	find_best();

	iteration = 0;

	non_change_itrs = 0;
	if (P::stop_condition & Best_non_change)
	{
		*prev_best = *best;
	}
}

void DE::next_generation ()
{
	for (int i = 0; i < population.size(); ++i)
	{
		vector<Chromosome*> in(3);
		vector<Chromosome*> out(1);
		in[0] = population[Randomizer::integer_rand(0, P::population_size)];
		in[1] = population[Randomizer::integer_rand(0, P::population_size)];
		in[2] = population[Randomizer::integer_rand(0, P::population_size)];
		mutation -> set_data(in);
		mutation -> apply();
		mutation -> get_data(out);

		in.resize(2);
		in[0] = population[i];
		in[1] = out[0];
		crossover -> set_data(in);
		crossover -> apply();
		crossover -> get_data(out);

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

bool DE::stop ()
{
	bool ret = false;

	// Stop by maximum iteration number reached
	if (P::stop_condition & Iter_num)
		if (iteration >= P::max_iter_num)
			ret = true;
	// Stop by non-changing best
	if (P::stop_condition & Best_non_change)
		if (*best == *prev_best)
		{
			if (non_change_itrs >= P::n_change_iter_num)
				ret = true;
			else
				non_change_itrs++;
		}
		else
		{
			*prev_best = *best;
			non_change_itrs = 0;
		}
	// Stop by exact solution reached (with enough precision)
	if (P::stop_condition & Exact_solution)
		if (abs(best -> fitness() - P::exact_solution) <= P::epsilon)
			ret = true;

	return ret;
}

void DE::print_population (ostream& out)
{
	out << "Population:\n";
	for (int i = 0; i < population.size(); ++i)
		population[i] -> print(out);
	out << endl;
}
void DE::print_results (ostream& out)
{
	/*for (int i = 0; i < population.size(); ++i)
		population[i] -> print(out);
	out << endl;*/
	out << "Best:\n";
	best -> print(out);
	out <<  iteration << " iterations\n";
}

void DE::print_parameters (ostream& out)
{
	out << "Population size: " << population.size() << endl;
	out << "Problem dimension: " << P::problem_dimension << endl;
	mutation -> print(out);
	crossover -> print(out);
	selection -> print(out);
	if (P::stop_condition & Iter_num)
		out << "Algorithm stops when " << P::max_iter_num << " iterations proceed\n";
	if (P::stop_condition & Best_non_change)
		out << "Algorithm stops when for " << P::n_change_iter_num << " iterations best value doesn't change\n";
	if (P::stop_condition & Exact_solution)
		out << "Algorithm stops when distance between current solution and exact solution (" << P::exact_solution << ") is less than " << P::epsilon << "\n";
	out << endl;
	population[0] -> print_info(out);
}

Chromosome* DE::find_best ()
{
	int idx_best = 0;
	for (int i = 1; i < population.size(); ++i)
		if (*population[i] > *population[idx_best])
			idx_best = i;
	*best = *population[idx_best];
	return best;
}
Fitness_type DE::get_avg_fitness ()
{
	Fitness_type out = 0;
	for (int i = 1; i < population.size(); ++i)
		out += population[i] -> fitness();
	return out/population.size();
}
Chromosome* DE::find_worst ()
{
	int idx_worst = 0;
	for (int i = 1; i < population.size(); ++i)
		if (*population[i] < *population[idx_worst])
			idx_worst = i;
	return population[idx_worst];
}

// Main method:
void DE::execute ()
{
	//init();
	for (; ; ++iteration)
	{
		if (stop())
			break;

		next_generation();
	}
	//print_results();
}
