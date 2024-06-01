#pragma once
#include "../ui.hpp"
#include <set>
UiComponent debugging_component() {
	UiComponent c;
	auto debug = fetch<UiDebugInfo>();
	if (!debug->is_enabled) return c;

	c.render = [=] {
		auto& shader = fetch_shader("paper");
		auto& rectangle = fetch_model("assets/rectangle.dae");
		auto render_data = fetch<RenderData>();
		auto& camera = render_data->camera;
		auto& world = render_data->world;
		auto simulation_elapsed = render_data->simulation_elapsed();
		auto lighting = fetch<Lighting>();

		if (debug->show_hitbox) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			shader.use();
			auto entities = world->get_entities_with<Body, DebugInfo>();
			for (auto& [id, body, debug_info] : entities) {
				auto x = body->x + body->vx * simulation_elapsed;
				auto y = body->y + body->vy * simulation_elapsed;
				auto trans = glm::translate(glm::mat4{ 1 }, glm::vec3(x, y, 0));
				trans = glm::scale(trans, glm::vec3(body->w, body->h, 1));
				shader.setVec3("color1", 1, 1, 1);
				shader.setMat4("model", trans);
				rectangle.Draw(shader);
				render_data->render_text([=](Text& t) {
					t.set_content(debug_info->name.c_str());
					t.set_color(1, 1, 1, 1);
					t.set_transform(x - body->w / 2, y + body->h / 2, 1, 0.04);
					});
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (debug->show_coords) {
			auto [_1,_2,body] = world->get_entities_with<Wolf,Body>()[0];
			auto x = body->x + body->vx * simulation_elapsed;
			auto y = body->y + body->vy * simulation_elapsed;
			auto coords = std::format("({:.1f},{:.1f})", x,y);
			render_data->render_text([=](Text& t) {
				t.set_content(coords.c_str());
				t.set_color(1, 1, 1, 1);
				t.set_transform(x + body->w / 2, y + body->h / 2, 1, 0.04);
				});
		}

		};
	return c;
}