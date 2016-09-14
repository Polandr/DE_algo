#include <parameters.h>
#include <parallel.h>

// Parameters for algorithm:
int Parameters::population_size = 1;
int Parameters::problem_dimension = 1;
int Parameters::start_dimension = 0;
int Parameters::local_dimension = 1;

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
double Parameters::CR = 0;


void Parameters::init()
{
	population_size = 26;
	problem_dimension = 1024;
	start_dimension = Processors::start_partition(problem_dimension,Processors::coord());
	local_dimension = Processors::partition(problem_dimension,Processors::coord());

	stop_condition = Iter_num;
	max_iter_num = 3000;
	max_evolution_time = 3200;
	n_change_iter_num = 100;
	exact_solution = 0;
	epsilon = 1e-5;

	mutation = DE_1;
	crossover = Homogeneous;
	F1 = 0.55;
	F2 = 0.55;
	std::vector<double> alpha(problem_dimension,0.05);
	A = alpha;
	CR = 0.05;
}
