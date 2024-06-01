#pragma once
#include "../prelude.hpp"

void clear_obstacles(ecs::EntitiesWith<Obstacle, Body, Health> obstacles, ecs::EntityApi api, Elapsed elapsed) {
	for (auto& [id, obstacle, body, health] : obstacles) {
		if (health->current > 0) continue;
		api.remove(id);
		if (obstacle->made_of == ParticleType::none) continue;
		api.spawn()
			.with(ParticleDomain{ .type = obstacle->made_of })
			.with(DebugInfo{ .name = "particle domain" })
			.with(*body)
			.with(LocomotionFlying{ .falloff = 0.7 })
			.with(Life{ .from = elapsed.ticks, .until = elapsed.ticks + 16,.delete_on_death = true })
			;
	}
}