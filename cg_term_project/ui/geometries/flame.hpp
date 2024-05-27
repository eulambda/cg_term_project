#pragma once
#include "../ui.hpp"

UiComponent flame_component() {
	auto scaling_life = fn_from_points({ {0,0},{0.1,1},{0.9,1}, { 1,0 } });
	auto rotating_life = fn_from_points({ {0,2},{0.1,1},{0.9,1}, { 1,2 } });
	auto noisy = [](double x) {return 0.3 * std::sin(x * 200);};
	auto particle_path = [](double x) {return 0.3 * std::sin(x * 2);};

	UiComponent c;

	c.on_app_started = [] {
		auto& scaling = fetch_curve("flame.scaling");
		scaling.y_x = [](double x) { return 1 + 0.1 * std::sin(x * 20);};
		auto& rotating = fetch_curve("flame.rotating");
		rotating.y_x = [](double x) { return x;};
		};

	c.render = [=] {
		auto render_data = fetch<RenderData>();
		auto world = render_data->world;
		auto flames = world->get_entities_with<HitDamage, Body, Facing, Life>();
		if (flames.empty()) return;

		auto render_elapsed = render_data->render_elapsed();
		fetch_curve("flame.scaling").t += render_elapsed;
		fetch_curve("flame.rotating").t += render_elapsed;

		auto& model = fetch_model("assets/triangle.dae");
		auto& shader = fetch_shader("paper");
		shader.use();

		auto simulation_elapsed = render_data->simulation_elapsed();
		auto ticks = world->get_resource<Elapsed>()->ticks;
		for (auto& [id, hit_damage, body, facing, life] : flames) {
			auto trans_0 = glm::translate(glm::mat4{ 1 }, glm::vec3(body->x, body->y, 0));
			auto life_ratio = life->ratio(ticks + simulation_elapsed);
			switch (facing->inner) {
			case FacingValue::pos_x:
				trans_0 = glm::rotate(trans_0, glm::radians(0.0f), glm::vec3(0, 1, 0));
				break;
			case FacingValue::neg_x:
				trans_0 = glm::rotate(trans_0, glm::radians(180.0f), glm::vec3(0, 1, 0));
				break;
			}
			trans_0 = glm::translate(trans_0, glm::vec3(1, 0, 0));

			auto& scaling = fetch_curve("flame.scaling");
			auto& rotating = fetch_curve("flame.rotating");
			shader.setVec3("objectColor", 1, 0, 0);
			for (int i = 0;i <= 4;i++) {
				auto t = 4 * life_ratio - i / 4.0;
				if (t > 1 || t < 0) continue;
				auto trans = glm::translate(trans_0, glm::vec3(i, 0, 0));
				auto s = scaling.eval() * scaling_life(t) * (1 + noisy(i));
				trans = glm::scale(trans, glm::vec3(s, s, s));
				auto r = rotating.eval() * rotating_life(t) + noisy(i);
				trans = glm::rotate(trans, (float)r, glm::vec3(0, 0, 1));
				shader.setMat4("model", trans);
				model.Draw(shader);
			}
			shader.setVec3("objectColor", 1, 0.5, 0.5);
			for (int i = 0;i <= 4;i++) {
				auto t = 4 * life_ratio - i / 4.0;
				if (t > 1 || t < 0) continue;
				auto x = (t) * (i / 2.0 + 2) + noisy(i);
				auto y = particle_path(t + i) * (0.1 + i / 4.0) + noisy(i);
				auto trans = glm::translate(trans_0, glm::vec3(x, y, 0.1));
				auto s = scaling.eval() * 0.8;
				trans = glm::scale(trans, glm::vec3(s, s, s));
				auto r = rotating.eval() * 3;
				trans = glm::rotate(trans, (float)r, glm::vec3(0, 0, 1));
				shader.setMat4("model", trans);
				model.Draw(shader);
			}
			shader.setVec3("objectColor", 1, 1, 1);
			for (int i = 0;i <= 4;i++) {
				auto t = 4 * life_ratio - i / 4.0;
				if (t > 1 || t < 0) continue;
				auto x = (t) * (i / 4.0 + 2) + i / 2.0 + noisy(i);
				auto y = particle_path(t + i) * (0.1 + i / 4.0) + 1.5 * noisy(i);
				auto trans = glm::translate(trans_0, glm::vec3(x, y, 0.1));
				auto s = 0.2;
				trans = glm::scale(trans, glm::vec3(s, s, s));
				shader.setMat4("model", trans);
				model.Draw(shader);
			}
		}
		};

	return c;
}