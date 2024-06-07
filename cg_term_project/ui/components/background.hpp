#pragma once
#include "../ui.hpp"
#include <format>

UiComponent background_component() {
	UiComponent c;

	c.on_app_started = [] {
		auto& shader = fetch_shader("background");
		shader.initShader("assets/default.vs", "assets/emission.fs");
		auto& model = fetch_model("assets/background_stuff.dae");
		auto id = load_texture("assets/noise.png");
		model.meshes[0].textures.push_back(Texture{ .id = id,.type = "texture_diffuse",.path = "assets/noise.png" });
		};
	c.render = [=] {
		auto render_data = fetch<RenderData>();
		auto& camera = fetch<RenderData>()->camera;
		auto lighting = fetch<Lighting>();

		auto world = render_data->world;
		auto curr_time = render_data->curr_time;
		auto simulation_elapsed = render_data->simulation_elapsed();
		auto render_elapsed = render_data->render_elapsed();

		auto& shader = fetch_shader("background");
		auto& model = fetch_model("assets/background_stuff.dae");

		shader.use();
		shader.setMat4("view", camera.view());
		shader.setVec3("viewPos", camera.position);
		shader.setMat4("projection", camera.projection);

		auto wolves = world->get_entities_with<Wolf, Body>();
		if (wolves.empty()) return;
		auto& [_1, _2, body] = *wolves.begin();
		auto env = lighting->environment;
		shader.setVec3("color1", env[0] + 0.05f, env[1], env[2] + 0.05f);
		shader.setVec3("color2", env[0], env[1] + 0.05f, env[2]);

		int size = 200;
		for (int i = -1;i <= 1;i++) {
			for (int j = -1;j <= 1;j++) {
				auto x = (int)body->x / size * size + i * size;
				auto y = (int)body->y / size * size + j * size;
				auto trans = glm::translate(glm::mat4{ 1 }, glm::vec3(x, y, -50));
				if (sin(37 * x + 71 * y) > 0) {
					trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0, 1, 0));
				}
				trans = glm::scale(trans, glm::vec3(size, size, size));
				shader.setMat4("model", trans);
				model.Draw(shader);
			}
		}
		};
	return c;
}