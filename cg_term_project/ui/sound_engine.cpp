#include "ui.hpp"
#include "sound_engine.hpp"
#include "../essentials/json_parser.hpp"
void SoundEngine::load() {
	engine = irrklang::createIrrKlangDevice();
	if (!engine) {
		return;
	}
	auto json = json::Value::parse("assets/sound.json");
	if (!json.as_obj()) {
		return;
	}
	auto& obj = *json.as_obj();

	bgm_file = obj["bgm"].str_or("");
	max_volume = (float)obj["volume"].num_or(0.05);

	engine->play2D(bgm_file.c_str(), true);
	engine->setSoundVolume(current_volume);
}

void SoundEngine::on_before_render() {
	auto render_data = fetch<RenderData>();
	auto render_elapsed = render_data->render_elapsed();
	auto weight = (float)glm::exp2(-render_elapsed / 5);

	auto stage = render_data->world->get_resource<Stage>();
	if (stage->screen_texts.size()) {
		target_volume = max_volume / 4;
	}
	else {
		target_volume = max_volume;
	}
	current_volume = current_volume * weight + target_volume * (1- weight);
	engine->setSoundVolume(current_volume);
}
