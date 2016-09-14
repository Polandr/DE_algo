#include <iostream>
#include <cmath>
#include <operators.h>
#include <randomizer.h>
#include <factory.h>

using namespace std;

// Genetic operators:

Genetic_operator::~Genetic_operator ()
{
	for (int i = 0; i < in_data.size(); ++i)
		delete in_data[i];
	in_data.resize(0);	
	for (int i = 0; i < out_data.size(); ++i)
		delete out_data[i];
	out_data.resize(0);
}
void Genetic_operator::set_data (vector<Chromosome*>& data)
// Deep copy!
{
	int size = (data.size() < in_data.size()) ? data.size() : in_data.size();
	for (int i = 0; i < size; ++i)
		*in_data[i] = *data[i];
}
void Genetic_operator::get_data (vector<Chromosome*>& data)
// Instance copy!
{
	int size = (data.size() < out_data.size()) ? data.size() : out_data.size();
	for (int i = 0; i < size; ++i)
		data[i] = out_data[i];
}


// Mutation:

// DE/.../1:
DE_1_mutation::DE_1_mutation (F_type& f):
F(f)
{
	in_data.resize(3);
	out_data.resize(1);
	for (int i = 0; i < in_data.size(); ++i)
		in_data[i] = Factory::produce_chromosome();
	for (int i = 0; i < out_data.size(); ++i)
		out_data[i] = Factory::produce_chromosome();
}
void DE_1_mutation::apply ()
{
	*out_data[0] = *in_data[0] + (*in_data[1] - *in_data[2]) * F;
}
void DE_1_mutation::print (ostream& out)
{
	out << "Mutation DE/.../1:\n";
	out << "\tIn data size: " << in_data.size() << endl;
	out << "\tOut data size: " << out_data.size() << endl;
	out << "\tF: " << F << endl;
}
// DE/.../2:
DE_2_mutation::DE_2_mutation (F_type& f1, F_type& f2):
F1(f1), F2(f2)
{
	in_data.resize(5);
	out_data.resize(1);
	for (int i = 0; i < in_data.size(); ++i)
		in_data[i] = Factory::produce_chromosome();
	for (int i = 0; i < out_data.size(); ++i)
		out_data[i] = Factory::produce_chromosome();
}
void DE_2_mutation::apply ()
{
	*out_data[0] = *in_data[0] + (*in_data[1] - *in_data[2]) * F1 + (*in_data[3] - *in_data[4]) * F2;
}
void DE_2_mutation::print (ostream& out)
{
	out << "Mutation DE/.../2:\n";
	out << "\tIn data size: " << in_data.size() << endl;
	out << "\tOut data size: " << out_data.size() << endl;
	out << "\tF1: " << F1 << endl;
	out << "\tF2: " << F2 << endl;
}

// Crossover:

