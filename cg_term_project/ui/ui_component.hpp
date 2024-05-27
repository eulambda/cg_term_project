#pragma once
#include <string>
#include <functional>

struct UiComponent {
	std::string type;
	std::function<void(void)> on_app_exiting{ [] {} };
	std::function<void(void)> on_app_started{ [] {} };
	std::function<void(void)> on_before_render{ [] {} };
	std::function<void(void)> render{ [] {} };
};

