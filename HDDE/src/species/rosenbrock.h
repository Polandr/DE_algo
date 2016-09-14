// Chromosome class for the Rosenbrock's function:
//
// The problem is to find global minimum of next function (Rosenbrock's function):
// (1-x)*(1-x) + 100*(y-x*x)*(y-x*x) in 2D
// sum (0..N-1) ((1-x[i])*(1-x[i]) + 100*(x[i+1]-x[i]*x[i])*(x[i+1]-x[i]*x[i])) in N dimensions
// Exact answer is (1,...,1)

#ifndef ROSENBROCK_H
#define ROSENBROCK_H

#include <iostream>
#include <vector>
#include <types.h>
#include <chromosome.h>

class Rosenbrock_chromosome : public Chromosome
{
	std::vector<Gene_type> data;

public:

	unsigned int size () const { return data.size()*sizeof(Gene_type); }
	void randomize ();
	void get_data (void* _data) const;
	void set_data (void* _data);

	Rosenbrock_chromosome ();
	Rosenbrock_chromosome (const int dim);
	Rosenbrock_chromosome (const std::vector<Gene_type> _data);
	Rosenbrock_chromosome (const Rosenbrock_chromosome& other);
	~Rosenbrock_chromosome ();

	Fitness_type fitness () const;
	Fitness_type distributed_fitness () const;

	Gene_type& operator [] (int idx) {return data[idx];}
	const Gene_type get_gene (int idx) const {return data[idx];}
	void set_gene (int idx, Gene_type gene) {data[idx] = gene;}
	const int gene_num () const {return data.size();}

	Chromosome& operator + (const Chromosome& other) const;
	Chromosome& operator - (const Chromosome& other) const;
	Chromosome& operator * (F_type& F) const;
	Chromosome& operator = (const Chromosome& other);

	bool operator > (Chromosome& other);
	// This chromosome better than the other
	bool operator < (Chromosome& other);
	// This chromosome worse than the other
	bool operator == (Chromosome& other);
	// Chromosomes are equal
	bool operator != (Chromosome& other);
	// Chromosomes are unequal

	void print (std::ostream& out) const;
	void print_info (std::ostream& out) const;
};

#endif // ROSENBROCK_H
