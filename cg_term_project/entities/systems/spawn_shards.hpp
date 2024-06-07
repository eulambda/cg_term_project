#pragma once
#include "../prelude.hpp"

void spawn_shards(ecs::EntitiesWith<ShardSpawner, Compound, Body, Health, DamageReceiver> spawners, ecs::EntityApi api, Elapsed elapsed) {
	for (auto& [id, spawner, compound, body, health, damage_receiver] : spawners) {
		if (health->receiving == 0) continue;
		if (compound->made_of == ParticleType::none) continue;
		auto rand = std::sin(id + elapsed.ticks) * 0.5 + 0.5;
		auto size = std::min(body->w, body->h) / 4 * (1.0 - rand * 0.5);
		auto new_receiver = *damage_receiver;
		new_receiver.multiplier_knockback = 1;
		if (std::sin(id + elapsed.ticks * 17) > 0.9) {
			api.spawn()
				.with(Obstacle{})
				.with(Body{ .w = size,.h = size,.x = body->x - 0.6 * size,.y = body->y1(),.vx = -0.5 * rand + body->vx,.vy = 1 + body->vy })
				.with(LocomotionWalking{})
				.with(DebugInfo{ .name = "shard" })
				.with(*compound)
				.with(Health{ .max = 10, .current = 10 })
				.with(Facing{ .inner = FacingValue::neg_x })
				.with(new_receiver)
				.with(HitDamage{ .from = id,.power = 1,.knockback = 5 })
				.with(Life{ .from = elapsed.ticks,.until = elapsed.ticks + 24 })
				;
		}
		if (std::sin(id + elapsed.ticks * 23) > 0.9) {
			api.spawn()
				.with(Obstacle{})
				.with(Body{ .w = size,.h = size,.x = body->x + 0.6 * size,.y = body->y1(),.vx = +0.5 * rand + body->vx,.vy = 1 + body->vy })
				.with(LocomotionWalking{})
				.with(DebugInfo{ .name = "shard" })
				.with(*compound)
				.with(Health{ .max = 10, .current = 10 })
				.with(Facing{ .inner = FacingValue::pos_x })
				.with(new_receiver)
				.with(HitDamage{ .from = id,.power = 1,.knockback = 5 })
				.with(Life{ .from = elapsed.ticks,.until = elapsed.ticks + 24 })
				;
		}
	}
}