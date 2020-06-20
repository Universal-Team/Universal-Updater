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
#include "formatting.hpp"
#include "json.hpp"
#include "scriptHelper.hpp"
#include "unistore_v1.hpp"
#include <unistd.h>

extern std::unique_ptr<Config> config;
extern bool touching(touchPosition touch, Structs::ButtonPos button);
extern u32 getColor(std::string colorString);
extern bool isScriptSelected;

extern u32 barColor;
extern u32 bgTopColor;
extern u32 bgBottomColor;
extern u32 TextColor;
extern u32 progressBar;
extern u32 selected;
extern u32 unselected;
extern bool didAutoboot;

UniStoreV1::UniStoreV1(nlohmann::json &JSON, const std::string sheetPath, bool displayInf) {
	this->storeJson = JSON;
	this->displayInformations = displayInf;

	if (access(sheetPath.c_str(), F_OK) != 0) {
		this->sheetHasLoaded = false;
	} else {
		Gui::loadSheet(sheetPath.c_str(), this->sheet);
		this->sheetHasLoaded = true;
	}

	u32 colorTemp;
	colorTemp = getColor(ScriptHelper::getString(this->storeJson, "storeInfo", "barColor"));
	barColor = colorTemp == 0 ? config->barColor() : colorTemp;

	colorTemp = getColor(ScriptHelper::getString(this->storeJson, "storeInfo", "bgTopColor"));
	bgTopColor = colorTemp == 0 ? config->topBG() : colorTemp;

	colorTemp = getColor(ScriptHelper::getString(this->storeJson, "storeInfo", "bgBottomColor"));
	bgBottomColor = colorTemp == 0 ? config->bottomBG() : colorTemp;

	colorTemp = getColor(ScriptHelper::getString(this->storeJson, "storeInfo", "textColor"));
	TextColor = colorTemp == 0 ? config->textColor() : colorTemp;

	colorTemp = getColor(ScriptHelper::getString(this->storeJson, "storeInfo", "selectedColor"));
	selected = colorTemp == 0 ? config->selectedColor() : colorTemp;

	colorTemp = getColor(ScriptHelper::getString(this->storeJson, "storeInfo", "unselectedColor"));
	unselected = colorTemp == 0 ? config->unselectedColor() : colorTemp;

	colorTemp = getColor(ScriptHelper::getString(this->storeJson, "storeInfo", "progressbarColor"));
	progressBar = colorTemp == 0 ? config->progressbarColor() : colorTemp;
}

void UniStoreV1::drawBlend(int key, int x, int y) const {
	C2D_ImageTint tint;
	C2D_SetImageTint(&tint, C2D_TopLeft, C2D_Color32(0, 0, 0, 180), 0.5);
	C2D_SetImageTint(&tint, C2D_TopRight, C2D_Color32(0, 0, 0, 180), 0.5);
	C2D_SetImageTint(&tint, C2D_BotLeft, C2D_Color32(0, 0, 0, 180), 0.5);
	C2D_SetImageTint(&tint, C2D_BotRight, C2D_Color32(0, 0, 0, 180), 0.5);
	C2D_DrawImageAt(C2D_SpriteSheetGetImage(this->sheet, key), x, y, 0.5f, &tint);
}

void UniStoreV1::parseObjects() {
	this->objects.clear();
	for(auto it = this->storeJson.begin();it != this->storeJson.end(); it++) {
		if (it.key() != "storeInfo") {
			this->objects.push_back(it.key());
		}
	}
}

UniStoreV1::~UniStoreV1() {
	// Only unload if sheet has loaded.
	if (this->sheetHasLoaded) {
		Gui::unloadSheet(this->sheet);
	}
}

