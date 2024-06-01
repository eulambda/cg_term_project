#pragma once
#include "../ui.hpp"

UiComponent bars_component() {
	UiComponent c;

	c.render = [=] {
		auto render_data = fetch<RenderData>();
		auto render_elapsed = render_data->render_elapsed();
		auto simulation_elapsed = render_data->simulation_elapsed();
		auto world = render_data->world;
		auto ticks = world->get_resource<Elapsed>()->ticks + simulation_elapsed;

		auto& shader = fetch_shader("paper");
		auto& bar = fetch_model("assets/rectangle.dae");
		shader.use();

		for (auto& [_, body, health] : world->get_entities_with<Body, Health>()) {
			auto health_ratio = (std::max(health->current + health->receiving * simulation_elapsed, 0.0)) / health->max;
			if (!health_ratio || health_ratio==1) continue;

			auto x = body->x + body->vx * simulation_elapsed;
			auto y = body->y0() + body->vy * simulation_elapsed - 1;
			auto trans_bar = glm::translate(glm::mat4{ 1 }, glm::vec3(x, y, 0.6));
			trans_bar = glm::scale(trans_bar, glm::vec3(2 * health_ratio, 0.1, 0.1));
			shader.setMat4("model", trans_bar);
			shader.setVec3("color1", 1.5, 0, 0);
			bar.Draw(shader);
		}
		for (auto& [_, body, roar_charged] : world->get_entities_with<Body, RoarCharged>()) {
			if (!roar_charged->is_charging) continue;
			auto charged_ratio = (std::min(roar_charged->val + simulation_elapsed, (double)roar_charged->max)) / roar_charged->max;

			auto x = body->x + body->vx * simulation_elapsed;
			auto y = body->y1() + body->vy * simulation_elapsed + 1;
			auto trans_bar = glm::translate(glm::mat4{ 1 }, glm::vec3(x, y, 0.6));
			trans_bar = glm::scale(trans_bar, glm::vec3(2 * charged_ratio, 0.1, 0.1));
			shader.setMat4("model", trans_bar);
			shader.setVec3("color1", 0.5, 0.5, 1.5);
			bar.Draw(shader);
		}
		};
	return c;
}