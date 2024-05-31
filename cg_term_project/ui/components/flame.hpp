#pragma once
#include "../ui.hpp"

UiComponent flame_component() {
	auto scaling_life = fn_from_points({ {0,0},{0.1,1},{0.9,1}, { 1,0 } });
	auto rotating_life = fn_from_points({ {0,2},{0.1,1},{0.9,1}, { 1,2 } });
	auto noise = [](double x) {return 0.3 * std::sin(x * 200); };
	auto particle_path = [](double x) {return 0.3 * std::sin(x * 2); };

	UiComponent c;

	c.on_app_started = [] {
		auto& scaling = fetch_curve("flame.scaling");
		scaling.y_x = [](double x) { return 1 + 0.1 * std::sin(x * 20); };
		auto& rotating = fetch_curve("flame.rotating");
		rotating.y_x = [](double x) { return x; };
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
			if (hit_damage->type != DamageType::fire) continue;
			auto trans_0 = glm::mat4{ 1 };
			auto z = facing->sign_x();
			auto life_ratio = life->ratio(ticks + simulation_elapsed);
			switch (facing->inner) {
			case FacingValue::pos_x:
				trans_0 = glm::translate(trans_0, glm::vec3(body->x0(), body->y, 0));
				trans_0 = glm::rotate(trans_0, glm::radians(0.0f), glm::vec3(0, 1, 0));
				break;
			case FacingValue::neg_x:
				trans_0 = glm::translate(trans_0, glm::vec3(body->x1(), body->y, 0));
				trans_0 = glm::rotate(trans_0, glm::radians(180.0f), glm::vec3(0, 1, 0));
				break;
			}
			trans_0 = glm::translate(trans_0, glm::vec3(1, 0, 0));

			auto& scaling = fetch_curve("flame.scaling");
			auto& rotating = fetch_curve("flame.rotating");
			shader.setVec3("objectColor", 2, 0, 0);
			double i_max = 4;
			double speed = 1.5;
			for (int i = 0; i < i_max; i++) {
				auto t = speed * life_ratio - (speed - 1) * i / (i_max - 1);
				t = 3.0 * t - 0.6;
				auto x = i / i_max * body->w;
				if (t > 1 || t < 0) continue;
				auto trans = glm::translate(trans_0, glm::vec3(x, 0, z));
				auto s = scaling.eval() * scaling_life(t) * (1 + noise(i));
				trans = glm::scale(trans, glm::vec3(s, s, s));
				auto r = rotating.eval() * rotating_life(t) + noise(i);
				trans = glm::rotate(trans, (float)r, glm::vec3(0, 0, 1));
				shader.setMat4("model", trans);
				model.Draw(shader);
			}
			shader.setVec3("objectColor", 2, 1, 1);
			i_max = 5;
			speed = 2;
			for (int i = 0; i < i_max; i++) {
				auto t = speed * life_ratio - (speed - 1) * i / (i_max - 1);
				t = 2.5 * t - 0.2;
				auto x = i / i_max * body->w;
				x = 0.5 * x + 0.5 * t + noise(i);
				auto y = particle_path(t + i) * (0.1 + 2 * i / (i_max - 1)) + noise(i);
				if (t > 1 || t < 0) continue;
				auto trans = glm::translate(trans_0, glm::vec3(x, y, 1.1 * z));
				auto s = scaling.eval() * scaling_life(t) * 0.6;
				trans = glm::scale(trans, glm::vec3(s, s, s));
				auto r = rotating.eval() * 3;
				trans = glm::rotate(trans, (float)r, glm::vec3(0, 0, 1));
				shader.setMat4("model", trans);
				model.Draw(shader);
			}
			i_max = 5;
			speed = 2;
			for (int i = 0; i < i_max; i++) {
				auto t = speed * life_ratio - (speed - 1) * i / (i_max - 1);
				t = 2.0 * t - 0.5;
				auto x = i / i_max * body->w;
				x = 0.5 * x + 0.5 * t + noise(i);
				auto y = particle_path(t + i + 8) * (0.1 + 2 * i / (i_max - 1)) + noise(i + 8);
				if (t > 1 || t < 0) continue;
				auto trans = glm::translate(trans_0, glm::vec3(x, y, 1.1 * z));
				auto s = scaling.eval() * scaling_life(t) * 0.6;
				trans = glm::scale(trans, glm::vec3(s, s, s));
				auto r = rotating.eval() * 3;
				trans = glm::rotate(trans, (float)r, glm::vec3(0, 0, 1));
				shader.setMat4("model", trans);
				model.Draw(shader);
			}
			shader.setVec3("objectColor", 2, 2, 2);
			i_max = 10;
			speed = 3;
			for (int i = 0; i < i_max; i++) {
				auto t = speed * life_ratio - (speed - 1) * i / (i_max - 1);
				auto x = i / i_max * body->w;
				x = 0.5 * x + 0.5 * t;
				if (t > 1 || t < 0) continue;
				x = 1.1 * x + noise(i);
				auto y = particle_path(t + i) * (0.1 + 2*i / (i_max - 1)) + 1.5 * noise(i);
				auto trans = glm::translate(trans_0, glm::vec3(x, y, 1.2 * z));
				auto s = 0.2;
				trans = glm::scale(trans, glm::vec3(s, s, s));
				shader.setMat4("model", trans);
				model.Draw(shader);
			}
		}
		};

	return c;
}