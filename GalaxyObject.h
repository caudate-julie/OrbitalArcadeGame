#pragma once
#include "Point.h"

enum Type { star, flyer };
enum Subtype { main, bot, regular };

struct GalaxyObject
{
	Point position;
	Point direction;
	double size;
	Type type;
	Subtype subtype;
};