void UniStoreV1::Draw(void) const {
	std::string entryAmount = std::to_string(this->Selection+1) + " | " + std::to_string((int)this->storeJson.at("storeContent").size());
	std::string info;
	GFX::DrawTop();
	// Top Background.
	if (this->storeJson.at("storeInfo").contains("iconIndexTop") && sheetHasLoaded == true) {
		Gui::DrawSprite(this->sheet, this->storeJson["storeInfo"]["iconIndexTop"], 0, 0);
	}

	// Icon.
	if (this->storeJson.at("storeContent").at(Selection).at("info").contains("iconIndex") && sheetHasLoaded == true) {
		if (this->storeJson.at("storeContent").at(Selection).at("info").contains("posX") && this->storeJson.at("storeContent").at(Selection).at("info").contains("posY")) {
			Gui::DrawSprite(this->sheet, this->storeJson["storeContent"][Selection]["info"]["iconIndex"], this->storeJson["storeContent"][Selection]["info"]["posX"], this->storeJson["storeContent"][Selection]["info"]["posY"]);
		} else {
			Gui::DrawSprite(this->sheet, this->storeJson["storeContent"][Selection]["info"]["iconIndex"], 175, 155);
		}
	}

	if (displayInformations != false) {
		if (config->useBars() == true) {
			Gui::DrawStringCentered(0, 0, 0.7f, TextColor, std::string(this->storeJson["storeInfo"]["title"]), 400);
			Gui::DrawString(397-Gui::GetStringWidth(0.6f, entryAmount), 239-Gui::GetStringHeight(0.6f, entryAmount), 0.6f, TextColor, entryAmount);
		} else {
			Gui::DrawStringCentered(0, 2, 0.7f, TextColor, std::string(this->storeJson["storeInfo"]["title"]), 400);
			Gui::DrawString(397-Gui::GetStringWidth(0.6f, entryAmount), 237-Gui::GetStringHeight(0.6f, entryAmount), 0.6f, TextColor, entryAmount);
		}

		Gui::DrawStringCentered(0, 32, 0.6f, TextColor, Lang::get("TITLE") + std::string(this->storeJson["storeContent"][Selection]["info"]["title"]), 400);
		Gui::DrawStringCentered(0, 57, 0.6f, TextColor, Lang::get("AUTHOR") + std::string(this->storeJson["storeContent"][Selection]["info"]["author"]), 400);
		Gui::DrawStringCentered(0, 82, 0.6f, TextColor, Lang::get("DESC") + std::string(this->storeJson["storeContent"][Selection]["info"]["description"]), 400);

		if (this->storeJson["storeContent"][Selection]["info"]["version"] != "") {
			Gui::DrawStringCentered(0, 107, 0.6f, TextColor, Lang::get("VERSION") + std::string(this->storeJson["storeContent"][Selection]["info"]["version"]), 400);
		} else {
			Gui::DrawStringCentered(0, 107, 0.6f, TextColor, Lang::get("VERSION") + Lang::get("UNKNOWN"), 400);
		}

		if (this->storeJson["storeContent"][Selection]["info"]["fileSize"] != 0) {
			Gui::DrawStringCentered(0, 132, 0.6f, TextColor, Lang::get("FILE_SIZE") + formatBytes(int64_t(this->storeJson["storeContent"][Selection]["info"]["fileSize"])), 400);
		} else {
			Gui::DrawStringCentered(0, 132, 0.6f, TextColor, Lang::get("FILE_SIZE") + Lang::get("UNKNOWN"), 400);
		}
	}
	
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	GFX::DrawBottom();

	// Bottom Background.
	if (this->storeJson.at("storeInfo").contains("iconIndexBottom") && sheetHasLoaded == true) {
		Gui::DrawSprite(this->sheet, this->storeJson["storeInfo"]["iconIndexBottom"], 0, 0);
	}

	GFX::DrawArrow(295, -1);
	GFX::DrawArrow(315, 240, 180.0);
	GFX::DrawArrow(0, 218, 0, 1);
	GFX::DrawSpriteBlend(sprites_dropdown_idx, arrowPos[3].x, arrowPos[3].y);

	if (config->viewMode() == 0) {
		for(int i = 0; i < ENTRIES_PER_SCREEN && i < (int)this->storeJson.at("storeContent").size(); i++) {
			info = this->storeJson["storeContent"][screenPos + i]["info"]["title"];
			if (screenPos + i == Selection) {
				if (this->storeJson.at("storeInfo").contains("buttonLarge") && sheetHasLoaded == true) {
					Gui::DrawSprite(this->sheet, this->storeJson["storeInfo"]["buttonLarge"], 0, 40+(i*57));
				} else {
					if (!dropDownMenu) {
						Gui::drawAnimatedSelector(0, 40+(i*57), 320, 45, .060, TRANSPARENT, selected);
					}
				}
			} else {
				if (this->storeJson.at("storeInfo").contains("buttonLarge") && sheetHasLoaded == true) {
					this->drawBlend(this->storeJson["storeInfo"]["buttonLarge"], 0, 40+(i*57));
				} else {
					Gui::Draw_Rect(0, 40+(i*57), 320, 45, unselected);
				}
			}
			Gui::DrawStringCentered(0, 50+(i*57), 0.7f, TextColor, info, 320);
		}
	} else if (config->viewMode() == 1) {
		for(int i = 0; i < ENTRIES_PER_LIST && i < (int)this->storeJson.at("storeContent").size(); i++) {
			info = this->storeJson["storeContent"][screenPosList + i]["info"]["title"];
			if (screenPosList + i == Selection) {
				if (this->storeJson.at("storeInfo").contains("buttonSmall") && sheetHasLoaded == true) {
					Gui::DrawSprite(this->sheet, this->storeJson["storeInfo"]["buttonSmall"], 0, (i+1)*27);
				} else {
					if (!dropDownMenu) {
						Gui::drawAnimatedSelector(0, (i+1)*27, 320, 25, .060, TRANSPARENT, selected);
					}
				}
			} else {
				if (this->storeJson.at("storeInfo").contains("buttonSmall") && sheetHasLoaded == true) {
					this->drawBlend(this->storeJson["storeInfo"]["buttonSmall"], 0, (i+1)*27);
				} else {
					Gui::Draw_Rect(0, (i+1)*27, 320, 25, unselected);
				}
			}
			Gui::DrawStringCentered(0, ((i+1)*27)+1, 0.7f, TextColor, info, 320);
		}
	}

	// DropDown Menu.
	if (dropDownMenu) {
		// Draw Operation Box.
		Gui::Draw_Rect(0, 25, 140, 44, barColor);
		Gui::drawAnimatedSelector(dropPos[0].x, dropPos[0].y, dropPos[0].w, dropPos[0].h, .090, TRANSPARENT, selected);
		// Draw Dropdown Icons.
		GFX::DrawSpriteBlend(sprites_view_idx, dropPos[0].x, dropPos[0].y);
		// Dropdown Text.
		Gui::DrawString(dropPos[0].x+30, dropPos[0].y+5, 0.4f, TextColor, Lang::get("VIEW_DDM"), 100);
	}

	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
}

