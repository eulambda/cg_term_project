#include "json_parser.hpp"
#include <vector>
#include <map>
#include <sstream>
#include <fstream>

namespace json {
	inline CharReader::It CharReader::begin() { return It{ is }; }
	inline CharReader::It CharReader::end() { return It{}; }

	inline Tokenizer::It& Tokenizer::It::operator++() {
		next = TokenType::eof;
		return *this;
	}
	inline bool Tokenizer::It::operator==(const It& it) const {
		return inner == it.inner;
	}
	inline bool Tokenizer::It::consume(TokenType type) {
		if (type != std::get<0>(this->operator*())) return false;
		this->operator++();
		return true;
	}
	inline Tokenizer::It Tokenizer::begin() { return It{ reader.begin() }; }
	inline Tokenizer::It Tokenizer::end() { return It{ reader.end() }; }

	Value Value::create_arr(std::vector<Value> arr) {
		Value val;
		val.m_type = ValueType::arr;
		val.arr = arr;
		return val;
	}
	Value Value::create_obj(std::map<std::string, Value> obj) {
		Value val;
		val.m_type = ValueType::obj;
		val.obj = obj;
		return val;
	}
	Value Value::create_number(double num) {
		Value val;
		val.m_type = ValueType::number;
		val.num = num;
		return val;
	}
	Value Value::create_string(std::string str) {
		Value val;
		val.m_type = ValueType::string;
		val.str = str;
		return val;
	}
	Value Value::create_bool(bool boolean) {
		Value val;
		val.m_type = ValueType::boolean;
		val.boolean = boolean;
		return val;
	}
	ValueType Value::type() {
		return m_type;
	}
	std::vector<Value>* Value::as_arr() {
		if (m_type != ValueType::arr) return nullptr; else return &arr;
	}
	std::map<std::string, Value>* Value::as_obj() {
		if (m_type != ValueType::obj) return nullptr; else return &obj;
	}
	std::string* Value::as_str() {
		if (m_type != ValueType::string) return nullptr; else return &str;
	}
	double* Value::as_num() {
		if (m_type != ValueType::number) return nullptr; else return &num;
	}
	bool* Value::as_bool() {
		if (m_type != ValueType::boolean) return nullptr; else return &boolean;
	}

	bool Value::is_valid() {
		return m_type != ValueType::invalid;
	}

