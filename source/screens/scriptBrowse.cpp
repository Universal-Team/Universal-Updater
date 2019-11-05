/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019 VoltZ, Epicpkmn11, Flame, RocketRobz, TotallyNotGuy
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

#include "download/download.hpp"

#include "screens/mainMenu.hpp"
#include "screens/scriptBrowse.hpp"

#include "utils/config.hpp"

#define ENTRIES_PER_SCREEN 2

nlohmann::json jsonFileBrowse;
std::string metaFile = "sdmc:/3ds/Universal-Updater/ScriptInfo.json";

extern std::string get(nlohmann::json json, const std::string &key, const std::string &key2);

nlohmann::json openMetaFile() {
	FILE* file = fopen(metaFile.c_str(), "rt");
	nlohmann::json jsonFile;
	if(file)    jsonFile = nlohmann::json::parse(file, nullptr, false);
	fclose(file);
	return jsonFile;
}


void ScriptBrowse::Draw(void) const {
	Gui::DrawTop();
	Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, Lang::get("Scriptbrowse"), 400);
	Gui::DrawBottom();
}


void ScriptBrowse::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_B) {
		Gui::screenBack();
		return;
	}
}