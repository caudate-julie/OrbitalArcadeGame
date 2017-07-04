#include <random>

/**------------------------------------------------------------
  Creates double random number in given boundaries.
  -----------------------------------------------------------*/
double d_random(double start, double end)
{
	return (static_cast<double>(rand())) / RAND_MAX * (end - start) + start;
}
