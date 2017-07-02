#include <random>

double f_random(double start, double end)
{
	return (static_cast<double>(rand())) / RAND_MAX * (end - start) + start;
}
