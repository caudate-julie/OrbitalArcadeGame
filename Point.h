#pragma once
#include <cmath>
#include <random>

/**------------------------------------------------------------
  2D-vector and basic operations: +, +=, -, -=, ==, 
  Also scalar multiplication (postfix only) and *= C.
  Special functions: module,  clockwise 90* turn 
  and unit vector given angle.
  Void constuctor gives (0, 0) point.
  -----------------------------------------------------------*/
struct Point
{
	float x;
	float y;

	/*--------------------------------------------------------*/
	Point()
	{
		this->x = this->y = 0;
	}

	/*--------------------------------------------------------*/
	Point(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	/*--------------------------------------------------------*/
	Point(int x, int y)
	{
		this->x = static_cast<float>(x);
		this->y = static_cast<float>(y);
	}

	/*--------------------------------------------------------*/
	Point(double x, double y)
	{
		this->x = static_cast<float>(x);
		this->y = static_cast<float>(y);
	}

	/*--------------------------------------------------------*/
	Point(float alpha)
	{
		this->x = cos(alpha);
		this->y = sin(alpha);
	}

	/*--------------------------------------------------------*/
	Point T() const
	{
		return Point(-this->y, this->x);
	}

	/*--------------------------------------------------------*/
	float module() const
	{
		return sqrt(this->x * this->x + this->y * this->y);
	}

	/*--------------------------------------------------------*/
	Point operator+(const Point &other) const
	{
		return Point(this->x + other.x, this->y + other.y);
	}

	/*--------------------------------------------------------*/
	Point operator-(const Point &other) const
	{
		return Point(this->x - other.x, this->y - other.y);
	}

	/*--------------------------------------------------------*/
	Point operator-() const
	{
		return Point(-this->x, -this->y);
	}

	/*--------------------------------------------------------*/
	bool operator==(const Point &other) const
	{
		return this->x == other.x && this->y == other.y;
	}

	/*--------------------------------------------------------*/
	Point& operator+=(const Point &other)
	{
		this->x += other.x;
		this->y += other.y;
		return *this;
	}

	/*--------------------------------------------------------*/
	Point& operator-=(const Point &other)
	{
		this->x -= other.x;
		this->y -= other.y;
		return *this;	
	}

	/*--------------------------------------------------------*/
	Point operator*(const float C) const
	{
		return Point(x * C, y * C);
	}

	/*--------------------------------------------------------*/
	Point& operator*=(const float C)
	{
		this->x *= C;
		this->y *= C;
		return *this;	
	}
};
