#pragma once
#include "../ecs/prelude.hpp"

// enums
enum class DamageType { normal, fire, wind };
enum class FacingValue { pos_x, neg_x };

// resources
struct CharacterInput {
	bool operator==(const CharacterInput&) const = default;
	bool left{ 0 }, right{ 0 }, jump{ 0 }, jumping{ 0 };
	bool emit_flame{ 0 };
	bool charge_roar{ 0 };
};
struct Elapsed {
	bool operator==(const Elapsed&) const = default;
	size_t ticks{ 0 };
};
struct Wolf {
	bool operator==(const Wolf&) const = default;
	size_t entity_id;
};
struct Pig {
	bool operator==(const Pig&) const = default;
	size_t entity_id;
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
	int sign_x() const;
};
struct LocomotionWalking {
	bool operator==(const LocomotionWalking&) const = default;
	bool is_touching_floor{ false };
	bool is_touching_wall{ false };
};
struct LocomotionFlying {
	bool operator==(const LocomotionFlying&) const = default;
};
struct Mass {
	bool operator==(const Mass&) const = default;
	double val;
};
struct HitDamage {
	bool operator==(const HitDamage&) const = default;
	size_t from;
	double power{ 0 };
	double knockback{ 0 };
	DamageType type{ DamageType::normal };
};
struct Health {
	bool operator==(const Health&) const = default;
	int max{ 100 };
	int current{ 100 };
	int receiving{ 0 };
};
struct DamageReceiver {
	bool operator==(const DamageReceiver&) const = default;
	int multiplier{ 1 };
	int multiplier_fire{ 1 };
	int multiplier_wind{ 1 };
	int multiplier_knockback{ 1 };
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
struct RoarCharged {
	bool operator==(const RoarCharged&) const = default;
	size_t val{ 0 };
	size_t max{ 5 };
	double ratio() const;
	DamageType type{ DamageType::normal };
	bool is_charging{ false };
};

// apis
ecs::World create_world();
ecs::SystemForest create_system_forest(ecs::World* world);
