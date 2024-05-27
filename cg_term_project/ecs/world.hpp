#pragma once

#include <map>
#include <set>
#include <vector>
#include <stdexcept>
#include <sstream>
#include "utils.hpp"

namespace ecs::internal {
	class World {
		friend class SystemForest;
	private:
		class EntityView {
		private:
			World* world;
			size_t m_id;
		public:
			EntityView(World* world, size_t m_id);

			template<typename T>
			EntityView& with(T component);
			size_t id();
		};

	private:
		Locked<EntityStorage> entity_storage{ Locked{EntityStorage{}} };
		Locked<ResourceStorage> resource_storage{ Locked{ResourceStorage{}} };
		Locked<ComponentStorage> component_storage{ Locked{ComponentStorage{}} };

		template <typename T>
		void set_component(size_t entity_id, T component);

		template <typename T>
		void intersect(std::set<size_t>& ids);

		template <typename T>
		auto fetch_states();
	public:
		EntityView spawn_entity();
		EntityView spawn_entity(size_t id);

		template <typename T>
		void manage_resource(T&& resource);

		template <typename T>
		T* get_resource();

		template <typename T>
		T* get_component(size_t entity_id);

		template<typename ...Ts>
		auto get_entity_with(size_t entity_id);

		template<typename ...Ts>
		auto get_entities_with();

		template <typename T>
		void register_component();
	};

	template<typename T>
	inline auto World::fetch_states() {
		using U = std::invoke_result_t<decltype(StateToArg::revert<T>), T>;
		if constexpr (is_resource_v<T>) {
			return *resource_storage.with_locked([](ResourceStorage* inner) {return inner->get_resource<U>(); });
		}
		else if constexpr (is_component_v<T>) {
			U entities_state{};
			using V = decltype(entities_state.inner)::value_type;
			using Tail = std::invoke_result_t<decltype(behead<V>), V>;
			std::set<size_t> intersected{ entity_storage.with_locked([](EntityStorage* inner) {return inner->entity_ids; }) };

			intersect<Tail>(intersected);

			component_storage.with_locked([&](ComponentStorage* component_storage) {
				std::transform(
					intersected.begin(), intersected.end(), std::back_inserter(entities_state.inner), [=](size_t id) {
						auto id_tuple = std::make_tuple(id);
						auto comps = ([=]<auto ...I>(std::index_sequence<I...>) {
							return std::make_tuple(((*component_storage->get_component_lut<std::tuple_element_t<I + 1, V>>())[id])...);
						})(std::make_index_sequence<std::tuple_size_v<V>-1>{});
						return std::tuple_cat(id_tuple, comps);
					});
				}
			);
			return entities_state;
		}
		else if constexpr (std::is_same_v<T, EntityApi>) {
			return EntityApiState{};
		}
	}

	template<typename T>
	inline void World::intersect(std::set<size_t>& ids) {
		component_storage.with_locked([&](ComponentStorage* component_storage) {
			std::set<size_t> tmp{};
			[&] <auto...I>(std::index_sequence<I...>) {
				([&]() {
					using C = std::tuple_element_t<I, T>;
					tmp.clear();
					auto comp = std::views::keys(*component_storage->get_component_lut<C>());
					std::set_intersection(ids.begin(), ids.end(), comp.begin(), comp.end(), std::inserter(tmp, tmp.begin()));
					tmp.swap(ids);
					}(), ...);
			}(std::make_index_sequence<std::tuple_size_v<T>>{});
			}
		);

	}

	template<typename T>
	inline World::EntityView& World::EntityView::with(T component) {
		world->set_component(m_id, component);
		return *this;
	}

	template <typename T>
	inline void World::register_component() {
		component_storage.with_locked([](ComponentStorage* inner) {
			inner->register_component<T>();
			}
		);
	}

	template <typename T>
	inline void World::set_component(size_t entity_id, T component) {
		component_storage.with_locked([&](ComponentStorage* inner) {
			auto lut = inner->get_component_lut<T>();
			(*lut)[entity_id] = component;
			}
		);
	}

	template<typename T>
	inline void World::manage_resource(T&& resource) {
		resource_storage.with_locked([&](ResourceStorage* inner) {
			inner->manage_resource(resource);
			}
		);
	}
	template<typename T>
	inline T* World::get_resource() {
		return resource_storage.with_locked([](ResourceStorage* inner) {
			return inner->get_resource<T>();
			}
		);
	}
	template<typename T>
	inline T* World::get_component(size_t entity_id)
	{
		return component_storage.with_locked([=](ComponentStorage* inner) {
			auto lut = inner->get_component_lut<T>();
			return &(*lut)[entity_id];
			}
		);
	}
	template<typename ...Ts>
	inline auto World::get_entity_with(size_t entity_id)
	{
		using T = std::tuple<Ts...>;
		using Tp = std::tuple<Ts*...>;
		return component_storage.with_locked([&](ComponentStorage* component_storage) {
			return ([=]<auto ...I>(std::index_sequence<I...>) {
				return std::make_tuple(&((*component_storage->get_component_lut<std::tuple_element_t<I, T>>())[entity_id])...);
			})(std::make_index_sequence<std::tuple_size_v<Tp>>{});
			}
		);
	}
	template<typename ...Ts>
	inline auto World::get_entities_with()
	{
		std::set<size_t> intersected{ entity_storage.with_locked([](EntityStorage* inner) {return inner->entity_ids; }) };
		using T = std::tuple<Ts...>;
		intersect<T>(intersected);

		using Tp = std::tuple<size_t, Ts*...>;
		std::vector<Tp> tuples{};
		tuples.reserve(intersected.size());
		component_storage.with_locked([&](ComponentStorage* component_storage) {
			std::transform(
				intersected.begin(), intersected.end(), std::back_inserter(tuples), [=](size_t id) {
					auto id_tuple = std::make_tuple(id);
					auto comps = ([=]<auto ...I>(std::index_sequence<I...>) {
						return std::make_tuple(&((*component_storage->get_component_lut<std::tuple_element_t<I, T>>())[id])...);
					})(std::make_index_sequence<std::tuple_size_v<Tp>-1>{});
					return std::tuple_cat(id_tuple, comps);
				});
			}
		);

		return tuples;
	}
}


