#include <cstdlib>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm.h>
#include <parameters.h>
#include <parallel.h>
#include <types.h>


using std::cout;
using std::endl;

// Default test is on 32 nodes
int main(int argc, char const *argv[])
{
	P::init();
	assert(argc > 1);
	//int val = std::atoi(argv[1]);
	int val1 = std::atoi(argv[1]);
	double val2 = std::atof(argv[2]);

	//double val = std::atof(argv[1]);
	//std::string filename("sphere_");
	//std::string filename("results/mutation/mut_");
	std::string filename("results/sphere1024_");
	filename += argv[1];
	filename += "_";
	filename += argv[2];
	filename += ".txt";
	std::ofstream file(filename.c_str());

	Processors::start();

	//Processors::root_print("Population size test\n",file);
	//Processors::print_info(file);

	P::population_size = val1;
	P::F1 = val2;

	parallel_DE algorithm;
	algorithm.init();
	algorithm.print_parameters(file);
	Processors::endline(file);

	Processors::root_print("Start test:\n",file);
	for (int experiment_num = 0 ; experiment_num < 1 ; experiment_num++)
	{
		algorithm.execute();
		if (Processors::is_root())
			file << val1 << " " << val2 << endl;
		algorithm.print_results(file);
		algorithm.reset();
	}

	file.close();

	Processors::finish();

	return 0;
}
