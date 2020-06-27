#ifndef _UNIVERSAL_UPDATER_STRING_UTILS_HPP
#define _UNIVERSAL_UPDATER_STRING_UTILS_HPP

#include "common.hpp"

bool matchPattern(std::string pattern, std::string tested);

namespace StringUtils {
	std::string format(const std::string& fmt_str, ...);
}

#endif