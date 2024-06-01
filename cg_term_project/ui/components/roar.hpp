#pragma once
#include "../ui.hpp"

UiComponent roar_component() {
	auto scaling_life = fn_from_points({ {0,0},{0.2,1},{0.8,1}, { 1,0 } });
	auto rotating_life = fn_from_points({ {0,2},{0.2,1},{0.8,1}, { 1,2 } });
	auto noise = [](double x) {return 0.7 * std::sin(x * 200); };
	auto particle_path = [](double x) {return 0.3 * std::sin(x * 2); };

	UiComponent c;

	c.on_app_started = [] {
		auto& scaling = fetch_curve("roar.scaling");
		scaling.y_x = [](double x) { return 1 + 0.1 * std::sin(x * 20); };
		auto& rotating = fetch_curve("roar.rotating");
		rotating.y_x = [](double x) { return x / 20; };
		};

	c.render = [=] {
		auto render_data = fetch<RenderData>();
		auto world = render_data->world;
		auto roars = world->get_entities_with<Roar, HitDamage, Body, Facing, Life>();
		if (roars.empty()) return;

		auto render_elapsed = render_data->render_elapsed();
		fetch_curve("roar.scaling").t += render_elapsed;
		fetch_curve("roar.rotating").t += render_elapsed;

		auto& model = fetch_model("assets/rectangle.dae");
		auto& model_big = fetch_model("assets/rectangle_big.dae");
		auto& shader = fetch_shader("paper");
		shader.use();

		auto simulation_elapsed = render_data->simulation_elapsed();
		auto ticks = world->get_resource<Elapsed>()->ticks;
		for (auto& [id, _, hit_damage, body, facing, life] : roars) {
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

			auto& scaling = fetch_curve("roar.scaling");
			auto& rotating = fetch_curve("roar.rotating");

			double i_max = 4;
			double speed = 1.5;
			if (hit_damage->power > 0) {
				shader.setVec3("color1", 1, 1, 1.5f);
				shader.setVec3("color2", 1.5f, 1.5f, 1.5f);
				for (int i = 0; i < i_max; i++) {
					auto t = speed * life_ratio - (speed - 1) * i / (i_max - 1);
					t = 3.0 * t - 0.6;
					auto x = i / i_max * body->w;
					if (t > 1 || t < 0) continue;
					auto trans = glm::translate(trans_0, glm::vec3(x, noise(i * 10), z));
					auto s = scaling.eval() * scaling_life(t) * (1.2 + 0.2 * noise(i));
					trans = glm::scale(trans, glm::vec3(s, s, s));
					auto r = rotating.eval() * rotating_life(t) + noise(i);
					trans = glm::rotate(trans, (float)r, glm::vec3(0, 0, 1));
					shader.setMat4("model", trans);
					model_big.Draw(shader);
				}
				shader.setVec3("color1", 1, 1, 2);
				shader.setVec3("color2", 1, 1, 2);
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
			}
			shader.setVec3("color1", 1, 1, 2);
			shader.setVec3("color2", 1, 1, 2);
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
			shader.setVec3("color1", 2, 2, 2);
			shader.setVec3("color2", 2, 2, 2);
			i_max = 10;
			speed = 7;
			for (int i = 0; i < i_max; i++) {
				auto t = speed * life_ratio - (speed - 1) * i / (i_max - 1);
				auto x = i / i_max * body->w;
				x = 0.8 * x + 0.2 * t;
				if (t > 1 || t < 0) continue;
				x = 1.2 * x + noise(i);
				auto y = particle_path(t + i) * (0.1 + 2 * i / (i_max - 1)) + 1.5 * noise(i);
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