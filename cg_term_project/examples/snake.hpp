#pragma once
#include "../ecs/prelude.hpp"

namespace examples::snake {

	// enums
	enum class DirectionValue {
		left, right, up, down, none
	};

	// resources
	struct Elapsed {
		bool operator==(const Elapsed&) const = default;
		int ticks{ 0 };
	};
	struct Board {
		bool operator==(const Board&) const = default;
		int width{ 11 };
		int height{ 11 };
	};
	struct Snake {
		bool operator==(const Snake&) const = default;
		int x{ 5 };
		int y{ 5 };
		int length{ 1 };
		int highscore{ 0 };
		DirectionValue dir{ DirectionValue::none };
		bool dead{ false };
		bool change_dir(DirectionValue dir_cand);
	};
	struct Cells {
		bool operator==(const Cells&) const = default;
		enum class Type { empty, head, trail, meal };
		std::vector<Type> types;
	};

	// components
	struct SnakeTrailCell {
		bool operator==(const SnakeTrailCell&) const = default;
		int x;
		int y;
		int age{ 0 };
	};
	struct MealCell {
		bool operator==(const MealCell&) const = default;
		int x;
		int y;
	};

	// apis
	ecs::World create_world();
	ecs::SystemForest create_system_forest(ecs::World* world);
}