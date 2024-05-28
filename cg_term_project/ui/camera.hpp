#pragma once
#include <glm/glm.hpp>

struct Camera {
	glm::vec3 position{ 0,0,20 };
	glm::vec3 target{ 0,0,0 };
	glm::vec3 up{ 0,1,0 };
	glm::mat4 projection{ 1 };
	float fov{ glm::radians(45.0f) };
	float distance_near{ 0.1f };
	float distance_far{ 100.0f };
	float pan_inertia{0.05f };
	float boom_inertia{ 0.08f };
	glm::mat4 view();
	void on_framebuffer_resized();
	void on_app_started();
	void on_before_render();
};
