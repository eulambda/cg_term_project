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

	world.spawn_entity().with(Floor{}).with(Body{ .w = 1,.h = 50,.x = -1,.y = 0 });
	world.spawn_entity().with(Floor{}).with(Body{ .w = 1,.h = 50,.x = 17,.y = 0 });
	world.spawn_entity().with(Floor{}).with(Body{ .w = 16,.h = 1,.x = 8,.y = -1 });
	world.spawn_entity().with(Floor{}).with(Body{ .w = 2,.h = 1,.x = 1,.y = 0 });
	world.spawn_entity().with(Floor{}).with(Body{ .w = 2,.h = 1,.x = 15,.y = 0 });
	world.spawn_entity().with(Floor{}).with(Body{ .w = 2,.h = 1,.x = 4,.y = 6 });

	auto wolf_id = world.spawn_entity()
		.with(LocomotionWalking{})
		.with(Body{ .w = 2,.h = 2, .x = 2,.y = 4 })
		.with(FrozenState{})
		.id();

	world.manage_resource(CharacterInput{});
	world.manage_resource(Elapsed{});
	world.manage_resource(Stage{});
	world.manage_resource(Wolf{ .entity_id = wolf_id });
	world.manage_resource(Pig{});

	return world;
}