#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cmath>
#include <species/rastrigin.h>
#include <parameters.h>
#include <randomizer.h>

const double PI = std::acos(-1);

using namespace std;

void Rastrigin_chromosome::randomize ()
{
	for (int i = 0; i < data.size(); ++i)
		data[i] = Randomizer::randomize_gene(-5.12,5.12);	
}
void Rastrigin_chromosome::get_data (void* _data) const
{
	for (int i = 0; i < data.size(); ++i)
		(static_cast<Gene_type*>(_data))[i] = data[i];
	//memcpy(_data, reinterpret_cast<void*>(data), data.size() * sizeof(Gene_type));
}
void Rastrigin_chromosome::set_data (void* _data)
{
	for (int i = 0; i < P::problem_dimension; ++i)
		data[i] = (static_cast<Gene_type*>(_data))[i];
	//memcpy(reinterpret_cast<void*>(data), _data, data.size() * sizeof(Gene_type));
}

Rastrigin_chromosome::Rastrigin_chromosome ()
{
	data.resize(P::problem_dimension);
}
Rastrigin_chromosome::Rastrigin_chromosome (const vector<Gene_type> _data)
{
	data = _data;
}
Rastrigin_chromosome::Rastrigin_chromosome (const Rastrigin_chromosome& other)
{
	data.resize(P::problem_dimension);
	for (int i = 0; i < data.size(); ++i)
		data[i] = other.get_gene(i);
}
Rastrigin_chromosome::~Rastrigin_chromosome ()
{}

Fitness_type Rastrigin_chromosome::fitness () const
{
	Fitness_type sum = A * data.size();
	for (int i = 0; i < data.size(); ++i)
	{
		sum += data[i]*data[i] - A * cos(2*PI*data[i]);
	}
	return sum;
}

Chromosome& Rastrigin_chromosome::operator + (const Chromosome& other) const
{
	Rastrigin_chromosome& out(const_cast<Rastrigin_chromosome&>(*this));
	for (int i = 0; i < data.size(); ++i)
		out[i] += (dynamic_cast<const Rastrigin_chromosome&>(other)).get_gene(i);
	return out;
}
Chromosome& Rastrigin_chromosome::operator - (const Chromosome& other) const
{
	Rastrigin_chromosome& out(const_cast<Rastrigin_chromosome&>(*this));
	for (int i = 0; i < data.size(); ++i)
		out[i] -= (dynamic_cast<const Rastrigin_chromosome&>(other)).get_gene(i);
	return out;
}
Chromosome& Rastrigin_chromosome::operator * (F_type& F) const
{
	Rastrigin_chromosome& out(const_cast<Rastrigin_chromosome&>(*this));
	for (int i = 0; i < data.size(); ++i)
		out[i] *= F;
	return out;
}
Chromosome& Rastrigin_chromosome::operator = (const Chromosome& other)
{
	data.resize(P::problem_dimension);
	for (int i = 0; i < P::problem_dimension; ++i)
		data[i] = (dynamic_cast<const Rastrigin_chromosome&>(other)).get_gene(i);
	return *this;
}

bool Rastrigin_chromosome::operator > (Chromosome& other)
{
	return fitness() < (dynamic_cast<Rastrigin_chromosome&>(other)).fitness();
}
bool Rastrigin_chromosome::operator < (Chromosome& other)
{
	return fitness() > (dynamic_cast<Rastrigin_chromosome&>(other)).fitness();
}
bool Rastrigin_chromosome::operator == (Chromosome& other)
{
	return fitness() == (dynamic_cast<Rastrigin_chromosome&>(other)).fitness();
}
bool Rastrigin_chromosome::operator != (Chromosome& other)
{
	return fitness() != (dynamic_cast<Rastrigin_chromosome&>(other)).fitness();
}

void Rastrigin_chromosome::print (ostream& out) const
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

void Rastrigin_chromosome::print_info (std::ostream& out) const
{
	out << "The problem is to find global minimum of next function (Rastrigin's function):\n";
	out << "A*N + sum (0..N-1) (x[i]*x[i] + A*cos(2*PI*x[i])) in N dimensions (A = 10)\n";
	out << "Exact answer is f(0,...,0) = 0\n";
}
