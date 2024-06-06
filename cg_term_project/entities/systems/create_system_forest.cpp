#include "../prelude.hpp"
#include "run_pig_agent.hpp"
#include "apply_character_input.hpp"
#include "apply_elastic_collision.hpp"
#include "apply_hit_damages.hpp"
#include "apply_velocity.hpp"
#include "constrain_velocity.hpp"
#include "update_elapsed.hpp"
#include "print_console.hpp"
#include "load_stage.hpp"
#include "clear_dead.hpp"
#include "clear_pig_houses.hpp"
#include "activate_portal.hpp"

ecs::SystemForest create_system_forest(ecs::World* world) {
	auto system_forest = ecs::SystemForest(world);

	system_forest.add(update_elapsed)
		.followed_by(load_stage)
		.followed_by(apply_velocity)
		.followed_by(run_pig_agent)
		.followed_by(apply_character_input)
		.followed_by(apply_hit_damages)
		.followed_by(apply_elastic_collision)
		.followed_by(constrain_velocity)
		.followed_by(clear_dead)
		.followed_by(clear_pig_houses)
		.followed_by(activate_portal)
		.followed_by(print_console)
		;

	return system_forest;
}