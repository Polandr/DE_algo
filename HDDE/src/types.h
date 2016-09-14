#ifndef TYPES_H
#define TYPES_H

// Main algorithm:
class DE;
class parallel_DE;

// Interface classes:
class Chromosome;
class Genetic_operator;

// Interface realizations:
// Chromosomes:
class Rosenbrock_chromosome;
class Rastrigin_chromosome;
class Sphere_model_chromosome;

// Operators:
// Mutations:
class DE_1_mutation;
class DE_2_mutation;
// Crossovers:
class Homogeneous_crossover;
class Flat_crossover;
class Arithmetic_crossover;
class Geometric_crossover;
class BLX_alpha_crossover;
class DE_crossover;
class syn_DE_crossover;
// Selections:
class Fitness_selection;

// Service type definition:
typedef unsigned char Byte;
typedef double Fitness_type;
typedef double F_type;
typedef double Gene_type;

// Types for factory:
typedef Sphere_model_chromosome Chromosome_type;
typedef DE_1_mutation Default_mutation;
typedef syn_DE_crossover Default_crossover;
typedef Fitness_selection Default_selection;

#endif // TYPES_H
