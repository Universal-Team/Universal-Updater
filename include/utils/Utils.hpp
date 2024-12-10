// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_UTILS_HPP
#define _UNIVERSAL_UPDATER_UTILS_HPP

#include <string>
#include <vector>

namespace Utils {
	uint64_t AvailableSpace();
	void MakeDirs(const std::string &Dest);
	std::string VectorToString(const std::vector<std::string> &Fetch);
	std::string LowerCase(const std::string &STR);
	std::string FormatBytes(const uint64_t Bytes);
	bool MatchPattern(const std::string &Pattern, const std::string &Tested);
};

#endif