#pragma once
#include <glm/glm.hpp>
#include "../essentials/json_parser.hpp"
struct Camera {
	float fov;
	float distance_near;
	float distance_far;
	float pan_inertia;
	float boom_inertia;
	glm::vec3 position;
	glm::vec3 target{ 0,0,0 };
	glm::vec3 up{ 0,1,0 };
	glm::mat4 projection{ 1 };
	glm::mat4 view();
	void on_framebuffer_resized();
	void on_app_started();
	void on_before_render();
	void load();
};
