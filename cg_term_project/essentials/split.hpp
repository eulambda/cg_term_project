#pragma once
#include <regex>

std::vector<std::string> split(const std::string& s, const std::regex& sep_regex = std::regex{ "\\n" }) {
	std::sregex_token_iterator iter(s.begin(), s.end(), sep_regex, -1);
	std::sregex_token_iterator end;
	return { iter, end };
}