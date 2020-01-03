/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

#include "screens/scriptBrowse.hpp"

#include "utils/fileBrowse.h"
#include "utils/json.hpp"

#include <unistd.h>

extern bool touching(touchPosition touch, Structs::ButtonPos button);

nlohmann::json infoJson;

#define metaFile "sdmc:/3ds/Universal-Updater/ScriptInfo.json"

std::string maxScripts;

void fixInfo(nlohmann::json &json) {
	for(uint i=0;i<json.size();i++) {
		if(!json[i].contains("title"))	json[i]["title"] = "TITLE";
		if(!json[i].contains("author"))	json[i]["author"] = "AUTHOR";
		if(!json[i].contains("shortDesc"))	json[i]["shortDesc"] = "SHORTDESC";
		if(!json[i].contains("revision"))	json[i]["revision"] = 0;
		if(!json[i].contains("curRevision"))	json[i]["curRevision"] = 0;
		if(!json[i].contains("version"))	json[i]["revision"] = 0;
	}
}

nlohmann::json infoFromScript(const std::string &path) {
	nlohmann::json in, out;

	FILE* file = fopen(path.c_str(), "r");
	if(!file) {
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

	if (Config::UseBars == true) {
		Gui::DrawString(397-Gui::GetStringWidth(0.6f, revision), 239-Gui::GetStringHeight(0.6f, revision), 0.6f, Config::TxtColor, revision);
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, std::string(infoJson[selection]["title"]), 400);
	} else {
		Gui::DrawString(397-Gui::GetStringWidth(0.6f, revision), 237-Gui::GetStringHeight(0.6f, revision), 0.6f, Config::TxtColor, revision);
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, std::string(infoJson[selection]["title"]), 400);
	}

	Gui::DrawStringCentered(0, 120, 0.6f, Config::TxtColor, std::string(infoJson[selection]["shortDesc"]), 400);
	if(infoJson[selection]["curRevision"] < infoJson[selection]["revision"]) {
		Gui::DrawStringCentered(0, 219, 0.7f, Config::TxtColor, Lang::get("OUTDATED_SCRIPT"), 370);
	} else if(infoJson[selection]["curRevision"] == infoJson[selection]["revision"]) {
		Gui::DrawStringCentered(0, 219, 0.7f, Config::TxtColor, Lang::get("UP-TO-DATE"), 370);
	} else if(infoJson[selection]["curRevision"] > infoJson[selection]["revision"]) {
		Gui::DrawStringCentered(0, 219, 0.7f, Config::TxtColor, Lang::get("FUTURE_SCRIPT"), 370);
	}
	Gui::DrawBottom();
	Gui::DrawArrow(295, -1);
	Gui::DrawArrow(315, 240, 180.0);
	Gui::DrawArrow(0, 218, 0, 1);

	Gui::spriteBlend(sprites_download_all_idx, arrowPos[3].x, arrowPos[3].y);
	Gui::spriteBlend(sprites_view_idx, arrowPos[4].x, arrowPos[4].y);
	//Gui::spriteBlend(sprites_search_idx, arrowPos[5].x, arrowPos[5].y);

	Gui::DrawStringCentered(-23, 1, 0.6f, Config::TxtColor, std::to_string(selection + 1) + " / " + maxScripts);

	if (Config::viewMode == 0) {
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
	} else if (Config::viewMode == 1) {
		for(int i=0;i<ENTRIES_PER_LIST && i<(int)infoJson.size();i++) {
			if(screenPosList + i == selection) {
				Gui::Draw_Rect(0, (i+1)*27, 320, 25, Config::SelectedColor);
			} else { 
				Gui::Draw_Rect(0, (i+1)*27, 320, 25, Config::UnselectedColor);
			}

			if(infoJson[screenPosList+i]["curRevision"] < infoJson[screenPosList+i]["revision"]) {
				Gui::Draw_Rect(302, ((i+1)*27)+7, 11, 11, C2D_Color32(0xfb, 0x5b, 0x5b, 255));
			} else {
				Gui::Draw_Rect(302, ((i+1)*27)+7, 11, 11, C2D_Color32(0xa5, 0xdd, 0x81, 255));
			}
			Gui::DrawStringCentered(0, ((i+1)*27)+1, 0.7f, Config::TxtColor, infoJson[screenPosList+i]["title"], 320);
		}
	}
}


