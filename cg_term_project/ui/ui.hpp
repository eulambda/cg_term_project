#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLT_IMPLEMENTATION
#include "../gltext.h"

#include <shader.h>
#include <texture.h>

#include <vector>
#include "../ecs/prelude.hpp"
#include "../entities/prelude.hpp"
#include "../essentials/curve.hpp"
#include "ui_component.hpp"
#include "data_loader.hpp"
#include "camera.hpp"
#include "lighting.hpp"
#include "text.hpp"

struct WindowConfig {
	int width{ 1080 };
	int height{ 720 };
	char const* title{ "The three little pigs" };
	GLFWwindow* glfw_window;
	float aspect_ratio();
};

struct Postprocessor {
	GLuint FBO;
	GLuint RBO;
	GLuint VAO;
	Texture2D texture;
	void load();
	void on_before_render();
	void on_after_render();
	void on_framebuffer_resized();
	void setup_framebuffer();
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

	// text
	std::vector<std::function<void(Text&)>> queued_texts;
	void render_text(std::function<void(Text&)>);

	void load();
};
struct RenderConfig {
	double seconds_per_tick{ 1 / 24.0 };
};
struct UiDebugInfo {
	bool is_enabled{ false };
	bool show_coords{ false };
	bool show_hitbox{ false };
	bool enable_stage_navigation{ false };

	void load();
};
void include_components();
void launch();

void on_keyboard_action(GLFWwindow* window, int key, int scancode, int action, int mods);
void on_mouse_button(GLFWwindow* window, int button, int action, int mods);
void on_mouse_moved(GLFWwindow* window, double x, double y);
void on_framebuffer_resized(GLFWwindow* window, int width, int height);
unsigned int load_texture(const char* path);