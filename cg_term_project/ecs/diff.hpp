#pragma once

#include "utils.hpp"

namespace ecs::internal {
	struct Diff {
		std::function<void()> apply;
	};
	struct ResourceDiff {
		std::function<void(ResourceStorage*)> apply;
	};
	struct EntityDiff {
		std::function<void(EntityStorage*, ComponentStorage*)> apply;
	};

	template<typename ...Ts>
	inline std::vector<ResourceDiff> diff_resources(States<Ts...>& draft, States<Ts...>& base, ResourceStorage* resource_storage) {
		std::vector<ResourceDiff> diffs{};

		[&] <auto...I>(std::index_sequence<I...>) {
			([&]() {
				using IthArg = std::tuple_element_t<I, std::tuple<Ts...>>;
				if constexpr (is_resource_v<IthArg> && is_mutable_v<IthArg>) {
					if (draft.touched_index.find(I) == draft.touched_index.end()) return;
					auto* lhs = &std::get<I>(base.inner);
					auto* rhs = &std::get<I>(draft.inner);
					if (*lhs == *rhs) return;
					using InnerT = std::remove_pointer_t<decltype(lhs)>;
					std::function<void(ResourceStorage*)> apply_change = [=](ResourceStorage* resource_storage) {*resource_storage->get_resource<InnerT>() = *rhs; };
					diffs.emplace_back(apply_change);
				}
				}(), ...);
		}(std::make_index_sequence<sizeof...(Ts)>{});

		return diffs;
	}

	template<typename ...Ts>
	inline  std::vector<Diff> diff_components(States<Ts...>& draft, States<Ts...>& base, ComponentStorage* component_storage) {
		std::vector<Diff> diffs{};

		auto iterate_comopnents = [&](auto* lhs, auto* rhs) {
			using T = std::remove_pointer_t<decltype(lhs)>;
			[&] <auto...I>(std::index_sequence<I...>) {
				size_t entity_id = std::get<0>(*lhs);
				([&]() {
					using C = std::tuple_element_t<I + 1, T>;
					auto lhs_comp = &std::get<I + 1>(*lhs);
					auto rhs_comp = &std::get<I + 1>(*rhs);
					if (*lhs_comp == *rhs_comp) return;
					std::function<void()> apply_change = [=]() {
						(*component_storage->get_component_lut<C>())[entity_id] = *rhs_comp;
						};
					diffs.emplace_back(apply_change);
					}(), ...);
			}(std::make_index_sequence<std::tuple_size_v<T>-1>{});
			};

		[&] <auto...I>(std::index_sequence<I...>) {
			([&] {
				using IthArg = std::tuple_element_t<I, std::tuple<Ts...>>;
				if constexpr (is_component_v<IthArg> && is_mutable_v<IthArg>) {
					if (draft.touched_index.find(I) == draft.touched_index.end()) return;
					auto& lhs = std::get<I>(base.inner);
					auto& rhs = std::get<I>(draft.inner);
					using V = decltype(lhs.inner)::value_type;
					auto zipped = std::vector<std::tuple<V&, V&>>{};
					std::transform(lhs.inner.begin(), lhs.inner.end(), rhs.inner.begin(), std::back_inserter(zipped), [](V& lhs_item, V& rhs_item) {
						return std::tie(lhs_item, rhs_item);
						});
					std::for_each(zipped.begin(), zipped.end(), [&](auto& zipped_item) {
						iterate_comopnents(&std::get<0>(zipped_item), &std::get<1>(zipped_item));
						});
				}
				}(), ...);
		}(std::make_index_sequence<sizeof...(Ts)>{});

		return diffs;
	}

	template<typename ...Ts>
	inline  std::vector<EntityDiff> diff_entities(States<Ts...>& draft, EntityStorage* entity_storage, ComponentStorage* component_storage) {
		std::vector<EntityDiff> diffs{};
		[&] <auto...I>(std::index_sequence<I...>) {
			([&] {
				using IthArg = std::tuple_element_t<I, std::tuple<Ts...>>;
				if constexpr (std::is_same_v<IthArg, EntityApi>) {
					EntityApiState* state = &std::get<I>(draft.inner);
					std::ranges::for_each(state->to_remove, [&](size_t id) {
						std::function<void(EntityStorage*, ComponentStorage*)> apply_change = [=](EntityStorage* entity_storage, ComponentStorage*) {
							entity_storage->dispose_entity_id(id);
							std::ranges::for_each(component_storage->delete_entity, [=](std::function<void(size_t)> fn) {
								fn(id);
								});
							};
						diffs.emplace_back(apply_change);
						});
					if (state->to_spawn.empty()) return;
					std::function<void(EntityStorage*, ComponentStorage*)> apply_change = [=](EntityStorage* entity_storage, ComponentStorage* component_storage) {
						state->trigger_deferred([=] {return entity_storage->issue_entity_id(); }, component_storage);
						};
					diffs.emplace_back(apply_change);
				}}
			(), ...);
		}(std::make_index_sequence<sizeof...(Ts)>{});

		return diffs;
	}

}