#pragma once
#include "../ui.hpp"

UiComponent butterflies_component() {
	UiComponent c;

	c.on_app_started = [] {
		auto& shader = fetch_shader("butterfly");
		shader.initShader("assets/skeleton.vs", "assets/thin_paper.fs");
		shader.use();
		shader.setFloat("ambientStrength", 0.5f);
		shader.setFloat("specularStrength", 0.01f);
		shader.setFloat("specularPower", 64.0f);

		auto lighting = fetch<Lighting>();
		shader.setVec3("ambientColor", lighting->environment);
		for (int i = 0;i < lighting->camera_lights.size();i++) {
			auto key = std::format("camera_lights[{}]", i);
			auto& light = lighting->camera_lights[i];
			shader.setFloat(key + ".intensity", light.intensity);
			shader.setVec3(key + ".color", light.color);
			shader.setVec3(key + ".offset", light.offset);
		}
		};
	c.render = [=] {
		auto render_data = fetch<RenderData>();
		auto& camera = fetch<RenderData>()->camera;
		auto lighting = fetch<Lighting>();

		auto world = render_data->world;
		auto curr_time = render_data->curr_time;
		auto simulation_elapsed = render_data->simulation_elapsed();
		auto render_elapsed = render_data->render_elapsed();

		auto& shader = fetch_shader("butterfly");
		auto& butterfly = fetch_model("assets/butterfly.dae");
		auto& animator = fetch_animator("assets/butterfly.dae");

		shader.use();
		shader.setMat4("view", camera.view());
		shader.setVec3("viewPos", camera.position);
		shader.setMat4("projection", camera.projection);

		animator.UpdateAnimation((float)render_elapsed * 10);
		auto matrices = animator.GetFinalBoneMatrices();

		for (int i = 0;i < matrices.size();i++) {
			shader.setMat4(std::format("finalBonesMatrices[{}]", i), matrices[i]);
		}

		auto noise = [](double x) {
			return (float)sin(x / 10);
			};

		for (auto& [id, _, body] : world->get_entities_with<Butterfly, Body>()) {
			auto x = body->x + body->vx * simulation_elapsed;
			auto y = body->y + body->vy * simulation_elapsed;
			auto s = body->w / 2;
			auto z = noise(x + y);
			auto rx = 10 * noise(x * 20 + y * 20 + 1 + body->vx + body->vy) + 1.0f;
			auto ry = 10 * noise(x*10 + y*10 + 1 + body->vx + body->vy);
			auto rz = 30 * noise(x + y + 1 + body->vx + body->vy);
			auto trans = glm::translate(glm::mat4{ 1 }, glm::vec3(x, y, z));
			trans = glm::rotate(trans, glm::radians(rz), glm::vec3(0, 0, 1));
			trans = glm::rotate(trans, glm::radians(ry), glm::vec3(1, 0, 0));
			trans = glm::rotate(trans, glm::radians(rx), glm::vec3(1, 0, 0));
			trans = glm::scale(trans, glm::vec3(s, s, s));
			shader.setMat4("model", trans);
			shader.setVec3("color1", 0.2f, 0.4f, 1.5f);
			shader.setVec3("color2", 0.2f, 0.4f, 1.5f);
			butterfly.Draw(shader);
		}
		};
	return c;
}