#include "stringutils.hpp"

bool matchPattern(std::string pattern, std::string tested) {
	std::regex patternRegex(pattern);
	return regex_match(tested, patternRegex);
}

std::string StringUtils::format(const std::string& fmt_str, ...) {
	va_list ap;
	char* fp = NULL;
	va_start(ap, fmt_str);
	vasprintf(&fp, fmt_str.c_str(), ap);
	va_end(ap);
	std::unique_ptr<char, decltype(free)*> formatted(fp, free);
	return std::string(formatted.get());
}