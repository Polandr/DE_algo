#include <cstdlib>
#include <cstring>
#include <iostream>
#include <species/rosenbrock.h>
#include <parameters.h>
#include <randomizer.h>

using namespace std;

void Rosenbrock_chromosome::randomize ()
{
	for (int i = 0; i < data.size(); ++i)
		data[i] = Randomizer::randomize_gene(-2.048,2.048);	
}
void Rosenbrock_chromosome::get_data (void* _data) const
{
	for (int i = 0; i < data.size(); ++i)
		(static_cast<Gene_type*>(_data))[i] = data[i];
	//memcpy(_data, reinterpret_cast<void*>(data), data.size() * sizeof(Gene_type));
}
void Rosenbrock_chromosome::set_data (void* _data)
{
	for (int i = 0; i < P::problem_dimension; ++i)
		data[i] = (static_cast<Gene_type*>(_data))[i];
	//memcpy(reinterpret_cast<void*>(data), _data, data.size() * sizeof(Gene_type));
}

Rosenbrock_chromosome::Rosenbrock_chromosome ()
{
	data.resize(P::problem_dimension);
}
Rosenbrock_chromosome::Rosenbrock_chromosome (const vector<Gene_type> _data)
{
	data = _data;
}
Rosenbrock_chromosome::Rosenbrock_chromosome (const Rosenbrock_chromosome& other)
{
	data.resize(P::problem_dimension);
	for (int i = 0; i < data.size(); ++i)
		data[i] = other.get_gene(i);
}
Rosenbrock_chromosome::~Rosenbrock_chromosome ()
{}

Fitness_type Rosenbrock_chromosome::fitness () const
{
	Fitness_type sum(0);
	for (int i = 0; i < data.size()-1; ++i)
	{
		sum += (1 - data[i]) * (1 - data[i]);
		sum += 100 * (data[i+1] - data[i] * data[i]) * (data[i+1] - data[i] * data[i]);
	}
	return sum;
}

Chromosome& Rosenbrock_chromosome::operator + (const Chromosome& other) const
{
	Rosenbrock_chromosome& out(const_cast<Rosenbrock_chromosome&>(*this));
	for (int i = 0; i < data.size(); ++i)
		out[i] += (dynamic_cast<const Rosenbrock_chromosome&>(other)).get_gene(i);
	return out;
}
Chromosome& Rosenbrock_chromosome::operator - (const Chromosome& other) const
{
	Rosenbrock_chromosome& out(const_cast<Rosenbrock_chromosome&>(*this));
	for (int i = 0; i < data.size(); ++i)
		out[i] -= (dynamic_cast<const Rosenbrock_chromosome&>(other)).get_gene(i);
	return out;
}
Chromosome& Rosenbrock_chromosome::operator * (F_type& F) const
{
	Rosenbrock_chromosome& out(const_cast<Rosenbrock_chromosome&>(*this));
	for (int i = 0; i < data.size(); ++i)
		out[i] *= F;
	return out;
}
Chromosome& Rosenbrock_chromosome::operator = (const Chromosome& other)
{
	data.resize(P::problem_dimension);
	for (int i = 0; i < P::problem_dimension; ++i)
		data[i] = (dynamic_cast<const Rosenbrock_chromosome&>(other)).get_gene(i);
	return *this;
}

bool Rosenbrock_chromosome::operator > (Chromosome& other)
{
	return fitness() < (dynamic_cast<Rosenbrock_chromosome&>(other)).fitness();
}
bool Rosenbrock_chromosome::operator < (Chromosome& other)
{
	return fitness() > (dynamic_cast<Rosenbrock_chromosome&>(other)).fitness();
}
bool Rosenbrock_chromosome::operator == (Chromosome& other)
{
	return fitness() == (dynamic_cast<Rosenbrock_chromosome&>(other)).fitness();
}
bool Rosenbrock_chromosome::operator != (Chromosome& other)
{
	return fitness() != (dynamic_cast<Rosenbrock_chromosome&>(other)).fitness();
}

void Rosenbrock_chromosome::print (ostream& out) const
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
	out << flush;
}

void Rosenbrock_chromosome::print_info (std::ostream& out) const
{
	out << "The problem is to find global minimum of next function (Rosenbrock's function):\n";
	out << "sum (0..N-1) ((1-x[i])*(1-x[i]) + 100*(x[i+1]-x[i]*x[i])*(x[i+1]-x[i]*x[i])) in N dimensions\n";
	out << "Exact answer is f(1,...,1) = 0\n";
}
