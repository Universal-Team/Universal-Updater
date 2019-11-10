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

#include <unistd.h>

#define ENTRIES_PER_SCREEN 3
#define ENTRIES_PER_LIST 7

nlohmann::json infoJson;

#define metaFile "sdmc:/3ds/Universal-Updater/ScriptInfo.json"

extern std::string get(nlohmann::json json, const std::string &key, const std::string &key2);
std::string maxScripts;

void fixInfo(nlohmann::json &json) {
    for(uint i=0;i<json.size();i++) {
        if(!json[i].contains("title"))    json[i]["title"] = "TITLE";
        if(!json[i].contains("author"))    json[i]["author"] = "AUTHOR";
		if(!json[i].contains("shortDesc"))	json[i]["shortDesc"] = "SHORTDESC";
        if(!json[i].contains("revision"))    json[i]["revision"] = 0;
        if(!json[i].contains("curRevision"))    json[i]["curRevision"] = 0;
        if(!json[i].contains("version"))    json[i]["revision"] = 0;
    }
}

nlohmann::json infoFromScript(const std::string &path) {
	nlohmann::json in, out;

	FILE* file = fopen(path.c_str(), "r");
	if(!file) {
		fclose(file);
		return out;
	}
	in = nlohmann::json::parse(file, nullptr, false);
	fclose(file);

	if(in.contains("info")) {
		if(in["info"].contains("title") && in["info"]["title"].is_string())	out["title"] = in["info"]["title"];
		if(in["info"].contains("author") && in["info"]["author"].is_string())	out["author"] = in["info"]["author"];
		if(in["info"].contains("shortDesc") && in["info"]["shortDesc"].is_string())	out["shortDesc"] = in["info"]["shortDesc"];
		if(in["info"].contains("version") && in["info"]["version"].is_number())	out["version"] = in["info"]["version"];
		if(in["info"].contains("revision") && in["info"]["revision"].is_number())	out["revision"] = in["info"]["revision"];
	}

	return out;
}

void findExistingFiles(nlohmann::json &json) {
	nlohmann::json current;
	chdir(Config::ScriptPath.c_str());
	std::vector<DirEntry> dirContents;
	getDirectoryContents(dirContents);
	for(uint i=0;i<dirContents.size();i++) {
		current[i] = infoFromScript(dirContents[i].name);
	}
	fixInfo(current);

	for(uint i=0;i<json.size();i++) {
		for(uint j=0;j<current.size();j++) {
			if(current[j]["title"] == json[i]["title"]) {
				Gui::DrawString(0, 0, .5f, WHITE, json[i]["title"]);
				json[i]["curRevision"] = current[j]["revision"];
			}
		}
	}
}

ScriptBrowse::ScriptBrowse() {
	DisplayMsg(Lang::get("GETTING_SCRIPT_LIST"));

	// Get repo info
	downloadToFile("https://github.com/Universal-Team/extras/raw/scripts/info/scriptInfo.json", metaFile);
	FILE* file = fopen(metaFile, "r");
	if(file)	infoJson = nlohmann::json::parse(file, nullptr, false);
	fclose(file);
	fixInfo(infoJson);
	findExistingFiles(infoJson);
	maxScripts = std::to_string(infoJson.size());
}

