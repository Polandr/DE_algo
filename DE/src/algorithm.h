// Differential evolution algorithm class itself

#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <iostream>
#include <vector>
#include <chromosome.h>
#include <operators.h>
#include <types.h>

class DE
{
	std::vector<Chromosome*> population;
	std::vector<Chromosome*> new_population;
	Chromosome* best;

	Genetic_operator* mutation;
	Genetic_operator* crossover;
	Genetic_operator* selection;

	int iteration;

	Chromosome* prev_best;
	int non_change_itrs;

	bool stop ();

public:

	~DE ();

	void init ();
	void reset ();

	void next_generation ();

	void print_population (std::ostream& out = std::cout);
	void print_results (std::ostream& out = std::cout);
	void print_parameters (std::ostream& out = std::cout);

	Chromosome* find_best ();
	Fitness_type get_avg_fitness ();
	Chromosome* find_worst ();
	Chromosome* get_best () {return best;}

	// Main method:
	void execute ();
};

class parallel_DE
{
	std::vector<Chromosome*> population;
	std::vector<Chromosome*> new_population;
	Chromosome* best;

	Genetic_operator* mutation;
	Genetic_operator* crossover;
	Genetic_operator* selection;

	int iteration;

	Chromosome* prev_best;
	int non_change_itrs;

	void* exchange_buf;
	Chromosome* global_best;

	bool stop ();
	std::vector<int> bests_idxs(int exchange_num);
	std::vector<int> worsts_idxs(int exchange_num);
	std::vector<int> random_idxs(int exchange_num);

public:

	~parallel_DE();

	void init ();
	void reset ();

	void next_generation ();

	void print_population (std::ostream& out = std::cout);
	void print_results (std::ostream& out = std::cout);
	void print_parameters (std::ostream& out = std::cout);

	Chromosome* find_best ();
	Fitness_type avg_fitness ();
	Chromosome* find_worst ();

	// Parallel operations:
	Fitness_type get_global_best_fitness ();
	Fitness_type get_global_avg_fitness ();
	Fitness_type get_global_worst_fitness ();
	void gather_bests ();
	void broadcast_best ();
	void exchange (int iteration);

	// Main method:
	void execute ();
};

#endif // ALGORITHM_H
