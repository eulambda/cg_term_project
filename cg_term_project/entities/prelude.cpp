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

void Facing::set_sign_x(int sign_x) {
	if (sign_x > 0) {
		inner = FacingValue::pos_x;
	}
	else if (sign_x < 0) {
		inner = FacingValue::neg_x;
	}

}

double RoarCharged::ratio() const {
	return val / (double)max;
}


void Stage::load(std::string asset_path) {
	queued.push(StageAction{ .type = StageActionType::load,.str = asset_path });
}

void Stage::pause(int ticks) {
	queued.push(StageAction{ .type = StageActionType::pause,.str = "",.num = ticks });
}

double Stage::transition_lerped(double t) {
	return t * transition + (1 - t) * transition_prev;
}
