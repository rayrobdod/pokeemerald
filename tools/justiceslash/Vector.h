#ifndef GUARD_VECTOR_H
#define GUARD_VECTOR_H

#include <ostream>

class Vector
{
public:
	double x;
	double y;

	Vector();
	Vector(double x, double y);
	~Vector();

	Vector operator+(const Vector&) const;
	Vector operator-(const Vector&) const;
	Vector operator*(double) const;
	double distanceSq(const Vector&) const;

	inline Vector plus(const Vector& other) const {return *this + other;}
	inline Vector minus(const Vector& other) const {return *this - other;}
	inline Vector times(double other) const {return *this * other;}
};

std::ostream& operator<<(std::ostream& os, const Vector& self);

#endif        //  #ifndef GUARD_VECTOR_H
