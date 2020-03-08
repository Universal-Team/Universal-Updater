/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

#include "download.hpp"
#include "fileBrowse.hpp"
#include "json.hpp"
#include "scriptBrowse.hpp"

#include <unistd.h>

extern bool touching(touchPosition touch, Structs::ButtonPos button);

void fixInfo(nlohmann::json &json) {
	for(uint i=0;i<json.size();i++) {
		if(!json[i].contains("title"))	json[i]["title"] = "TITLE";
		if(!json[i].contains("author"))	json[i]["author"] = "AUTHOR";
		if(!json[i].contains("shortDesc"))	json[i]["shortDesc"] = "SHORTDESC";
		if(!json[i].contains("revision"))	json[i]["revision"] = 0;
		if(!json[i].contains("curRevision"))	json[i]["curRevision"] = -1;
		if(!json[i].contains("version"))	json[i]["revision"] = -1;
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
	Msg::DisplayMsg(Lang::get("GETTING_SCRIPT_LIST"));

	// Get repo info
//	downloadToFile("https://github.com/Universal-Team/Universal-Updater-Scripts/raw/master/info/scriptInfo.json", metaFile);
	FILE* file = fopen(metaFile, "r");
	if(file)	infoJson = nlohmann::json::parse(file, nullptr, false);
	fclose(file);
	fixInfo(infoJson);
	findExistingFiles(infoJson);
	maxScripts = infoJson.size();
}

void ScriptBrowse::Draw(void) const {
	if (mode == 0) {
		DrawBrowse();
	} else {
		DrawGlossary();
	}
}

void ScriptBrowse::DrawBrowse(void) const {
	GFX::DrawTop();
	std::string revision = std::to_string(int64_t(infoJson[Selection]["curRevision"]));
	revision += " | ";
	revision += std::to_string(int64_t(infoJson[Selection]["revision"]));

	if (Config::UseBars == true) {
		Gui::DrawString(397-Gui::GetStringWidth(0.6f, revision), 239-Gui::GetStringHeight(0.6f, revision), 0.6f, Config::TxtColor, revision);
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, std::string(infoJson[Selection]["title"]), 400);
	} else {
		Gui::DrawString(397-Gui::GetStringWidth(0.6f, revision), 237-Gui::GetStringHeight(0.6f, revision), 0.6f, Config::TxtColor, revision);
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, std::string(infoJson[Selection]["title"]), 400);
	}

	Gui::DrawStringCentered(0, 120, 0.6f, Config::TxtColor, std::string(infoJson[Selection]["shortDesc"]), 400);
	if (infoJson[Selection]["curRevision"] == -1) {
		Gui::DrawStringCentered(0, 219, 0.7f, Config::TxtColor, Lang::get("SCRIPT_NOT_FOUND"), 370);
	} else if(infoJson[Selection]["curRevision"] < infoJson[Selection]["revision"]) {
		Gui::DrawStringCentered(0, 219, 0.7f, Config::TxtColor, Lang::get("OUTDATED_SCRIPT"), 370);
	} else if(infoJson[Selection]["curRevision"] == infoJson[Selection]["revision"]) {
		Gui::DrawStringCentered(0, 219, 0.7f, Config::TxtColor, Lang::get("UP-TO-DATE"), 370);
	} else if(infoJson[Selection]["curRevision"] > infoJson[Selection]["revision"]) {
		Gui::DrawStringCentered(0, 219, 0.7f, Config::TxtColor, Lang::get("FUTURE_SCRIPT"), 370);
	}
	GFX::DrawBottom();
	GFX::DrawArrow(295, -1);
	GFX::DrawArrow(315, 240, 180.0);
	GFX::DrawArrow(0, 218, 0, 1);

	GFX::DrawSpriteBlend(sprites_download_all_idx, arrowPos[3].x, arrowPos[3].y);
	GFX::DrawSpriteBlend(sprites_view_idx, arrowPos[4].x, arrowPos[4].y);
	//Gui::spriteBlend(sprites_search_idx, arrowPos[5].x, arrowPos[5].y);

	Gui::DrawStringCentered(0, 1, 0.6f, Config::TxtColor, std::to_string(Selection + 1) + " | " + std::to_string(maxScripts));

	if (Config::viewMode == 0) {
		for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)infoJson.size();i++) {
			Gui::Draw_Rect(0, 40+(i*57), 320, 45, Config::UnselectedColor);
			if(screenPos + i == Selection) {
				Gui::drawAnimatedSelector(0, 40+(i*57), 320, 45, .060, TRANSPARENT, Config::SelectedColor);
			}
			if (infoJson[screenPos+i]["curRevision"] == -1) {
				Gui::Draw_Rect(295, 45+(i*59), 20, 20, C2D_Color32(255, 128, 0, 255));
			} else if(infoJson[screenPos+i]["curRevision"] < infoJson[screenPos+i]["revision"]) {
				Gui::Draw_Rect(295, 45+(i*59), 20, 20, C2D_Color32(0xfb, 0x5b, 0x5b, 255));
			} else if(infoJson[screenPos+i]["curRevision"] == infoJson[screenPos+i]["revision"]) {
				Gui::Draw_Rect(295, 45+(i*59), 20, 20, C2D_Color32(0xa5, 0xdd, 0x81, 255));
			} else if(infoJson[screenPos+i]["curRevision"] > infoJson[screenPos+i]["revision"]) {
				Gui::Draw_Rect(295, 45+(i*59), 20, 20, C2D_Color32(255, 255, 0, 255));
			}

			Gui::DrawStringCentered(0, 38+(i*57), 0.7f, Config::TxtColor, infoJson[screenPos+i]["title"], 317);
			Gui::DrawStringCentered(0, 62+(i*57), 0.7f, Config::TxtColor, infoJson[screenPos+i]["author"], 317);
		}
	} else if (Config::viewMode == 1) {
		for(int i=0;i<ENTRIES_PER_LIST && i<(int)infoJson.size();i++) {
			Gui::Draw_Rect(0, (i+1)*27, 320, 25, Config::UnselectedColor);
			if(screenPosList + i == Selection) {
				Gui::drawAnimatedSelector(0, (i+1)*27, 320, 25, .060, TRANSPARENT, Config::SelectedColor);
			}

			// Script not found.
			if (infoJson[screenPosList+i]["curRevision"] == -1) {
				Gui::Draw_Rect(302, ((i+1)*27)+7, 11, 11, C2D_Color32(255, 128, 0, 255));
				// Script outdaed.
			} else if(infoJson[screenPosList+i]["curRevision"] < infoJson[screenPosList+i]["revision"]) {
				Gui::Draw_Rect(302, ((i+1)*27)+7, 11, 11, C2D_Color32(0xfb, 0x5b, 0x5b, 255));
				// Script up-to-date.
			} else if(infoJson[screenPosList+i]["curRevision"] == infoJson[screenPosList+i]["revision"]) {
				Gui::Draw_Rect(302, ((i+1)*27)+7, 11, 11, C2D_Color32(0xa5, 0xdd, 0x81, 255));
				// Future script.
			} else if(infoJson[screenPosList+i]["curRevision"] > infoJson[screenPosList+i]["revision"]) {
				Gui::Draw_Rect(302, ((i+1)*27)+7, 11, 11, C2D_Color32(255, 255, 0, 255));
			}

			Gui::DrawStringCentered(0, ((i+1)*27)+1, 0.7f, Config::TxtColor, infoJson[screenPosList+i]["title"], 317);
		}
	}
}


