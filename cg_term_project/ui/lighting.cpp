#include "../essentials/json_parser.hpp"
#include "lighting.hpp"

void Lighting::load() {
	auto json = json::Value::parse("assets/lighting.json");
	auto& obj = *json.as_obj();

	auto& env = *obj["environment"].as_arr();
	environment = glm::vec3(*env[0].as_num(), *env[1].as_num(), *env[2].as_num());
	camera_lights.clear();
	for (auto& camera_light_json : *obj["camera_lights"].as_arr()) {
		auto& camera_light = *camera_light_json.as_obj();

		auto& color = *camera_light["color"].as_arr();
		auto& offset = *camera_light["offset"].as_arr();
		camera_lights.push_back(CameraLight{
			.intensity = (float)*camera_light["intensity"].as_num(),
			.color = glm::vec3(*color[0].as_num(),*color[1].as_num(),*color[2].as_num()),
			.offset = glm::vec3(*offset[0].as_num(),*offset[1].as_num(),*offset[2].as_num()),
			}
		);
	}
}
