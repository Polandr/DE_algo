// Genetic operator interface for further realization

#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <vector>
#include <chromosome.h>
#include <types.h>

// Operator interface:

class Genetic_operator
{
protected:
	std::vector<Chromosome*> in_data;
	std::vector<Chromosome*> out_data;

public:
	virtual ~Genetic_operator ();
	virtual void set_data (std::vector<Chromosome*>& data);
	virtual void get_data (std::vector<Chromosome*>& data);
	virtual void apply () = 0;
	virtual void print (std::ostream& out) {}
};

// Operator realizations:

// Mutation:
class DE_1_mutation: public Genetic_operator
{
	F_type F;

public:
	DE_1_mutation (F_type& f);
	~DE_1_mutation () {};
	void set_F (F_type& f) { F = f; }
	void apply ();
	void print (std::ostream& out);
};
class DE_2_mutation: public Genetic_operator
{
	F_type F1;
	F_type F2;

public:
	DE_2_mutation (F_type& f1, F_type& f2);
	~DE_2_mutation () {};
	void set_F1 (F_type& f1) { F1 = f1; }
	void set_F2 (F_type& f2) { F2 = f2; }
	void apply ();
	void print (std::ostream& out);
};

// Crossover:
class Homogeneous_crossover: public Genetic_operator
{
	std::vector<double> probs; // should be in (0,1)

public:
	Homogeneous_crossover (std::vector<double> probs_);
	~Homogeneous_crossover () {};
	void apply ();
	void print (std::ostream& out);
};
class Flat_crossover: public Genetic_operator
{

public:
	Flat_crossover ();
	~Flat_crossover () {};
	void apply ();
	void print (std::ostream& out);
};
class Arithmetic_crossover: public Genetic_operator
{
	std::vector<double> alpha; // should be in (0,1)

public:
	Arithmetic_crossover (std::vector<double> a);
	~Arithmetic_crossover () {};
	void apply ();
	void print (std::ostream& out);
};
class Geometric_crossover: public Genetic_operator
{
	std::vector<double> alpha; // should be in (0,1)

public:
	Geometric_crossover (std::vector<double> a);
	~Geometric_crossover () {};
	void apply ();
	void print (std::ostream& out);
};
class BLX_alpha_crossover: public Genetic_operator
{
	std::vector<double> alpha;

public:
	BLX_alpha_crossover (std::vector<double> a);
	~BLX_alpha_crossover () {};
	void apply ();
	void print (std::ostream& out);
};
class DE_crossover: public Genetic_operator
{

public:
	DE_crossover ();
	~DE_crossover () {};
	void apply ();
	void print (std::ostream& out);
};

// Selection:
class Fitness_selection: public Genetic_operator
{

public:
	Fitness_selection ();
	~Fitness_selection () {};
	void apply ();
	void print (std::ostream& out);
};

#endif // UTILITY_H
