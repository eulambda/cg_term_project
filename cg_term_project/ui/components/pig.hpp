#pragma once
#include "../ui.hpp"

UiComponent pig_component() {
	auto rotating_vy = [](double t) {return glm::radians(-15.0) * std::clamp(t, -1.0, 1.0); };
	UiComponent c;

	c.on_app_started = [] {
		auto& shaking = fetch_curve("pig.shaking");
		shaking.y_x = [](double x) { return 0.1 * std::sin(x * 20); };
		auto& flipping = fetch_curve("pig.flipping");
		flipping.x_t = [](double t) {return std::clamp(t, -0.1, 0.0); };
		flipping.y_x = [](double x) {return x * glm::radians(180.0) / 0.1; };
		auto& dying = fetch_curve("pig.dying");
		dying.x_t = [](double t) {return std::clamp(t, 0.0, 0.1); };
		dying.y_x = [](double x) {return x * glm::radians(180.0) / 0.1; };
		};

	c.render = [=] {
		auto render_data = fetch<RenderData>();
		auto render_elapsed = render_data->render_elapsed();
		auto simulation_elapsed = render_data->simulation_elapsed();
		auto world = render_data->world;
		auto ticks = world->get_resource<Elapsed>()->ticks + simulation_elapsed;
		auto pigs = world->get_entities_with<Pig, Body, Facing, Health>();
		if (pigs.empty()) return;

		auto& [_1, pig, body, facing, health] = pigs[0];

		fetch_curve("pig.shaking").t += (body->vx + body->vy) * render_elapsed;
		fetch_curve("pig.flipping").t += render_elapsed * facing->sign_x();
		if (pig->action == PigAction::dying) {
			fetch_curve("pig.dying").t += render_elapsed;
		}
		else {
			fetch_curve("pig.dying").t = 0;
		}

		auto& model = fetch_model("assets/wolf.dae");
		auto& shader = fetch_shader("paper");

		auto pig_x = body->x + simulation_elapsed * body->vx;
		auto pig_y = body->y0() + simulation_elapsed * body->vy;
		auto shaking = fetch_curve("pig.shaking").eval();
		auto flipping = fetch_curve("pig.flipping").eval();
		auto dying = fetch_curve("pig.dying").eval();

		auto trans = glm::translate(glm::mat4{ 1 }, glm::vec3(pig_x + shaking, pig_y, 0));
		trans = glm::rotate(trans, (float)flipping, glm::vec3(0, 1, 0));
		auto r = shaking + rotating_vy(body->vy);
		trans = glm::rotate(trans, (float)r, glm::vec3(0, 0, 1));

		trans = glm::translate(trans, glm::vec3(0, +body->h / 2, 0));
		trans = glm::rotate(trans, (float)dying, glm::vec3(1, 0, 0));
		trans = glm::translate(trans, glm::vec3(0, -body->h / 2, 0));

		auto body_color = glm::vec3(0, 0.2f, 0.2f);
		shader.use();
		shader.setVec3("color1", 0.5f, 0.8f, 0.8f);
		shader.setVec3("color2", 0, 0.2f, 0.2f);
		shader.setMat4("model", trans);
		model.Draw(shader);
		};
	return c;
}