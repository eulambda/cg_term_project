#pragma once
#include "../ui.hpp"

UiComponent stage_component() {
	UiComponent c;

	c.render = [] {
		auto& cube = fetch_model("assets/cube.dae");
		auto& obstacle_hay = fetch_model("assets/obstacle_hay.dae");
		auto& obstacle_wood = fetch_model("assets/obstacle_wood.dae");
		auto& shader = fetch_shader("paper");
		shader.use();

		auto render_data = fetch<RenderData>();
		auto simulation_elapsed = render_data->simulation_elapsed();
		auto world = render_data->world;

		shader.setVec3("color1", 1, 1, 1);
		for (auto& [id, floor, body] : world->get_entities_with<Floor, Body>()) {
			if (floor->type != FloorType::ground) continue;
			auto x = body->x + body->vx * simulation_elapsed;
			auto y = body->y + body->vy * simulation_elapsed;
			auto trans = glm::translate(glm::mat4{ 1 }, glm::vec3(x, y, 0));
			trans = glm::scale(trans, glm::vec3(body->w, body->h, 1));
			shader.setMat4("model", trans);
			cube.Draw(shader);
		}
		for (auto& [id, _, compound, body] : world->get_entities_with<Obstacle, Compound, Body>()) {
			auto x = body->x + body->vx * simulation_elapsed;
			auto y = body->y + body->vy * simulation_elapsed;
			auto trans = glm::translate(glm::mat4{ 1 }, glm::vec3(x, y, 0));
			trans = glm::scale(trans, glm::vec3(body->w, body->h, 1));
			shader.setMat4("model", trans);
			if (compound->made_of == ParticleType::hay) obstacle_hay.Draw(shader);
			else if (compound->made_of == ParticleType::wood) obstacle_wood.Draw(shader);
		}
		for (auto& [id, pig_house, compound, body] : world->get_entities_with<PigHouse, Compound, Body>()) {
			auto x = body->x + body->vx * simulation_elapsed;
			auto y = body->y + body->vy * simulation_elapsed;
			auto trans = glm::translate(glm::mat4{ 1 }, glm::vec3(x, y, -1));
			trans = glm::scale(trans, glm::vec3(body->w, body->h, 0.2));
			shader.setMat4("model", trans);
			cube.Draw(shader);
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