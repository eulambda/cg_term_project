#pragma once
#include "../prelude.hpp"
#include "../../essentials/sign.hpp"

void run_pig_agent(
	ecs::EntitiesWithWritable<Pig, Health, Body, LocomotionWalking, Facing> pigs,
	ecs::EntitiesWith<Wolf, Health, Body, LocomotionWalking, Facing> wolves,
	ecs::EntitiesWith<Obstacle, Body> obstacles,
	ecs::Writable<Stage> stage,
	Elapsed elapsed

) {
	if (pigs.empty() || wolves.empty()) return;
	auto& [_1, pig, pig_health, pig_body, pig_locomotion, pig_facing] = *pigs.begin();
	auto& [_3, _4, wolf_health, wolf_body, wolf_locomotion, wolf_facing] = *wolves.begin();
	if (wolf_health->current == 0) return;

	if (pig->action == PigAction::dying && elapsed.ticks > pig->until) {
		stage->to_load = pig->to_load;
		return;
	}
	if (pig_health->current == 0 && pig->action != PigAction::dying) {
		pig->action = PigAction::dying;
		pig->until = elapsed.ticks + 24;
		return;
	}
	if (pig_health->current == 0) {
		return;
	}
	auto ax = 0.0, ay = 0.0;
	if (pig->action == PigAction::run && elapsed.ticks > pig->until) {
		pig->until = elapsed.ticks + 24;
		pig->action = PigAction::run;
		pig_facing->set_sign_x(-pig_facing->sign_x());
	}


	auto distance = std::abs(wolf_body->x_distance_to(*pig_body));
	for (auto& [_5, _6, body] : obstacles) {
		distance = std::min(distance, std::abs(body->x_distance_to(*pig_body)));
	}
	if (distance < 3) {
		ax = pig_facing->sign_x();
		ay = 4;
	}
	else {
		ax = pig_facing->sign_x();
	}
	if (pig_locomotion->is_touching_wall) {
		ay += 2;
	}

	ax = std::clamp(ax, -0.5, 0.5);

	if (ax > 0) pig_facing->inner = FacingValue::pos_x;
	if (ax < 0) pig_facing->inner = FacingValue::neg_x;
	pig_body->vx += ax;
	if (pig_locomotion->is_touching_floor) {
		pig_body->vy += std::clamp(ay, 0.0, 4.0);
	}

}