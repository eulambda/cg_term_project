#pragma once
#include <string>
#include <map>
#include <vector>
#include <iostream>

namespace json {
	struct CharReader {
		std::istream& is;
		using It = std::istream_iterator<char>;
		It begin();
		It end();
	};

	enum class TokenType { invalid, start_obj, end_obj, start_arr, end_arr, number, colon, comma, eof, string, boolean };
	enum class ValueType { invalid, arr, obj, number, string, boolean };
	struct Tokenizer {
		CharReader& reader;
		struct It {
			using Inner = std::istream_iterator<char>;
			Inner inner;
			TokenType next{ TokenType::eof };
			double next_number{ 0 };
			std::string next_string{};
			inline std::tuple<TokenType, double, std::string> operator*();
			It& operator++();
			bool operator==(const It& it) const;
			bool consume(TokenType type);
		};
		It begin();
		It end();
	};
	class Value {
		ValueType m_type{ ValueType::invalid };
		std::vector<Value> arr;
		std::map<std::string, Value> obj;
		double num;
		std::string str;
		bool boolean;
	public:
		static Value create_arr(std::vector<Value> arr);
		static Value create_obj(std::map<std::string, Value> obj);
		static Value create_number(double num);
		static Value create_string(std::string str);
		static Value create_bool(bool boolean);

		bool is_valid();
		ValueType type();
		std::vector<Value>* as_arr();
		std::map<std::string, Value>* as_obj();
		std::string* as_str();
		double* as_num();
		bool* as_bool();

		static Value parse_num(Tokenizer::It& it);
		static Value parse_str(Tokenizer::It& it);
		static Value parse_bool(Tokenizer::It& it);
		static Value parse_arr(Tokenizer::It& it);
		static Value parse_obj(Tokenizer::It& it);
		static Value parse_any(Tokenizer::It& it);
		static Value parse(std::string filepath);
	};
	std::ostream& operator<<(std::ostream& os, Value& val);
}