#include "../ui.hpp"
#include "wolf.hpp"
#include "pig.hpp"
#include "flame.hpp"
#include "roar.hpp"
#include "stage.hpp"
#include "paper_shader.hpp"
#include "debugging.hpp"
#include "bars.hpp"
#include "grass.hpp"

void include_components() {
	auto& components = fetch<RenderData>()->components;

	components.emplace_back(paper_shader());

	components.emplace_back(wolf_component());
	components.emplace_back(pig_component());
	components.emplace_back(flame_component());
	components.emplace_back(roar_component());
	components.emplace_back(stage_component());
	components.emplace_back(bars_component());
	components.emplace_back(grass_component());

	components.emplace_back(debugging_component());
}