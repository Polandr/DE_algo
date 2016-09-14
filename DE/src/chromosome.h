// Chromosome interface for further realization for exact problem

#ifndef CHROMOSOME_H
#define CHROMOSOME_H

#include <iostream>
#include <types.h>

class Chromosome
{

public:

	virtual unsigned int size () const = 0;
	virtual void randomize () = 0;
	virtual void get_data (void* _data) const = 0;
	virtual void set_data (void* _data) = 0;

	virtual ~Chromosome () {}

	virtual Fitness_type fitness () const = 0;

	virtual Gene_type& operator [] (int idx) {}
	virtual const Gene_type get_gene (int idx) const {}
	virtual void set_gene (int idx, Gene_type gene) {}
	virtual const int gene_num () const {return 0;}

	virtual Chromosome& operator + (const Chromosome& other) const = 0;
	virtual Chromosome& operator - (const Chromosome& other) const = 0;
	virtual Chromosome& operator * (F_type& F) const = 0;
	virtual Chromosome& operator = (const Chromosome& other) = 0;

	virtual bool operator > (Chromosome& other) = 0;
	// This chromosome better than the other
	virtual bool operator < (Chromosome& other) = 0;
	// This chromosome worse than the other
	virtual bool operator == (Chromosome& other) = 0;
	// Chromosomes are equal
	virtual bool operator != (Chromosome& other) = 0;
	// Chromosomes are unequal

	virtual void print (std::ostream& out) const = 0;
	virtual void print_info (std::ostream& out) const {};
};

#endif // CHROMOSOME_H
