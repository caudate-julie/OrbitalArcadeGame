#pragma once
#include "Point.h"
#include <cmath>

Point::Point()
{
	this->x = this->y = 0;
}


/*--------------------------------------------------------*/
Point::Point(double x, double y)
{
	this->x = x;
	this->y = y;
}

/*--------------------------------------------------------*/
Point::Point(double alpha)
{
	this->x = cos(alpha);
	this->y = sin(alpha);
}

/*--------------------------------------------------------*/
Point Point::T() const
{
	return Point(-this->y, this->x);
}

/*--------------------------------------------------------*/
double Point::module() const
{
	return sqrt(this->x * this->x + this->y * this->y);
}

/*--------------------------------------------------------*/
Point Point::operator+(const Point &other) const
{
	return Point(this->x + other.x, this->y + other.y);
}

/*--------------------------------------------------------*/
Point Point::operator-(const Point &other) const
{
	return Point(this->x - other.x, this->y - other.y);
}

/*--------------------------------------------------------*/
Point Point::operator-() const
{
	return Point(-this->x, -this->y);
}

/*--------------------------------------------------------*/
bool Point::operator==(const Point &other) const
{
	return this->x == other.x && this->y == other.y;
}

/*--------------------------------------------------------*/
Point& Point::operator+=(const Point &other)
{
	this->x += other.x;
	this->y += other.y;
	return *this;
}

/*--------------------------------------------------------*/
Point& Point::operator-=(const Point &other)
{
	this->x -= other.x;
	this->y -= other.y;
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
	this->x *= C;
	this->y *= C;
	return *this;	
}

sf::Vector2f Point::vector()
{
	return sf::Vector2f(
		static_cast<float>(this->x), 
		static_cast<float>(this->y)
		);
}
