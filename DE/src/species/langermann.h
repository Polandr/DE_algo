// Chromosome class for the Langermann's function:
//
// The problem is to find global minimum of next function (Langermann's function):
// -sum (0..M-1) (C[i] * exp(-sum(0..D-1)((x[j]-A[i,j])*(x[j]-A[i,j])) / PI) * cos(PI * sum(0..D-1)((x[j]-A[i,j])*(x[j]-A[i,j])))) in D dimensions
// For M = 5, D = 2
// C = [1,2,5,2,3]
// A = [[3,5],[5,2],[2,1],[1,4],[7,9]]
// Exact answer is f(2.00299219,1.006096) = -5.1621259

#ifndef LANGERMANN_H
#define LANGERMANN_H

#include <iostream>
#include <vector>
#include <types.h>
#include <chromosome.h>

class Langermann_chromosome : public Chromosome
{
	std::vector<Gene_type> data;

public:

	unsigned int size () const { return data.size()*sizeof(Gene_type); }
	void randomize ();
	void get_data (void* _data) const;
	void set_data (void* _data);

	Langermann_chromosome ();
	Langermann_chromosome (const std::vector<Gene_type> _data);
	Langermann_chromosome (const Langermann_chromosome& other);
	~Langermann_chromosome ();

	Fitness_type fitness () const;

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

#endif // LANGERMANN_H