void UniStoreV1::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (keyRepeatDelay)	keyRepeatDelay--;

	//DropDown Logic.
	if (dropDownMenu) {
		if ((hDown & KEY_SELECT) || (hDown & KEY_TOUCH && touching(touch, arrowPos[3]))) {
			dropDownMenu = false;
		}
		if (hDown & KEY_DOWN) {
			if (dropSelection < 1)	dropSelection++;
		}
		if (hDown & KEY_UP) {
			if (dropSelection > 0)	dropSelection--;
		}

		if (hDown & KEY_A) {
			if (config->viewMode() == 0) {
				config->viewMode(1);
			} else {
				config->viewMode(0);
			}
			dropDownMenu = false;
		}

		if (hDown & KEY_TOUCH) {
			if (touching(touch, dropPos[0])) {
				if (config->viewMode() == 0) {
					config->viewMode(1);
				} else {
					config->viewMode(0);
				}
				dropDownMenu = false;
			}
		}
	} else {
		if ((hDown & KEY_SELECT) || (hDown & KEY_TOUCH && touching(touch, arrowPos[3]))) {
			dropSelection = 0;
			dropDownMenu = true;
		}

		if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
			if (!didAutoboot) didAutoboot = true;
			Gui::screenBack(config->screenFade());
			return;
		}

		// Go one entry up.
		if ((hHeld & KEY_UP && !keyRepeatDelay) || (hDown & KEY_TOUCH && touching(touch, arrowPos[0]))) {
			if (Selection > 0) {
				Selection--;
			} else {
				Selection = (int)this->storeJson.at("storeContent").size()-1;
			}
			
			keyRepeatDelay = config->keyDelay();
		}

		// Go one entry down.
		if ((hHeld & KEY_DOWN && !keyRepeatDelay) || (hDown & KEY_TOUCH && touching(touch, arrowPos[1]))) {
			if (Selection < (int)this->storeJson.at("storeContent").size()-1) {
				Selection++;
			} else {
				Selection = 0;
			}
			
			keyRepeatDelay = config->keyDelay();
		}

		if ((hHeld & KEY_RIGHT && !keyRepeatDelay)) {
			if (config->viewMode() == 0) {
				if (Selection < (int)this->storeJson.at("storeContent").size()-1-3) {
					Selection += 3;
				} else {
					Selection = (int)this->storeJson.at("storeContent").size()-1;
				}
			} else {
				if (Selection < (int)this->storeJson.at("storeContent").size()-1-6) {
					Selection += 7;
				} else {
					Selection = (int)this->storeJson.at("storeContent").size()-1;
				}
			}
			
			keyRepeatDelay = config->keyDelay();
		}

		if ((hHeld & KEY_LEFT && !keyRepeatDelay)) {
			if (config->viewMode() == 0) {
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
			
			keyRepeatDelay = config->keyDelay();
		}

		// Execute touched Entry.
		if (hDown & KEY_TOUCH) {
			if (config->viewMode() == 0) {
				for(int i = 0; i < ENTRIES_PER_SCREEN && i < (int)this->storeJson.at("storeContent").size(); i++) {
					if (touch.py > 40+(i*57) && touch.py < 40+(i*57)+45) {
						Selection = screenPos + i;
						std::string info = this->storeJson["storeContent"][Selection]["info"]["title"];
						if (Msg::promptMsg(Lang::get("EXECUTE_STORE") + "\n\n" + info)) {
							execute();
						}
					}
				}
			} else if (config->viewMode() == 1) {
				for(int i = 0; i < ENTRIES_PER_LIST && i < (int)this->storeJson.at("storeContent").size(); i++) {
					if (touch.py > (i+1)*27 && touch.py < (i+2)*27) {
						Selection = screenPosList + i;
						std::string info = this->storeJson["storeContent"][Selection]["info"]["title"];
						if (Msg::promptMsg(Lang::get("EXECUTE_STORE") + "\n\n" + info)) {
							execute();
						}
					}
				}
			}
		}
		// Execute that Entry.
		if (hDown & KEY_A) {
			std::string info = this->storeJson["storeContent"][Selection]["info"]["title"];
			if (Msg::promptMsg(Lang::get("EXECUTE_STORE") + "\n\n" + info)) {
				execute();
			}
		}

		if (config->viewMode() == 0) {
			if (Selection < screenPos) {
				screenPos = Selection;
			} else if (Selection > screenPos + ENTRIES_PER_SCREEN - 1) {
				screenPos = Selection - ENTRIES_PER_SCREEN + 1;
			}
		} else if (config->viewMode() == 1) {
			if (Selection < screenPosList) {
				screenPosList = Selection;
			} else if (Selection > screenPosList + ENTRIES_PER_LIST - 1) {
				screenPosList = Selection - ENTRIES_PER_LIST + 1;
			}
		}
	}
}


