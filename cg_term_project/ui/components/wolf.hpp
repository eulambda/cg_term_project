#pragma once
#include "../ui.hpp"

UiComponent wolf_component() {
	auto rotating_frozen = fn_from_points({ {0,0},{0.2,glm::radians(-15.0)},{0.8,glm::radians(-5.0)},{1,0} });
	auto rotating_charging = fn_from_points({ {0,0},{0.2,glm::radians(5.0)},{1,glm::radians(10.0)} });
	auto rotating_vy = [](double t) {return glm::radians(-15.0) * std::clamp(t, -1.0, 1.0); };
	UiComponent c;

	c.on_app_started = [] {
		auto& shaking = fetch_curve("wolf.shaking");
		shaking.y_x = [](double x) { return 0.1 * std::sin(x * 20);};
		auto& flipping = fetch_curve("wolf.flipping");
		flipping.x_t = [](double t) {return std::clamp(t, -0.1, 0.0);};
		flipping.y_x = [](double x) {return x * glm::radians(180.0) / 0.1;};
		auto& dying = fetch_curve("wolf.dying");
		dying.x_t = [](double t) {return std::clamp(t, 0.0, 0.1); };
		dying.y_x = [](double x) {return x * glm::radians(180.0) / 0.1; };
		};

	c.render = [=] {
		auto render_data = fetch<RenderData>();
		auto render_elapsed = render_data->render_elapsed();
		auto simulation_elapsed = render_data->simulation_elapsed();
		auto world = render_data->world;
		auto ticks = world->get_resource<Elapsed>()->ticks + simulation_elapsed;

		auto wolves = world->get_entities_with<Wolf, Body, Facing, RoarCharged, FrozenState, Health>();
		if (wolves.empty()) return;
		auto [id, _, body, facing, roar_charged, frozen_state, health] = *wolves.begin();

		fetch_curve("wolf.shaking").t += (body->vx + body->vy) * render_elapsed;
		fetch_curve("wolf.flipping").t += render_elapsed * facing->sign_x();
		if (health->current == 0) {
			fetch_curve("wolf.dying").t += render_elapsed;
		}
		else {
			fetch_curve("wolf.dying").t = 0;
		}

		auto& model = fetch_model("assets/wolf.dae");
		auto& shader = fetch_shader("paper");

		auto wolf_x = body->x + simulation_elapsed * body->vx;
		auto wolf_y = body->y0() + simulation_elapsed * body->vy;
		auto shaking = fetch_curve("wolf.shaking").eval();
		auto flipping = fetch_curve("wolf.flipping").eval();
		auto dying = fetch_curve("wolf.dying").eval();
		auto frozen = frozen_state->ratio(ticks);
		auto charging = 0.0;
		if (roar_charged->is_charging) {
			charging = roar_charged->val + simulation_elapsed;
			charging = std::min(charging, (double)roar_charged->max) / roar_charged->max;
		}

		auto trans = glm::translate(glm::mat4{ 1 }, glm::vec3(wolf_x + shaking, wolf_y, 0));
		trans = glm::rotate(trans, (float)flipping, glm::vec3(0, 1, 0));
		auto r = shaking + rotating_frozen(frozen) + rotating_vy(body->vy) + rotating_charging(charging);
		trans = glm::rotate(trans, (float)r, glm::vec3(0, 0, 1));
		trans = glm::translate(trans, glm::vec3(0, +body->h / 2, 0));
		trans = glm::rotate(trans, (float)dying, glm::vec3(1, 0, 0));
		trans = glm::translate(trans, glm::vec3(0, -body->h / 2, 0));

		shader.use();
		shader.setVec3("color1", 0.8f, 0.8f, 0.8f);
		shader.setVec3("color2", 0.2f, 0.2f, 0.2f);
		shader.setMat4("model", trans);
		model.Draw(shader);
		};
	return c;
}