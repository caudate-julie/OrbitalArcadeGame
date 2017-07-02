#include <random>

float f_random(float start, float end)
{
	return (static_cast<float>(rand())) / RAND_MAX * (end - start) + start;
}

float f_random(double start, double end)
{
	return f_random(static_cast<float>(start), static_cast<float>(end));
}

float f_random(int start, int end)
{
	return f_random(static_cast<float>(start), static_cast<float>(end));
}
