#include "animation.hpp"

double Animation::eval() {
	return y_x(x);
}
void Animation::set(double t) {
	x = x_t(t);
}
Animation::Animation(std::function<double(double)> y_x, std::function<double(double)> x_t) :y_x{ y_x }, x_t{ x_t }
{
}
double Animation::Var::operator=(double rhs)
{
	parent->set(rhs);
	return rhs;
}
double Animation::Var::operator+=(double rhs)
{
	rhs += parent->x;
	parent->set(rhs);
	return rhs;
}
//inline double Animation::Var::operator+=(double rhs)
//{
//	rhs += parent->x;
//	parent->set(rhs);
//	return rhs;
//}