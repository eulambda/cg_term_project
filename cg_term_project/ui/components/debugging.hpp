#pragma once
#include "../ui.hpp"
#include <set>
UiComponent debugging_component() {
	UiComponent c;
	auto debugging = json::Value::parse("assets/debug.json");
	if (!debugging.is_valid()) return c;
	auto& obj = *debugging.as_obj();
	std::set<std::string> features{};
	for (auto& f : *obj["features"].as_arr()) {
		features.insert(*f.as_str());
	}
	if (features.size() == 0) return c;

	bool show_hitbox = features.find("show_hitbox") != features.end();

	c.render = [=] {
		auto& shader = fetch_shader("paper");
		auto& rectangle = fetch_model("assets/rectangle.dae");
		auto render_data = fetch<RenderData>();
		auto& camera = render_data->camera;
		auto& world = render_data->world;
		auto simulation_elapsed = render_data->simulation_elapsed();
		auto lighting = fetch<Lighting>();

		if (show_hitbox) {
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
		};
	return c;
}