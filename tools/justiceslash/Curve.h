#ifndef GUARD_CURVE_H
#define GUARD_CURVE_H

#include <ostream>
#include <optional>
#include "Vector.h"

namespace Curve {

class Curve {
public:
	virtual Vector apply(double t) const = 0;
	virtual std::optional<double> closestT(Vector target) const = 0;
	virtual std::optional<double> distanceSq(Vector target) const = 0;
};

class Line : Curve {
private:
	Vector p1;
	Vector p2;

public:
	Line(Vector p1, Vector p2);
	Line(double x1, double y1, double x2, double y2);

	Vector apply(double t) const override;
	std::optional<double> closestT(Vector target) const override;
	std::optional<double> distanceSq(Vector target) const override;
};

class Cubic : Curve {
private:
	Vector p1;
	Vector ctrl1;
	Vector ctrl2;
	Vector p2;

public:
	Cubic(Vector p1, Vector ctrl1, Vector ctrl2, Vector p2);
	Cubic(double x1, double y1, double ctrlx1, double ctrly1, double ctrlx2, double ctrly2, double x2, double y2);

	Vector apply(double t) const override;
	std::optional<double> closestT(Vector target) const override;
	std::optional<double> distanceSq(Vector target) const override;
};

}

#endif        //  #ifndef GUARD_CURVE_H
