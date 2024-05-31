#pragma once
#include "../ui.hpp"
#include <format>
UiComponent paper_shader() {
	UiComponent c;
	c.on_app_started = [] {
		auto& shader = fetch_shader("paper");
		shader.initShader("assets/default.vs", "assets/default.fs");
		shader.use();
		shader.setFloat("ambientStrength", 0.1f);
		shader.setFloat("specularStrength", 0.5f);
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
	c.on_before_render = [] {
		auto& shader = fetch_shader("paper");
		auto& camera = fetch<RenderData>()->camera;
		auto lighting = fetch<Lighting>();

		shader.use();
		shader.setMat4("view", camera.view());
		shader.setVec3("viewPos", camera.position);
		shader.setMat4("projection", camera.projection);
		};
	return c;
}