// Differential evolution algorithm class itself

#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <iostream>
#include <vector>
#include <chromosome.h>
#include <operators.h>
#include <types.h>

class HD_DE
{
	std::vector<Chromosome*> population;
	std::vector<Chromosome*> new_population;

	Genetic_operator* mutation;
	Genetic_operator* crossover;

	std::vector<Fitness_type> fitnesses;
	Fitness_type prev_best_fitness;

	int iteration;
	int non_change_itrs;

	bool stop ();

public:

	~HD_DE();

	void init ();
	void reset ();

	void next_generation ();

	void print_population (std::ostream& out = std::cout);
	void print_results (std::ostream& out = std::cout);
	void print_parameters (std::ostream& out = std::cout);

	// Get best chromosome for either all nodes or only for root
	Chromosome* assemble_chromosome (int idx, bool for_all = false);
	Chromosome* assemble_best (bool for_all = false) { return assemble_chromosome(best_idx(),for_all); }
	int best_idx ();
	Fitness_type best_fitness () { return fitnesses[best_idx()]; }
	int worst_idx ();
	Fitness_type worst_fitness ()  { return fitnesses[worst_idx()]; }
	Fitness_type avg_fitness ();

	// Main method:
	void execute ();
};

#endif // ALGORITHM_H
