#pragma once

#include <irrklang/irrKlang.h>
#include <string>
struct SoundEngine {
	irrklang::ISoundEngine* engine{ nullptr };
	std::string bgm_file{};
	float current_volume{ 0 };
	float target_volume{ 0 };
	float max_volume{ 0.1f };
	void load();
	void on_before_render();
};