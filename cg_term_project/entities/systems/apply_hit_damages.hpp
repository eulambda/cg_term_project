#pragma once
#include "../prelude.hpp"

void apply_hit_damages(ecs::EntitiesWithWritable<Body, Health, DamageReceiver> characters, ecs::EntitiesWith<HitDamage, Body, Facing> damage_sources) {
	for (auto& [character_id, character_body, health, receiver] : characters) {
		health->current = std::clamp(health->current + health->receiving, 0, health->max);
		health->receiving = 0;
		for (auto& [_, damage, damage_body, damage_facing] : damage_sources) {
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
			if (multiplier == 0 || damage->knockback == 0 || receiver->multiplier_knockback == 0) continue;
			auto knockback = damage->knockback * receiver->multiplier_knockback * multiplier;
			character_body->vx += knockback * damage_facing->sign_x();
		}
	}
}