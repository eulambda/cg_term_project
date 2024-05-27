#pragma once
#include "../ecs/prelude.hpp"

// enums
enum class DamageType { none, fire };
enum class FacingValue { pos_x, neg_x };

// resources
struct CharacterInput {
	bool operator==(const CharacterInput&) const = default;
	bool left{ 0 }, right{ 0 }, jump{ 0 }, jumping{ 0 };
	bool emit_flame{ 0 };
};
struct Elapsed {
	bool operator==(const Elapsed&) const = default;
	size_t ticks{ 0 };
};
struct Stage {
	bool operator==(const Stage&) const = default;
	double width{ 11.0 };
	double height{ 11.0 };
};
struct Wolf {
	bool operator==(const Wolf&) const = default;
	size_t entity_id{ 0 };
	bool dead{ false };
};
struct Pig {
	bool operator==(const Pig&) const = default;
	bool dead{ false };
};

// components
struct Body {
	bool operator==(const Body&) const = default;
	double w{ 0.0 }, h{ 0.0 };
	double x{ 0.0 }, y{ 0.0 };
	double vx{ 0.0 }, vy{ 0.0 };
	bool is_colliding_x(const Body& body) const;
	bool is_colliding_y(const Body& body) const;
	bool is_colliding(const Body& body) const;
	double x0() const;
	double x1() const;
	double y0() const;
	double y1() const;
	double x_distance_to(const Body& body) const;
	double y_distance_to(const Body& body) const;
	bool is_touching_x(const Body& body) const;
	bool is_touching_y(const Body& body) const;
};
struct Floor {
	bool operator==(const Floor&) const = default;
};
struct Facing {
	bool operator==(const Facing&) const = default;
	FacingValue inner{ FacingValue::pos_x };
};
struct LocomotionWalking {
	bool operator==(const LocomotionWalking&) const = default;
	bool is_touching_floor{ false };
	bool is_touching_wall{ false };
};
struct LocomotionFlying {
	bool operator==(const LocomotionFlying&) const = default;
};
struct HitDamage {
	bool operator==(const HitDamage&) const = default;
	size_t from;
	double power{ 0 };
	DamageType type{ DamageType::none };
};
struct Life {
	bool operator==(const Life&) const = default;
	size_t from{ 0 };
	size_t until{ 0 };
	bool delete_on_death{ false };
	double ratio(double ticks);
};
struct FrozenState {
	bool operator==(const FrozenState&) const = default;
	size_t from{ 0 };
	size_t until{ 0 };
	double ratio(double ticks);
};

// apis
ecs::World create_world();
ecs::SystemForest create_system_forest(ecs::World* world);
