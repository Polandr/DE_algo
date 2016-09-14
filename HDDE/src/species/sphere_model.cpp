#include <cstdlib>
#include <cstring>
#include <iostream>
#include <species/sphere_model.h>
#include <parameters.h>
#include <randomizer.h>
#include <parallel.h>

using namespace std;

void Sphere_model_chromosome::randomize ()
{
	for (int i = 0; i < data.size(); ++i)
		data[i] = Randomizer::randomize_gene(-1,1);	
}
void Sphere_model_chromosome::get_data (void* _data) const
{
	for (int i = 0; i < data.size(); ++i)
		(static_cast<Gene_type*>(_data))[i] = data[i];
	//memcpy(_data, reinterpret_cast<void*>(data), data.size() * sizeof(Gene_type));
}
void Sphere_model_chromosome::set_data (void* _data)
{
	for (int i = 0; i < P::local_dimension; ++i)
		data[i] = (static_cast<Gene_type*>(_data))[i];
	//memcpy(reinterpret_cast<void*>(data), _data, data.size() * sizeof(Gene_type));
}

Sphere_model_chromosome::Sphere_model_chromosome ()
{
	data.resize(P::local_dimension);
}
Sphere_model_chromosome::Sphere_model_chromosome (const int dim)
{
	data.resize(dim);
}
Sphere_model_chromosome::Sphere_model_chromosome (const vector<Gene_type> _data)
{
	data = _data;
}
Sphere_model_chromosome::Sphere_model_chromosome (const Sphere_model_chromosome& other)
{
	data.resize(other.gene_num());
	for (int i = 0; i < data.size(); ++i)
		data[i] = other.get_gene(i);
}
Sphere_model_chromosome::~Sphere_model_chromosome ()
{}

Fitness_type Sphere_model_chromosome::fitness () const
{
	Fitness_type sum(0);
	for (int i = 0; i < data.size(); ++i)
	{
		sum += data[i] * data[i];
	}
	return sum;
}
Fitness_type Sphere_model_chromosome::distributed_fitness () const
{
	Fitness_type sum(0);
	for (int i = 0; i < data.size(); ++i)
	{
		sum += data[i] * data[i];
	}
	// Works only if Fitness_type is castable to double
	sum = Processors::sum_reduction(sum);
	return sum;
}

Chromosome& Sphere_model_chromosome::operator + (const Chromosome& other) const
{
	Sphere_model_chromosome& out(const_cast<Sphere_model_chromosome&>(*this));
	for (int i = 0; i < data.size(); ++i)
		out[i] += (dynamic_cast<const Sphere_model_chromosome&>(other)).get_gene(i);
	return out;
}
Chromosome& Sphere_model_chromosome::operator - (const Chromosome& other) const
{
	Sphere_model_chromosome& out(const_cast<Sphere_model_chromosome&>(*this));
	for (int i = 0; i < data.size(); ++i)
		out[i] -= (dynamic_cast<const Sphere_model_chromosome&>(other)).get_gene(i);
	return out;
}
Chromosome& Sphere_model_chromosome::operator * (F_type& F) const
{
	Sphere_model_chromosome& out(const_cast<Sphere_model_chromosome&>(*this));
	for (int i = 0; i < data.size(); ++i)
		out[i] *= F;
	return out;
}
Chromosome& Sphere_model_chromosome::operator = (const Chromosome& other)
{
	data.resize(other.gene_num());
	for (int i = 0; i < data.size(); ++i)
		data[i] = (dynamic_cast<const Sphere_model_chromosome&>(other)).get_gene(i);
	return *this;
}

bool Sphere_model_chromosome::operator > (Chromosome& other)
{
	return fitness() < (dynamic_cast<Sphere_model_chromosome&>(other)).fitness();
}
bool Sphere_model_chromosome::operator < (Chromosome& other)
{
	return fitness() > (dynamic_cast<Sphere_model_chromosome&>(other)).fitness();
}
bool Sphere_model_chromosome::operator == (Chromosome& other)
{
	return fitness() == (dynamic_cast<Sphere_model_chromosome&>(other)).fitness();
}
bool Sphere_model_chromosome::operator != (Chromosome& other)
{
	return fitness() != (dynamic_cast<Sphere_model_chromosome&>(other)).fitness();
}

void Sphere_model_chromosome::print (ostream& out) const
{
	out << "(";
	for (int i = 0; i < data.size(); ++i)
	{
		out << data[i];
		if (i != data.size()-1)
			out << ",";
	}
	out << ") ";
	if (data.size() > 1)
		out << fitness() << endl;
	else
		out << endl;
}

void Sphere_model_chromosome::print_info (ostream& out) const
{
	out << "The problem is to find global minimum of next function (Rosenbrock's function):\n";
	out << "sum (0..N-1) (x[i] * x[i]) in N dimensions\n";
	out << "Exact answer is f(0,...,0) = 0\n";
}
