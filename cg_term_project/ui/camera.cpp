#include "ui.hpp"

void Camera::on_app_started() {
	auto window_config = fetch<WindowConfig>();
	projection = glm::perspective(fov, window_config->aspect_ratio(), distance_near, distance_far);
}
void Camera::on_framebuffer_resized() {
	auto window_config = fetch<WindowConfig>();
	projection = glm::perspective(fov, window_config->aspect_ratio(), distance_near, distance_far);
}
void Camera::on_before_render() {
	auto render_data = fetch<RenderData>();
	auto render_elapsed = render_data->render_elapsed();
	auto simulation_elapsed = render_data->simulation_elapsed();
	auto world = render_data->world;
	auto wolf = world->get_resource<Wolf>();
	auto [wolf_body, wolf_facing] = world->get_entity_with<Body, Facing>(wolf->entity_id);
	auto x = wolf_body->x + wolf_body->vx * simulation_elapsed;
	auto y = wolf_body->y + wolf_body->vy * simulation_elapsed;
	switch (wolf_facing->inner) {
	case FacingValue::pos_x:
		x += 2;
		break;
	case FacingValue::neg_x:
		x -= 2;
		break;
	}
	if (wolf_body->vy >= 0) y += 2;

	auto focus = glm::vec3(x, y, 0);
	auto pan_weight = (float)glm::exp2(-render_elapsed / pan_inertia);
	auto boom_weight = (float)glm::exp2(-render_elapsed / boom_inertia);
	target = glm::vec3(pan_weight) * target + glm::vec3(1 - pan_weight) * focus;
	position = glm::vec3(boom_weight, boom_weight, 1) * position + glm::vec3(1 - boom_weight, 1 - boom_weight, 0) * focus;
}
glm::mat4 Camera::view() {
	return glm::lookAt(position, target, up);
}
void Camera::load() {
	auto json = json::Value::parse("assets/camera.json");
	auto& obj = *json.as_obj();

	fov = glm::radians((float)*obj["fov"].as_num());
	distance_near = (float)*obj["distance_near"].as_num();
	distance_far = (float)*obj["distance_far"].as_num();
	pan_inertia = (float)*obj["pan_inertia"].as_num();
	boom_inertia = (float)*obj["boom_inertia"].as_num();
	position = glm::vec3{ 0,0,*obj["pos_z"].as_num() };
}