#ifndef LANG_HPP
#define LANG_HPP

#include "json.hpp"

#include <string>

namespace Lang {
	std::string get(const std::string &key);
	void load(const std::string lang);
}

#endif