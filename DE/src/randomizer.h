#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <cstdlib>
#include <types.h>

class Randomizer
{
	static unsigned seed;

public:
	static double normalized_rand ();
	static double double_rand (double l = 0, double r = 1);
	static int integer_rand (int l = 0, int r = RAND_MAX);
	static Gene_type randomize_gene (Gene_type l = 0, Gene_type r = 1);	
};

#endif // RANDOMIZER_H
