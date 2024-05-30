#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <cube.h>

#include <vector>
#include "../ecs/prelude.hpp"
#include "../entities/prelude.hpp"
#include "../essentials/curve.hpp"
#include "ui_component.hpp"
#include "data_loader.hpp"
#include "camera.hpp"

struct WindowConfig {
	int width{ 720 };
	int height{ 480 };
	char const* title{ "term project" };
	GLFWwindow* glfw_window;
	float aspect_ratio();
};
struct Lighting {
	glm::vec3 position{ 1,2,2 };
	glm::vec3 color{ 1,1,1 };
	glm::vec3 environment{ 0,0.1f,0.2f };
};
struct RenderData {
	// components
	std::vector<UiComponent> components;
	Camera camera;

	// ECS data
	ecs::World* world;
	ecs::SystemForest* system_forest;

	// elapsed
	double curr_time{ -1 };
	double last_rendered_time{ -1 };
	double last_simulated_time{ -1 };
	double render_elapsed();
	double simulation_elapsed();

	void load();
};
struct RenderConfig {
	double seconds_per_tick{ 1 / 24.0 };
};
void include_components();
void initialize_window();
void on_app_started();
void on_app_exiting();
void on_before_render();
void render();
void launch();

void on_keyboard_action(GLFWwindow* window, int key, int scancode, int action, int mods);
void on_mouse_button(GLFWwindow* window, int button, int action, int mods);
void on_mouse_moved(GLFWwindow* window, double x, double y);
void on_framebuffer_resized(GLFWwindow* window, int width, int height);
