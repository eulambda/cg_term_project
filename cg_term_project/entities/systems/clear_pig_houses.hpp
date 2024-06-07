#pragma once
#include "../prelude.hpp"

void clear_pig_houses(ecs::EntitiesWith<PigHouse, ShardSpawner, Compound, Body, Health> pig_houses, ecs::EntityApi api, Elapsed elapsed) {
	for (auto& [id, pig_house, shard_spawner, compound, body, health] : pig_houses) {
		if (health->current > 0) continue;
		api.remove(id);
		if (compound->made_of == ParticleType::none) continue;
		api.spawn()
			.with(ParticleDomain{ .type = compound->made_of })
			.with(DebugInfo{ .name = "particle domain" })
			.with(*body)
			.with(LocomotionFlying{ .falloff = 0.7 })
			.with(Life{ .from = elapsed.ticks, .until = elapsed.ticks + 16,.delete_on_death = true })
			;
		api.spawn()
			.with(Pig{ .action = PigAction::run, .until = 0, .to_load = pig_house->to_load })
			.with(DebugInfo{ .name = "pig" })
			.with(LocomotionWalking{})
			.with(*shard_spawner)
			.with(*compound)
			.with(Body{ .w = 2,.h = 2, .x = body->x,.y = body->y })
			.with(Facing{ .inner = FacingValue::pos_x })
			.with(Mass{ 2 })
			.with(Health{})
			.with(DamageReceiver{ .multiplier_fire = 20 })
			;
	}
}