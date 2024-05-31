#include "prelude.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <sstream>
#include "../essentials/sign.hpp"

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
void apply_character_input(
	ecs::EntitiesWithWritable<Body, LocomotionWalking, Facing, FrozenState> characters,
	Wolf wolf, ecs::Writable<CharacterInput> input, ecs::EntityApi api, Elapsed elapsed
) {
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
		double w = 4;
		double x_offset = (2 + w / 2) * facing->sign_x();
		frozen_state->from = elapsed.ticks;
		frozen_state->until = elapsed.ticks + 6;
		api.spawn()
			.with(Body{ .w = w,.h = 1, .x = wolf_character->x + x_offset,.y = wolf_character->y + 0.2 })
			.with(LocomotionFlying{})
			.with(HitDamage{ .from = wolf.entity_id,.power = 1,.knockback = 1, .type = DamageType::fire })
			.with(Life{ .from = elapsed.ticks, .until = elapsed.ticks + 10, .delete_on_death = true })
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
void apply_hit_damages(ecs::EntitiesWithWritable<Body, Health, DamageReceiver> characters, ecs::EntitiesWith<HitDamage, Body,Facing> damage_sources) {
	for (auto& [character_id, character_body, health, receiver] : characters) {
		health->current = std::clamp(health->current + health->receiving, 0, health->max);
		health->receiving = 0;
		for (auto& [_, damage, damage_body,damage_facing] : damage_sources) {
			if (character_id == damage->from) continue;
			if (!character_body->is_colliding(*damage_body)) continue;
			double power = damage->power * receiver->multiplier;
			if (damage->type == DamageType::fire) power *= receiver->multiplier_fire;
			if (damage->type == DamageType::wind) power *= receiver->multiplier_wind;
			health->receiving -= (int)power;
			if (damage->knockback == 0 || receiver->multiplier_knockback == 0) continue;
			auto knockback = damage->knockback * receiver->multiplier_knockback;
			character_body->vx += knockback * damage_facing->sign_x();
		}
	}
}
std::tuple<double, double> solve_elastic_collision(double m0, double m1, double v0, double v1) {
	double u0 = ((m0 - m1) * v0 + 2 * m1 * v1) / (m0 + m1);
	double u1 = ((m1 - m0) * v1 + 2 * m0 * v0) / (m0 + m1);
	return std::make_tuple(u0, u1);
}
void apply_elastic_collision(ecs::EntitiesWithWritable<Body, Mass, LocomotionWalking> walking_characters) {
	for (auto& [id0, body0, mass0, locomotion0] : walking_characters) {
		for (auto& [id1, body1, mass1, locomition1] : walking_characters) {
			if (id0 >= id1) continue;
			if (!body0->is_colliding(*body1)) return;
			if (body0->is_colliding_y(*body1) && body1->x_distance_to(*body0) != 0) {
				auto [uy0, uy1] = solve_elastic_collision(mass0->val, mass1->val, body0->vy, body1->vy);
				body0->vy = uy0;
				body1->vy = uy1;
				if (body0->y > body1->y) body0->vy = 4;
				else body1->vy = 4;
			}
			if (body0->is_colliding_x(*body1) && body1->y_distance_to(*body0) != 0) {
				auto [ux0, ux1] = solve_elastic_collision(mass0->val, mass1->val, body0->vx, body1->vx);
				body0->vx = ux0;
				body1->vx = ux1;

			}
		}
	}
}
void constrain_velocity(ecs::EntitiesWith<Floor, Body> floors, ecs::EntitiesWithWritable<Body, LocomotionWalking> characters) {
	const double speed_lower_cut = 0.01;
	const double wall_stiffness = 1.0;
	const double floor_stiffness = 1.0;
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
		.followed_by(apply_hit_damages)
		.followed_by(apply_elastic_collision)
		.followed_by(constrain_velocity)
		.followed_by(print_console)
		;

	return system_forest;
}