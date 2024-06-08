#pragma once
#include "../prelude.hpp"

void constrain_velocity(ecs::EntitiesWith<Floor, Body> floors, ecs::EntitiesWithWritable<Body, LocomotionWalking> characters, ecs::EntitiesWithWritable<Body, LocomotionStationery> stationery_bodies) {
	const double speed_lower_cut = 0.01;
	const double wall_stiffness = 1.0;
	const double floor_stiffness = 1.0;
	const double max_speed_x = 1.0;
	const double max_speed_y = 1.5;
	for (auto& [character_id, character, locomotion] : characters) {
		if (std::abs(character->vx) < speed_lower_cut) character->vx = 0;
		if (std::abs(character->vy) < speed_lower_cut) character->vy = 0;
		character->vx = std::clamp(character->vx, -max_speed_x, max_speed_x);
		character->vy = std::clamp(character->vy, -max_speed_y, max_speed_y);

		locomotion->is_touching_floor = false;
		locomotion->is_touching_wall = false;
		for (auto& [id, _, floor_0] : floors) {
			if (id == character_id) continue;
			Body floor = *floor_0;
			floor.vx = 0;
			floor.vy = 0;
			if (!floor.is_colliding(*character)) continue;

			if (floor.is_colliding_y(*character) && character->x_distance_to(floor) != 0) {
				character->vy = character->y_distance_to(floor) / floor_stiffness;
				if (std::abs(character->vy) < speed_lower_cut) {
					character->vy = 0;
					character->y += character->y_distance_to(floor);
				}
				if (character->y0() >= floor.y1()) {
					locomotion->is_touching_floor = true;
				}
			}
			else {
				character->vx = character->x_distance_to(floor) / wall_stiffness;
				if (std::abs(character->vx) < speed_lower_cut) {
					character->vx = 0;
					character->x += character->x_distance_to(floor);
					locomotion->is_touching_wall = true;
				}
			}
		}
	}
	for (auto& [_1, body, _2] : stationery_bodies) {
		body->vx = 0;
		body->vy = 0;
	}
}