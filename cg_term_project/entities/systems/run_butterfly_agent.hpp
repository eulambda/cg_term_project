#pragma once
#include "../prelude.hpp"
#include "../../essentials/sign.hpp"

void run_butterfly_agent(
	ecs::EntitiesWithWritable<Butterfly, Health, Body> butterflies,
	Elapsed elapsed
) {
	if (butterflies.empty()) return;
	auto noise = [](size_t x) {
		return sin(256 * (int)x + 4567) + sin(512 * (int)x + 456) / 2 + sin(1024 * (int)x + 45) / 4;
		};

	for (auto& [id, butterfly, health, body] : butterflies) {
		if (elapsed.ticks % 10 != 0) {
			body->vy -= 1.0/4000;
			body->vx = std::clamp(body->vx, -0.3, 0.3);
			body->vy = std::clamp(body->vy, -0.1, 0.1);
			continue;
		}
		body->vx += (butterfly->start_x - body->x) / 4000 + noise(id + elapsed.ticks) / 10;
		body->vy += (butterfly->start_y - body->y) / 200 + (noise(id + 2 * elapsed.ticks) + 1) / 50;
		body->vx = std::clamp(body->vx, -0.3, 0.3);
		body->vy = std::clamp(body->vy, -0.1, 0.1);
	}
}