#pragma once
#include <glm/glm.hpp>
#include <vector>
struct CameraLight {
	float intensity;
	glm::vec3 color;
	glm::vec3 offset;
};
struct Lighting {
	std::vector<CameraLight> camera_lights;
	glm::vec3 environment{ 0,0.1f,0.2f };
	void load();
};