	std::ostream& operator<<(std::ostream& os, Value& val) {
		auto num = val.as_num();
		auto str = val.as_str();
		auto arr = val.as_arr();
		auto obj = val.as_obj();
		if (num) {
			os << *num;
		}
		else if (str) {
			os << '"' << *str << '"';
		}
		else if (arr) {
			os << '[';
			for (auto& element : *arr) {
				os << element;
				os << ", ";
			}
			os << ']';
		}
		else if (obj) {
			os << "{\n";
			for (auto& [k, v] : *obj) {
				os << '"' << k << '"' << ": " << v << ",\n";
			}
			os << '}';
		}
		return os;
	}
	Value Value::parse_num(Tokenizer::It& it) {
		Value val;
		auto [t, tn, ts] = *it;
		if (t != TokenType::number) return val;
		val.m_type = ValueType::number;
		val.num = tn;
		it.consume(t);
		return val;
	}
	Value Value::parse_str(Tokenizer::It& it) {
		Value val;
		auto [t, tn, ts] = *it;
		if (t != TokenType::string) return val;
		val.m_type = ValueType::string;
		val.str = ts;
		it.consume(t);
		return val;
	}
	Value Value::parse_bool(Tokenizer::It& it) {
		Value val;
		auto [t, tn, ts] = *it;
		if (t != TokenType::boolean) return val;
		val.m_type = ValueType::boolean;
		val.boolean = tn != 0;
		it.consume(t);
		return val;
	}
	Value Value::parse_arr(Tokenizer::It& it) {
		Value val;
		auto [t, tn, ts] = *it;
		if (t != TokenType::start_arr) return val;
		val.m_type = ValueType::arr;
		it.consume(t);
		while (it != Tokenizer::It{}) {
			auto [t, tn, ts] = *it;
			if (it.consume(TokenType::end_arr) || it.consume(TokenType::eof)) break;
			auto any = parse_any(it);
			if (!any.is_valid()) break;
			val.arr.push_back(any);
			it.consume(TokenType::comma);
		}
		return val;
	}
	Value Value::parse_obj(Tokenizer::It& it) {
		Value obj;
		auto [t, tn, ts] = *it;
		if (t != TokenType::start_obj) return obj;
		obj.m_type = ValueType::obj;
		it.consume(t);
		while (it != Tokenizer::It{}) {
			if (it.consume(TokenType::end_obj) || it.consume(TokenType::eof)) break;
			auto key = parse_str(it);
			if (!key.is_valid()) break;
			if (!it.consume(TokenType::colon)) break;
			auto val = parse_any(it);
			if (!val.is_valid()) break;
			obj.obj.insert(std::make_pair(*key.as_str(), val));
			it.consume(TokenType::comma);
		}
		return obj;
	}
	Value Value::parse_any(Tokenizer::It& it) {
		auto num = parse_num(it);
		if (num.is_valid()) return num;
		auto str = parse_str(it);
		if (str.is_valid()) return str;
		auto boolean = parse_bool(it);
		if (boolean.is_valid()) return boolean;
		auto arr = parse_arr(it);
		if (arr.is_valid()) return arr;
		auto obj = parse_obj(it);
		if (obj.is_valid()) return obj;
		return Value{};
	}
	Value Value::parse(std::string filepath) {
		std::ifstream is{ filepath };
		CharReader reader{ is };
		Tokenizer tokenizer{ reader };
		auto it = tokenizer.begin();
		return parse_any(it);
	}
	inline std::tuple<TokenType, double, std::string> Tokenizer::It::operator*() {
		if (next != TokenType::eof) return std::make_tuple(next, next_number, next_string);
		if (inner == Inner{}) return std::make_tuple(TokenType::eof, next_number, next_string);
		char c{ 0 };
		while (inner != Inner{}) {
			c = *inner;
			if (c != ' ' && c != '\n' && c != '\r') break;
			inner++;
		}
		if (c == '{') {
			next = TokenType::start_obj;
			inner++;
		}
		else if (c == '}') {
			next = TokenType::end_obj;
			inner++;
		}
		else if (c == '[') {
			next = TokenType::start_arr;
			inner++;
		}
		else if (c == ']') {
			next = TokenType::end_arr;
			inner++;
		}
		else if (c == ',') {
			next = TokenType::comma;
			inner++;
		}
		else if (c == ':') {
			next = TokenType::colon;
			inner++;
		}
		else if (c == '-' || c == '+' || ('0' <= c && c <= '9')) {
			std::stringstream ss{};
			ss << c;
			inner++;
			while (inner != Inner{}) {
				c = *inner;
				if (c != '-' && c != '+' && !('0' <= c && c <= '9') && c != '.') break;
				ss << c;
				inner++;
			}
			next_number = std::stod(ss.str());
			next = TokenType::number;
		}
		else if (c == '\'' || c == '"') {
			std::stringstream ss{};
			bool is_escaping = false;
			inner++;
			while (inner != Inner{}) {
				auto c1 = *inner;
				if (!is_escaping && c1 == c) {
					inner++;
					break;
				}
				if (c1 == '\\') is_escaping = true;
				if (!is_escaping) ss << c1;
				if (is_escaping) {
					if (c1 == 'n') ss << '\n';
					if (c1 == '\'') ss << '\'';
					if (c1 == '"') ss << '"';
					is_escaping = false;
				}
				inner++;
			}

			next_string = ss.str();
			next = TokenType::string;
		}
		else if (c == 't' || c == 'f') {
			std::stringstream ss{};
			ss << c;
			inner++;
			while (inner != Inner{}) {
				c = *inner;
				if (c < 'a' || c>'z') break;
				inner++;
			}
			if (ss.str() == "true") {
				next_number = 1;
				next = TokenType::boolean;
			}
			else if (ss.str() == "false") {
				next_number = 0;
				next = TokenType::boolean;
			}
			else {
				next = TokenType::invalid;
			}
		}
		return std::make_tuple(next, next_number, next_string);
	}
}