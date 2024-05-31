#include "prelude.hpp"

ecs::World create_world() {
	auto world = ecs::World();

	world.register_component<Body>();
	world.register_component<Floor>();
	world.register_component<Facing>();
	world.register_component<LocomotionWalking>();
	world.register_component<LocomotionFlying>();
	world.register_component<HitDamage>();
	world.register_component<Life>();
	world.register_component<FrozenState>();
	world.register_component<Mass>();
	world.register_component<Health>();
	world.register_component<DamageReceiver>();
	world.register_component<BreathCharged>();

	world.spawn_entity().with(Floor{}).with(Body{ .w = 1,.h = 50,.x = -17,.y = 0 });
	world.spawn_entity().with(Floor{}).with(Body{ .w = 1,.h = 50,.x = 17,.y = 0 });
	world.spawn_entity().with(Floor{}).with(Body{ .w = 32,.h = 1,.x = 0,.y = -1 });
	world.spawn_entity().with(Floor{}).with(Body{ .w = 2,.h = 1,.x = -16,.y = 0 });
	world.spawn_entity().with(Floor{}).with(Body{ .w = 2,.h = 1,.x = 16,.y = 0 });
	world.spawn_entity().with(Floor{}).with(Body{ .w = 2,.h = 1,.x = 4,.y = 6 });

	auto wolf = world.spawn_entity();
	auto wolf_id = wolf.id();
	wolf.with(LocomotionWalking{})
		.with(Body{ .w = 2,.h = 2, .x = 2,.y = 4 })
		.with(Facing{ .inner = FacingValue::pos_x })
		.with(FrozenState{})
		.with(Mass{ 1 })
		.with(Health{})
		.with(DamageReceiver{})
		.with(HitDamage{ .from = wolf_id,.power = 1, .knockback = 0,.type = DamageType::normal })
		.with(BreathCharged{ .val = 0, .max=5, .type = DamageType::wind })
		.id();

	auto pig_id = world.spawn_entity()
		.with(LocomotionWalking{})
		.with(Body{ .w = 2,.h = 2, .x = 8,.y = 4 })
		.with(Facing{ .inner = FacingValue::pos_x })
		.with(Mass{ 2 })
		.with(Health{})
		.with(DamageReceiver{ .multiplier = 1,.multiplier_fire = 20 })
		.id();


	world.manage_resource(CharacterInput{});
	world.manage_resource(Elapsed{});
	world.manage_resource(Wolf{ .entity_id = wolf_id });
	world.manage_resource(Pig{ .entity_id = pig_id });

	return world;
}