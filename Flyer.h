#pragma once
#include "Point.h"
#include <SFML\Graphics.hpp>

/**------------------------------------------------------------
  Flyer is the main game character (also base class for bots).
  -----------------------------------------------------------*/
class Flyer
{
public:
	Flyer();
	~Flyer(void);

	Point position;
	Point velocity;

	void move(const Point& acceleration);
	Point engine_acceleration(char direction) const;
};