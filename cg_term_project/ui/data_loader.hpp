#pragma once

#include <model.h>
#include <string>

template<typename T>
concept Loadable = !std::is_same_v<void,decltype(T::load)>;

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

inline Model& fetch_model(std::string path);
inline Curve& fetch_curve(std::string name);
inline Shader& fetch_shader(std::string name);

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
	if (found == lut.end()) throw std::exception{ (name + ": failed to fetch animation").c_str() };
	return lut[name];
}

inline Shader& fetch_shader(std::string name) {
	static std::map<std::string, Shader> lut{};
	auto found = lut.find(name);
	if (found == lut.end()) found = lut.emplace(name, Shader{}).first;
	if (found == lut.end()) throw std::exception{ (name + ": failed to fetch shader").c_str() };
	return lut[name];
}