// Execute Entry.
Result UniStoreV1::execute() {
	Result ret = NONE; // No Error has been occured now.
	for(int i = 0; i < (int)this->storeJson.at("storeContent").at(Selection).at("script").size(); i++) {
		if (ret == NONE) {
			std::string type = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("type");
			if (type == "deleteFile") {
				bool missing = false;
				std::string file, message;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("file"))	file = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("file");
				else	missing = true;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("message"))	message = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("message");
				if (!missing)	ret = ScriptHelper::removeFile(file, message);
				else	ret = SYNTAX_ERROR;

			} else if (type == "downloadFile") {
				bool missing = false;
				std::string file, output, message;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("file"))	file = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("file");
				else	missing = true;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("output"))	output = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("output");
				else	missing = true;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("message"))	message = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("message");
				if (!missing)	ret = ScriptHelper::downloadFile(file, output, message);
				else	ret = SYNTAX_ERROR;

			} else if (type == "downloadRelease") {
				bool missing = false, includePrereleases = false, showVersions = false;
				std::string repo, file, output, message;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("repo"))	repo = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("repo");
				else	missing = true;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("file"))	file = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("file");
				else	missing = true;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("output"))	output = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("output");
				else	missing = true;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("includePrereleases") && this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("includePrereleases").is_boolean())
					includePrereleases = this->storeJson.at(Selection).at("script").at(i).at("includePrereleases");
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("showVersions") && this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("showVersions").is_boolean())
					showVersions = this->storeJson.at(Selection).at("script").at(i).at("showVersions");
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("message"))	message = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("message");
				if (!missing)	ret = ScriptHelper::downloadRelease(repo, file, output, includePrereleases, showVersions, message);
				else	ret = SYNTAX_ERROR;

			} else if (type == "extractFile") {
				bool missing = false;
				std::string file, input, output, message;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("file"))	file = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("file");
				else	missing = true;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("input"))	input = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("input");
				else	missing = true;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("output"))	output = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("output");
				else	missing = true;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("message"))	message = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("message");
				if (!missing)	ScriptHelper::extractFile(file, input, output, message);
				else	ret = SYNTAX_ERROR;

			} else if (type == "installCia") {
				bool missing = false, updateSelf = false;
				std::string file, message;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("file"))	file = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("file");
				else	missing = true;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("updateSelf") && this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("updateSelf").is_boolean()) {
					updateSelf = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("updateSelf");
				}
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("message"))	message = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("message");
				if (!missing)	ScriptHelper::installFile(file, updateSelf, message);
				else	ret = SYNTAX_ERROR;
	
			} else if (type == "mkdir") {
				bool missing = false;
				std::string directory, message;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("directory"))	directory = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("directory");
				else	missing = true;
				if (!missing)	makeDirs(directory.c_str());
				else	ret = SYNTAX_ERROR;

			} else if (type == "rmdir") {
				bool missing = false;
				std::string directory, message, promptmsg;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("directory"))	directory = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("directory");
				else	missing = true;
				promptmsg = Lang::get("DELETE_PROMPT") + "\n" + directory;
				if (!missing) {
					if (access(directory.c_str(), F_OK) != 0 ) {
						ret = DELETE_ERROR;
					} else {
						if (Msg::promptMsg(promptmsg)) {
							removeDirRecursive(directory.c_str());
						}
					}
				}
				else	ret = SYNTAX_ERROR;

			} else if (type == "mkfile") {
				bool missing = false;
				std::string file;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("file"))	file = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("file");
				else	missing = true;
				if (!missing)	ScriptHelper::createFile(file.c_str());
				else	ret = SYNTAX_ERROR;

			} else if (type == "timeMsg") {
				bool missing = false;
				std::string message;
				int seconds;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("message"))	message = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("message");
				else	missing = true;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("seconds") && this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("seconds").is_number())
				seconds = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("seconds");
				else	missing = true;
				if (!missing)	ScriptHelper::displayTimeMsg(message, seconds);
				else	ret = SYNTAX_ERROR;

			} else if (type == "saveConfig") {
				config->save();

			} else if (type == "notImplemented") {
				notImplemented();

			} else if (type == "bootTitle") {
				std::string TitleID = "";
				std::string message = "";
				bool isNAND = false, missing = false;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("TitleID"))	TitleID = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("TitleID");
				else	missing = true;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("NAND") && this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("NAND").is_boolean())	isNAND = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("NAND");
				else	missing = true;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("message"))	message = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("message");
				else	missing = true;
				if (!missing)	ScriptHelper::bootTitle(TitleID, isNAND, message);
				else	ret = SYNTAX_ERROR;
				
			} else if (type == "promptMessage") {
				std::string Message = "";
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("message"))	Message = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("message");
				ret = ScriptHelper::prompt(Message);

			} else if (type == "copy") {
				std::string Message = "", source = "", destination = "";
				bool missing = false;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("source"))	source = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("source");
				else	missing = true;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("destination"))	destination = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("destination");
				else	missing = true;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("message"))	Message = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("message");
				if (!missing)	ret = ScriptHelper::copyFile(source, destination, Message);
				else	ret = SYNTAX_ERROR;

			} else if (type == "move") {
				std::string Message = "", oldFile = "", newFile = "";
				bool missing = false;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("old"))	oldFile = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("old");
				else	missing = true;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("new"))	newFile = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("new");
				else	missing = true;
				if (this->storeJson.at("storeContent").at(Selection).at("script").at(i).contains("message"))	Message = this->storeJson.at("storeContent").at(Selection).at("script").at(i).at("message");
				if (!missing)	ret = ScriptHelper::renameFile(oldFile, newFile, Message);
				else	ret = SYNTAX_ERROR;
			}
		}
	}
	if (ret == NONE)	doneMsg();
	else if (ret == FAILED_DOWNLOAD)	Msg::DisplayWarnMsg(Lang::get("DOWNLOAD_ERROR"));
	else if (ret == SCRIPT_CANCELED)	Msg::DisplayWarnMsg(Lang::get("SCRIPT_CANCELED"));
	else if (ret == SYNTAX_ERROR)		Msg::DisplayWarnMsg(Lang::get("SYNTAX_ERROR"));
	else if (ret == COPY_ERROR)			Msg::DisplayWarnMsg(Lang::get("COPY_ERROR"));
	else if (ret == MOVE_ERROR)			Msg::DisplayWarnMsg(Lang::get("MOVE_ERROR"));
	else if (ret == DELETE_ERROR)		Msg::DisplayWarnMsg(Lang::get("DELETE_ERROR"));
	return ret;
}