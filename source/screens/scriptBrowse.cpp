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
extern void downloadFailed();

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
				json[i]["curRevision"] = current[j]["revision"];
			}
		}
	}
}

ScriptBrowse::ScriptBrowse() {
	Msg::DisplayMsg(Lang::get("GETTING_SCRIPT_LIST"));

	// Get repo info
	if (downloadToFile("https://github.com/Universal-Team/Universal-Updater-Scripts/raw/master/info/scriptInfo.json", metaFile) != 0) {
		downloadFailed();
		loaded = false;
		return;
	}

	FILE* file = fopen(metaFile, "r");
	if(file) {
		infoJson = nlohmann::json::parse(file, nullptr, false);
		fclose(file);
		fixInfo(infoJson);
		findExistingFiles(infoJson);
		maxScripts = infoJson.size();
		loaded = true;
		// File is not able to be parsed, go screen back.
	} else {
		loaded = false;
	}
}

void ScriptBrowse::refresh() {
	if (checkWifiStatus() == true) {
		if (Msg::promptMsg(Lang::get("REFRESH_SCRIPTBROWSE_PROMPT"))) {
			Msg::DisplayMsg(Lang::get("GETTING_SCRIPT_LIST"));
			if (downloadToFile("https://github.com/Universal-Team/Universal-Updater-Scripts/raw/master/info/scriptInfo.json", metaFile) != 0) {
				downloadFailed();
				loaded = false;
				return;
			}
			FILE* file = fopen(metaFile, "r");
			if(file) {
				infoJson = nlohmann::json::parse(file, nullptr, false);
				fclose(file);
				fixInfo(infoJson);
				findExistingFiles(infoJson);
				maxScripts = infoJson.size();
				Selection = 0;
				loaded = true;
			} else {
				loaded = false;
			}
		}
	} else {
		notConnectedMsg();
	}
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
	if (loaded) {
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
		if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
		GFX::DrawBottom();
		GFX::DrawArrow(295, -1);
		GFX::DrawArrow(315, 240, 180.0);
		GFX::DrawArrow(0, 218, 0, 1);

		GFX::DrawSpriteBlend(sprites_dropdown_idx, arrowPos[3].x, arrowPos[3].y);

		Gui::DrawStringCentered(0, 1, 0.6f, Config::TxtColor, std::to_string(Selection + 1) + " | " + std::to_string(maxScripts));

		if (Config::viewMode == 0) {
			for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)infoJson.size();i++) {
				Gui::Draw_Rect(0, 40+(i*57), 320, 45, Config::UnselectedColor);
				if(screenPos + i == Selection) {
					if (!dropDownMenu) {
						Gui::drawAnimatedSelector(0, 40+(i*57), 320, 45, .060, TRANSPARENT, Config::SelectedColor);
					}
				}
				if (infoJson[screenPos+i]["curRevision"] == -1) {
					Gui::Draw_Rect(295, 45+(i*59), 20, 20, Config::notFound);
				} else if(infoJson[screenPos+i]["curRevision"] < infoJson[screenPos+i]["revision"]) {
					Gui::Draw_Rect(295, 45+(i*59), 20, 20, Config::outdated);
				} else if(infoJson[screenPos+i]["curRevision"] == infoJson[screenPos+i]["revision"]) {
					Gui::Draw_Rect(295, 45+(i*59), 20, 20, Config::uptodate);
				} else if(infoJson[screenPos+i]["curRevision"] > infoJson[screenPos+i]["revision"]) {
					Gui::Draw_Rect(295, 45+(i*59), 20, 20, Config::future);
				}

				Gui::DrawStringCentered(0, 38+(i*57), 0.7f, Config::TxtColor, infoJson[screenPos+i]["title"], 317);
				Gui::DrawStringCentered(0, 62+(i*57), 0.7f, Config::TxtColor, infoJson[screenPos+i]["author"], 317);
			}
		} else if (Config::viewMode == 1) {
			for(int i=0;i<ENTRIES_PER_LIST && i<(int)infoJson.size();i++) {
				Gui::Draw_Rect(0, (i+1)*27, 320, 25, Config::UnselectedColor);
				if(screenPosList + i == Selection) {
					if (!dropDownMenu) {
						Gui::drawAnimatedSelector(0, (i+1)*27, 320, 25, .060, TRANSPARENT, Config::SelectedColor);
					}
				}

				// Script not found.
				if (infoJson[screenPosList+i]["curRevision"] == -1) {
					Gui::Draw_Rect(302, ((i+1)*27)+7, 11, 11, Config::notFound);
					// Script outdaed.
				} else if(infoJson[screenPosList+i]["curRevision"] < infoJson[screenPosList+i]["revision"]) {
					Gui::Draw_Rect(302, ((i+1)*27)+7, 11, 11, Config::outdated);
					// Script up-to-date.
				} else if(infoJson[screenPosList+i]["curRevision"] == infoJson[screenPosList+i]["revision"]) {
					Gui::Draw_Rect(302, ((i+1)*27)+7, 11, 11, Config::uptodate);
					// Future script.
				} else if(infoJson[screenPosList+i]["curRevision"] > infoJson[screenPosList+i]["revision"]) {
					Gui::Draw_Rect(302, ((i+1)*27)+7, 11, 11, Config::future);
				}

				Gui::DrawStringCentered(0, ((i+1)*27)+1, 0.7f, Config::TxtColor, infoJson[screenPosList+i]["title"], 317);
			}
		}

		// DropDown Menu.
		if (dropDownMenu) {
			// Draw Operation Box.
			Gui::Draw_Rect(0, 25, 140, 130, Config::Color1);
			for (int i = 0; i < 3; i++) {
				if (dropSelection == i) {
					Gui::drawAnimatedSelector(dropPos2[i].x, dropPos2[i].y, dropPos2[i].w, dropPos2[i].h, .090, TRANSPARENT, Config::SelectedColor);
				} else {
					Gui::Draw_Rect(dropPos2[i].x, dropPos2[i].y, dropPos2[i].w, dropPos2[i].h, Config::UnselectedColor);
				}
			}
			// Draw Dropdown Icons.
			GFX::DrawSpriteBlend(sprites_download_all_idx, dropPos[0].x, dropPos[0].y);
			GFX::DrawSpriteBlend(sprites_update_idx, dropPos[1].x, dropPos[1].y);
			GFX::DrawSpriteBlend(sprites_view_idx, dropPos[2].x, dropPos[2].y);
			// Dropdown Text.
			Gui::DrawString(dropPos[0].x+30, dropPos[0].y+5, 0.4f, Config::TxtColor, Lang::get("DOWNLOAD_ALL_DDM"), 100);
			Gui::DrawString(dropPos[1].x+30, dropPos[1].y+5, 0.4f, Config::TxtColor, Lang::get("REFRESH_BROWSE_DDM"), 100);
			Gui::DrawString(dropPos[2].x+30, dropPos[2].y+5, 0.4f, Config::TxtColor, Lang::get("VIEW_DDM"), 100);
		}
		if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	} else {
		GFX::DrawBottom();
		if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	}
}


