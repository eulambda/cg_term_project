#include <iostream>
#include "ui.hpp"
#include "../essentials/split.hpp"

void render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto render_data = fetch<RenderData>();

	for (auto& component : render_data->components) {
		component.render();
	}

	static GLTtext* glt_text = gltCreateText();
	Text text{ glt_text };
	auto view = render_data->camera.view();
	auto& projection = render_data->camera.projection;
	gltBeginDraw();
	for (auto& queued_text : render_data->queued_texts) {
		queued_text(text);
		gltColor(text.r, text.g, text.b, text.a);
		gltDrawText3D(glt_text, text.x, text.y, text.z, text.scale, glm::value_ptr(view), glm::value_ptr(projection));
	}
	render_data->queued_texts.clear();
	gltEndDraw();
}

void on_before_render() {
	auto render_data = fetch<RenderData>();
	auto postprocessor = fetch<Postprocessor>();
	postprocessor->on_before_render();
	render_data->camera.on_before_render();

	for (auto& component : render_data->components) {
		component.on_before_render();
	}
	return;
}
void on_after_render() {
	auto postprocessor = fetch<Postprocessor>();
	postprocessor->on_after_render();
	return;
}

void on_app_started() {
	auto render_data = fetch<RenderData>();
	render_data->camera.on_app_started();

	for (auto& component : render_data->components) {
		component.on_app_started();
	}
}

void on_app_exiting() {
	auto render_data = fetch<RenderData>();

	for (auto& component : render_data->components) {
		component.on_app_exiting();
	}
	return;
}

void on_mouse_button(GLFWwindow* window, int button, int action, int mods) {
	return;
}

void on_mouse_moved(GLFWwindow* window, double x, double y) {
	return;
}

void load_icon(GLFWwindow* window) {
	GLFWimage images[1];
	images[0].pixels = stbi_load("assets/icon.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
	glfwSetWindowIcon(window, 1, images);
	stbi_image_free(images[0].pixels);
}
bool initialize_window() {
	// glfw: initialize and configure
	if (!glfwInit()) {
		return false;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// glfw window creation
	auto config = fetch<WindowConfig>();
	auto window = glfwCreateWindow(config->width, config->height, config->title, NULL, NULL);
	if (window == nullptr) {
		return false;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return false;
	}
	glfwSetFramebufferSizeCallback(window, on_framebuffer_resized);
	glfwSetKeyCallback(window, on_keyboard_action);
	glfwSetMouseButtonCallback(window, on_mouse_button);
	glfwSetCursorPosCallback(window, on_mouse_moved);
	load_icon(window);

	// glt
	if (!gltInit()) {
		return false;
	}

	// OpenGL states
	auto lighting = fetch<Lighting>();
	glClearColor(lighting->environment.r, lighting->environment.g, lighting->environment.b, 1.0f);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	config->glfw_window = window;
	return true;
}
void cleanup() {
	auto window_config = fetch<WindowConfig>();

	gltTerminate();
	glfwDestroyWindow(window_config->glfw_window);
	window_config->glfw_window = nullptr;
	glfwTerminate();
}

void launch() {
	if (!initialize_window()) return;
	auto window_config = fetch<WindowConfig>();
	if (!window_config->glfw_window) return;

	auto world = create_world();
	auto system_forest = create_system_forest(&world);
	auto render_config = fetch<RenderConfig>();
	auto render_data = fetch<RenderData>();
	render_data->world = &world;
	render_data->system_forest = &system_forest;
	render_data->world->get_resource<SimulationSpeed>()->seconds_per_tick = render_config->seconds_per_tick;

	include_components();
	on_app_started();
	int frames_before_tick = 0;
	render_data->curr_time = glfwGetTime();
	render_data->last_rendered_time = render_data->curr_time;
	render_data->last_simulated_time = render_data->curr_time;
	double next_update = render_data->curr_time + render_config->seconds_per_tick;
	while (!glfwWindowShouldClose(window_config->glfw_window)) {
		render_data->last_rendered_time = render_data->curr_time;
		render_data->curr_time = glfwGetTime();
		on_before_render();
		render();
		on_after_render();
		glfwSwapBuffers(window_config->glfw_window);
		if (next_update <= render_data->curr_time) {
			system_forest.run();
			next_update += render_config->seconds_per_tick;
			render_data->last_simulated_time = render_data->curr_time;
		}
		glfwPollEvents();
	}
	on_app_exiting();
	cleanup();
}

void on_framebuffer_resized(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	auto config = fetch<WindowConfig>();
	config->width = width;
	config->height = height;

	fetch<RenderData>()->camera.on_framebuffer_resized();
	fetch<Postprocessor>()->on_framebuffer_resized();
	return;
}
float WindowConfig::aspect_ratio() {
	return (float)width / (float)height;
}
double RenderData::render_elapsed() {
	return curr_time - last_rendered_time;
}
double RenderData::simulation_elapsed() {
	auto seconds_per_tick = fetch<RenderConfig>()->seconds_per_tick;
	return std::clamp((curr_time - last_simulated_time) / seconds_per_tick, 0.0, 1.0);
}

void RenderData::render_text(std::function<void(Text&)> fn) {
	queued_texts.push_back(fn);
}

void RenderData::load() {
	camera.load();
}

void UiDebugInfo::load() {
	auto debug_v = json::Value::parse("assets/debug.json");
	if (!debug_v.is_valid()) return;
	auto& debug = *debug_v.as_obj();
	std::set<std::string> features{};
	for (auto& f : *debug["features"].as_arr()) {
		features.insert(*f.as_str());
	}
	is_enabled = features.size() > 0;
	show_hitbox = features.end() != features.find("show_hitbox");
	enable_stage_navigation = features.end() != features.find("enable_stage_navigation");
	show_coords = features.end() != features.find("show_coords");
}

unsigned int load_texture(const char* path) {
	unsigned int texture;

	// Create texture ids.
	glGenTextures(1, &texture);

	// All upcomming GL_TEXTURE_2D operations now on "texture" object
	glBindTexture(GL_TEXTURE_2D, texture);

	// Set texture parameters for wrapping.
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture parameters for filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);   // vertical flip the texture
	unsigned char* image = stbi_load(path, &width, &height, &nrChannels, 0);
	if (!image) {
		printf("texture %s loading error ... \n", path);
	}

	GLenum format;
	if (nrChannels == 1) format = GL_RED;
	else if (nrChannels == 3) format = GL_RGB;
	else if (nrChannels == 4) format = GL_RGBA;

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	return texture;
}

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
	static GLTtext* glt_text = gltCreateText();
	if (stage->screen_texts.size() > 0) {
		auto config = fetch<WindowConfig>();
		auto scale = config->width / 500.0f;
		auto x = config->width / 2.0f;
		auto y = config->height / 2.0f;
		gltBeginDraw();
		gltColor(1, 1, 1, 1);
		for (auto& line : split(stage->screen_texts.front())) {
			gltSetText(glt_text, line.c_str());
			gltDrawText2DAligned(glt_text, x, y, scale, GLT_CENTER, GLT_BOTTOM);
			y += gltGetLineHeight(scale);
		}
		if ((int)(render_data->curr_time * 1.5) % 2 == 0) {
			gltSetText(glt_text, "-");
			gltDrawText2DAligned(glt_text, x, y, scale, GLT_CENTER, GLT_TOP);
		}
		gltEndDraw();
	}
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