void ScriptBrowse::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (keyRepeatDelay)	keyRepeatDelay--;
	if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		infoJson.clear();
		Screen::back();
		return;
	}

	if ((hHeld & KEY_DOWN && !keyRepeatDelay) || (hDown & KEY_TOUCH && touching(touch, arrowPos[1]))) {
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

	if ((hHeld & KEY_UP && !keyRepeatDelay) || (hDown & KEY_TOUCH && touching(touch, arrowPos[0]))) {
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

	if (hDown & KEY_TOUCH) {
		if (Config::viewMode == 0) {
			for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)infoJson.size();i++) {
				if(touch.py > 40+(i*57) && touch.py < 40+(i*57)+45) {
					if (infoJson.size() != 0) {
						std::string fileName = Lang::get("DOWNLOADING") + std::string(infoJson[screenPos + i]["title"]);
						std::string titleFix = infoJson[screenPos + i]["title"]; 
						for (int l = 0; l < (int)titleFix.size(); l++) {
							if (titleFix[l] == '/') {
								titleFix[l] = '-';
							}
						}
						DisplayMsg(fileName);
						downloadToFile(infoJson[screenPos + i]["url"], Config::ScriptPath + titleFix + ".json");
						infoJson[screenPos + i]["curRevision"] = infoJson[screenPos + i]["revision"];
					}
				}
			}
		} else if (Config::viewMode == 1) {
			for(int i=0;i<ENTRIES_PER_LIST && i<(int)infoJson.size();i++) {
				if(touch.py > (i+1)*27 && touch.py < (i+2)*27) {
					if (infoJson.size() != 0) {
						std::string fileName = Lang::get("DOWNLOADING") + std::string(infoJson[screenPosList + i]["title"]);
						std::string titleFix = infoJson[screenPosList + i]["title"]; 
						for (int l = 0; l < (int)titleFix.size(); l++) {
							if (titleFix[l] == '/') {
								titleFix[l] = '-';
							}
						}
						DisplayMsg(fileName);
						downloadToFile(infoJson[screenPosList + i]["url"], Config::ScriptPath + titleFix + ".json");
						infoJson[screenPosList + i]["curRevision"] = infoJson[screenPosList + i]["revision"];
					}
				}
			}
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

	if ((hDown & KEY_X) || (hDown & KEY_TOUCH && touching(touch, arrowPos[4]))) {
		if (Config::viewMode == 0) {
			Config::viewMode = 1;
		} else {
			Config::viewMode = 0;
		}
	}

	if (Config::viewMode == 0) {
		if(selection < screenPos) {
			screenPos = selection;
		} else if (selection > screenPos + ENTRIES_PER_SCREEN - 1) {
			screenPos = selection - ENTRIES_PER_SCREEN + 1;
		}
	} else if (Config::viewMode == 1) {
		if(selection < screenPosList) {
			screenPosList = selection;
		} else if (selection > screenPosList + ENTRIES_PER_LIST - 1) {
			screenPosList = selection - ENTRIES_PER_LIST + 1;
		}
	}

	if ((hDown & KEY_Y) || (hDown & KEY_TOUCH && touching(touch, arrowPos[3]))) {
		if (infoJson.size() != 0) {
			for (int i = 0; i < (int)infoJson.size(); i++) {
				int current = i+1;
				int total = infoJson.size();
				std::string fileName = Lang::get("DOWNLOADING") + std::string(infoJson[i]["title"]);
				std::string titleFix = infoJson[i]["title"]; 
				for (int l = 0; l < (int)titleFix.size(); l++) {
					if (titleFix[l] == '/') {
						titleFix[l] = '-';
					}
				}
				DisplayMsg(fileName + " " + std::to_string(current) + " / " + std::to_string(total));
				downloadToFile(infoJson[i]["url"], Config::ScriptPath + titleFix + ".json");
				infoJson[i]["curRevision"] = infoJson[i]["revision"];
			}
		}
	}
}