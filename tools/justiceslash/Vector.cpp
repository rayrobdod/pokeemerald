#include "Vector.h"

Vector::Vector() : x(0), y(0)
{
}

Vector::Vector(double x, double y)
{
	this->x = x;
	this->y = y;
}

Vector::~Vector()
{
}

Vector Vector::operator+(const Vector& other) const
{
	return Vector(this->x + other.x, this->y + other.y);
}

Vector Vector::operator-(const Vector& other) const
{
	return Vector(this->x - other.x, this->y - other.y);
}

Vector Vector::operator*(double multiplicand) const
{
	return Vector(this->x * multiplicand, this->y * multiplicand);
}

double Vector::distanceSq(const Vector& other) const
{
	return (this->x - other.x) * (this->x - other.x) + (this->y - other.y) * (this->y - other.y);
}

std::ostream& operator<<(std::ostream& os, const Vector& self)
{
	os << "<" << self.x << ", " << self.y << ">";
	return os;
}
