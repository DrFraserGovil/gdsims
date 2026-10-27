#include <cmath>
#include "BoundaryStrategy.h"

/**
 * Calculates the shortest periodic distance between two points.
 * @param[in] p1 first point
 * @param[in] p2 second point
 * @return The shortest distance between the points. 
 */
double ToroidalBoundaryStrategy::distance(const Point &p1, const Point &p2)
{
    double x_dist = 0;
	double y_dist = 0;

	if (std::abs(p1.x - p2.x) > side_x - std::abs(p1.x - p2.x)) {
		x_dist = side_x - std::abs(p1.x - p2.x);
	} 
	else {
		x_dist = std::abs(p1.x - p2.x);
	}

	if (std::abs(p1.y - p2.y) > side_y - std::abs(p1.y - p2.y)) {
		y_dist = side_y - std::abs(p1.y - p2.y);
	}
	else {
		y_dist = std::abs(p1.y - p2.y);
	}

	return std::sqrt((x_dist * x_dist) + (y_dist * y_dist));
}

/**
 * Returns the relative position of the second point to the first.
 * @param[in] loc1 point 1
 * @param[in] loc2 point 2
 * @return The relative coordinates of the second point.
 */
Point ToroidalBoundaryStrategy::relative_pos(const Point &p1, const Point &p2) 
{
	Point t2=p2;
	if (2*(p2.x - p1.x) > side_x) {
		t2.x = p2.x - side_x;
	}
	if (2*(p1.x - p2.x) > side_x) {
		t2.x = p2.x + side_x;
	}
	if (2*(p2.y - p1.y) > side_y) {
		t2.y = p2.y - side_y;
	}
	if (2*(p1.y - p2.y) > side_y) {
		t2.y = p2.y + side_y;
	}
	return t2;
}

/**
 * Calculates the distance between two points. 
 * @param[in] p1 first point
 * @param[in] p2 second point
 * @return The distance between the points.
 */
double EdgeBoundaryStrategy::distance(const Point &p1, const Point &p2)
{
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

/**
 * Returns the relative position of the second point to the first.
 * @param[in] loc1 point 1
 * @param[in] loc2 point 2
 * @return The relative coordinates of the second point.
 */
Point EdgeBoundaryStrategy::relative_pos(const Point &p1, const Point &p2) 
{
	return p2;
}
