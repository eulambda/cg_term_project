#pragma once
#include "../ui.hpp"

UiComponent particles_component() {
	UiComponent c;

	auto scaling_life = fn_from_points({ {0,0},{0.3,1},{0.7,1}, { 1,0 } });
	auto rotating_life = fn_from_points({ {0,2},{0.1,1},{0.9,1}, { 1,2 } });
	auto random_vec = [](double t) {return glm::vec3(sin(t * 512 + 128), sin(t * 256 + 64), sin(t * 128 + 32));};

	c.render = [=] {
		auto render_data = fetch<RenderData>();
		auto curr_time = render_data->curr_time;
		auto render_elapsed = render_data->render_elapsed();
		auto simulation_elapsed = render_data->simulation_elapsed();
		auto world = render_data->world;
		auto ticks = world->get_resource<Elapsed>()->ticks + simulation_elapsed;

		auto& shader = fetch_shader("paper");
		auto& triangle = fetch_model("assets/triangle.dae");
		auto& rectangle = fetch_model("assets/rectangle.dae");
		shader.use();

		for (auto& [id, body, life, particle_domain] : world->get_entities_with<Body, Life, ParticleDomain>()) {
			auto x0 = body->x0() + body->vx * simulation_elapsed;
			auto y0 = body->y0() + body->vy * simulation_elapsed;
			auto life_ratio = life->ratio(ticks + simulation_elapsed);
			auto ticks_rel = ticks + simulation_elapsed - life->from;


			if (particle_domain->type == ParticleType::hay) {
				shader.setVec3("color1", 1, 1, 0.5f);
				shader.setVec3("color2", 0.5f, 1, 0.2f);
			}
			else if (particle_domain->type == ParticleType::wood) {
				shader.setVec3("color1", 1, 0.8f, 0.8f);
				shader.setVec3("color2", 0.2f, 0.2f, 0.2f);
			}
			int density = 3;
			for (int dx = 0;dx < density;dx++) {
				for (int dy = 0;dy < density;dy++) {
					double seed = 0.1;
					auto noise = [&] {
						double t = seed++ + id + 13 * dx + 17 * dy;
						return (sin(t * 1024 + 512) + sin(t * 128 + 256) + sin(t * 32 + 128)) / 3;
						};

					auto x = x0 + body->w * dx / density;
					x += noise() * 0.2;
					x += ticks_rel * noise() * 0.1;
					auto y = y0 + body->h * dy / density;
					y += noise() * 0.2;
					y += ticks_rel * noise() * 0.5;
					y += -ticks_rel * ticks_rel * (1 + noise()) * 0.02;
					auto z = noise();
					z += ticks_rel * noise() * 0.1;

					auto s = scaling_life(life_ratio) * (noise() * 0.2 + 0.8);
					s *= (min(body->w * body->h, 4.0) * 0.3 + 0.7);
					s *= 0.5;
					auto r = rotating_life(life_ratio) * 180 + curr_time * 5;
					auto trans = glm::translate(glm::mat4{ 1 }, glm::vec3(x, y, z));
					trans = glm::scale(trans, glm::vec3(s, s, s));
					trans = glm::rotate(trans, (float)r, glm::vec3(0, 0, 1));
					if (noise() > 0) {
						trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(1, 0, 0));
					}
					shader.setMat4("model", trans);
					if (particle_domain->type == ParticleType::hay) {
						triangle.Draw(shader);
					}
					else if (particle_domain->type == ParticleType::wood) {
						rectangle.Draw(shader);
					}
				}
			}

		}
		};
	return c;
}