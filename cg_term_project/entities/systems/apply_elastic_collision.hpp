#pragma once
#include "../prelude.hpp"

std::tuple<double, double> solve_elastic_collision(double m0, double m1, double v0, double v1) {
	double u0 = ((m0 - m1) * v0 + 2 * m1 * v1) / (m0 + m1);
	double u1 = ((m1 - m0) * v1 + 2 * m0 * v0) / (m0 + m1);
	return std::make_tuple(u0, u1);
}
void apply_elastic_collision(ecs::EntitiesWithWritable<Body, Mass, LocomotionWalking> walking_characters) {
	for (auto& [id0, body0, mass0, locomotion0] : walking_characters) {
		for (auto& [id1, body1, mass1, locomition1] : walking_characters) {
			if (id0 >= id1) continue;
			if (!body0->is_colliding(*body1)) return;
			if (body0->is_colliding_y(*body1) && body1->x_distance_to(*body0) != 0) {
				auto [uy0, uy1] = solve_elastic_collision(mass0->val, mass1->val, body0->vy, body1->vy);
				body0->vy = uy0;
				body1->vy = uy1;
				if (body0->y > body1->y) body0->vy = 4;
				else body1->vy = 4;
			}
			else {
				auto [ux0, ux1] = solve_elastic_collision(mass0->val, mass1->val, body0->vx, body1->vx);
				body0->vx = ux0;
				body1->vx = ux1;

			}
		}
	}
}