void ScriptBrowse::DrawGlossary(void) const {
	GFX::DrawTop();
	if (Config::UseBars == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, Lang::get("GLOSSARY"), 400);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, Lang::get("GLOSSARY"), 400);
	}

	Gui::Draw_Rect(20, 30, 30, 30, C2D_Color32(255, 128, 0, 255));
	Gui::DrawString(65, 35, 0.7f, Config::TxtColor, Lang::get("SCRIPT_NOT_FOUND"), 300);

	Gui::Draw_Rect(20, 70, 30, 30, C2D_Color32(0xfb, 0x5b, 0x5b, 255));
	Gui::DrawString(65, 75, 0.7f, Config::TxtColor, Lang::get("OUTDATED_SCRIPT"), 300);

	Gui::Draw_Rect(20, 110, 30, 30, C2D_Color32(0xa5, 0xdd, 0x81, 255));
	Gui::DrawString(65, 115, 0.7f, Config::TxtColor, Lang::get("UP-TO-DATE"), 300);

	Gui::Draw_Rect(20, 150, 30, 30, C2D_Color32(255, 255, 0, 255));
	Gui::DrawString(65, 155, 0.7f, Config::TxtColor, Lang::get("FUTURE_SCRIPT"), 300);

	Gui::DrawString(15, 185, 0.7f, Config::TxtColor, std::to_string(int64_t(infoJson[Selection]["curRevision"])) + " | " + std::to_string(int64_t(infoJson[Selection]["revision"])), 40);
	Gui::DrawString(65, 185, 0.7f, Config::TxtColor, Lang::get("REVISION"), 300);
	GFX::DrawBottom();

	GFX::DrawSpriteBlend(sprites_download_all_idx, 20, 40);
	Gui::DrawString(50, 42, 0.6f, Config::TxtColor, Lang::get("DOWNLOAD_ALL"), 260);
	GFX::DrawSpriteBlend(sprites_view_idx, 20, 70);
	Gui::DrawString(50, 72, 0.6f, Config::TxtColor, Lang::get("CHANGE_VIEW_MODE"), 260);

	GFX::DrawArrow(20, 100);
	Gui::DrawString(50, 102, 0.6f, Config::TxtColor, Lang::get("ENTRY_UP"), 260);
	GFX::DrawArrow(42, 155, 180.0);
	Gui::DrawString(50, 132, 0.6f, Config::TxtColor, Lang::get("ENTRY_DOWN"), 260);
	GFX::DrawArrow(20, 160, 0, 1);
	Gui::DrawString(50, 162, 0.6f, Config::TxtColor, Lang::get("GO_BACK"), 260);

	Gui::DrawString(10, 192, 0.6f, Config::TxtColor, std::to_string(Selection + 1) + " | " + std::to_string(maxScripts), 35);
	Gui::DrawString(50, 192, 0.6f, Config::TxtColor, Lang::get("ENTRY"), 260);

	GFX::DrawArrow(0, 218, 0, 1);
}

