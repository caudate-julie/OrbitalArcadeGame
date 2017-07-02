#include "Star.h"
#include "Configuration.h"
#include "auxiliary.h"

/**------------------------------------------------------------
  Constructor - given position, makes random size and (once)
  size-based or random type.
  -----------------------------------------------------------*/
Star::Star()
{
	Configuration& conf = Configuration::get();
	this->position = Point(0, 0);
	this->size = pow(f_random(sqrt(conf.STAR_MIN_SIZE), sqrt(conf.STAR_MAX_SIZE)), 2);
	this->type = 'W';
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
	my_info.position = this->position;
	my_info.direction = Point();
	my_info.type = star;
	my_info.subtype = regular;
	my_info.size = this->size;
	return my_info;
}