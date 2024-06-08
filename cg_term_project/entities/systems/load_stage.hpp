#pragma once
#include "../prelude.hpp"
#include "../../essentials/json_parser.hpp"
#include <iostream>
#include <regex>
#include <format>

std::vector<std::string> load_screen_texts(json::Value& screen_texts_v) {
	auto screen_texts = screen_texts_v.as_arr();
	std::vector<std::string> loaded{};
	if (!screen_texts) return loaded;
	for (auto& screen_text_v : *screen_texts) {
		auto screen_text = screen_text_v.as_str();
		if (!screen_text) continue;
		loaded.push_back(*screen_text);
	}
	return loaded;
}
void spawn_pig_houses(ecs::EntityApi& api, json::Value& pig_houses_v) {
	auto pig_houses = pig_houses_v.as_arr();
	if (!pig_houses) return;
	for (auto& pig_house_v : *pig_houses) {
		auto pig_house = pig_house_v.as_obj();
		if (!pig_house) return;
		auto w = (*pig_house)["w"].as_num();
		auto h = (*pig_house)["h"].as_num();
		auto x = (*pig_house)["x"].as_num();
		auto y = (*pig_house)["y"].as_num();
		auto health = (int)(*pig_house)["health"].num_or(100);
		auto multiplier_normal = (int)(*pig_house)["multiplier_normal"].num_or(0);
		auto multiplier_fire = (int)(*pig_house)["multiplier_fire"].num_or(0);
		auto multiplier_wind = (int)(*pig_house)["multiplier_wind"].num_or(0);
		auto to_load_on_clear = (*pig_house)["to_load_on_clear"].str_or("");
		auto made_of_s = (*pig_house)["made_of"].str_or("none");
		if (!w || !h || !x || !y) return;
		auto made_of = ParticleType::none;
		if (made_of_s == "hay") made_of = ParticleType::hay;
		else if (made_of_s == "wood") made_of = ParticleType::wood;
		else if (made_of_s == "brick") made_of = ParticleType::brick;

		api.spawn()
			.with(PigHouse{ .to_load = to_load_on_clear })
			.with(DebugInfo{ .name = "pig house" })
			.with(Compound{ .made_of = made_of })
			.with(Body{ .w = *w,.h = *h,.x = *x,.y = *y })
			.with(Health{ .max = health,.current = health })
			.with(LocomotionStationery{})
			.with(DamageReceiver{ .multiplier_normal = multiplier_normal,.multiplier_fire = multiplier_fire, .multiplier_wind = multiplier_wind,.multiplier_knockback = 0 })
			.with(ShardSpawner{})
			;
	}
}
void spawn_obstacles(ecs::EntityApi& api, json::Value& obstacles_v) {
	auto obstacles = obstacles_v.as_arr();
	if (!obstacles) return;

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
			.with(Floor{ .type = FloorType::special })
			.with(Obstacle{})
			.with(DebugInfo{ .name = "obstacle " + made_of_s })
			.with(Compound{ .made_of = made_of })
			.with(Body{ .w = *w,.h = *h,.x = *x,.y = *y })
			.with(LocomotionWalking{})
			.with(Health{ .max = health,.current = health })
			.with(DamageReceiver{ .multiplier_normal = multiplier_normal,.multiplier_fire = multiplier_fire, .multiplier_wind = multiplier_wind,.multiplier_knockback = multiplier_knockback })
			;
	}
}
void spawn_portals(ecs::EntityApi& api, json::Value& portals_v) {
	auto portals = portals_v.as_arr();
	if (!portals) return;
	for (auto& portal_v : *portals) {
		auto portal = portal_v.as_obj();
		if (!portal) return;
		auto w = (*portal)["w"].as_num();
		auto h = (*portal)["h"].as_num();
		auto x = (*portal)["x"].as_num();
		auto y = (*portal)["y"].as_num();
		auto to_load = (*portal)["to_load"].as_str();
		auto pause = (int)(*portal)["pause"].num_or(0);
		if (!w || !h || !x || !y || !to_load) return;

		api.spawn()
			.with(Portal{ .to_load = *to_load,.pause = pause })
			.with(DebugInfo{ .name = "portal to " + *to_load })
			.with(Body{ .w = *w,.h = *h,.x = *x,.y = *y })
			;
	}
}
void spawn_floors(ecs::EntityApi& api, json::Value& floors_v, json::Value& props_v) {
	auto floors = floors_v.as_arr();
	auto props = props_v.as_obj();
	if (!floors) return;
	auto grass_density = 0.0;
	auto butterfly_chance = 0.0;
	auto cake = 0.0;
	auto flower_density = 0.0;
	if (props) {
		grass_density = (*props)["grass_density"].num_or(0);
		grass_density = std::clamp(grass_density, 0.0, 1.0);

		butterfly_chance = (*props)["butterfly_chance"].num_or(0);
		butterfly_chance = std::clamp(butterfly_chance, 0.0, 1.0);

		flower_density = (*props)["flower_density"].num_or(0);
		flower_density = std::clamp(flower_density, 0.0, 1.0);

		cake = (*props)["cake"].num_or(0);
	}

	auto noise_2d = [](double x, double y) {
		auto z = sin(x + 8) + sin(x * 4 + 16) / 2 + sin(x * 16 + 32) / 4 + sin(x * 64 + 64) / 4;
		z += sin(y + 128) + sin(y * 5 + 256) / 2 + sin(y * 16 + 512) / 4 + sin(y * 64 + 1024) / 4;
		z = (z + 4) / 8;
		return z;
		};
	auto get_grass_size = [=](double x, double y) {
		auto z = noise_2d(x, y);
		z -= 1 - grass_density;
		z = std::max(z, 0.0);
		return 2 * z;
		};
	auto get_flower_size = [=](double x, double y) {
		auto z = noise_2d(x, y);
		z -= 1 - flower_density;
		z = std::max(z, 0.0);
		return 2 * z;
		};
	auto noise = [](double x) {
		return sin(x * 200 + 100);
		};
	for (auto& floor_v : *floors) {
		auto floor = floor_v.as_obj();
		if (!floor) return;
		auto w = (*floor)["w"].as_num();
		auto h = (*floor)["h"].as_num();
		auto x = (*floor)["x"].as_num();
		auto y = (*floor)["y"].as_num();
		if (!w || !h || !x || !y) return;
		auto body = Body{ .w = *w,.h = *h,.x = *x,.y = *y };
		api.spawn().with(Floor{}).with(body);
		for (auto xi = body.x0();xi < body.x1();xi += 0.4) {
			auto d = get_grass_size(xi, body.y);
			if (d == 0) continue;
			auto x_final = xi + noise(xi + body.y);
			x_final = std::clamp(x_final, body.x0(), body.x1());
			api.spawn()
				.with(Grass{})
				.with(Body{ .w = d,.h = 1,.x = x_final,.y = body.y1() + 0.5 })
				.with(Health{ .max = 1, .current = 1 })
				.with(DamageReceiver{ .multiplier_normal = 0,.multiplier_fire = 1,.multiplier_wind = 0,.multiplier_knockback = 0 })
				;
		}
		for (auto xi = body.x0();xi < body.x1();xi += 1) {
			if (abs(noise(xi * 2 + body.y)) >= butterfly_chance) continue;
			auto x_final = xi + noise(xi * 2 + body.y);
			x_final = std::clamp(x_final, body.x0(), body.x1());
			auto y_final = body.y1() + 2;
			api.spawn()
				.with(Butterfly{ .start_x = x_final,.start_y = y_final })
				.with(LocomotionFlying{})
				.with(DebugInfo{ .name = "butterfly" })
				.with(Body{ .w = 1,.h = 1,.x = x_final,.y = y_final })
				.with(Health{ .max = 1, .current = 1 })
				.with(DamageReceiver{ .multiplier_normal = 0,.multiplier_fire = 1,.multiplier_wind = 1,.multiplier_knockback = 1 })
				;
		}
		for (auto xi = body.x0(); xi < body.x1(); xi += 0.4) {
			auto d = get_flower_size(xi, body.y);
			if (d == 0) continue;
			auto x_final = xi + noise(xi + body.y);
			x_final = std::clamp(x_final, body.x0(), body.x1());
			api.spawn()
				.with(Flower{})
				.with(Body{ .w = d * 0.1 + 0.1,.h = 1,.x = x_final,.y = body.y1() + 0.5 })
				.with(Health{ .max = 1, .current = 1 })
				.with(DamageReceiver{ .multiplier_normal = 0,.multiplier_fire = 1,.multiplier_wind = 0,.multiplier_knockback = 0 })
				;
		}
	}
	if (cake) {
		api.spawn()
			.with(Obstacle{})
			.with(Compound{ .made_of = ParticleType::cake })
			.with(LocomotionStationery{})
			.with(DebugInfo{ .name = "cake" })
			.with(Body{ .w = 1,.h = 1,.x = 0,.y = 3 })
			.with(Health{ .max = 1, .current = 1 })
			.with(DamageReceiver{ .multiplier_normal = 1,.multiplier_fire = 1,.multiplier_wind = 1,.multiplier_knockback = 1 })
			;
	}
}
void spawn_texts(ecs::EntityApi& api, SimulationSpeed& simulation_speed, ecs::Writable<Elapsed>& elapsed, json::Value& stage_texts_v) {
	auto stage_texts = stage_texts_v.as_arr();
	if (!stage_texts) return;
	auto record_seconds = (int)(simulation_speed.seconds_per_tick * elapsed->ticks);
	auto record_string = std::format("{:01}:{:02}", record_seconds / 60, record_seconds % 60);
	for (auto& stage_text_v : *stage_texts) {
		auto stage_text = stage_text_v.as_obj();
		if (!stage_text) return;
		auto x = (*stage_text)["x"].as_num();
		auto y = (*stage_text)["y"].as_num();
		auto contents = (*stage_text)["contents"].as_str();
		if (!x || !y || !contents) return;
		*contents = std::regex_replace(*contents, std::regex{ "\\{record\\}" }, record_string);
		api.spawn().with(StageText{ .contents = *contents, .x = *x,.y = *y });
	}
}
void load_stage(
	ecs::EntitiesWithWritable<Wolf, Body, FrozenState, Health> wolves, ecs::Writable<Stage> stage, ecs::EntityApi api
	, ecs::EntitiesWith<Body> bodies
	, ecs::EntitiesWith<StageText> prev_stage_texts
	, ecs::Writable<Elapsed> elapsed
	, SimulationSpeed simulation_speed
) {
	stage->transition_prev = stage->transition;
	stage->is_paused = true;

	if (stage->screen_texts.size()) {
		return;
	}
	if (!stage->queued.size()) {
		stage->transition = std::min(stage->transition + 0.1, 1.0);
		stage->is_paused = false;
		return;
	}
	auto& queued = stage->queued.front();
	if (queued.type == StageActionType::pause) {
		queued.num--;
		stage->transition = std::min(queued.num / 24.0, 1.0);
		if (queued.num > 0) return;
		stage->queued.pop();
		return;
	}
	if (queued.type != StageActionType::load) return;
	auto json = json::Value::parse(queued.str);
	stage->queued.pop();

	if (!json.is_valid()) {
		std::cout << "Error: failed to load stage configuration." << std::endl;
		return;
	}
	auto obj = json.as_obj();
	if (!obj) return;
	auto& start_v = (*obj)["start"];
	auto& reset_ticks_v = (*obj)["reset_ticks"];
	auto start = start_v.as_obj();
	auto reset_ticks = reset_ticks_v.bool_or(false);
	if (!start) return;

	auto start_x = (*start)["x"].as_num();
	auto start_y = (*start)["y"].as_num();
	if (!start_x || !start_y) return;

	if (wolves.empty()) return;

	auto& [wolf_id, wolf, wolf_body, wolf_frozen_state, wolf_health] = *wolves.begin();
	wolf_body->vx = 0;
	wolf_body->vy = 0;
	wolf_body->x = *start_x;
	wolf_body->y = *start_y;

	for (auto& [id, _] : bodies) if (id != wolf_id) api.remove(id);
	for (auto& [id, _] : prev_stage_texts) api.remove(id);

	spawn_floors(api, (*obj)["floors"], (*obj)["props"]);
	spawn_texts(api, simulation_speed, elapsed, (*obj)["stage_texts"]);
	spawn_portals(api, (*obj)["portals"]);
	spawn_obstacles(api, (*obj)["obstacles"]);
	spawn_pig_houses(api, (*obj)["pig_houses"]);

	for (auto& screen_text : load_screen_texts((*obj)["screen_texts"])) {
		stage->screen_texts.push(screen_text);
	}

	if (reset_ticks) {
		elapsed->ticks = 0;
		wolf_frozen_state->from = 0;
		wolf_frozen_state->until = 0;
		wolf_health->current = wolf_health->max;
		wolf->is_dying = false;
	}
}