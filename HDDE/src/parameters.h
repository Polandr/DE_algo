#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <vector>
#include <types.h>

// There can be combination of stop conditions so we should have 2 types for stop condition:
enum Stop_cond {Iter_num = (1 << 0), Time_out = (1 << 1), Best_non_change = (1 << 2), Exact_solution = (1 << 3)};
typedef int Stop_condition;
enum Mutation {DE_1, DE_2, Default_mut};
enum Crossover {Homogeneous, Flat, Arithmetic, Geometric, BLX_alpha, DE, syn_DE, Default_cros};

struct Parameters
{
	static int population_size;
	static int problem_dimension;
	static int start_dimension;
	static int local_dimension;

	// Stop condition:
	static Stop_condition stop_condition;
	static int max_iter_num; // For Iter_num
	static double max_evolution_time; // For Time_out
	static int n_change_iter_num; // For Best_non_change
	static Fitness_type exact_solution; // For Exact_solution
	static double epsilon; // For Exact_solution

	// Parameters for operators:
	static Mutation mutation;
	static Crossover crossover;
	static F_type F1;
	static F_type F2;
	static std::vector<double> A;
	static double CR;

	static void init ();
};

typedef Parameters P;

#endif // PARAMETERS_H
