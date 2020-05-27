#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#include "common.hpp"

bool matchPattern(std::string pattern, std::string tested);

namespace StringUtils {
	std::string format(const std::string& fmt_str, ...);
}

#endif