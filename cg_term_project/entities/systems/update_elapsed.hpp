#pragma once
#include "../prelude.hpp"

void update_elapsed(
	ecs::Writable<Elapsed> elapsed, ecs::EntityApi api,
	ecs::EntitiesWithWritable<Life> mortals,
	ecs::EntitiesWithWritable<Life, Health> mortals_with_health
) {
	elapsed->ticks++;
	for (auto& [id, life] : mortals) {
		if (elapsed->ticks < life->until) continue;
		if (life->delete_on_death) api.remove(id);
	}
	for (auto& [id, life, health] : mortals_with_health) {
		if (elapsed->ticks < life->until) continue;
		health->current = 0;
	}
}
