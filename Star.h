#pragma once
#include "Point.h"
#include "GalaxyObject.h"

/**------------------------------------------------------------
  Stars are centers of gravitation.
  * position - absolute (whether or not they are in the screen,
  defined by graphics module),
  * size - both raduis and mass (yeah, mass is linear! not too
  unphysical actually),
  * type - is aimed to different drawing options.

  All fields are public at the time for quick access.
  -----------------------------------------------------------*/
class Star
{
public:
	Star();
	~Star(void);

	Point position;
	float size;
	char type;

	virtual GalaxyObject info() const;
};
