#include "ui.hpp"

void on_keyboard_action(GLFWwindow* window, int key, int scancode, int action, int mods) {
	auto debug = fetch<UiDebugInfo>();
	auto get_input = [] {
		return fetch<RenderData>()->world->get_resource<CharacterInput>();
		};
	auto goto_next_stage = [] {
		auto& world = fetch<RenderData>()->world;
		auto portals = world->get_entities_with<Portal>();
		if (portals.empty()) return;
		auto& [_, portal] = *portals.begin();
		world->get_resource<Stage>()->to_load = portal->to_load;
		};
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (key == GLFW_KEY_LEFT) {
		if (action == GLFW_PRESS) get_input()->left = true;
		if (action == GLFW_RELEASE)	get_input()->left = false;
	}
	if (key == GLFW_KEY_RIGHT) {
		if (action == GLFW_PRESS) get_input()->right = true;
		if (action == GLFW_RELEASE)	get_input()->right = false;
	}
	if (key == GLFW_KEY_Z) {
		if (action == GLFW_PRESS) get_input()->jump = true;
		if (action == GLFW_PRESS) get_input()->jumping = true;
		if (action == GLFW_RELEASE) get_input()->jumping = false;
	}
	if (key == GLFW_KEY_X) {
		if (action == GLFW_PRESS) get_input()->emit_flame = true;
		if (action == GLFW_RELEASE) get_input()->emit_flame = false;
	}
	if (key == GLFW_KEY_C) {
		if (action == GLFW_PRESS) get_input()->charge_roar = true;
		if (action == GLFW_RELEASE) get_input()->charge_roar = false;
	}
	if (debug->enable_stage_navigation && key == GLFW_KEY_1) {
		if (action == GLFW_PRESS) goto_next_stage();
	}
	return;
}
