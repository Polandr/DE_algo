// Chromosome class for the Sphere model function:
//
// The problem is to find global minimum of next function (Sphere model):
// sum (0..N-1) (x[i] * x[i]) in N dimensions
// Exact answer is f(0,...,0) = 0

#ifndef SPHERE_H
#define SPHERE_H

#include <iostream>
#include <vector>
#include <types.h>
#include <chromosome.h>

class Sphere_model_chromosome : public Chromosome
{
	std::vector<Gene_type> data;

public:

	unsigned int size () const { return data.size()*sizeof(Gene_type); }
	void randomize ();
	void get_data (void* _data) const;
	void set_data (void* _data);

	Sphere_model_chromosome ();
	Sphere_model_chromosome (const std::vector<Gene_type> _data);
	Sphere_model_chromosome (const Sphere_model_chromosome& other);
	~Sphere_model_chromosome ();

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

#endif // SPHERE_H
