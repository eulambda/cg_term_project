#pragma once

#include "utils.hpp"
#include "world.hpp"
#include "system_forest.hpp"

namespace ecs {
	using World = ecs::internal::World;
	using SystemForest = ecs::internal::SystemForest;

	using EntityApi = ecs::internal::EntityApi;

	template<typename T>
	using Writable = ecs::internal::Writable<T>;
	template<typename ...Ts>
	using EntitiesWith = ecs::internal::EntitiesWith<Ts...>;
	template<typename ...Ts>
	using EntitiesWithWritable = ecs::internal::EntitiesWithWritable<Ts...>;
}