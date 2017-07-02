#pragma once
#include "Point.h"

enum Type { star, flyer };
enum Subtype { main, bot, regular };

struct GalaxyObject
{
	Point position;
	Point direction;
	float size;
	Type type;
	Subtype subtype;
};