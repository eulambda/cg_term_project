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
		auto lighting = fetch<Lighting>();

		if (show_hitbox) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			shader.use();
			auto entities = world->get_entities_with<Body>();
			for (auto& [id, body] : entities) {
				auto trans = glm::translate(glm::mat4{ 1 }, glm::vec3(body->x, body->y, 0));
				trans = glm::scale(trans, glm::vec3(body->w, body->h, 1));
				shader.setVec3("objectColor", 1, 1, 1);
				shader.setMat4("model", trans);
				rectangle.Draw(shader);
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		};
	return c;
}