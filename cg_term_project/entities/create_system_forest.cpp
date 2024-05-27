#include "prelude.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <sstream>

void run_pig_agent(ecs::Writable<Pig> pig, Wolf wolf) {
	if (pig->dead) return;
	if (wolf.dead) return;
}
void apply_character_input(
	ecs::EntitiesWithWritable<Body, LocomotionWalking, Facing, FrozenState> characters,
	Wolf wolf, ecs::Writable<CharacterInput> input, ecs::EntityApi api, Elapsed elapsed
) {
	if (wolf.dead) return;
	auto wolf_fetched = characters.get_by_id(wolf.entity_id);
	if (!wolf_fetched) return;
	auto& [_, wolf_character, locomotion, facing, frozen_state] = *wolf_fetched;

	if (frozen_state->ratio((double)elapsed.ticks) < 1.0) {
		input->jump = false;
		return;
	}

	double dx = 0;
	if (input->left) {
		dx -= 1;
		facing->inner = FacingValue::neg_x;
	}
	if (input->right) {
		dx += 1;
		facing->inner = FacingValue::pos_x;
	}

	bool is_touching_floor = locomotion->is_touching_floor;
	double ax = 0, ay = 0;
	if (!is_touching_floor) ax = dx * 0.2;
	if (is_touching_floor) ax = dx * 0.7;
	if (is_touching_floor && input->jump) ay = 2;
	if (!is_touching_floor && input->jumping) ay = 0.2;
	if (input->emit_flame) {
		input->emit_flame = false;
		double mouth_x = facing->inner == FacingValue::pos_x ? 2 : -2;
		frozen_state->from = elapsed.ticks;
		frozen_state->until = elapsed.ticks + 6;
		api.spawn()
			.with(Body{ .w = 4,.h = 1, .x = wolf_character->x + mouth_x,.y = wolf_character->y + .5,.vx = 0.1 })
			.with(LocomotionFlying{})
			.with(HitDamage{ .from = wolf.entity_id,.power = 1 })
			.with(Life{ .from = elapsed.ticks, .until = elapsed.ticks + 24, .delete_on_death = true })
			.with(Facing{ facing->inner })
			;
	}

	wolf_character->vx += ax;
	wolf_character->vy += ay;
	input->jump = false;
}
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
void update_elapsed(
	ecs::Writable<Elapsed> elapsed, ecs::EntityApi api,
	ecs::EntitiesWithWritable<Life> mortals
) {
	elapsed->ticks++;
	for (auto& [id, life] : mortals) {
		if (elapsed->ticks < life->until) continue;
		if (life->delete_on_death) api.remove(id);
	}
}
void constrain_velocity(ecs::EntitiesWith<Floor, Body> floors, ecs::EntitiesWithWritable<Body, LocomotionWalking> characters) {
	const double speed_lower_cut = 0.01;
	const double wall_stiffness = 1.1;
	const double floor_stiffness = 1.1;
	const double max_speed_x = 1.0;
	const double max_speed_y = 1.5;
	for (auto& [_, character, locomotion] : characters) {
		if (std::abs(character->vx) < speed_lower_cut) character->vx = 0;
		if (std::abs(character->vy) < speed_lower_cut) character->vy = 0;
		character->vx = std::clamp(character->vx, -max_speed_x, max_speed_x);
		character->vy = std::clamp(character->vy, -max_speed_y, max_speed_y);

		locomotion->is_touching_floor = false;
		locomotion->is_touching_wall = false;
		for (auto& [id, _, floor] : floors) {
			if (!floor->is_colliding(*character)) continue;

			if (floor->is_colliding_y(*character) && character->x_distance_to(*floor) != 0) {
				character->vy = character->y_distance_to(*floor) / floor_stiffness;
				if (std::abs(character->vy) < speed_lower_cut) {
					character->vy = 0;
					character->y += character->y_distance_to(*floor);
				}
				if (character->y0() >= floor->y1()) {
					locomotion->is_touching_floor = true;
				}
			}
			if (floor->is_colliding_x(*character) && character->y_distance_to(*floor) != 0) {
				character->vx = character->x_distance_to(*floor) / wall_stiffness;
				if (std::abs(character->vx) < speed_lower_cut) {
					character->vx = 0;
					character->x += character->x_distance_to(*floor);
					locomotion->is_touching_wall = true;
				}
			}
		}
	}
}
void print_console(ecs::EntitiesWith<Body, LocomotionWalking> characters) {
	for (auto& [_, character, locomotion] : characters) {
		//std::cout << character->vy << ", " << character->y << ", " << locomotion->is_flying << std::endl;
	}
}

ecs::SystemForest create_system_forest(ecs::World* world) {
	auto system_forest = ecs::SystemForest(world);

	system_forest.add(update_elapsed)
		.followed_by(apply_velocity)
		.followed_by(run_pig_agent)
		.followed_by(apply_character_input)
		.followed_by(constrain_velocity)
		.followed_by(print_console)
		;

	return system_forest;
}