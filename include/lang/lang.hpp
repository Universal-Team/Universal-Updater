#ifndef LANG_HPP
#define LANG_HPP

#include "utils/json.hpp"

#include <string>

namespace Lang {
	std::string get(const std::string &key);
	void load(int lang);
}

#endif