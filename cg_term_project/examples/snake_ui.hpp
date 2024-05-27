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

namespace examples::snake {
	struct Camera {
		glm::vec3 position{ 0,0,7 };
		glm::vec3 target{ 0,0,0 };
		glm::vec3 up{ 0,1,0 };
		double fov{ glm::radians(45.0f) };
		double distance_near{ 0.1f };
		double distance_far{ 100.0f };
		double pan_inertia{ 200.0f };
		double boom_inertia{ 400.0f };
		glm::mat4 view();
	};
	struct WindowConfig {
		int width{ 800 };
		int height{ 800 };
		char const* title{ "term project" };
		GLFWwindow* glfw_window;
		double aspect_ratio();
	};
	struct Material {
		double ambient_strength;
		double specular_strength;
		double specular_power;
	};
	struct Lighting {
		glm::vec3 position{ 1,2,2 };
		glm::vec3 color{ 1,1,1 };
		glm::vec3 environment{ 0,0.1f,0.2f };
	};
	struct RenderData {

		// cube settings
		Cube cube;
		Shader cube_shader{ "assets/cube.vs", "assets/cube.fs" };
		double cube_scale{ 0.25f };
		double cube_margin{ 0.1f };
		glm::vec3 into_3d_coords(int x, int y);
		Material cube_material{ .ambient_strength{0.1f},.specular_strength{0.5f},.specular_power{64.0f} };
		glm::vec3 cube_color_head{ 1,0.1,0.1 };
		glm::vec3 cube_color_trail{ 0.8,0.8,0.1 };
		glm::vec3 cube_color_meal{ 0.8,1,1 };

		// ECS data
		ecs::World* world;
		ecs::SystemForest* system_forest;

		// elapsed
		double curr_time{ -1 };
		double prev_time{ -1 };
		double elapsed();
	};
	struct RenderConfig {
		double seconds_per_tick{ 1/4.0 };
	};
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

	template<typename T>
	T* fetch() {
		static T* ptr = nullptr;
		if (!ptr) ptr = new T{};
		return ptr;
	}
}