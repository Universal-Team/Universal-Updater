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

#include "utils/fileBrowse.h"
#include "utils/json.hpp"

#define ENTRIES_PER_SCREEN 3

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

nlohmann::json parseInfo() {
    FILE* file = fopen(metaFile.c_str(), "rt");
    if(!file) {
        printf("File not found\n");
        fclose(file);
        return {"", ""};
    }
    nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
    fclose(file);

    for(uint i=0;i<json.size();i++) {
        if(!json[i].contains("title"))    json[i]["title"] = "TITLE";
        if(!json[i].contains("author"))    json[i]["author"] = "AUTHOR";
        if(!json[i].contains("revision"))    json[i]["revision"] = 0;
        if(!json[i].contains("version"))    json[i]["revision"] = 0;
    }
    return json;
}


ScriptBrowse::ScriptBrowse() {
	DisplayMsg(Lang::get("GETTING_SCRIPT_LIST"));
	downloadToFile("https://github.com/Universal-Team/extras/raw/master/ScriptsInfo/scriptInfo.json", "sdmc:/3ds/Universal-Updater/ScriptInfo.json");
	jsonFileBrowse = parseInfo();
}

void ScriptBrowse::Draw(void) const {
	Gui::DrawTop();
	Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, Lang::get("SCRIPTBROWSE"), 400);

	Gui::DrawStringCentered(0, 80, 0.7f, Config::TxtColor, Lang::get("TITLE") + std::string(jsonFileBrowse[screenPos+selection]["title"]), 400);
	Gui::DrawStringCentered(0, 100, 0.7f, Config::TxtColor, Lang::get("AUTHOR") + std::string(jsonFileBrowse[screenPos+selection]["author"]), 400);
	Gui::DrawStringCentered(0, 120, 0.7f, Config::TxtColor, Lang::get("INSTALLED_REV"), 400);
	Gui::DrawStringCentered(0, 140, 0.7f, Config::TxtColor, Lang::get("CURRENT_REV")+ std::to_string(jsonFileBrowse[screenPos+selection]["revision"].get<int64_t>()), 400);
	Gui::DrawBottom();
	for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)jsonFileBrowse.size();i++) {
		if(screenPos + i == selection) {
			Gui::Draw_Rect(0, 40+(i*57), 320, 45, Config::SelectedColor);
		} else { 
			Gui::Draw_Rect(0, 40+(i*57), 320, 45, Config::UnselectedColor);
		}
		Gui::DrawStringCentered(0, 38+(i*57), 0.7f, Config::TxtColor, jsonFileBrowse[screenPos+i]["title"], 320);
		Gui::DrawStringCentered(0, 62+(i*57), 0.7f, Config::TxtColor, jsonFileBrowse[screenPos+i]["author"], 320);
	}
}


void ScriptBrowse::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (keyRepeatDelay)	keyRepeatDelay--;
	if (hDown & KEY_B) {
		jsonFileBrowse.clear();
		Gui::screenBack();
		return;
	}

	if (hHeld & KEY_DOWN && !keyRepeatDelay) {
		if (selection < (int)jsonFileBrowse.size()-1) {
			selection++;
		} else {
			selection = 0;
		}
		if (fastMode == true) {
			keyRepeatDelay = 3;
		} else if (fastMode == false){
			keyRepeatDelay = 6;
		}
	}

	if (hHeld & KEY_UP && !keyRepeatDelay) {
		if (selection > 0) {
			selection--;
		} else {
			selection = (int)jsonFileBrowse.size()-1;
		}
		if (fastMode == true) {
			keyRepeatDelay = 3;
		} else if (fastMode == false){
			keyRepeatDelay = 6;
		}
	}

	if (hDown & KEY_A) {
		if (jsonFileBrowse.size() != 0) {
			std::string fileName = Lang::get("DOWNLOADING") + std::string(jsonFileBrowse[selection]["title"]);

		std::string titleFix = jsonFileBrowse[selection]["title"]; 
		for (int i = 0; i < (int)titleFix.size(); i++) {
   			if (titleFix[i] == '/') {
        		titleFix[i] = '-';
    		}
		}
			DisplayMsg(fileName);

			downloadToFile(jsonFileBrowse[selection]["url"], Config::ScriptPath + titleFix + ".json");
		}
	}

	if(selection < screenPos) {
		screenPos = selection;
	} else if (selection > screenPos + ENTRIES_PER_SCREEN - 1) {
		screenPos = selection - ENTRIES_PER_SCREEN + 1;
	}
}