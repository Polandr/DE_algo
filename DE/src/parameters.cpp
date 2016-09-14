#include <parameters.h>

// Parameters for algorithm:
int Parameters::population_size = 1;
int Parameters::problem_dimension = 1;

Stop_condition Parameters::stop_condition = Iter_num;
int Parameters::max_iter_num = 1;
double Parameters::max_evolution_time = 0;
int Parameters::n_change_iter_num = 1;
Fitness_type Parameters::exact_solution = 0;
double Parameters::epsilon = 0;

// Parameters for operators:
Mutation Parameters::mutation = Default_mut;
Crossover Parameters::crossover = Default_cros;
F_type Parameters::F1 = 0;
F_type Parameters::F2 = 0;
std::vector<double> Parameters::A;

// Parameters for parallel algorithm:
// Part of population to be exchanged (in [0,1]):
double Parameters::exchange_part = 0;
// Number of iterations between exchanges, if it's 0 than there are no exchanges in the algorihm:
int Parameters::exchange_iter = 0;
Exchange_strategy Parameters::exchange_strategy = Rotation;
Exchange_select_strategy Parameters::exchange_select_strategy = Random;


void Parameters::init()
{
	//population_size = 25;
	population_size = 100;
	problem_dimension = 1024;

	stop_condition = Iter_num | Time_out;
	max_iter_num = 100;
	max_evolution_time = 3000;
	n_change_iter_num = 100;
	exact_solution = 0;
	epsilon = 1e-1;

	mutation = DE_1;
	crossover = DE;
	F1 = 0.55;
	F2 = 0.55;
	std::vector<double> alpha(problem_dimension,0.05);
	A = alpha;

	exchange_part = 0.5;
	exchange_iter = 10;
	exchange_strategy = Rotation;
	exchange_select_strategy = Random;
}
