#ifndef FACTORY_H
#define FACTORY_H

#include <vector>
#include <chromosome.h>
#include <operators.h>

class Factory
{
public:

	static Chromosome* produce_chromosome ();
	static Chromosome* produce_chromosome (const std::vector<Gene_type> data);
	static Chromosome* produce_chromosome (const int dim);
	static Genetic_operator* produce_mutation ();
	static Genetic_operator* produce_crossover ();
	static Genetic_operator* produce_selection ();
};

#endif // FACTORY_H
