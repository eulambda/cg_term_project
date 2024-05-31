#include "prelude.hpp"



double get_ratio(size_t from, size_t until, double ticks) {
	if (ticks < from) return 0;
	if (until < ticks) return 1;
	if (until == from) return 1;
	return (double)(ticks - from) / (double)(until - from);
}
double Life::ratio(double ticks) {
	return get_ratio(from, until, ticks);
}

double FrozenState::ratio(double ticks) {
	return get_ratio(from, until, ticks);
}

int Facing::sign_x() const {
	return (inner == FacingValue::pos_x) ? 1 : -1;
}

double BreathCharged::ratio() const {
	return val/(double)max;
}
