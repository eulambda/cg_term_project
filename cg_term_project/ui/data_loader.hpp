#pragma once

#include <model.h>
#include <animator.h>
#include <string>

template<typename T>
concept Loadable = !std::is_same_v<void, decltype(T::load)>;

template<Loadable T>
T* fetch() {
	static T* ptr = nullptr;
	if (!ptr) {
		ptr = new T{};
		ptr->load();
	}
	return ptr;
}
template<typename T>
T* fetch() {
	static T* ptr = nullptr;
	if (!ptr) ptr = new T{};
	return ptr;
}

inline Animator& fetch_animator(std::string path);
inline Animation& fetch_animation(std::string path);
inline Model& fetch_model(std::string path);
inline Curve& fetch_curve(std::string name);
inline Shader& fetch_shader(std::string name);

inline Animator& fetch_animator(std::string path) {
	static std::map<std::string, Animator> lut{};
	auto found = lut.find(path);
	if (found == lut.end()) {
		auto& animation = fetch_animation(path);
		found = lut.try_emplace(path, &animation).first;
	}
	if (found == lut.end()) throw std::exception{ (path + ": failed to load animator").c_str() };
	return found->second;
}
inline Animation& fetch_animation(std::string path) {
	static std::map<std::string, Animation> lut{};
	auto found = lut.find(path);
	if (found == lut.end()) {
		auto& model = fetch_model(path);
		found = lut.try_emplace(path, path, &model).first;
	}
	if (found == lut.end()) throw std::exception{ (path + ": failed to load animation").c_str() };
	return found->second;
}

inline Model& fetch_model(std::string path) {
	static std::map<std::string, Model> lut{};
	auto found = lut.find(path);
	if (found == lut.end()) found = lut.emplace(path, path).first;
	if (found == lut.end()) throw std::exception{ (path + ": failed to load model").c_str() };
	return found->second;
}

inline Curve& fetch_curve(std::string name) {
	static std::map<std::string, Curve> lut{};
	auto found = lut.find(name);
	if (found == lut.end()) found = lut.emplace(name, Curve{}).first;
	if (found == lut.end()) throw std::exception{ (name + ": failed to fetch curve").c_str() };
	return lut[name];
}

inline Shader& fetch_shader(std::string name) {
	static std::map<std::string, Shader> lut{};
	auto found = lut.find(name);
	if (found == lut.end()) found = lut.emplace(name, Shader{}).first;
	if (found == lut.end()) throw std::exception{ (name + ": failed to fetch shader").c_str() };
	return lut[name];
}
