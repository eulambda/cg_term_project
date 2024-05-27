#pragma once
#include <functional>

struct Curve {
private:
	double x{ 0 };
	struct Var {
		Curve* parent;
		inline double operator=(double rhs);
		inline double operator+=(double rhs);
		inline double operator-=(double rhs);
	};
public:
	std::function<double(double)> y_x{ [](double t) {return t;} };
	std::function<double(double)> x_t{ [](double t) {return t;} };
	Var t{ this };
	inline double eval();
	inline double eval(double t);
	inline void set(double t);
	inline Curve(Curve&& moved) noexcept;
	inline Curve() {};
};

inline double Curve::eval() {
	return y_x(x);
}
inline double Curve::eval(double t) {
	set(t);
	return eval();
}
inline void Curve::set(double t) {
	x = x_t(t);
}
inline double Curve::Var::operator=(double rhs)
{
	parent->set(rhs);
	return rhs;
}
inline double Curve::Var::operator+=(double rhs)
{
	rhs += parent->x;
	parent->set(rhs);
	return rhs;
}
inline double Curve::Var::operator-=(double rhs)
{
	rhs = parent->x - rhs;
	parent->set(rhs);
	return rhs;
}
inline Curve::Curve(Curve&& moved) noexcept :y_x{ moved.y_x }, x_t{ moved.x_t } {}

inline std::function<double(double)> fn_from_points(
	std::vector<std::tuple<double, double>> points,
	std::function<double(double, double, double)> easing = [](double ratio, double lb, double ub) {return lb + (ub - lb) * ratio; }
) {
	std::sort(points.begin(), points.end(),
		[](std::tuple<double, double> lhs, std::tuple<double, double> rhs) {
			return std::get<0>(lhs) < std::get<0>(rhs);
		}
	);
	return [=](double x) {
		auto found = std::ranges::find_if(points, [=](const std::tuple<double, double>& point) {
			return std::get<0>(point) > x;
			});
		auto index = found - points.begin();
		if (index == 0) return std::get<1>(points[0]);
		if (index == points.size()) return std::get<1>(points[points.size() - 1]);
		auto [x0, y0] = points[index - 1];
		auto [x1, y1] = points[index];
		auto ratio = (x - x0) / (x1 - x0);
		return easing(ratio, y0, y1);
		};
}
