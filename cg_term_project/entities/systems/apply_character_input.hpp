#pragma once
#include "../prelude.hpp"


void apply_character_input(
	ecs::EntitiesWithWritable<Wolf, Body, LocomotionWalking, Facing, FrozenState, RoarCharged, Health> wolves,
	ecs::Writable<CharacterInput> input, ecs::EntityApi api, Elapsed elapsed
) {
	if (wolves.empty()) return;
	auto& [wolf_id, _, wolf_body, locomotion, facing, frozen_state, roar_charged, health] = *wolves.begin();

	if (health->current == 0) {
		return;
	}
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
	if (is_touching_floor) ax = dx * 0.3;
	if (is_touching_floor && input->jump) ay = 1.5;
	if (!is_touching_floor && input->jumping) ay = 0.2;
	if (input->emit_flame) {
		input->emit_flame = false;
		double w = 4;
		double h = 2;
		double x_offset = (2 + w / 2) * facing->sign_x();
		frozen_state->from = elapsed.ticks;
		frozen_state->until = elapsed.ticks + 6;
		api.spawn()
			.with(Flame{})
			.with(DebugInfo{ .name = "flame" })
			.with(Body{ .w = w,.h = h, .x = wolf_body->x + x_offset,.y = wolf_body->y + 0.2 })
			.with(LocomotionFlying{})
			.with(HitDamage{ .from = wolf_id,.power = 1,.knockback = 1, .type = DamageType::fire })
			.with(Life{ .from = elapsed.ticks, .until = elapsed.ticks + 10, .delete_on_death = true })
			.with(Facing{ facing->inner })
			;
		ax = -0.1 * facing->sign_x();
	}
	if (input->charge_roar) {
		roar_charged->val = std::min(roar_charged->val + 1, roar_charged->max);
		roar_charged->is_charging = true;
		ax *= 0.2;
		ay *= 0;
	}
	if (!input->charge_roar && roar_charged->is_charging) {
		double ratio = roar_charged->ratio();
		double power = ratio == 1 ? 1 : 0;
		double w = ratio == 1 ? 6 : 3;
		double h = ratio == 1 ? 4 : 2;
		double x_offset = (2 + w / 2) * facing->sign_x();
		frozen_state->from = elapsed.ticks;
		frozen_state->until = elapsed.ticks + 15;
		api.spawn()
			.with(Roar{})
			.with(DebugInfo{ .name = "roar" })
			.with(Body{ .w = w,.h = h, .x = wolf_body->x + x_offset,.y = wolf_body->y + 0.2 })
			.with(LocomotionFlying{})
			.with(HitDamage{ .from = wolf_id,.power = power,.knockback = 1, .type = roar_charged->type })
			.with(Life{ .from = elapsed.ticks, .until = elapsed.ticks + 15, .delete_on_death = true })
			.with(Facing{ facing->inner })
			;
		ax = -0.5 * ratio * facing->sign_x();
		roar_charged->val = 0;
		roar_charged->is_charging = false;
	}

	wolf_body->vx += ax;
	wolf_body->vy += ay;
	input->jump = false;
}