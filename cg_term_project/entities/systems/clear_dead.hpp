#pragma once
#include "../prelude.hpp"

void clear_dead(
	ecs::EntitiesWith<Obstacle, Compound, Body, Health> obstacles,
	ecs::EntitiesWith<Grass, Body, Health> grass_entities,
	ecs::EntitiesWith<Butterfly, Body, Health> butterflies,
	ecs::EntityApi api, Elapsed elapsed) {
	const double max_speed_x = 1.0;
	for (auto& [id, _, compound, body, health] : obstacles) {
		if (health->current > 0) continue;
		api.remove(id);
		if (compound->made_of == ParticleType::none) continue;
		auto new_body = *body;
		new_body.vx = std::clamp(health->receiving_knockback, -max_speed_x, max_speed_x);
		api.spawn()
			.with(ParticleDomain{ .type = compound->made_of })
			.with(new_body)
			.with(LocomotionFlying{ .falloff = 0.7 })
			.with(Life{ .from = elapsed.ticks, .until = elapsed.ticks + 16,.delete_on_death = true })
			;
	}

	for (auto& [id, _, body, health] : grass_entities) {
		if (health->current > 0) continue;
		api.remove(id);
		auto new_body = *body;
		new_body.vx = std::clamp(health->receiving_knockback, -max_speed_x, max_speed_x);
		api.spawn()
			.with(ParticleDomain{ .type = ParticleType::hay })
			.with(new_body)
			.with(LocomotionFlying{ .falloff = 0.7 })
			.with(Life{ .from = elapsed.ticks, .until = elapsed.ticks + 16,.delete_on_death = true })
			;
	}

	for (auto& [id, _, body, health] : butterflies) {
		if (health->current > 0) continue;
		api.remove(id);
		api.spawn()
			.with(ParticleDomain{ .type = ParticleType::hay })
			.with(*body)
			.with(LocomotionFlying{ .falloff = 0.7 })
			.with(Life{ .from = elapsed.ticks, .until = elapsed.ticks + 16,.delete_on_death = true })
			;
	}
}