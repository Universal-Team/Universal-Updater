// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#include "Utils.hpp"

#include <cstring>
#include <regex>
#include <sys/stat.h>
#include <sys/statvfs.h>


uint64_t Utils::AvailableSpace() {
	struct statvfs ST;

	#ifdef __3DS__
		statvfs("sdmc:/", &ST);

	#elif __NDS__
		statvfs("/", &ST);
	#endif

	return (uint64_t)ST.f_bsize * (uint64_t)ST.f_bavail;
};


void Utils::MakeDirs(const std::string &Dest) {
	for (char *Slashpos = strchr(Dest.c_str() + 1, '/'); Slashpos != NULL; Slashpos = strchr(Slashpos + 1, '/')) {
		char Bak = *(Slashpos);
		*(Slashpos) = '\0';

		mkdir(Dest.c_str(), 0777);

		*(Slashpos) = Bak;
	}
};


std::string Utils::VectorToString(const std::vector<std::string> &Fetch) {
	std::string Temp = "";

	if (Fetch.size() < 1) return ""; // Smaller than 1 --> Return empty.

	for (size_t Idx = 0; Idx < Fetch.size(); Idx++) {
		if (Idx != Fetch.size() - 1) Temp += Fetch[Idx] + ", ";
		else Temp += Fetch[Idx];
	}

	return Temp;
};


std::string Utils::LowerCase(const std::string &STR) {
	std::string Lower;
	transform(STR.begin(), STR.end(), std::back_inserter(Lower), tolower); // Transform the string to lowercase.

	return Lower;
};

std::string Utils::FormatBytes(const uint64_t bytes) {
	char out[32];

	if (bytes == 1)					sniprintf(out, sizeof(out), "%lld Byte", bytes);
	else if (bytes < (1ull << 10))	sniprintf(out, sizeof(out), "%lld Bytes", bytes);
	else if (bytes < (1ull << 20))	sniprintf(out, sizeof(out), "%.1lld KiB", bytes >> 10);
	else if (bytes < (1ull << 30))	sniprintf(out, sizeof(out), "%.1lld MiB", bytes >> 20);
	else if (bytes < (1ull << 40))	snprintf(out, sizeof(out), "%.1f GiB", (float)bytes / (1ull << 30));
	else							snprintf(out, sizeof(out), "%.1f TiB", (float)bytes / (1ull << 40));

	return out;
}


bool Utils::MatchPattern(const std::string &Pattern, const std::string &Tested) {
	std::regex PatternRegex(Pattern);
	return regex_match(Tested, PatternRegex);
};