#pragma once
#include "../ui.hpp"

UiComponent stage_component() {
	UiComponent c;

	c.render = [] {
		auto& model = fetch_model("assets/cube.dae");
		auto& shader = fetch_shader("paper");
		shader.use();

		auto render_data = fetch<RenderData>();
		auto simulation_elapsed = render_data->simulation_elapsed();
		auto world = render_data->world;

		shader.setVec3("color1", 1, 1, 1);
		for (auto& [id, _, floor] : world->get_entities_with<Floor, Body>()) {
			auto x = floor->x + floor->vx * simulation_elapsed;
			auto y = floor->y + floor->vy * simulation_elapsed;
			auto trans = glm::translate(glm::mat4{ 1 }, glm::vec3(x, y, 0));
			trans = glm::scale(trans, glm::vec3(floor->w, floor->h, 1));
			shader.setMat4("model", trans);
			model.Draw(shader);
		}

		for (auto& [id, stage_text] : world->get_entities_with<StageText>()) {
			auto x = stage_text->x;
			auto y = stage_text->y;
			auto contents = stage_text->contents;
			render_data->render_text([=](Text& text) {
				text.set_color(1, 1, 1, 1);
				text.set_content(contents.c_str());
				text.set_transform(x, y, -1, 0.05);
				});
		}
		};
	return c;
}