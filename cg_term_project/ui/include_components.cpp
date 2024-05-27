#include "ui.hpp"
#include "geometries/wolf.hpp"
#include "geometries/flame.hpp"
#include "geometries/stage.hpp"
#include "shaders/paper_shader.hpp"

void include_components() {
	auto& components = fetch<RenderData>()->components;

	components.emplace_back(paper_shader());

	components.emplace_back(wolf_component());
	components.emplace_back(flame_component());
	components.emplace_back(stage_component());
}