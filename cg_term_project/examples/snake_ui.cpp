#include <chrono>
#include <thread>
#include <iostream>
#include "snake_ui.hpp"
#include "snake.hpp"

namespace examples::snake {

	void render() {
		auto render_data = fetch<RenderData>();
		auto world = render_data->world;
		auto board = world->get_resource<Board>();
		auto cells = world->get_resource<Cells>();

		size_t board_size = board->width * board->height;
		if (cells->types.size() < board_size) return;

		auto& cube_shader = render_data->cube_shader;
		auto& cube = render_data->cube;
		cube_shader.use();

		auto get_color_for = [&](Cells::Type type) {
			switch (type) {
			case Cells::Type::head:
				return render_data->cube_color_head;
			case Cells::Type::trail:
				return render_data->cube_color_trail;
			case Cells::Type::meal:
				return render_data->cube_color_meal;
			default:
				return glm::vec3{};
			}
			};
		for (int y = 0; y < board->height; y++) {
			for (int x = 0; x < board->width; x++) {
				auto type = cells->types[y * board->width + x];
				if (type == Cells::Type::empty) continue;
				glm::mat4 model{ 1 };
				model = glm::translate(model, render_data->into_3d_coords(x, y));
				model = glm::scale(model, glm::vec3{ (float)render_data->cube_scale });
				cube_shader.setMat4("model", model);
				cube_shader.setVec3("objectColor", get_color_for(type));
				cube.draw(&cube_shader);
			}
		}
	}

	void on_before_render() {
		auto camera = fetch<Camera>();
		auto render_data = fetch<RenderData>();
		auto window_config = fetch<WindowConfig>();
		auto& cube_shader = render_data->cube_shader;
		auto elapsed = render_data->elapsed();
		auto world = render_data->world;
		auto snake = world->get_resource<Snake>();

		auto snake_coords = render_data->into_3d_coords(snake->x, snake->y);
		auto pan_weight = (float)glm::exp2(-camera->pan_inertia * elapsed);
		auto boom_weight = (float)glm::exp2(-camera->boom_inertia * elapsed);
		camera->target = glm::vec3(1 - pan_weight) * camera->target + glm::vec3(pan_weight) * snake_coords;
		camera->position = glm::vec3(1 - boom_weight, 1 - boom_weight, 1) * camera->position + glm::vec3(boom_weight, boom_weight, 0) * snake_coords;

		cube_shader.use();

		auto projection = glm::perspective(camera->fov, window_config->aspect_ratio(), camera->distance_near, camera->distance_far);
		cube_shader.setMat4("view", camera->view());
		cube_shader.setVec3("viewPos", camera->position);
		cube_shader.setMat4("projection", projection);

		return;
	}

	void on_app_started() {
		auto window_config = fetch<WindowConfig>();
		auto render_data = fetch<RenderData>();

		auto& cube_shader = render_data->cube_shader;
		cube_shader.use();

		auto& cube_material = render_data->cube_material;
		cube_shader.setFloat("ambientStrength", (float)cube_material.ambient_strength);
		cube_shader.setFloat("specularStrength", (float)cube_material.specular_strength);
		cube_shader.setFloat("specularPower", (float)cube_material.specular_power);

		auto lighting = fetch<Lighting>();
		cube_shader.setVec3("lightColor", lighting->color);
		cube_shader.setVec3("lightPos", lighting->position);
	}

	void on_app_exiting() {
		return;
	}

	void on_keyboard_action(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
		return;
	}

	void on_mouse_button(GLFWwindow* window, int button, int action, int mods) {
		return;
	}

	void on_mouse_moved(GLFWwindow* window, double x, double y) {
		return;
	}

	void initialize_window() {
		// glfw: initialize and configure
		if (!glfwInit()) {
			return;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		// glfw window creation
		auto config = fetch<WindowConfig>();
		auto window = glfwCreateWindow(config->width, config->height, config->title, NULL, NULL);
		if (window == nullptr) {
			return;
		}
		glfwMakeContextCurrent(window);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			return;
		}
		glfwSetFramebufferSizeCallback(window, on_framebuffer_resized);
		glfwSetKeyCallback(window, on_keyboard_action);
		glfwSetMouseButtonCallback(window, on_mouse_button);
		glfwSetCursorPosCallback(window, on_mouse_moved);

		// OpenGL states
		auto lighting = fetch<Lighting>();
		glClearColor(lighting->environment.r, lighting->environment.g, lighting->environment.b, 1.0f);
		glEnable(GL_DEPTH_TEST);

		config->glfw_window = window;
		return;
	}



	void launch() {
		initialize_window();
		auto window_config = fetch<WindowConfig>();
		if (!window_config->glfw_window) return;

		auto world = examples::snake::create_world();
		auto system_forest = examples::snake::create_system_forest(&world);
		auto render_config = fetch<RenderConfig>();
		auto render_data = fetch<RenderData>();
		render_data->world = &world;
		render_data->system_forest = &system_forest;

		on_app_started();
		int frames_before_tick = 0;
		render_data->prev_time = render_data->curr_time = glfwGetTime();
		double next_update = render_data->curr_time + render_config->seconds_per_tick;
		while (!glfwWindowShouldClose(window_config->glfw_window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			render_data->prev_time = render_data->curr_time;
			render_data->curr_time = glfwGetTime();
			on_before_render();
			render();
			glfwSwapBuffers(window_config->glfw_window);
			if (next_update <= render_data->curr_time) {
				system_forest.run();
				next_update += render_config->seconds_per_tick;
			}
			glfwPollEvents();
		}
		on_app_exiting();
		glfwDestroyWindow(window_config->glfw_window);
		window_config->glfw_window = nullptr;
		glfwTerminate();
	}

	void on_framebuffer_resized(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
		auto config = fetch<WindowConfig>();
		config->width = width;
		config->height = height;
		return;
	}
	double WindowConfig::aspect_ratio() {
		return (double)width / (double)height;
	}
	glm::mat4 Camera::view() {
		return glm::lookAt(position, target, up);
	}
	glm::vec3 RenderData::into_3d_coords(int x, int y)
	{
		auto render_data = fetch<RenderData>();
		auto board = render_data->world->get_resource<Board>();
		auto delta = render_data->cube_scale + render_data->cube_margin;
		auto x1 = (double)(x - board->width / 2) * delta;
		auto y1 = (double)(y - board->height / 2) * delta;
		return glm::vec3{ x1,y1,0 };
	}
	double RenderData::elapsed()
	{
		return curr_time - prev_time;
	}
}
