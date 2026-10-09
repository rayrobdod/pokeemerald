#include "AlgebraicFunc.h"

#include <cmath>
#include <iomanip>

AlgebraicFunc::AlgebraicFunc(std::vector<double> coeffs) : coeffs(coeffs)
{
}

AlgebraicFunc::AlgebraicFunc(std::initializer_list<double> coeffs) : coeffs(coeffs)
{
}

AlgebraicFunc::~AlgebraicFunc()
{
}

double AlgebraicFunc::operator[](size_t pos) const
{
	return this->coeffs[pos];
}

size_t AlgebraicFunc::size() const
{
	return this->coeffs.size();
}

double AlgebraicFunc::apply(double n) const
{
	double retval = 0;
	for (signed i = this->coeffs.size() - 1; i >= 0; i--) {
		retval *= n;
		retval += this->coeffs[i];
	}
	return retval;
}

AlgebraicFunc AlgebraicFunc::derivative(void) const
{
	std::vector<double> newcoeffs(this->coeffs.size() - 1, 0.0);
	for (unsigned i = 1; i < this->coeffs.size(); i++) {
		newcoeffs[i - 1] = i * this->coeffs[i];
	}
	return AlgebraicFunc(newcoeffs);
}

AlgebraicFunc AlgebraicFunc::operator+(const AlgebraicFunc& other) const
{
	std::vector<double> longer;
	std::vector<double> shorter;

	if (this->coeffs.size() > other.coeffs.size()) {
		longer = this->coeffs;
		shorter = other.coeffs;
	} else {
		shorter = this->coeffs;
		longer = other.coeffs;
	}

	std::vector<double> newcoeffs(longer);
	for (unsigned i = 0; i < shorter.size(); i++) {
		newcoeffs[i] += shorter[i];
	}

	return AlgebraicFunc(newcoeffs);
}

AlgebraicFunc AlgebraicFunc::square() const
{
	std::vector<double> newcoeffs(this->coeffs.size() * 2 - 1, 0.0);
	for (unsigned i = 0; i < this->coeffs.size(); i++)
	for (unsigned j = 0; j < this->coeffs.size(); j++)
	{
		newcoeffs[i + j] += this->coeffs[i] * this->coeffs[j];
	}
	return AlgebraicFunc(newcoeffs);
}

std::optional<double> AlgebraicFunc::trySolveForZero(double min, double max, double epsilon) const
{
	double minV = this->apply(min);
	double maxV = this->apply(max);

	while (maxV - minV > epsilon) {
		double mid = min + (max - min) / 2;
		double midV = this->apply(mid);

		if (std::signbit(maxV) != std::signbit(midV)) {
			minV = midV;
			min = mid;
		} else if (std::signbit(minV) != std::signbit(midV)) {
			maxV = midV;
			max = mid;
		} else {
			return std::nullopt;
		}
	}

	return std::make_optional(min + (max - min) / 2);
}

std::ostream& operator<<(std::ostream& os, const AlgebraicFunc& self)
{
	std::ios fmtstate(nullptr);
	fmtstate.copyfmt(os);
	os << std::setprecision(2);

	for (unsigned i = 0; i < self.size(); i++)
	{
		if (i != 0) {
			os << " + ";
		}
		os << self[i];
		switch (i) {
		case 0:
			break;
		case 1:
			os << " t";
			break;
		case 2:
			os << " t²";
			break;
		case 3:
			os << " t³";
			break;
		default:
			os << " t^" << i;
			break;
		}
	}
	os.copyfmt(fmtstate);
	return os;
}
