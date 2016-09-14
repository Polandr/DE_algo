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
	assert(argc == 2);
	int val = std::atoi(argv[1]);
	std::string filename("results/node_num/res");
	//std::string filename("results/pop_size/res");
	//std::string filename("res");
	filename += argv[1];
	filename += ".txt";
	std::ofstream file(filename.c_str());

	Processors::start();
	P::init();

	//Processors::root_print("Population size test\n",file);
	//Processors::print_info(file);

	//P::population_size = val;
	HD_DE algorithm;
	algorithm.init();
	algorithm.print_parameters(file);
	Processors::endline(file);

	Processors::root_print("Start test:\n",file);
	for (int experiment_num = 0 ; experiment_num < 10 ; experiment_num++)
	{
		algorithm.execute();
		if (Processors::is_root())
			file << val << endl;
		algorithm.print_results(file);
		algorithm.reset();
	}

	file.close();

	Processors::finish();

	return 0;
}
