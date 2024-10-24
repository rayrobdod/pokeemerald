#include "Curve.h"

#include <iostream>
#include "AlgebraicFunc.h"

// C(t) = p1 + (p2 - p1)*t

Curve::Line::Line(Vector p1, Vector p2): p1(p1), p2(p2) {}
Curve::Line::Line(double x1, double y1, double x2, double y2): p1(x1, y1), p2(x2, y2) {}

Vector Curve::Line::apply(double t) const
{
	Vector a = this->p1;
	Vector b = this->p2 - this->p1;
	return (b * t) + a;
}

std::optional<double> Curve::Line::closestT(Vector target) const
{
	// D^2 = (x1 - x2)^2 + (y1 - y2)^2
	// D^2 = ((p1.x - target.x) + (p2.x - p1.x) * t)^2 + ((p1.y - target.y) + (p2.y - p1.y) * t)^2
	// min is dD^2/dt == 0

	Vector a = this->p1 - target;
	Vector b = this->p2 - this->p1;

	return AlgebraicFunc({a.x, b.x}).square()
		.plus(AlgebraicFunc({a.y, b.y}).square())
		.derivative()
		.trySolveForZero(0, 1, 0.01);
}

std::optional<double> Curve::Line::distanceSq(Vector target) const
{
	std::optional<double> closest_tOpt = this->closestT(target);

	if (! closest_tOpt) {return std::nullopt;}

	double closest_t = closest_tOpt.value();

	return std::make_optional(this->apply(closest_t).distanceSq(target));
}




// Cubic curve equation is:
// C(t) = ((1-t)³)p1 + 3((1-t)²)(t)ctrl1 + 3(1-t)(t²)ctrl2 + (t³)p2
// C(t) = p1 - 3t * p1 + 3t² * p1 - t³ * p1
// 			+ 3t * ctrl1 - 6t² * ctrl1 + 3t³ * ctrl1
//			+ 3t² * ctrl2 - 3t³ * ctrl2 + t³ * p2
// C(t) = p1 + (-3 * p1 + 3 * ctrl1)t + (3 * p1 - 6 * ctrl1 + 3 * ctrl2)t² + (- p1 + 3 * ctrl1 - 3 * ctrl2 + p2) t³

// distance square is:
// D² = (C(t).x - x₀)² + (C(t).y - y₀)²

// min distance is solve for derivative of distance squared == 0


Curve::Cubic::Cubic(Vector p1, Vector ctrl1, Vector ctrl2, Vector p2) :
	p1(p1),
	ctrl1(ctrl1),
	ctrl2(ctrl2),
	p2(p2)
{}

Curve::Cubic::Cubic(double x1, double y1, double ctrlx1, double ctrly1, double ctrlx2, double ctrly2, double x2, double y2) :
	p1(x1, y1),
	ctrl1(ctrlx1, ctrly1),
	ctrl2(ctrlx2, ctrly2),
	p2(x2, y2)
{}

Vector Curve::Cubic::apply(double t) const
{
	Vector a = this->p1;
	Vector b = this->p1.times(-3).plus(this->ctrl1.times(3));
	Vector c = this->p1.times(3).minus(this->ctrl1.times(6)).plus(this->ctrl2.times(3));
	Vector d = this->p1.times(-1).plus(this->ctrl1.times(3)).minus(this->ctrl2.times(3)).plus(this->p2);

	return d.times(t).plus(c).times(t).plus(b).times(t).plus(a);
}

std::optional<double> Curve::Cubic::closestT(Vector target) const
{
	Vector a = this->p1.minus(target);
	Vector b = this->p1.times(-3).plus(this->ctrl1.times(3));
	Vector c = this->p1.times(3).minus(this->ctrl1.times(6)).plus(this->ctrl2.times(3));
	Vector d = this->p1.times(-1).plus(this->ctrl1.times(3)).minus(this->ctrl2.times(3)).plus(this->p2);

	return AlgebraicFunc({a.x, b.x, c.x, d.x}).square()
			.plus(AlgebraicFunc({a.y, b.y, c.y, d.y}).square())
			.derivative()
			.trySolveForZero(0, 1, 0.01);
}

std::optional<double> Curve::Cubic::distanceSq(Vector target) const
{
	std::optional<double> closest_tOpt = this->closestT(target);

	if (! closest_tOpt) {return std::nullopt;}

	double closest_t = closest_tOpt.value();

	return std::make_optional(this->apply(closest_t).distanceSq(target));
}
