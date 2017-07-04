#pragma once
#include "Point.h"
#include <SFML\Graphics.hpp>
#include "GalaxyObject.h"

/**------------------------------------------------------------
  Flyer is the main game character (also base class for bots).
  -----------------------------------------------------------*/
class Flyer
{
public:
	Flyer();
	virtual ~Flyer(void);

	Point position;
	Point velocity;

	void move(const Point& acceleration);
	Point engine_acceleration(char direction) const;
	virtual GalaxyObject info() const;
};
