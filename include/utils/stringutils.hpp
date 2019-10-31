#pragma once

#include "common.hpp"

bool matchPattern(std::string pattern, std::string tested);

namespace StringUtils
{
	std::string format(const std::string& fmt_str, ...);
}