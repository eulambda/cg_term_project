#include "../ui.hpp"
#include "wolf.hpp"
#include "pig.hpp"
#include "flame.hpp"
#include "stage.hpp"
#include "paper_shader.hpp"

void include_components() {
	auto& components = fetch<RenderData>()->components;

	components.emplace_back(paper_shader());

	components.emplace_back(wolf_component());
	components.emplace_back(pig_component());
	components.emplace_back(flame_component());
	components.emplace_back(stage_component());
}