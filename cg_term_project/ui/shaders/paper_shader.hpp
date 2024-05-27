#pragma once
#include "../ui.hpp"

UiComponent paper_shader() {
	UiComponent c;
	c.on_app_started = [] {
		auto& shader = fetch_shader("paper");
		shader.initShader("assets/cube.vs", "assets/cube.fs");
		shader.use();
		shader.setFloat("ambientStrength", 0.1f);
		shader.setFloat("specularStrength", 0.5f);
		shader.setFloat("specularPower", 64.0f);

		auto lighting = fetch<Lighting>();
		shader.setVec3("lightColor", lighting->color);
		shader.setVec3("lightPos", lighting->position);
		};
	c.on_before_render = [] {
		auto& shader = fetch_shader("paper");
		auto& camera = fetch<RenderData>()->camera;

		shader.use();
		shader.setMat4("view", camera.view());
		shader.setVec3("viewPos", camera.position);
		shader.setMat4("projection", camera.projection);
		};
	return c;
}