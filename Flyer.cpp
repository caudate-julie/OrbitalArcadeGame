#include "Flyer.h"
#include "Configuration.h"

/**------------------------------------------------------------
  Creates a flyer with starting velocity in the middle
  of the screen.
  -----------------------------------------------------------*/
Flyer::Flyer(void) 
{
	Configuration& conf = Configuration::get();
	this->velocity = Point(2, 0);
	this->position = Point(conf.WIDTH / 2, conf.HEIGHT / 2);
}

/**------------------------------------------------------------
  Void destructor.
  -----------------------------------------------------------*/
Flyer::~Flyer(void) {}

/**------------------------------------------------------------
  Given acceleration vector, changes position and velocity.
  Delta t equals 1.
  -----------------------------------------------------------*/
void Flyer::move(const Point& acceleration)
{
	this->velocity += acceleration;
	this->position += this->velocity;
}

/**------------------------------------------------------------
  User may add momentary acceleration to flyer from keyboard.
  Also bots make voluntary moves to avoid collisions.
  Acceleration is perpendicular to flyer's current velocity.
  Char should be 'L' - left or 'R' - right.
  Char 'N' means no acceleration (thread concurrency stub).

  Returns the acceleration vector (not added directly to 
  velocity for bots' mock flights).
  -----------------------------------------------------------*/
Point Flyer::engine_acceleration(char direction) const
{
	if (direction == 'N') return Point(0, 0);
	Configuration& conf = Configuration::get();
	int orientation = (direction == 'R') ? 1 : -1;
	return this->velocity.T() * (orientation / this->velocity.module() * conf.ENGINE);
}