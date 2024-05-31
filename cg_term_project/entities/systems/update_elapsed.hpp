#pragma once
#include "../prelude.hpp"

void update_elapsed(
	ecs::Writable<Elapsed> elapsed, ecs::EntityApi api,
	ecs::EntitiesWithWritable<Life> mortals
) {
	elapsed->ticks++;
	for (auto& [id, life] : mortals) {
		if (elapsed->ticks < life->until) continue;
		if (life->delete_on_death) api.remove(id);
	}
}
