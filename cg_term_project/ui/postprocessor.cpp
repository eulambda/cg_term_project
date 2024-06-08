#pragma once
#include "ui.hpp"

void Postprocessor::load() {
	setup_framebuffer();
	// initialize render data and uniforms
	auto& shader = fetch_shader("postprocessor");
	shader.initShader("assets/postprocessor.vs", "assets/postprocessor.fs");
	shader.setInt("scene", 0);
	// configure VAO/VBO
	unsigned int VBO;
	float vertices[] = {
		// pos        // tex
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void Postprocessor::on_before_render() {
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
}

void Postprocessor::on_after_render() {
	// now resolve multisampled color-buffer into intermediate FBO to store to texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glClear(GL_COLOR_BUFFER_BIT);

	// set uniforms/options
	auto& shader = fetch_shader("postprocessor");
	auto render_data = fetch<RenderData>();
	auto simulation_elapsed = render_data->simulation_elapsed();
	auto stage = render_data->world->get_resource<Stage>();
	shader.use();
	shader.setFloat("transition", (float)stage->transition_lerped(simulation_elapsed));
	// render textured quad
	glActiveTexture(GL_TEXTURE0);
	this->texture.Bind();
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	render_screen_texts();
	glBindVertexArray(0);
}

void Postprocessor::on_framebuffer_resized() {
	glDeleteFramebuffers(1, &this->FBO);
	glDeleteRenderbuffers(1, &this->RBO);
	glDeleteRenderbuffers(1, &RBO);
	setup_framebuffer();
}

void Postprocessor::setup_framebuffer() {
	auto config = fetch<WindowConfig>();
	// initialize renderbuffer/framebuffer object
	glGenFramebuffers(1, &this->FBO);
	glGenRenderbuffers(1, &this->RBO);
	// initialize the FBO/texture to blit multisampled color-buffer to; used for shader operations (for postprocessing effects)
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	this->texture.Generate(config->width, config->height, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture.ID, 0); // attach texture to framebuffer as its color attachment
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, config->width, config->height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	auto lighting = fetch<Lighting>();
	glClearColor(lighting->environment.r, lighting->environment.g, lighting->environment.b, 1.0f);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
