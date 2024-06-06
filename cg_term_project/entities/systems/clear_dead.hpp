#pragma once
#include "../prelude.hpp"

void clear_dead(
	ecs::EntitiesWith<Obstacle, Compound, Body, Health> obstacles,
	ecs::EntitiesWith<Grass, Body, Health> grass_entities,
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
