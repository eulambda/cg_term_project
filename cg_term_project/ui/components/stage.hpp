#pragma once
#include "../ui.hpp"

UiComponent stage_component() {
	UiComponent c;

	c.render = [] {
		auto& cube = fetch_model("assets/cube.dae");
		auto& house = fetch_model("assets/house.dae");
		auto& triangle = fetch_model("assets/triangle.dae");
		auto& rectangle = fetch_model("assets/rectangle.dae");
		auto& pentagon = fetch_model("assets/pentagon.dae");
		auto& cake = fetch_model("assets/cake.dae");
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
			if (body->w * body->h < 4 && compound->made_of != ParticleType::cake) {
				auto dz = std::sin(71 * id);
				trans = glm::translate(trans, glm::vec3(0, 0, dz));
				auto rz = 3 * std::sin(17 + 2 * x + 2 * y + 59 * id);
				trans = glm::rotate(trans, (float)rz, glm::vec3(0, 0, 1));
				auto rx = std::sin(59 * id) > 0 ? glm::radians(180.0f) : 0;
				trans = glm::rotate(trans, (float)rx, glm::vec3(1, 0, 0));
			}
			trans = glm::scale(trans, glm::vec3(body->w, body->h, 1));
			shader.setMat4("model", trans);
			if (body->w * body->h < 4) {
				if (compound->made_of == ParticleType::hay) {
					shader.setVec3("color1", 1, 1, 0.5f);
					shader.setVec3("color2", 0.5f, 1, 0.2f);
					triangle.Draw(shader);
				}
				else if (compound->made_of == ParticleType::wood) {
					shader.setVec3("color1", 0.3f, 0.4f, 0.4f);
					shader.setVec3("color2", 0.2f, 0.2f, 0.2f);
					rectangle.Draw(shader);
				}
				else if (compound->made_of == ParticleType::brick) {
					shader.setVec3("color1", 0.4f, 0.2f, 0.2f);
					shader.setVec3("color2", 0.2f, 0.15f, 0.15f);
					pentagon.Draw(shader);
				}
				else if (compound->made_of == ParticleType::cake) {
					shader.setVec3("color1", 1, 1, 0.9f);
					shader.setVec3("color2", 1, 0, 0);
					cake.Draw(shader);
				}
				continue;
			}

			if (compound->made_of == ParticleType::hay) {
				shader.setVec3("color1", 1, 1, 0.5f);
				cube.Draw(shader);
			}
			else if (compound->made_of == ParticleType::wood) {
				shader.setVec3("color1", 0.4f, 0.3f, 0.3f);
				cube.Draw(shader);
			}
		}

		for (auto& [id, pig_house, compound, body] : world->get_entities_with<PigHouse, Compound, Body>()) {
			auto x = body->x + body->vx * simulation_elapsed;
			auto y = body->y0() + body->vy * simulation_elapsed;
			auto trans = glm::translate(glm::mat4{ 1 }, glm::vec3(x, y, -1));
			trans = glm::scale(trans, glm::vec3(body->w, body->w, 1));

			if (compound->made_of == ParticleType::hay) {
				shader.setVec3("color1", 0.2f, 0.3f, 0.2f);
				shader.setVec3("color2", 0.15f, 0.2f, 0.15f);
			}
			else if (compound->made_of == ParticleType::wood) {
				shader.setVec3("color1", 0.3f, 0.3f, 0.2f);
				shader.setVec3("color2", 0.2f, 0.2f, 0.15f);
			}
			else if (compound->made_of == ParticleType::brick) {
				shader.setVec3("color1", 0.4f, 0.2f, 0.2f);
				shader.setVec3("color2", 0.2f, 0.15f, 0.15f);
			}
			shader.setMat4("model", trans);
			house.Draw(shader);
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