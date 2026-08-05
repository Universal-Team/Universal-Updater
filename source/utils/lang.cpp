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

#include "lang.hpp"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

std::map<std::string, std::string> Strings;

const std::string &Lang::get(const std::string &key) {
	// Count of a std::map always returns either 0 or 1
	if (Strings.count(key) == 0) return key;
	return Strings.at(key);
}

void Lang::load(const std::string &lang) {
	// Check if the path exists and fallback to en if not.
	std::string path = "romfs:/lang/" + lang + "/app.json";
	if (access(path.c_str(), F_OK) != 0)
		path = "romfs:/lang/en/app.json";

	FILE *values = fopen(path.c_str(), "rt");
	if (values) {
		rapidjson::Document json;
		char *readBuffer = new char[0x10000];
		rapidjson::FileReadStream is(values, readBuffer, 0x10000);
		json.ParseStream(is);
		delete[] readBuffer;
		fclose(values);

		if (json.IsObject()) {
			for (const auto &item : json.GetObject()) {
				if (item.value.IsString()) {
					Strings[item.name.GetString()] = item.value.GetString();
				}
			}
		}
	}
}