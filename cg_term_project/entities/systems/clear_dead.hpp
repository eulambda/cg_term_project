#pragma once
#include "../prelude.hpp"

void clear_dead(
	ecs::EntitiesWith<Obstacle, Compound, Body, Health> obstacles,
	ecs::EntitiesWith<Grass, Body, Health> grass_entities,
	ecs::EntitiesWith<Butterfly, Body, Health> butterflies,
	ecs::EntityApi api, Elapsed elapsed) {
	for (auto& [id, _, compound, body, health] : obstacles) {
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
	}

	for (auto& [id, _, body, health] : grass_entities) {
		if (health->current > 0) continue;
		api.remove(id);
		api.spawn()
			.with(ParticleDomain{ .type = ParticleType::hay })
			.with(DebugInfo{ .name = "grass" })
			.with(*body)
			.with(Life{ .from = elapsed.ticks, .until = elapsed.ticks + 16,.delete_on_death = true })
			;
	}

	for (auto& [id, _, body, health] : butterflies) {
		if (health->current > 0) continue;
		api.remove(id);
		api.spawn()
			.with(ParticleDomain{ .type = ParticleType::hay })
			.with(DebugInfo{ .name = "butterfly" })
			.with(*body)
			.with(LocomotionFlying{ .falloff = 0.7 })
			.with(Life{ .from = elapsed.ticks, .until = elapsed.ticks + 16,.delete_on_death = true })
			;
	}
}