void ScriptBrowse::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (keyRepeatDelay)	keyRepeatDelay--;
	if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		infoJson.clear();
		Gui::screenBack();
		return;
	}

	if (mode == 0) {
		if ((hHeld & KEY_DOWN && !keyRepeatDelay) || (hDown & KEY_TOUCH && touching(touch, arrowPos[1]))) {
			if (Selection < (int)infoJson.size()-1) {
				Selection++;
			} else {
				Selection = 0;
			}
			if (fastMode == true) {
				keyRepeatDelay = 3;
			} else if (fastMode == false){
				keyRepeatDelay = 6;
			}
		}

		if ((hHeld & KEY_UP && !keyRepeatDelay) || (hDown & KEY_TOUCH && touching(touch, arrowPos[0]))) {
			if (Selection > 0) {
				Selection--;
			} else {
				Selection = (int)infoJson.size()-1;
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
							Msg::DisplayMsg(fileName);
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
							Msg::DisplayMsg(fileName);
							downloadToFile(infoJson[screenPosList + i]["url"], Config::ScriptPath + titleFix + ".json");
							infoJson[screenPosList + i]["curRevision"] = infoJson[screenPosList + i]["revision"];
						}
					}
				}
			}
		}

		if (hDown & KEY_A) {
			if (infoJson.size() != 0) {
				std::string fileName = Lang::get("DOWNLOADING") + std::string(infoJson[Selection]["title"]);

			std::string titleFix = infoJson[Selection]["title"]; 
			for (int i = 0; i < (int)titleFix.size(); i++) {
				if (titleFix[i] == '/') {
					titleFix[i] = '-';
				}
			}
				Msg::DisplayMsg(fileName);	

				downloadToFile(infoJson[Selection]["url"], Config::ScriptPath + titleFix + ".json");
				infoJson[Selection]["curRevision"] = infoJson[Selection]["revision"];
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
			if(Selection < screenPos) {
				screenPos = Selection;
			} else if (Selection > screenPos + ENTRIES_PER_SCREEN - 1) {
				screenPos = Selection - ENTRIES_PER_SCREEN + 1;
			}
		} else if (Config::viewMode == 1) {
			if(Selection < screenPosList) {
				screenPosList = Selection;
			} else if (Selection > screenPosList + ENTRIES_PER_LIST - 1) {
				screenPosList = Selection - ENTRIES_PER_LIST + 1;
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
					Msg::DisplayMsg(fileName + " " + std::to_string(current) + " / " + std::to_string(total));
					downloadToFile(infoJson[i]["url"], Config::ScriptPath + titleFix + ".json");
					infoJson[i]["curRevision"] = infoJson[i]["revision"];
				}
			}
		}
	}

	// Switch to Glossary and back.
	if (hDown & KEY_RIGHT || hDown & KEY_LEFT) {
		if (mode == 0)	mode = 1;
		else	mode = 0;
	}
}