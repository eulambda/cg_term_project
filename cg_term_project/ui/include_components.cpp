#include "ui.hpp"
#include "geometries/wolf.hpp"
#include "geometries/flame.hpp"
#include "geometries/stage.hpp"
#include "shaders/paper.hpp"

void include_components() {
	auto& components = fetch<RenderData>()->components;

	// shaders
	components.emplace_back(get_paper_shader());

	// models
	components.emplace_back(get_wolf_model());
	components.emplace_back(get_flame_model());
	components.emplace_back(get_stage_model());
}