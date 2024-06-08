#pragma once
#include "../ui.hpp"

UiComponent flower_component() {
	UiComponent c;

	c.render = [=] {
		auto render_data = fetch<RenderData>();
		auto world = render_data->world;
		auto curr_time = render_data->curr_time;
		auto simulation_elapsed = render_data->simulation_elapsed();

		auto& shader = fetch_shader("paper");
		auto& bar = fetch_model("assets/flower.dae");
		shader.use();

		auto flower_random = [](size_t t) {
			return (float)sin(t * 200 + 100);
			};
		auto flower_breeze = [=](double x) {
			return (float)sin(curr_time + x);
			};

		for (auto& [id, grass, body] : world->get_entities_with<Flower, Body>()) {
			auto x = body->x;
			auto s = body->w / 0.2;
			auto y = body->y0() + s*0.7;
			auto z = flower_random(id) > 0 ? 0.45f : -0.45f;
			auto ry = flower_random(id * 2) > 0 ? 0.0f : 180.0f;
			auto rz = flower_random(id * 4) * 10;
			ry += flower_breeze(x * 0.8 + y * 0.8) * 2;
			rz += flower_breeze(x + y) * 5;
			auto trans = glm::translate(glm::mat4{ 1 }, glm::vec3(x, y, z));
			trans = glm::rotate(trans, glm::radians(rz), glm::vec3(0, 0, 1));
			//trans = glm::rotate(trans, glm::radians(ry), glm::vec3(0, 1, 0));
			trans = glm::scale(trans, glm::vec3(s, s, s));
			shader.setMat4("model", trans);
			shader.setVec3("color1", 0.4f, 0.8f, 0.2f);
			shader.setVec3("color2", 1.0f, 0.75f, 0.85f);
			bar.Draw(shader);
		}
		};
	return c;
}