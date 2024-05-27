#include "prelude.hpp"
#include <map>
#include <latch>
#include <execution>
#include <ranges>

namespace ecs::internal {
	size_t EntityStorage::issue_entity_id() {
		if (!unused_entity_ids.empty()) {
			auto first = unused_entity_ids.begin();
			auto id = *first;
			unused_entity_ids.erase(first);
			entity_ids.insert(id);
			return id;
		}
		auto id = entity_ids.size();
		entity_ids.insert(id);
		return id;
	}

	void EntityStorage::dispose_entity_id(size_t id) {
		entity_ids.erase(id);
		unused_entity_ids.insert(id);
	}

	World::EntityView World::spawn_entity() {
		auto id = entity_storage.with_locked([](EntityStorage* entity_storage) {return entity_storage->issue_entity_id(); });
		return World::EntityView{ this, id };
	}

	World::EntityView World::spawn_entity(size_t id) {
		return World::EntityView{ this, id };
	}


	World::EntityView::EntityView(World* world, size_t m_id) :world{ world }, m_id{ m_id } {
	}

	size_t World::EntityView::id()
	{
		return m_id;
	}

	EntityApi::QueuedEntity& EntityApi::spawn() {
		return to_spawn->emplace_back();
	}
	void EntityApi::remove(size_t id) {
		to_remove->insert(id);
	}
	EntityApi::EntityApi(EntityApiState& state) : to_remove{ &state.to_remove }, to_spawn{ &state.to_spawn }
	{
	}
	size_t get_function_id(void* fn) {
		return reinterpret_cast<size_t>(fn);
	}

	SystemForest::SystemForest(World* world) :world(world) {
	}

	void SystemForest::run() {
		std::map<size_t, std::latch> latches;
		for (auto& node : system_nodes) {
			latches.emplace(node.first, 1);
		}
		std::for_each(std::execution::par_unseq, system_nodes.begin(), system_nodes.end(), [&](auto& node) {
			for (auto dep : node.second.depends_on) {
				latches.at(dep).wait();
			}
			node.second.fn();
			latches.at(node.first).count_down();
			}
		);
	}

	SystemForest::SystemNodeView& SystemForest::SystemNodeView::depends_on(void* dep) {
		system_forest->system_nodes.at(id).depends_on.push_back(get_function_id(dep));
		return *this;
	}

	void EntityApiState::trigger_deferred(std::function<size_t(void)> get_id, ComponentStorage* component_storage) {

		std::ranges::for_each(to_spawn, [&](EntityApi::QueuedEntity& inner) {
			size_t entity_id = get_id();
			std::ranges::for_each(inner.deferred_pushes, [=](std::function<void(size_t, ComponentStorage*)> deferred_push) {
				deferred_push(entity_id, component_storage);
				});
			});
		to_spawn.clear();
	}
}
