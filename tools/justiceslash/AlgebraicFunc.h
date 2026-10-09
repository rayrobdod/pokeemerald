#ifndef GUARD_ALGEBRAICFUNC_H
#define GUARD_ALGEBRAICFUNC_H

#include <cstdlib>
#include <vector>
#include <optional>
#include <ostream>

class AlgebraicFunc
{
private:
	std::vector<double> coeffs;

public:
	AlgebraicFunc(std::vector<double>);
	AlgebraicFunc(std::initializer_list<double>);
	~AlgebraicFunc();

	double operator[](size_t pos) const;
	size_t size() const;

	double apply(double) const;

	AlgebraicFunc derivative(void) const;
	AlgebraicFunc operator+(const AlgebraicFunc&) const;
	inline AlgebraicFunc plus(const AlgebraicFunc& other) const {return *this + other;}
	AlgebraicFunc square() const;
	std::optional<double> trySolveForZero(double min, double max, double epsilon) const;
};

std::ostream& operator<<(std::ostream& os, const AlgebraicFunc& self);

#endif        //  #ifndef GUARD_ALGEBRAICFUNC_H
