#include <factory.h>
#include <parameters.h>
#include <types.h>

#include <species/sphere_model.h>
#include <species/rosenbrock.h>
#include <species/rastrigin.h>
#include <species/langermann.h>

Chromosome* Factory::produce_chromosome ()
{
	return new Chromosome_type();
}
Chromosome* Factory::produce_chromosome (const std::vector<Gene_type> data)
{
	return new Chromosome_type(data);
}
Genetic_operator* Factory::produce_mutation ()
{
	switch (P::mutation)
	{
		case DE_1:
			return new DE_1_mutation(P::F1);
		case DE_2:
			return new DE_2_mutation(P::F1,P::F2);
		default:
			return new Default_mutation(P::F1);
	}
}
Genetic_operator* Factory::produce_crossover ()
{
	switch (P::crossover)
	{
		case Homogeneous:
			return new Homogeneous_crossover(P::A);
		case Flat:
			return new Flat_crossover();
		case Arithmetic:
			return new Arithmetic_crossover(P::A);
		case Geometric:
			return new Geometric_crossover(P::A);
		case BLX_alpha:
			return new BLX_alpha_crossover(P::A);
		case DE:
			return new DE_crossover();
		default:
			return new Default_crossover();
	}
}
Genetic_operator* Factory::produce_selection ()
{
	return new Default_selection();
}
