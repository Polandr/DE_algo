#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>

#include <algorithm.h>
#include <parameters.h>
#include <parallel.h>

#include <factory.h>
#include <types.h>
#include <operators.h>

using namespace std;

Chromosome* assemble (Chromosome* chromosome, bool for_all)
{
	int max_part = Processors::partition(P::problem_dimension, 0);
	size_t message_size = max_part * sizeof(Gene_type);
	size_t total_size = message_size * Processors::size();
	void *gather_buf, *message_buf;
	if (for_all)
		gather_buf = malloc(total_size);
	else
		if (Processors::is_root())
			gather_buf = malloc(total_size);
	message_buf = malloc(message_size);
	chromosome -> get_data(message_buf);

	Processors::gather_to_root(message_buf, gather_buf, message_size);
	if (for_all)
		Processors::bcast_from_root(gather_buf, total_size);

	Chromosome* out;
	if (for_all)
	{
		int ofs = 0, k = 0;
		out = Factory::produce_chromosome(P::problem_dimension);
		for (int i = 0; i < Processors::size(); ++i, ofs += max_part)
			for (int j = 0; j < Processors::partition(P::problem_dimension,i); ++j)
			{
				out -> set_gene(k, (static_cast<Gene_type*>(gather_buf))[ofs+j]);
				k++;
			}
		free(gather_buf);
	}
	else
		if (Processors::is_root())
		{
			int ofs = 0, k = 0;
			out = Factory::produce_chromosome(P::problem_dimension);
			for (int i = 0; i < Processors::size(); ++i, ofs += max_part)
				for (int j = 0; j < Processors::partition(P::problem_dimension,i); ++j)
				{
					out -> set_gene(k, (static_cast<Gene_type*>(gather_buf))[ofs+j]);
					k++;
				}
			free(gather_buf);
		}
	free(message_buf);
	return out;
}

int main(int argc, char const *argv[])
{
	const int N = 4;

	Processors::start();
	P::init();

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

	/*vector<double> v(P::local_dimension);
	for (int i = 0; i < v.size(); ++i)
	{
		//v[i] = i+Processors::coord();
		v[i] = i+1;
	}
	Chromosome *chromosome = Factory::produce_chromosome(v);
	Fitness_type df = chromosome -> distributed_fitness();
	chromosome = assemble(chromosome,true);
	if (Processors::is_root())
	{
		double x = 0;
		vector<double> arg(P::problem_dimension);
		for (int i = 0; i < arg.size(); ++i, ++x)
		{
			if (i == 0 || i == 4 || i == 7)
				x = 0;
			arg[i] = x+1;
			cout << arg[i] << ", ";
		}
		Fitness_type sum(0);
		for (int i = 0; i < arg.size() - 1; ++i)
		{
			sum += (1 - arg[i]) * (1 - arg[i]);
			sum += 100 * (arg[i+1] - arg[i] * arg[i]) * (arg[i+1] - arg[i] * arg[i]);
		}
		cout << "-> " << sum << endl;

		//chromosome -> print(cout);

		cout << "Distributed fitness: " << df << endl;
	}*/

	Genetic_operator* crossover = Factory::produce_crossover();
	Genetic_operator* mutation = Factory::produce_mutation();

	vector<Chromosome*> chrs(N);
	for (int i = 0; i < chrs.size(); ++i)
	{
		vector<Gene_type> v(P::local_dimension,i + Processors::proc_coord);
		chrs[i] = Factory::produce_chromosome(v);
		Chromosome* tmp = assemble(chrs[i], false);
		if (Processors::is_root())
			tmp -> print(cout);
	}
	double F = 2;

	mutation -> set_data(chrs);
	mutation -> apply();
	mutation -> get_data(chrs);

	Processors::endline(cout);
	Chromosome* tmp = assemble(chrs[0],false);
		if (Processors::is_root())
			tmp -> print(cout);
	for (int i = 0; i < chrs.size(); ++i)
	{
		double val = chrs[i] -> distributed_fitness();
		if (Processors::is_root())
			cout << val << endl;
	}

	Processors::finish();
	return 0;
}
