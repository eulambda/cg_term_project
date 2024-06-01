#pragma once
#include "../prelude.hpp"
#include "../../essentials/json_parser.hpp"
#include <iostream>
void load_stage(
	ecs::EntitiesWithWritable<Wolf, Body> wolves, ecs::Writable<Stage> stage, ecs::EntityApi api
	, ecs::EntitiesWith<Floor> prev_floors
	, ecs::EntitiesWith<StageText> prev_stage_texts
	, ecs::EntitiesWith<Portal> prev_portals
) {
	if (!stage->to_load.length()) return;
	auto json = json::Value::parse(stage->to_load);
	stage->to_load.clear();
	stage->is_paused = false;

	if (!json.is_valid()) {
		std::cout << "Error: failed to load stage configuration." << std::endl;
		return;
	}
	auto obj = json.as_obj();
	if (!obj) return;
	auto& start_v = (*obj)["start"];
	auto& floors_v = (*obj)["floors"];
	auto& stage_texts_v = (*obj)["stage_texts"];
	auto& obstacles_v = (*obj)["obstacles"];
	auto& portals_v = (*obj)["portals"];
	auto start = start_v.as_obj();
	auto floors = floors_v.as_arr();
	auto stage_texts = stage_texts_v.as_arr();
	auto obstacles = obstacles_v.as_arr();
	auto portals = portals_v.as_arr();
	if (!start || !floors || !stage_texts || !obstacles || !portals) return;

	auto start_x = (*start)["x"].as_num();
	auto start_y = (*start)["y"].as_num();
	if (!start_x || !start_y) return;

	auto wolf_fetched = wolves.begin();
	if (wolf_fetched == wolves.end()) return;

	auto& [_1, _2, wolf_body] = *wolf_fetched;
	wolf_body->vx = 0;
	wolf_body->vy = 0;
	wolf_body->x = *start_x;
	wolf_body->y = *start_y;

	for (auto& [id, _] : prev_floors) api.remove(id);
	for (auto& [id, _] : prev_stage_texts) api.remove(id);
	for (auto& [id, _] : prev_portals) api.remove(id);

	for (auto& floor_v : *floors) {
		auto floor = floor_v.as_obj();
		if (!floor) return;
		auto w = (*floor)["w"].as_num();
		auto h = (*floor)["h"].as_num();
		auto x = (*floor)["x"].as_num();
		auto y = (*floor)["y"].as_num();
		if (!w || !h || !x || !y) return;
		api.spawn().with(Floor{}).with(Body{ .w = *w,.h = *h,.x = *x,.y = *y });
	}
	for (auto& stage_text_v : *stage_texts) {
		auto stage_text = stage_text_v.as_obj();
		if (!stage_text) return;
		auto x = (*stage_text)["x"].as_num();
		auto y = (*stage_text)["y"].as_num();
		auto contents = (*stage_text)["contents"].as_str();
		if (!x || !y || !contents) return;
		api.spawn().with(StageText{ .contents = *contents, .x = *x,.y = *y });
	}
	for (auto& obstacle_v : *obstacles) {
		auto obstacle = obstacle_v.as_obj();
		if (!obstacle) return;
		auto w = (*obstacle)["w"].as_num();
		auto h = (*obstacle)["h"].as_num();
		auto x = (*obstacle)["x"].as_num();
		auto y = (*obstacle)["y"].as_num();
		auto health = (int)(*obstacle)["health"].num_or(100);
		auto multiplier_normal = (int)(*obstacle)["multiplier_normal"].num_or(0);
		auto multiplier_fire = (int)(*obstacle)["multiplier_fire"].num_or(0);
		auto multiplier_wind = (int)(*obstacle)["multiplier_wind"].num_or(0);
		auto multiplier_knockback = (int)(*obstacle)["multiplier_knockback"].num_or(0);
		auto made_of_s = (*obstacle)["made_of"].str_or("none");
		if (!w || !h || !x || !y) return;
		auto made_of = ParticleType::none;
		if (made_of_s == "hay") made_of = ParticleType::hay;
		else if (made_of_s == "wood") made_of = ParticleType::wood;

		api.spawn()
			.with(Floor{})
			.with(Obstacle{ .made_of = made_of })
			.with(Body{ .w = *w,.h = *h,.x = *x,.y = *y })
			.with(LocomotionWalking{})
			.with(Health{ .max = health,.current = health })
			.with(DamageReceiver{ .multiplier_normal = multiplier_normal,.multiplier_fire = multiplier_fire, .multiplier_wind = multiplier_wind,.multiplier_knockback = multiplier_knockback })
			;
	}
	for (auto& portal_v : *portals) {
		auto portal = portal_v.as_obj();
		if (!portal) return;
		auto w = (*portal)["w"].as_num();
		auto h = (*portal)["h"].as_num();
		auto x = (*portal)["x"].as_num();
		auto y = (*portal)["y"].as_num();
		auto to_load = (*portal)["to_load"].as_str();
		if (!w || !h || !x || !y || !to_load) return;

		api.spawn()
			.with(Portal{ .to_load = *to_load })
			.with(DebugInfo{ .name = "portal to " + *to_load })
			.with(Body{ .w = *w,.h = *h,.x = *x,.y = *y })
			;
	}
}