void ScriptBrowse::DrawGlossary(void) const {
	GFX::DrawTop();
	if (loaded) {
		if (Config::UseBars == true) {
			Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, Lang::get("GLOSSARY"), 400);
		} else {
			Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, Lang::get("GLOSSARY"), 400);
		}

		Gui::Draw_Rect(20, 30, 30, 30, Config::notFound);
		Gui::DrawString(65, 35, 0.7f, Config::TxtColor, Lang::get("SCRIPT_NOT_FOUND"), 300);

		Gui::Draw_Rect(20, 70, 30, 30, Config::outdated);
		Gui::DrawString(65, 75, 0.7f, Config::TxtColor, Lang::get("OUTDATED_SCRIPT"), 300);

		Gui::Draw_Rect(20, 110, 30, 30, Config::uptodate);
		Gui::DrawString(65, 115, 0.7f, Config::TxtColor, Lang::get("UP-TO-DATE"), 300);

		Gui::Draw_Rect(20, 150, 30, 30, Config::future);
		Gui::DrawString(65, 155, 0.7f, Config::TxtColor, Lang::get("FUTURE_SCRIPT"), 300);

		Gui::DrawString(15, 185, 0.7f, Config::TxtColor, std::to_string(int64_t(infoJson[Selection]["curRevision"])) + " | " + std::to_string(int64_t(infoJson[Selection]["revision"])), 40);
		Gui::DrawString(65, 185, 0.7f, Config::TxtColor, Lang::get("REVISION"), 300);
		if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
		GFX::DrawBottom();
		GFX::DrawSpriteBlend(sprites_download_all_idx, 20, 25);
		Gui::DrawString(50, 27, 0.6f, Config::TxtColor, Lang::get("DOWNLOAD_ALL"), 260);
		GFX::DrawSpriteBlend(sprites_view_idx, 20, 55);
		Gui::DrawString(50, 57, 0.6f, Config::TxtColor, Lang::get("CHANGE_VIEW_MODE"), 260);
		GFX::DrawArrow(20, 85);
		Gui::DrawString(50, 87, 0.6f, Config::TxtColor, Lang::get("ENTRY_UP"), 260);
		GFX::DrawArrow(42, 140, 180.0);
		Gui::DrawString(50, 117, 0.6f, Config::TxtColor, Lang::get("ENTRY_DOWN"), 260);
		GFX::DrawArrow(20, 145, 0, 1);
		Gui::DrawString(50, 147, 0.6f, Config::TxtColor, Lang::get("GO_BACK"), 260);
		Gui::DrawString(10, 177, 0.6f, Config::TxtColor, std::to_string(Selection + 1) + " | " + std::to_string(maxScripts), 35);
		Gui::DrawString(50, 177, 0.6f, Config::TxtColor, Lang::get("ENTRY"), 260);
		GFX::DrawSpriteBlend(sprites_update_idx, 20, 195);
		Gui::DrawString(50, 197, 0.6f, Config::TxtColor, Lang::get("REFRESH_SCRIPTBROWSE"), 260);
		GFX::DrawArrow(0, 218, 0, 1);
		if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	} else {
		GFX::DrawBottom();
		if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	}
}
	

