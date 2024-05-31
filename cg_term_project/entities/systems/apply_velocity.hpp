#pragma once
#include "../prelude.hpp"

void apply_velocity(ecs::EntitiesWithWritable<Body, LocomotionWalking> walking_entities) {
	for (auto& [_, body, locomotion] : walking_entities) {
		if (body->vx != 0) {
			body->x += body->vx;
		}
		if (body->vy != 0) {
			body->y += body->vy;
		}
		if (body->vx != 0 && locomotion->is_touching_floor) {
			body->vx *= 0.5;
		}
		if (!locomotion->is_touching_floor) {
			body->vy -= 0.4;
		}
	}
}