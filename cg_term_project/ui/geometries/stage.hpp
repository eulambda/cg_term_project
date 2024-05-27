#pragma once
#include "../ui.hpp"

UiComponent get_stage_model() {
	UiComponent c{ "geometry" };

	c.render = [] {
		auto& model = fetch_model("assets/cube.dae");
		auto& shader = fetch_shader("paper");
		shader.use();

		auto render_data = fetch<RenderData>();
		auto simulation_elapsed = render_data->simulation_elapsed();
		auto world = render_data->world;


		shader.setVec3("objectColor", 1, 0, 0);
		auto trans = glm::scale(glm::mat4{ 1 }, glm::vec3(0.1, 0.1, 0.1));
		shader.setMat4("model", trans);
		model.Draw(shader);

		shader.setVec3("objectColor", 1, 1, 1);
		for (auto& [id, _, floor] : world->get_entities_with<Floor, Body>()) {
			auto trans = glm::translate(glm::mat4{ 1 }, glm::vec3(floor->x, -0.5 + floor->y, 0));
			trans = glm::scale(trans, glm::vec3(floor->w, floor->h, 1));
			shader.setMat4("model", trans);
			model.Draw(shader);
		}
		};
	return c;
}