#pragma once

#include <tuple>
#include <map>
#include <set>
#include <vector>
#include <functional>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <numeric>
#include <ranges>
#include <mutex>

namespace ecs::internal {
	size_t get_function_id(void* fn);
	struct ComponentStorage;

	template<typename T>
	auto behead(T tup) {
		return[&]<auto ...I>(std::index_sequence<I...>) {
			return std::make_tuple(std::get<I + 1>(tup)...);
		}(std::make_index_sequence<std::tuple_size_v<T>-1>{});
	};

	template<typename T>
	struct Locked {
	private:
		struct Inner {
			T val;
			std::mutex lock;
		};
		std::unique_ptr<Inner> inner;
	public:
		Locked(T&& val) :
			inner{ std::unique_ptr<Inner>{new Inner{val}} } {}
		template<typename F>
		auto with_locked(F op) {
			auto& inner_deref = *inner;
			inner_deref.lock.lock();
			if constexpr (std::is_same_v<void, decltype(op(&inner_deref.val))>) {
				op(&inner_deref.val);
				inner_deref.lock.unlock();
				return;
			}
			else {
				auto r = op(&inner_deref.val);
				inner_deref.lock.unlock();
				return r;
			}
		}
	};

	template<typename T>
	struct Writable {
	private:
		friend struct StateToArg;
		T* inner;
		std::function<void()> on_touch{ [] {} };
	public:
		Writable(T* inner, std::function<void()> on_touch) :inner{ inner }, on_touch{ on_touch } {}
		inline T* operator->() { on_touch(); return inner; }
		inline T const* operator->() const { return inner; }
	};
	class EntityApi {
	private:
		friend struct EntityApiState;
		class QueuedEntity {
			friend struct EntityApiState;
		private:
			std::vector<std::function<void(size_t, ComponentStorage*)>> deferred_pushes;
		public:
			template<typename T>
			QueuedEntity& with(T component);
		};
		std::vector<QueuedEntity>* to_spawn;
		std::set<size_t>* to_remove;
	public:
		EntityApi(EntityApiState& state);
		QueuedEntity& spawn();
		void remove(size_t id);
	};

	struct EntityApiState {
		std::vector<EntityApi::QueuedEntity> to_spawn;
		std::set<size_t> to_remove;
		void trigger_deferred(std::function<size_t(void)> get_id, ComponentStorage* component_storage);
	};

	template<typename ...Ts>
	struct States;

	template<typename ...Ts>
	struct Args {
		std::tuple<Ts...> inner;
	};

	template<typename TupleType, typename ...Ts>
	struct EntitiesWithGeneric {
	private:
		friend struct StateToArg;
		// `inner` is assumed to be sorted by `id`(the first element) in ascending order.
		std::vector<TupleType> inner{};
		std::function<void()> on_touch;
		TupleType* get_by_id_impl(size_t id) {
			auto ids = std::views::elements<0>(inner);
			auto found = std::lower_bound(ids.begin(), ids.end(), id);
			if (found == ids.end()) return nullptr;
			auto index = found - ids.begin();
			return &inner[index];
		}
	public:
		EntitiesWithGeneric(std::function<void()> on_touch) :on_touch{ on_touch } {};
		auto begin() {
			on_touch();
			return inner.begin();
		}
		auto end() {
			on_touch();
			return inner.end();
		}
		auto cbegin() {
			return inner.cbegin();
		}
		auto cend() {
			return inner.cend();
		}
		auto empty() { return inner.empty(); }

		TupleType* get_by_id(size_t id) {
			on_touch();
			return get_by_id_impl(id);
		}
		TupleType const* get_by_id(size_t id) const {
			return get_by_id_impl(id);
		}
		inline decltype(inner)* operator->() {
			on_touch();
			return &inner;
		}
		inline decltype(inner)const* operator->() const { return &inner; }
	};

	template<typename ...Ts>
	using EntitiesWith = EntitiesWithGeneric<std::tuple<size_t, Ts const*...>, Ts...>;
	template<typename ...Ts>
	using EntitiesWithWritable = EntitiesWithGeneric<std::tuple<size_t, Ts*...>, Ts...>;

	template<typename ...Ts>
	struct EntitiesWithStates {
		std::vector<std::tuple<size_t, Ts...>> inner;
	};

	struct StateToArg {
	private:
		template<typename T>
		struct Converter {
			static T convert(T state, std::function<void()> on_touch) { return state; }
			static T revert(T) { throw; }
		};

		template<typename T>
		struct Converter<Writable<T>> {
			static Writable<T> convert(T& state, std::function<void()> on_touch) { return Writable{ &state,on_touch }; }
			static T revert(Writable<T>) { throw; }
		};

		template<typename TupleType, typename ...Ts>
		struct Converter<EntitiesWithGeneric<TupleType, Ts...>> {
			static EntitiesWithGeneric<TupleType, Ts...> convert(EntitiesWithStates<Ts...>& state, std::function<void()> on_touch);
			static EntitiesWithStates<Ts...> revert(EntitiesWithGeneric<TupleType, Ts...>) { throw; }
		};

		template<>
		struct Converter<EntityApi> {
			static EntityApi convert(EntityApiState& state, std::function<void()> on_touch);
			static EntityApiState revert(EntityApi) { throw; }
		};

