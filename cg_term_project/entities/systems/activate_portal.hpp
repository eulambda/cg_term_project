#pragma once
#include "../prelude.hpp"

void activate_portal(ecs::Writable<Stage> stage, ecs::EntitiesWith<Wolf, Body> wolves, ecs::EntitiesWith<Portal, Body> portals) {
	if (stage->is_paused) return;

	for (auto& [_1, _2, wolf_body] : wolves) {
		for (auto& [_3, portal, portal_body] : portals) {
			if (!wolf_body->is_colliding(*portal_body)) return;
			if (portal->pause) stage->pause(portal->pause);
			stage->load(portal->to_load);
		}
	}
}