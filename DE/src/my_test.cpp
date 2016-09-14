#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>

#include <algorithm.h>
#include <parameters.h>
#include <parallel.h>

#include <factory.h>
#include <types.h>

using namespace std;

int main(int argc, char const *argv[])
{
	const int N = 3;
	P::init();

	Processors::start();

	/*size_t chromosome_size = P::problem_dimension * sizeof(Gene_type);
	void *buffer = malloc(N * chromosome_size);

	vector<Chromosome*> chrs(N);
	for (int i = 0; i < chrs.size(); ++i)
	{
		vector<Gene_type> v(P::problem_dimension,i + Processors::proc_coord);
		chrs[i] = Factory::produce_chromosome(v);
	}

	if (Processors::proc_coord == 0)
	{
		cout << "Proc 0:\n";
		for (int i = 0; i < chrs.size(); ++i)
		{
			chrs[i] -> print(cout);
		}
	}
	Processors::barrier();

	size_t ofs = 0;
	for (int i = 0; i < chrs.size(); ++i)
	{
		chrs[i] -> get_data(static_cast<Byte*>(buffer) + ofs);
		ofs += chromosome_size;
	}
	Processors::rotation_exchange(1, buffer, N * chromosome_size);
	ofs = 0;
	for (int i = 0; i < chrs.size(); ++i)
	{
		chrs[i] -> set_data(static_cast<Byte*>(buffer) + ofs);
		ofs += chromosome_size;
	}

	Processors::barrier();
	if (Processors::proc_coord == 2)
	{
		cout << "Proc 2:\n";
		for (int i = 0; i < chrs.size(); ++i)
		{
			chrs[i] -> print(cout);
		}
	}*/


	double val = Processors::proc_coord+1;
	double avg = Processors::average(val);
	Processors::barrier();
	if (Processors::proc_coord == 1)
	{
		cout << "Proc 1:\n";
		cout << "Average is: " << avg << endl;
	}

	Processors::finish();
	return 0;
}
