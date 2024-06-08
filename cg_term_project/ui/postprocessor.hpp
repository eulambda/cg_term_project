#pragma once
#include <texture.h>

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
	void render_screen_texts();
};