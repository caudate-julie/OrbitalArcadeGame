#include "Star.h"
#include <random>
#include "Configuration.h"
//#include <time.h>

/**------------------------------------------------------------
  Constructor - given position, makes random size and (once)
  size-based or random type.
  -----------------------------------------------------------*/
Star::Star()
{
	Configuration& conf = Configuration::get();
	this->position = Point(0, 0);
	this->size = (float)(rand() % conf.STAR_ADD_SIZE + conf.STAR_MIN_SIZE);
	this->type = 'W';
}

/**------------------------------------------------------------
  void destructor.
  -----------------------------------------------------------*/
Star::~Star(void) {}
