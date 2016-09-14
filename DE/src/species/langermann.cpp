#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cmath>
#include <species/langermann.h>
#include <parameters.h>
#include <randomizer.h>

using namespace std;

// For M = 5, D = 2
// C = [1,2,5,2,3]
// A = [[3,5],[5,2],[2,1],[1,4],[7,9]]

const int D = 2;
const int M = 5;
double C[5];
double A[10];

const double PI = std::acos(-1);

void Langermann_chromosome::randomize ()
{
	for (int i = 0; i < data.size(); ++i)
		data[i] = Randomizer::randomize_gene(0,10);	
}
void Langermann_chromosome::get_data (void* _data) const
{
	for (int i = 0; i < data.size(); ++i)
		(static_cast<Gene_type*>(_data))[i] = data[i];
	//memcpy(_data, reinterpret_cast<void*>(data), data.size() * sizeof(Gene_type));
}
void Langermann_chromosome::set_data (void* _data)
{
	for (int i = 0; i < D; ++i)
		data[i] = (static_cast<Gene_type*>(_data))[i];
	//memcpy(reinterpret_cast<void*>(data), _data, data.size() * sizeof(Gene_type));
}

Langermann_chromosome::Langermann_chromosome ()
{
	P::problem_dimension = D;

	C[0] = 1;
	C[1] = 2;
	C[2] = 5;
	C[3] = 2;
	C[4] = 3;

	A[0] = 3; A[1] = 5;
	A[2] = 5; A[3] = 2;
	A[4] = 2; A[5] = 1;
	A[6] = 1; A[7] = 4;
	A[8] = 7; A[9] = 9;

	data.resize(D);
}
Langermann_chromosome::Langermann_chromosome (const vector<Gene_type> _data)
{
	data = _data;
}
Langermann_chromosome::Langermann_chromosome (const Langermann_chromosome& other)
{
	data.resize(D);
	for (int i = 0; i < data.size(); ++i)
		data[i] = other.get_gene(i);
}
Langermann_chromosome::~Langermann_chromosome ()
{}

Fitness_type Langermann_chromosome::fitness () const
{
	Fitness_type sum(0);
	int ofs = 0;
	for (int i = 0; i < M; ++i)
	{
		Fitness_type loc_sum(0);
		for (int j = 0; j < D; ++j)
		{
			loc_sum += ((data[j] - A[ofs]) * (data[j] - A[ofs]));
			ofs++;
		}
		sum += C[i] * exp(-loc_sum/PI) * cos(PI*loc_sum);
	}
	return -sum;
}

Chromosome& Langermann_chromosome::operator + (const Chromosome& other) const
{
	Langermann_chromosome& out(const_cast<Langermann_chromosome&>(*this));
	for (int i = 0; i < data.size(); ++i)
		out[i] += (dynamic_cast<const Langermann_chromosome&>(other)).get_gene(i);
	return out;
}
Chromosome& Langermann_chromosome::operator - (const Chromosome& other) const
{
	Langermann_chromosome& out(const_cast<Langermann_chromosome&>(*this));
	for (int i = 0; i < data.size(); ++i)
		out[i] -= (dynamic_cast<const Langermann_chromosome&>(other)).get_gene(i);
	return out;
}
Chromosome& Langermann_chromosome::operator * (F_type& F) const
{
	Langermann_chromosome& out(const_cast<Langermann_chromosome&>(*this));
	for (int i = 0; i < data.size(); ++i)
		out[i] *= F;
	return out;
}
Chromosome& Langermann_chromosome::operator = (const Chromosome& other)
{
	data.resize(D);
	for (int i = 0; i < D; ++i)
		data[i] = (dynamic_cast<const Langermann_chromosome&>(other)).get_gene(i);
	return *this;
}

bool Langermann_chromosome::operator > (Chromosome& other)
{
	return fitness() < (dynamic_cast<Langermann_chromosome&>(other)).fitness();
}
bool Langermann_chromosome::operator < (Chromosome& other)
{
	return fitness() > (dynamic_cast<Langermann_chromosome&>(other)).fitness();
}
bool Langermann_chromosome::operator == (Chromosome& other)
{
	return fitness() == (dynamic_cast<Langermann_chromosome&>(other)).fitness();
}
bool Langermann_chromosome::operator != (Chromosome& other)
{
	return fitness() != (dynamic_cast<Langermann_chromosome&>(other)).fitness();
}

void Langermann_chromosome::print (ostream& out) const
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

void Langermann_chromosome::print_info (std::ostream& out) const
{
	out << "The problem is to find global minimum of next function (Rosenbrock's function):\n";
	out << "-sum (0..M-1) (C[i] * exp(-sum(0..D-1)((x[j]-A[i,j])*(x[j]-A[i,j])) / PI) * cos(PI * sum(0..D-1)((x[j]-A[i,j])*(x[j]-A[i,j])))) in D dimensions\n";
	out << "Exact answer for D = 2 is f(2.00299219,1.006096) = -5.1621259\n";
}
