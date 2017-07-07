#pragma once
#include "Point.h"
#include <cmath>

Point::Point()
	: x(0)
	, y(0)
{}


/*--------------------------------------------------------*/
Point::Point(double x, double y)
	: x(x)
	, y(y)
{}

/*--------------------------------------------------------*/
Point::Point(double alpha)
{
	x = cos(alpha);
	y = sin(alpha);
}

/*--------------------------------------------------------*/
Point Point::T() const
{
	return Point(-y, x);
}

/*--------------------------------------------------------*/
double Point::module() const
{
	return sqrt(x * x + y * y);
}

/*--------------------------------------------------------*/
Point Point::operator+(const Point &other) const
{
	return Point(x + other.x, y + other.y);
}

/*--------------------------------------------------------*/
Point Point::operator-(const Point &other) const
{
	return Point(x - other.x, y - other.y);
}

/*--------------------------------------------------------*/
Point Point::operator-() const
{
	return Point(-x, -y);
}

/*--------------------------------------------------------*/
bool Point::operator==(const Point &other) const
{
	return x == other.x && y == other.y;
}

/*--------------------------------------------------------*/
Point& Point::operator+=(const Point &other)
{
	x += other.x;
	y += other.y;
	return *this;
}

/*--------------------------------------------------------*/
Point& Point::operator-=(const Point &other)
{
	x -= other.x;
	y -= other.y;
	return *this;	
}

/*--------------------------------------------------------*/
Point Point::operator*(const double C) const
{
	return Point(x * C, y * C);
}

/*--------------------------------------------------------*/
Point& Point::operator*=(const double C)
{
	x *= C;
	y *= C;
	return *this;	
}

sf::Vector2f Point::vector()
{
	return sf::Vector2f(
		static_cast<float>(x), 
		static_cast<float>(y)
		);
}
