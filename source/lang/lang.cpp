/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 Universal-Team
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

#include "config.hpp"
#include "lang.hpp"

#include <stdio.h>
#include <unistd.h>

extern std::unique_ptr<Config> config;
nlohmann::json appJson;

std::string Lang::get(const std::string &key) {
	if (!appJson.contains(key)) {
		return "MISSING: " + key;
	}

	return appJson.at(key).get_ref<const std::string&>();
}

std::string langs[] = {"br", "da", "de", "en", "es", "fr", "it", "lt", "pl", "pt", "ru", "jp"};

void Lang::load(const std::string lang) {
	FILE* values;
	if (config->langPath() == 1) {
		// Check if exist.
		if (access("sdmc:/3ds/Universal-Updater/app.json", F_OK) == 0) {
			values = fopen(("sdmc:/3ds/Universal-Updater/app.json"), "rt");
			appJson = nlohmann::json::parse(values, nullptr, false);
			fclose(values);
			return;

		} else {
			values = fopen(("romfs:/lang/en/app.json"), "rt");
			appJson = nlohmann::json::parse(values, nullptr, false);
			fclose(values);
			return;
		}

	} else {
		// Check if exist.
		if (access(("romfs:/lang/" + lang + "/app.json").c_str(), F_OK) == 0) {
			values = fopen(std::string(("romfs:/lang/" + lang + "/app.json")).c_str(), "rt");
			appJson = nlohmann::json::parse(values, nullptr, false);
			fclose(values);
			return;

		} else {
			values = fopen(("romfs:/lang/en/app.json"), "rt");
			appJson = nlohmann::json::parse(values, nullptr, false);
			fclose(values);
			return;
		}
	}
}