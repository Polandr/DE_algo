#include <cstdlib>
#include <ctime>
#include <cmath>
#include <randomizer.h>
#include <parallel.h>

using namespace std;

unsigned Randomizer::seed = 0;

double Randomizer::normalized_rand ()
{
	Randomizer::seed += time(NULL) + Processors::coord();
	srand(Randomizer::seed);
	return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
}

double Randomizer::double_rand (double l, double r)
{
	l = (l < r)? l : r;
	return l + normalized_rand() * abs(r - l);
}

int Randomizer::integer_rand (int l, int r)
{
	l = (l < r)? l : r;
	Randomizer::seed += time(NULL) + Processors::coord();
	srand(Randomizer::seed);
	return l + rand() % abs(r - l);
}

Gene_type Randomizer::randomize_gene (Gene_type l, Gene_type r)
{
	return double_rand(l, r);
}
