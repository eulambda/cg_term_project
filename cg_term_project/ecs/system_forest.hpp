#pragma once

#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <ranges>
#include "world.hpp"
#include "diff.hpp"

namespace ecs::internal {
	class SystemForest {
	private:
		struct SystemNode { std::function<void()> fn; std::vector<size_t> depends_on; };
		struct SystemNodeView {
			SystemForest* system_forest;
			size_t id;
			SystemNodeView& depends_on(void* dep);
			template <typename ...Ts>
			SystemNodeView followed_by(void (*fn)(Ts...));
		};
		std::map<size_t, SystemNode> system_nodes;
		World* world;
	public:
		SystemForest(World* world);
		template <typename ...Ts>
		SystemNodeView add(void (*fn)(Ts...));
		void run();
	};

	template<typename ...Ts>
	inline SystemForest::SystemNodeView SystemForest::add(void(*fn)(Ts...)) {
		auto run_node = [=] {
			auto states = States<Ts...>{ std::make_tuple(world->fetch_states<Ts>()...) };
			auto draft = states;
			auto args = draft.into_args();

			std::apply(fn, args.inner);

			world->component_storage.with_locked([&](ComponentStorage* component_storage) {
				world->entity_storage.with_locked([&](EntityStorage* entity_storage) {
					auto entity_diffs = diff_entities(draft, entity_storage, component_storage);
					std::ranges::for_each(entity_diffs, [=](auto& diff) {
						diff.apply(entity_storage, component_storage);
						});
					}
				);
				}
			);
			world->resource_storage.with_locked([&](ResourceStorage* resource_storage) {
				auto resource_diffs = diff_resources(draft, states, resource_storage);
				std::ranges::for_each(resource_diffs, [=](auto& diff) {
					diff.apply(resource_storage);
					});
				}
			);

			world->component_storage.with_locked([&](ComponentStorage* component_storage) {
				auto comp_diffs = diff_components(draft, states, component_storage);
				std::ranges::for_each(comp_diffs, [](auto& diff) {
					diff.apply();
					});
				}
			);
			};

		auto depends_on = std::vector<size_t>();
		size_t id = get_function_id(fn);
		auto [_, ok] = system_nodes.emplace(id, SystemNode{ run_node,depends_on });
		if (!ok) throw std::runtime_error("failed to emplace a system node (maybe already registered)");
		return SystemNodeView{ .system_forest{this},.id{id} };
	}
	template<typename ...Ts>
	inline SystemForest::SystemNodeView SystemForest::SystemNodeView::followed_by(void(*fn)(Ts...)) {
		return system_forest->add(fn).depends_on((void*)id);
	}
}

