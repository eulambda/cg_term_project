#include "prelude.hpp"

ecs::World create_world() {
	auto world = ecs::World();

	world.register_component<Body>();
	world.register_component<Floor>();
	world.register_component<Facing>();
	world.register_component<LocomotionWalking>();
	world.register_component<LocomotionFlying>();
	world.register_component<LocomotionStationery>();
	world.register_component<HitDamage>();
	world.register_component<Life>();
	world.register_component<FrozenState>();
	world.register_component<Mass>();
	world.register_component<Health>();
	world.register_component<DamageReceiver>();
	world.register_component<RoarCharged>();
	world.register_component<DebugInfo>();
	world.register_component<StageText>();
	world.register_component<Wolf>();
	world.register_component<Pig>();
	world.register_component<Flame>();
	world.register_component<Roar>();
	world.register_component<Obstacle>();
	world.register_component<ParticleDomain>();
	world.register_component<Portal>();
	world.register_component<PigHouse>();
	world.register_component<Compound>();
	world.register_component<Grass>();
	world.register_component<Butterfly>();
	world.register_component<ShardSpawner>();
	world.register_component<Flower>();

	auto wolf = world.spawn_entity();
	auto wolf_id = wolf.id();
	wolf
		.with(Wolf{})
		.with(DebugInfo{ .name = "wolf" })
		.with(LocomotionWalking{})
		.with(Body{ .w = 2,.h = 2 })
		.with(Facing{ .inner = FacingValue::pos_x })
		.with(FrozenState{})
		.with(Mass{ 1 })
		.with(Health{ .max = 5,.current = 5 })
		.with(DamageReceiver{})
		.with(HitDamage{ .from = wolf_id,.power = 1, .knockback = 0,.type = DamageType::normal })
		.with(RoarCharged{ .val = 0, .max = 5, .type = DamageType::wind })
		;
	world.spawn_entity().with(Floor{}).with(Body{ .w = 10,.h = 1,.x = 0,.y = -2 });
	world.spawn_entity().with(StageText{ .contents = "failed to locate assets.",.x = 0,.y = 5 });

	world.manage_resource(SimulationSpeed{});
	world.manage_resource(CharacterInput{});
	world.manage_resource(Elapsed{});
	Stage stage{};
	stage.load("assets/stage1.json");
	world.manage_resource(stage);

	return world;
}