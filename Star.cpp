#include "Star.h"
#include "Configuration.h"
#include "auxiliary.h"

extern Configuration* config;
/**------------------------------------------------------------
  Constructor - given position, makes random size and (once)
  size-based or random type.
  -----------------------------------------------------------*/
Star::Star()
{
	position = Point(0, 0);
	size = pow(d_random(sqrt(config->STAR_MIN_SIZE), sqrt(config->STAR_MAX_SIZE)), 2);
	type = 'W';
}

/**------------------------------------------------------------
  void destructor.
  -----------------------------------------------------------*/
Star::~Star(void) {}

/**------------------------------------------------------------
  Makes class with info about the object to pass outside
  Game class (mainly to graphics class).
  -----------------------------------------------------------*/
GalaxyObject Star::info() const 
{
	GalaxyObject my_info;
	my_info.position = position;
	my_info.direction = Point();
	my_info.type = star;
	my_info.subtype = regular;
	my_info.size = size;
	return my_info;
}