#pragma once
#include "../prelude.hpp"

void apply_hit_damages(ecs::EntitiesWithWritable<Body, Health, DamageReceiver> characters, ecs::EntitiesWith<HitDamage, Body, Facing> damage_sources) {
	for (auto& [character_id, character_body, health, receiver] : characters) {
		health->current = std::clamp(health->current + health->receiving, 0, health->max);
		health->receiving = 0;
		health->receiving_knockback = 0;
		for (auto& [damage_source_id, damage, damage_body, damage_facing] : damage_sources) {
			if (damage_source_id == character_id) continue;
			if (character_id == damage->from) continue;
			if (!character_body->is_colliding(*damage_body)) continue;
			double power = damage->power;
			double multiplier = 1;
			if (damage->type == DamageType::normal) multiplier = receiver->multiplier_normal;
			if (damage->type == DamageType::fire) multiplier = receiver->multiplier_fire;
			if (damage->type == DamageType::wind) multiplier = receiver->multiplier_wind;
			if (multiplier == 0) continue;
			power *= multiplier;
			health->receiving -= (int)power;
			if (damage->knockback == 0) continue;
			auto knockback = damage->knockback * multiplier;
			health->receiving_knockback += knockback * damage_facing->sign_x();
		}
		character_body->vx += receiver->multiplier_knockback * health->receiving_knockback;
	}
}