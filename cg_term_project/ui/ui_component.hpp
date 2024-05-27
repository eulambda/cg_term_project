#pragma once
#include <string>
#include <GLFW/glfw3.h>

struct UiComponent {
	std::function<void(void)> on_app_exiting{ [] {} };
	std::function<void(void)> on_app_started{ [] {} };
	std::function<void(void)> on_before_render{ [] {} };
	std::function<void(void)> render{ [] {} };
};

