#pragma once
#include "../prelude.hpp"
#include "../../essentials/sign.hpp"

void run_pig_agent(
	ecs::EntitiesWithWritable<Pig, Health, Body, LocomotionWalking, Facing> pigs,
	ecs::EntitiesWith<Wolf, Health, Body, LocomotionWalking, Facing> wolves

) {
	auto pig_fetched = pigs.begin();
	auto wolf_fetched = wolves.begin();
	if (pig_fetched == pigs.end() || wolf_fetched == wolves.end()) return;
	auto& [_1, _2, pig_health, pig_body, pig_locomotion, pig_facing] = *pig_fetched;
	auto& [_3, _4, wolf_health, wolf_body, wolf_locomotion, wolf_facing] = *wolf_fetched;
	if (pig_health->current == 0 || wolf_health->current == 0) return;
	auto ax = 0.0, ay = 0.0;


	auto distance = wolf_body->x_distance_to(*pig_body);
	if (std::abs(distance) < 3) {
		ax = -distance - sign(distance) * 5;
		ay = 3;
	}
	else if (std::abs(distance) < 5) {
		ax = distance + sign(distance) * 5;
	}
	if (pig_locomotion->is_touching_wall) {
		ay += 1;
	}

	ax = std::clamp(ax, -0.2, 0.2);

	if (ax > 0) pig_facing->inner = FacingValue::pos_x;
	if (ax < 0) pig_facing->inner = FacingValue::neg_x;
	pig_body->vx += ax;
	if (pig_locomotion->is_touching_floor) {
		pig_body->vy += std::clamp(ay, 0.0, 3.0);
	}

}