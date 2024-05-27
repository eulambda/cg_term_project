#include "prelude.hpp"

bool Body::is_colliding_x(const Body& body) const {
	auto vx_rel = body.vx - vx;
	// check intersection
	if (
		!(body.x0() < x1() &&
			x0() < body.x1()) &&
		body.x0() + vx_rel <= x1() &&
		x0() <= body.x1() + vx_rel
		) {
		return true;
	}
	// check penetration
	auto is_x_penetrated =
		(x1() <= body.x0() && body.x1() + vx_rel <= x0()) ||
		(body.x1() <= x0() && x1() <= body.x0() + vx_rel);
	if (is_x_penetrated) {
		return true;
	}
	return false;
}
bool Body::is_colliding_y(const Body& body) const {
	auto vy_rel = body.vy - vy;
	// check intersection
	if (
		!(body.y0() < y1() &&
			y0() < body.y1()) &&
		body.y0() + vy_rel <= y1() &&
		y0() <= body.y1() + vy_rel
		) {
		return true;
	}
	// check penetration
	auto is_y_penetrated =
		(y1() <= body.y0() && body.y1() + vy_rel <= y0()) ||
		(body.y1() <= y0() && y1() <= body.y0() + vy_rel);
	if (is_y_penetrated) {
		return true;
	}
	return false;
}
bool Body::is_colliding(const Body& body) const {
	auto vx_rel = body.vx - vx;
	auto vy_rel = body.vy - vy;
	// check intersection
	if (
		body.x0() + vx_rel <= x1() &&
		body.y0() + vy_rel <= y1() &&
		x0() <= body.x1() + vx_rel &&
		y0() <= body.y1() + vy_rel
		) {
		return true;
	}
	// check penetration
	auto is_x_penetrated =
		(x1() <= body.x0() && body.x1() + vx_rel <= x0()) ||
		(body.x1() <= x0() && x1() <= body.x0() + vx_rel);
	auto is_y_penetrated =
		(y1() <= body.y0() && body.y1() + vy_rel <= y0()) ||
		(body.y1() <= y0() && y1() <= body.y0() + vy_rel);
	if (is_x_penetrated && is_y_penetrated) {
		return true;
	}
	return false;
}

double Body::x0() const {
	return x - w / 2;
}
double Body::x1() const {
	return x + w / 2;
}
double Body::y0() const {
	return y - h / 2;
}
double Body::y1() const {
	return y + h / 2;
}
double Body::x_distance_to(const Body& body) const {
	if (x < body.x) return body.x0() - x1();
	return body.x1() - x0();
}
double Body::y_distance_to(const Body& body) const {
	if (y < body.y) return body.y0() - y1();
	return body.y1() - y0();
}
bool Body::is_touching_x(const Body& body) const {
	return x0() <= body.x1() && body.x0() <= x1();
}
bool Body::is_touching_y(const Body& body) const {
	return y0() <= body.y1() && body.y0() <= y1();
}