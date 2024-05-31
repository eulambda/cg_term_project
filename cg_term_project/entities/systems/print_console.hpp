#pragma once
#include "../prelude.hpp"

void print_console(ecs::EntitiesWith<Body, LocomotionWalking> characters) {
	for (auto& [_, character, locomotion] : characters) {
		//std::cout << character->vy << ", " << character->y << ", " << locomotion->is_flying << std::endl;
	}
}