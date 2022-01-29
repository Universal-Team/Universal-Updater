/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2021 Universal-Team
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "Utils.hpp"
#include <cstring>
#include <regex>
#include <sys/stat.h>
#include <sys/statvfs.h>


uint64_t Utils::AvailableSpace() {
	struct statvfs ST;

	#ifdef __3DS__
		statvfs("sdmc:/", &ST);

	#elif ARM9
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