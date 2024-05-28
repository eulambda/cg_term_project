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

		auto [body, facing] = world->get_entity_with<Body, Facing>(pig->entity_id);

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

		auto wolf_x = body->x + simulation_elapsed * body->vx;
		auto wolf_y = body->y0() + simulation_elapsed * body->vy - 0.5;
		auto shaking = fetch_curve("pig.shaking").eval();
		auto flipping = fetch_curve("pig.flipping").eval();
		auto frozen = world->get_component<FrozenState>(pig->entity_id)->ratio(ticks);

		auto trans = glm::translate(glm::mat4{ 1 }, glm::vec3(wolf_x + shaking, wolf_y, 0));
		trans = glm::rotate(trans, (float)flipping, glm::vec3(0, 1, 0));
		auto r = shaking + rotating_vy(body->vy);
		trans = glm::rotate(trans, (float)r, glm::vec3(0, 0, 1));

		shader.setVec3("objectColor", 1, 0.2f, 0.2f);
		shader.setMat4("model", trans);
		model.Draw(shader);
		};
	return c;
}