	public:
		template<typename U>
		static U convert(auto& state, std::function<void()> on_touch) {
			return Converter<U>::convert(state, on_touch);
		}
		template<typename U>
		static auto revert(U arg) {
			return Converter<U>::revert(arg);
		}
	};

	struct EntityStorage {
		std::set<size_t> entity_ids;
		std::set<size_t> unused_entity_ids;
		size_t issue_entity_id();
		void dispose_entity_id(size_t id);
	};

	struct ResourceStorage {
		std::map<size_t, std::shared_ptr<void>> resource_lut;

		template <typename T>
		T* get_resource();
		template <typename T>
		void manage_resource(T&& resource);
	};

	struct ComponentStorage {
		std::map<size_t, std::shared_ptr<void>> component_lut;

		template <typename T>
		std::map<size_t, T>* get_component_lut();
		template <typename T>
		void register_component();
	};

	template<typename>
	inline constexpr bool is_resource_v = true;
	template<typename ...Ts>
	inline constexpr bool is_resource_v<EntitiesWithGeneric<Ts...>> = false;
	template<>
	inline constexpr bool is_resource_v<EntityApi> = false;

	template<typename>
	inline constexpr bool is_component_v = false;
	template<typename ...Ts>
	inline constexpr bool is_component_v<EntitiesWithGeneric<Ts...>> = true;

	template<typename>
	inline constexpr bool is_mutable_v = false;
	template<typename T>
	inline constexpr bool is_mutable_v<Writable<T>> = true;
	template<typename ...Ts>
	inline constexpr bool is_mutable_v<EntitiesWithWritable<Ts...>> = true;

	template<typename ...Ts>
	struct States {
		std::tuple<std::invoke_result_t<decltype(StateToArg::revert<Ts>), Ts>...> inner;
		std::set<size_t> touched_index;
		Args<Ts...> into_args();
		template<size_t I>
		std::function<void()> get_on_touch() {
			using T = std::tuple_element_t<I, std::tuple<Ts...>>;
			if constexpr (is_mutable_v<T>) {
				return [&] {touched_index.insert(I); };
			}
			else {
				return [] {};
			}

		}
	};

	template<typename T>
	size_t get_type_id() {
		static int ptr = 0;
		return reinterpret_cast<size_t>(&ptr);
	}

	template<typename ...Ts>
	inline Args<Ts...> States<Ts...>::into_args() {
		auto tuple = [&] <auto...I>(std::index_sequence<I...>) {
			return std::make_tuple(StateToArg::convert<Ts>(std::get<I>(inner), get_on_touch<I>())...);
		}(std::make_index_sequence<sizeof...(Ts)>{});
		return Args<Ts...>{ tuple };
	}

	template<typename TupleType, typename ...Ts>
	inline EntitiesWithGeneric<TupleType, Ts...> StateToArg::Converter<EntitiesWithGeneric<TupleType, Ts...>>::convert(EntitiesWithStates<Ts...>& state, std::function<void()> on_touch) {
		EntitiesWithGeneric<TupleType, Ts...> entities_with{ on_touch };
		std::transform(state.inner.begin(), state.inner.end(), std::back_inserter(entities_with.inner), [](auto& item) {
			auto id = std::make_tuple(std::get<0>(item));
			auto comps = [&] <auto...I>(std::index_sequence<I...>) {
				return std::make_tuple((&std::get<I + 1>(item))...);
			}(std::make_index_sequence<sizeof...(Ts)>{});

			return std::tuple_cat(id, comps);

			});

		return entities_with;
	}

	inline EntityApi StateToArg::Converter<EntityApi>::convert(EntityApiState& state, std::function<void()>) {
		return EntityApi(state);
	}

	template<typename T>
	inline std::map<size_t, T>* ComponentStorage::get_component_lut() {
		auto id = get_type_id<T>();
		auto found = component_lut.find(id);
		if (component_lut.end() == found)
		{
			std::stringstream ss;
			ss << "Component not registered: " << typeid(T).name();
			throw std::invalid_argument(ss.str());
		}
		return reinterpret_cast<std::map<size_t, T>*>(found->second.get());
	}

	template<typename T>
	inline void ComponentStorage::register_component() {
		auto lut = new std::map<size_t, T>();
		auto [_, ok] = component_lut.emplace(get_type_id<T>(), lut);
		if (!ok) delete lut;
	}

	template<typename T>
	inline T* ResourceStorage::get_resource() {
		return reinterpret_cast<T*>(resource_lut.at(get_type_id<T>()).get());
	}
	template<typename T>
	void ResourceStorage::manage_resource(T&& resource) {
		using U = std::remove_reference_t<T>;
		auto copied = new U(resource);
		auto [_, ok] = resource_lut.emplace(get_type_id<U>(), copied);
		if (!ok) delete copied;
	}

	template<typename T>
	inline EntityApi::QueuedEntity& EntityApi::QueuedEntity::with(T component) {
		auto deferred = [=](size_t entity_id, ComponentStorage* component_storage) {
			auto lut = component_storage->get_component_lut<T>();
			(*lut)[entity_id] = component;
			};
		deferred_pushes.push_back(deferred);
		return *this;
	}
}