// Homogeneous crossover:
Homogeneous_crossover::Homogeneous_crossover (vector<double> probs_):
probs(probs_)
{
	in_data.resize(2);
	out_data.resize(2);
	for (int i = 0; i < in_data.size(); ++i)
		in_data[i] = Factory::produce_chromosome();
	for (int i = 0; i < out_data.size(); ++i)
		out_data[i] = Factory::produce_chromosome();
}
void Homogeneous_crossover::apply ()
{
	for (int i = 0; i < in_data[0] -> gene_num(); ++i)
	{
		Gene_type gene_1 = in_data[0] -> get_gene(i);
		Gene_type gene_2 = in_data[1] -> get_gene(i);
		double rand_num = Randomizer::double_rand(0.0,1.0);
		if (rand_num <= probs[i])
		{
			out_data[0] -> set_gene(i, gene_1);
			out_data[1] -> set_gene(i, gene_2);
		}
		else
		{
			out_data[0] -> set_gene(i, gene_2);
			out_data[1] -> set_gene(i, gene_1);
		}			
	}
}
void Homogeneous_crossover::print (ostream& out)
{
	out << "Homogeneous crossover:\n";
	out << "\tIn data size: " << in_data.size() << endl;
	out << "\tOut data size: " << out_data.size() << endl;
	out << "\tprobs: ( ";
	bool printed = false;
	for (int i = 0; i < probs.size(); ++i)
	{		
		if (i != 0 && i != probs.size() - 1 && probs[i] == probs[i-1] && probs[i] == probs[i+1])
		{
			if (!printed)
			{
				out << "..., ";
				printed = true;
			}
		}
		else
		{
			out << probs[i];
			printed = false;
		}

		if (i != probs.size() - 1 && !printed)
			out << ", ";
		if (i == probs.size() - 1)
			out << " )\n";
	}
}
// Flat crossover:
Flat_crossover::Flat_crossover ()
{
	in_data.resize(2);
	out_data.resize(1);
	for (int i = 0; i < in_data.size(); ++i)
		in_data[i] = Factory::produce_chromosome();
	for (int i = 0; i < out_data.size(); ++i)
		out_data[i] = Factory::produce_chromosome();
}
void Flat_crossover::apply ()
{
	for (int i = 0; i < in_data[0] -> gene_num(); ++i)
	{
		Gene_type gene_1 = in_data[0] -> get_gene(i);
		Gene_type gene_2 = in_data[1] -> get_gene(i);
		out_data[0] -> set_gene(i, Randomizer::randomize_gene(gene_1,gene_2));
	}
}
void Flat_crossover::print (ostream& out)
{
	out << "Flat crossover:\n";
	out << "\tIn data size: " << in_data.size() << endl;
	out << "\tOut data size: " << out_data.size() << endl;
}
// Arithmetic crssover:
Arithmetic_crossover::Arithmetic_crossover (vector<double> a):
alpha(a)
{
	in_data.resize(2);
	out_data.resize(2);
	for (int i = 0; i < in_data.size(); ++i)
		in_data[i] = Factory::produce_chromosome();
	for (int i = 0; i < out_data.size(); ++i)
		out_data[i] = Factory::produce_chromosome();
}
void Arithmetic_crossover::apply ()
{
	for (int i = 0; i < in_data[0] -> gene_num(); ++i)
	{
		Gene_type gene = alpha[i] * (in_data[0] -> get_gene(i)) + (1-alpha[i]) * (in_data[1] -> get_gene(i));
		out_data[0] -> set_gene(i, gene);
		gene = (1-alpha[i]) * (in_data[0] -> get_gene(i)) + alpha[i] * (in_data[1] -> get_gene(i));
		out_data[1] -> set_gene(i, gene);
	}
}
void Arithmetic_crossover::print (ostream& out)
{
	out << "Arithmetic crossover:\n";
	out << "\tIn data size: " << in_data.size() << endl;
	out << "\tOut data size: " << out_data.size() << endl;
	out << "\talpha: ( ";
	bool printed = false;
	for (int i = 0; i < alpha.size(); ++i)
	{		
		if (i != 0 && i != alpha.size() - 1 && alpha[i] == alpha[i-1] && alpha[i] == alpha[i+1])
		{
			if (!printed)
			{
				out << "..., ";
				printed = true;
			}
		}
		else
		{
			out << alpha[i];
			printed = false;
		}

		if (i != alpha.size() - 1 && !printed)
			out << ", ";
		if (i == alpha.size() - 1)
			out << " )\n";
	}
}
// Geometric crssover:
Geometric_crossover::Geometric_crossover (vector<double> a):
alpha(a)
{
	in_data.resize(2);
	out_data.resize(2);
	for (int i = 0; i < in_data.size(); ++i)
		in_data[i] = Factory::produce_chromosome();
	for (int i = 0; i < out_data.size(); ++i)
		out_data[i] = Factory::produce_chromosome();
}
void Geometric_crossover::apply ()
{
	for (int i = 0; i < in_data[0] -> gene_num(); ++i)
	{
		if (in_data[0] -> get_gene(i) <= 0 && in_data[1] -> get_gene(i) <= 0)
		{
			Gene_type gene =  pow(-(in_data[0] -> get_gene(i)),alpha[i]) + pow(-(in_data[1] -> get_gene(i)),1 - alpha[i]);
			out_data[0] -> set_gene(i, -gene);
			gene = pow(-(in_data[0] -> get_gene(i)),1 - alpha[i]) + pow(-(in_data[1] -> get_gene(i)),alpha[i]);
			out_data[1] -> set_gene(i, -gene);
		}
		else
		if (in_data[0] -> get_gene(i) >= 0 && in_data[1] -> get_gene(i) >= 0)
		{
			Gene_type gene =  pow(in_data[0] -> get_gene(i),alpha[i]) + pow(in_data[1] -> get_gene(i),1 - alpha[i]);
			out_data[0] -> set_gene(i, gene);
			gene = pow(in_data[0] -> get_gene(i),1 - alpha[i]) + pow(in_data[1] -> get_gene(i),alpha[i]);
			out_data[1] -> set_gene(i, gene);
		}
		else
		{
			out_data[0] -> set_gene(i, in_data[0] -> get_gene(i));
			out_data[1] -> set_gene(i, in_data[1] -> get_gene(i));
		}
	}
}
void Geometric_crossover::print (ostream& out)
{
	out << "Geometric crossover:\n";
	out << "\tIn data size: " << in_data.size() << endl;
	out << "\tOut data size: " << out_data.size() << endl;
	out << "\talpha: ( ";
	bool printed = false;
	for (int i = 0; i < alpha.size(); ++i)
	{		
		if (i != 0 && i != alpha.size() - 1 && alpha[i] == alpha[i-1] && alpha[i] == alpha[i+1])
		{
			if (!printed)
			{
				out << "..., ";
				printed = true;
			}
		}
		else
		{
			out << alpha[i];
			printed = false;
		}

		if (i != alpha.size() - 1 && !printed)
			out << ", ";
		if (i == alpha.size() - 1)
			out << " )\n";
	}
}
// BLX-alpha crssover:
BLX_alpha_crossover::BLX_alpha_crossover (vector<double> a):
alpha(a)
{
	in_data.resize(2);
	out_data.resize(1);
	for (int i = 0; i < in_data.size(); ++i)
		in_data[i] = Factory::produce_chromosome();
	for (int i = 0; i < out_data.size(); ++i)
		out_data[i] = Factory::produce_chromosome();
}
void BLX_alpha_crossover::apply ()
{
	for (int i = 0; i < in_data[0] -> gene_num(); ++i)
	{
		Gene_type gene_1 = in_data[0] -> get_gene(i);
		Gene_type gene_2 = in_data[1] -> get_gene(i);
		gene_1 = (gene_1 < gene_2)? gene_1 : gene_2;
		gene_2 = (gene_1 < gene_2)? gene_2 : gene_1;
		Gene_type dst = gene_2 - gene_1;
		out_data[0] -> set_gene(i, Randomizer::randomize_gene(gene_1 - alpha[i]*dst, gene_2 + alpha[i]*dst));
	}
}
void BLX_alpha_crossover::print (ostream& out)
{
	out << "BLX-alpha crossover:\n";
	out << "\tIn data size: " << in_data.size() << endl;
	out << "\tOut data size: " << out_data.size() << endl;
	out << "\talpha: ( ";
	bool printed = false;
	for (int i = 0; i < alpha.size(); ++i)
	{		
		if (i != 0 && i != alpha.size() - 1 && alpha[i] == alpha[i-1] && alpha[i] == alpha[i+1])
		{
			if (!printed)
			{
				out << "..., ";
				printed = true;
			}
		}
		else
		{
			out << alpha[i];
			printed = false;
		}

		if (i != alpha.size() - 1 && !printed)
			out << ", ";
		if (i == alpha.size() - 1)
			out << " )\n";
	}
}
// DE crossover (in original article of Storn and Price):
DE_crossover::DE_crossover ()
{
	in_data.resize(2);
	out_data.resize(1);
	for (int i = 0; i < in_data.size(); ++i)
		in_data[i] = Factory::produce_chromosome();
	for (int i = 0; i < out_data.size(); ++i)
		out_data[i] = Factory::produce_chromosome();
}
void DE_crossover::apply ()
{
	for (int i = 0; i < in_data[0] -> gene_num(); ++i)
		out_data[0] -> set_gene(i, in_data[0] -> get_gene(i));
	int start = Randomizer::integer_rand(0, in_data[0] -> gene_num() - 1);
	int length = Randomizer::integer_rand(1, in_data[0] -> gene_num());
	for (int i = start; i < start + length; ++i)
	{
		int idx = i % in_data[0] -> gene_num();
		out_data[0] -> set_gene(idx, in_data[1] -> get_gene(idx));
	}
}
void DE_crossover::print (ostream& out)
{
	out << "DE crossover (invented by Price and Storn):\n";
	out << "\tIn data size: " << in_data.size() << endl;
	out << "\tOut data size: " << out_data.size() << endl;
}

// Selection:
// Tournament selection by fitness value
Fitness_selection::Fitness_selection ()
{
	in_data.resize(2);
	out_data.resize(1);
	for (int i = 0; i < in_data.size(); ++i)
		in_data[i] = Factory::produce_chromosome();
	for (int i = 0; i < out_data.size(); ++i)
		out_data[i] = Factory::produce_chromosome();
}
void Fitness_selection::apply ()
{
	if (*in_data[0] > *in_data[1])
		*out_data[0] = *in_data[0];
	else
		*out_data[0] = *in_data[1];
}
void Fitness_selection::print (ostream& out)
{
	out << "Selection by fitness value:\n";
	out << "\tIn data size: " << in_data.size() << endl;
	out << "\tOut data size: " << out_data.size() << endl;
}
