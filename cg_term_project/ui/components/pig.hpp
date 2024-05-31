#pragma once
#include "../ui.hpp"

UiComponent pig_component() {
	auto rotating_vy = [](double t) {return glm::radians(-15.0) * std::clamp(t, -1.0, 1.0); };
	UiComponent c;

	c.on_app_started = [] {
		auto& shaking = fetch_curve("pig.shaking");
		shaking.y_x = [](double x) { return 0.1 * std::sin(x * 20); };
		auto& flipping = fetch_curve("pig.flipping");
		flipping.x_t = [](double t) {return std::clamp(t, 0.0, 0.1); };
		flipping.y_x = [](double x) {return x * glm::radians(180.0) / 0.1; };
		};

	c.render = [=] {
		auto render_data = fetch<RenderData>();
		auto render_elapsed = render_data->render_elapsed();
		auto simulation_elapsed = render_data->simulation_elapsed();
		auto world = render_data->world;
		auto pig = world->get_resource<Pig>();
		auto ticks = world->get_resource<Elapsed>()->ticks + simulation_elapsed;

		auto [body, facing, health] = world->get_entity_with<Body, Facing, Health>(pig->entity_id);

		fetch_curve("pig.shaking").t += (body->vx + body->vy) * render_elapsed;
		switch (facing->inner) {
		case FacingValue::pos_x:
			fetch_curve("pig.flipping").t += render_elapsed;
			break;
		case FacingValue::neg_x:
			fetch_curve("pig.flipping").t += -render_elapsed;
			break;
		}

		auto& model = fetch_model("assets/wolf.dae");
		auto& shader = fetch_shader("paper");

		auto pig_x = body->x + simulation_elapsed * body->vx;
		auto pig_y = body->y0() + simulation_elapsed * body->vy;
		auto shaking = fetch_curve("pig.shaking").eval();
		auto flipping = fetch_curve("pig.flipping").eval();

		auto trans = glm::translate(glm::mat4{ 1 }, glm::vec3(pig_x + shaking, pig_y, 0));
		trans = glm::rotate(trans, (float)flipping, glm::vec3(0, 1, 0));
		auto r = shaking + rotating_vy(body->vy);
		trans = glm::rotate(trans, (float)r, glm::vec3(0, 0, 1));

		auto body_color = glm::vec3(0, 0.2f, 0.2f);
		shader.use();
		shader.setVec3("objectColor", body_color);
		shader.setMat4("model", trans);
		model.Draw(shader);

		auto health_ratio = (std::max(health->current + health->receiving * simulation_elapsed, 0.0)) / health->max;
		if (health_ratio) {
			auto& bar = fetch_model("assets/rectangle.dae");
			auto trans_bar = glm::translate(glm::mat4{ 1 }, glm::vec3(pig_x, pig_y + body->h + 1, 0));
			trans_bar = glm::scale(trans_bar, glm::vec3(2 * health_ratio, 0.1, 0.1));
			shader.use();
			shader.setMat4("model", trans_bar);
			shader.setVec3("objectColor", 1,0,0);
			bar.Draw(shader);
		}
		};
	return c;
}