void ScriptBrowse::Draw(void) const {
	Gui::DrawTop();
	std::string revision = std::to_string(int64_t(infoJson[selection]["curRevision"]));
	revision += " / ";
	revision += std::to_string(int64_t(infoJson[selection]["revision"]));

	Gui::DrawString(397-Gui::GetStringWidth(0.6f, revision), 237-Gui::GetStringHeight(0.6f, revision), 0.6f, Config::TxtColor, revision);
	Gui::DrawStringCentered(0, 3, 0.8f, Config::TxtColor, std::string(infoJson[selection]["title"]), 400);

	Gui::DrawStringCentered(0, 120, 0.6f, Config::TxtColor, std::string(infoJson[selection]["shortDesc"]), 400);
	if(infoJson[selection]["curRevision"] < infoJson[selection]["revision"]) {
		Gui::DrawStringCentered(0, 215, 0.7f, Config::TxtColor, Lang::get("OUTDATED_SCRIPT"), 400);
	} else if(infoJson[selection]["curRevision"] == infoJson[selection]["revision"]) {
		Gui::DrawStringCentered(0, 215, 0.7f, Config::TxtColor, Lang::get("UP-TO-DATE"), 400);
	} else if(infoJson[selection]["curRevision"] > infoJson[selection]["revision"]) {
		Gui::DrawStringCentered(0, 215, 0.7f, Config::TxtColor, Lang::get("FUTURE_SCRIPT"), 400);
	}
	Gui::DrawBottom();
	Gui::DrawString(317-Gui::GetStringWidth(0.6f, std::to_string(selection + 1) + " / " + maxScripts), 4, 0.6f, Config::TxtColor, std::to_string(selection + 1) + " / " + maxScripts);

	if (listMode == 0) {
		for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)infoJson.size();i++) {
			if(screenPos + i == selection) {
				Gui::Draw_Rect(0, 40+(i*57), 320, 45, Config::SelectedColor);
			} else { 
				Gui::Draw_Rect(0, 40+(i*57), 320, 45, Config::UnselectedColor);
			}

			if(infoJson[screenPos+i]["curRevision"] < infoJson[screenPos+i]["revision"]) {
				Gui::Draw_Rect(300, 45+(i*59), 20, 20, C2D_Color32(0xfb, 0x5b, 0x5b, 255));
			} else {
				Gui::Draw_Rect(300, 45+(i*59), 20, 20, C2D_Color32(0xa5, 0xdd, 0x81, 255));
			}
			Gui::DrawStringCentered(0, 38+(i*57), 0.7f, Config::TxtColor, infoJson[screenPos+i]["title"], 320);
			Gui::DrawStringCentered(0, 62+(i*57), 0.7f, Config::TxtColor, infoJson[screenPos+i]["author"], 320);
		}
	} else if (listMode == 1) {
		for(int i=0;i<ENTRIES_PER_LIST && i<(int)infoJson.size();i++) {
			if(screenPosList + i == selection) {
				Gui::Draw_Rect(0, 30+(i*25), 320, 30, Config::SelectedColor);
			} else { 
				Gui::Draw_Rect(0, 30+(i*25), 320, 30, Config::UnselectedColor);
			}

			if(infoJson[screenPosList+i]["curRevision"] < infoJson[screenPosList+i]["revision"]) {
				Gui::Draw_Rect(300, 35+(i*25), 10, 10, C2D_Color32(0xfb, 0x5b, 0x5b, 255));
			} else {
				Gui::Draw_Rect(300, 35+(i*25), 10, 10, C2D_Color32(0xa5, 0xdd, 0x81, 255));
			}
			Gui::DrawStringCentered(0, 35+(i*25), 0.7f, Config::TxtColor, infoJson[screenPosList+i]["title"], 320);
		}
	}
}


void ScriptBrowse::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (keyRepeatDelay)	keyRepeatDelay--;
	if (hDown & KEY_B) {
		infoJson.clear();
		Gui::screenBack();
		return;
	}

	if (hHeld & KEY_DOWN && !keyRepeatDelay) {
		if (selection < (int)infoJson.size()-1) {
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
			selection = (int)infoJson.size()-1;
		}
		if (fastMode == true) {
			keyRepeatDelay = 3;
		} else if (fastMode == false){
			keyRepeatDelay = 6;
		}
	}

	if (hDown & KEY_A) {
		if (infoJson.size() != 0) {
			std::string fileName = Lang::get("DOWNLOADING") + std::string(infoJson[selection]["title"]);

		std::string titleFix = infoJson[selection]["title"]; 
		for (int i = 0; i < (int)titleFix.size(); i++) {
   			if (titleFix[i] == '/') {
        		titleFix[i] = '-';
    		}
		}
			DisplayMsg(fileName);

			downloadToFile(infoJson[selection]["url"], Config::ScriptPath + titleFix + ".json");
			infoJson[selection]["curRevision"] = infoJson[selection]["revision"];
		}
	}

	if (hDown & KEY_R) {
		fastMode = true;
	}

	if (hDown & KEY_L) {
		fastMode = false;
	}

	if (hDown & KEY_X) {
		if (listMode == 0) {
			listMode = 1;
		} else {
			listMode = 0;
		}
	}

	if (listMode == 0) {
		if(selection < screenPos) {
			screenPos = selection;
		} else if (selection > screenPos + ENTRIES_PER_SCREEN - 1) {
			screenPos = selection - ENTRIES_PER_SCREEN + 1;
		}
	} else if (listMode == 1) {
		if(selection < screenPosList) {
			screenPosList = selection;
		} else if (selection > screenPosList + ENTRIES_PER_LIST - 1) {
			screenPosList = selection - ENTRIES_PER_LIST + 1;
		}
	}
}