void ScriptBrowse::DropDownLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (loaded) {
		if ((hDown & KEY_SELECT) || (hDown & KEY_TOUCH && touching(touch, arrowPos[3]))) {
			dropDownMenu = false;
		}

		if (hDown & KEY_DOWN) {
			if (dropSelection < 2)	dropSelection++;
		}

		if (hDown & KEY_UP) {
			if (dropSelection > 0)	dropSelection--;
		}

		if (hDown & KEY_A) {
			switch(dropSelection) {
				case 0:
					downloadAll();
					break;
				case 1:
					refresh();
					break;
				case 2:
					if (Config::viewMode == 0) {
						Config::viewMode = 1;
					} else {
						Config::viewMode = 0;
					}
					break;
			}
			dropDownMenu = false;
		}

		if (hDown & KEY_TOUCH) {
			if (touching(touch, dropPos2[0])) {
				downloadAll();
				dropDownMenu = false;
			} else if (touching(touch, dropPos2[1])) {
				refresh();
				dropDownMenu = false;
			} else if (touching(touch, dropPos2[2])) {
				if (Config::viewMode == 0) {
					Config::viewMode = 1;
				} else {
					Config::viewMode = 0;
				}
				dropDownMenu = false;
			}
		}
	}
}

void ScriptBrowse::downloadAll() {
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

void ScriptBrowse::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (loaded) {
		if (keyRepeatDelay)	keyRepeatDelay--;
		if (dropDownMenu) {
			DropDownLogic(hDown, hHeld, touch);
		} else {
			if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
				Gui::screenBack(Config::fading);
				return;
			}
			if (mode == 0) {
				if ((hHeld & KEY_DOWN && !keyRepeatDelay) || (hDown & KEY_TOUCH && touching(touch, arrowPos[1]))) {
					if (Selection < (int)infoJson.size()-1) {
						Selection++;
					} else {
						Selection = 0;
					}
					
					keyRepeatDelay = Config::keyDelay;
				}

				if ((hHeld & KEY_RIGHT && !keyRepeatDelay)) {
					if (Config::viewMode == 0) {
						if (Selection < (int)infoJson.size()-1-3) {
							Selection += 3;
						} else {
							Selection = (int)infoJson.size()-1;
						}
					} else {
						if (Selection < (int)infoJson.size()-1-6) {
							Selection += 7;
						} else {
							Selection = (int)infoJson.size()-1;
						}
					}

					keyRepeatDelay = Config::keyDelay;
				}

				if ((hHeld & KEY_LEFT && !keyRepeatDelay)) {
					if (Config::viewMode == 0) {
						if (Selection > 2) {
							Selection -= 3;
						} else {
							Selection = 0;
						}
					} else {
						if (Selection > 6) {
							Selection -= 7;
						} else {
							Selection = 0;
						}
					}

					keyRepeatDelay = Config::keyDelay;
				}

				if ((hDown & KEY_SELECT) || (hDown & KEY_TOUCH && touching(touch, arrowPos[3]))) {
					dropDownMenu = true;
				}

				if ((hHeld & KEY_UP && !keyRepeatDelay) || (hDown & KEY_TOUCH && touching(touch, arrowPos[0]))) {
					if (Selection > 0) {
						Selection--;
					} else {
						Selection = (int)infoJson.size()-1;
					}
					
					keyRepeatDelay = Config::keyDelay;
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
			}
			// Switch to Glossary and back.
			if (hDown & KEY_R || hDown & KEY_L) {
				if (mode == 0)	mode = 1;
				else	mode = 0;
			}
		}
	} else {
		if (hDown & KEY_B) {
			Gui::screenBack(Config::fading);
			return;
		}
	}
}