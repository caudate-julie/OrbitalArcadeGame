#pragma once
#include <SFML\Graphics.hpp>

/**------------------------------------------------------------
  2D-vector and basic operations: +, +=, -, -=, ==, 
  Also scalar multiplication (postfix only) and *= C.
  Special functions: module,  clockwise 90* turn 
  and unit vector given angle.
  Void constuctor gives (0, 0) point.
  -----------------------------------------------------------*/
struct Point
{
	double x;
	double y;

	Point();
	Point(double x, double y);
	Point(double alpha);
	Point T() const;
	double module() const;
	Point operator+(const Point &other) const;
	Point operator-(const Point &other) const;
	Point operator-() const;
	bool operator==(const Point &other) const;
	Point& operator+=(const Point &other);
	Point& operator-=(const Point &other);
	Point operator*(const double C) const;
	Point& operator*=(const double C);
	sf::Vector2f vector();
};
