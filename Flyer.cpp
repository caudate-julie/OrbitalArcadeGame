#include "Flyer.h"
#include "Configuration.h"
//#include <memory>

extern Configuration* config;

/**------------------------------------------------------------
  Creates a flyer with starting velocity in the middle
  of the screen.
  -----------------------------------------------------------*/
Flyer::Flyer(void) 
{
	velocity = Point(config->INIT_VELOCITY, 0.f);
	position = Point(0, 0);
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
	velocity += acceleration;
	position += velocity;
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
	int orientation = (direction == 'R') ? 1 : -1;
	return velocity.T() * (orientation / velocity.module() * config->ENGINE);
}

/**------------------------------------------------------------
  Makes class with info about the object to pass outside
  game class (mainly to graphics class).
  -----------------------------------------------------------*/
GalaxyObject Flyer::info() const 
{
	GalaxyObject my_info;
	my_info.position = position;
	my_info.direction = velocity;
	my_info.type = flyer;
	my_info.subtype = main;
	my_info.size = config->FLYER_SIZE;
	return my_info;
}