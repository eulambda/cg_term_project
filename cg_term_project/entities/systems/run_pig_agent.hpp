#pragma once
#include "../prelude.hpp"

void run_pig_agent(ecs::Writable<Pig> pig, Wolf wolf, ecs::EntitiesWithWritable<Health, Body, LocomotionWalking, Facing> characters) {
	auto pig_fetched = characters.get_by_id(pig->entity_id);
	auto wolf_fetched = characters.get_by_id(wolf.entity_id);
	if (!pig_fetched || !wolf_fetched) return;
	auto& [_1, pig_health, pig_body, pig_locomotion, pig_facing] = *pig_fetched;
	auto& [_2, wolf_health, wolf_body, wolf_locomotion, wolf_facing] = *wolf_fetched;
	if (pig_health->current == 0 || wolf_health->current == 0) return;
	auto ax = 0.0;
	if (wolf_body->x > pig_body->x) ax = wolf_body->x - 5 - pig_body->x;
	if (wolf_body->x < pig_body->x) ax = wolf_body->x + 5 - pig_body->x;
	if (std::abs(ax) < 5) return;
	ax = std::clamp(ax, -0.2, 0.2);
	pig_body->vx += ax;
	if (ax > 0) pig_facing->inner = FacingValue::pos_x;
	if (ax < 0) pig_facing->inner = FacingValue::neg_x;
}