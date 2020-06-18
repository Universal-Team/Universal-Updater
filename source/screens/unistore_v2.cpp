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
#include "json.hpp"
#include "scriptHelper.hpp"
#include "unistore_v2.hpp"

#include <unistd.h>

extern u32 getColor(std::string colorString);
extern bool touching(touchPosition touch, Structs::ButtonPos button);
#define STORE_ENTRIES	 9
#define DOWNLOAD_ENTRIES 5

UniStoreV2::UniStoreV2(nlohmann::json &JSON, const std::string sheetPath) {
	this->DBJson = JSON;

	if (access(sheetPath.c_str(), F_OK) != 0) {
		this->sheetLoaded = false;
	} else {
		Gui::loadSheet(sheetPath.c_str(), this->sheet);
		this->sheetLoaded = true;
	}

	// Get colors.
	this->barColorLight = getColor(ScriptHelper::getString(this->DBJson, "storeInfo", "barLight"));
	this->barColorDark  = getColor(ScriptHelper::getString(this->DBJson, "storeInfo", "barDark"));

	this->bgColorLight = getColor(ScriptHelper::getString(this->DBJson, "storeInfo", "bgLight"));
	this->bgColorDark  = getColor(ScriptHelper::getString(this->DBJson, "storeInfo", "bgDark"));

	this->textColorLight = getColor(ScriptHelper::getString(this->DBJson, "storeInfo", "textLight"));
	this->textColorDark  = getColor(ScriptHelper::getString(this->DBJson, "storeInfo", "textDark"));

	this->boxColorLight = getColor(ScriptHelper::getString(this->DBJson, "storeInfo", "boxLight"));
	this->boxColorDark  = getColor(ScriptHelper::getString(this->DBJson, "storeInfo", "boxDark"));

	this->outlineColorLight = getColor(ScriptHelper::getString(this->DBJson, "storeInfo", "outlineLight"));
	this->outlineColorDark  = getColor(ScriptHelper::getString(this->DBJson, "storeInfo", "outlineDark"));
}

UniStoreV2::~UniStoreV2() {
	// Only unload if sheet has loaded.
	if (this->sheetLoaded) {
		Gui::unloadSheet(this->sheet);
	}
}


u32 UniStoreV2::returnTextColor() const {
	return this->darkMode ? this->textColorDark : this->textColorLight;
}


// Base draws.
void UniStoreV2::DrawBaseTop(void) const {
	Gui::ScreenDraw(Top);
	Gui::Draw_Rect(0, 0, 400, 25, this->darkMode ? this->barColorDark : this->barColorLight);
	Gui::Draw_Rect(0, 25, 400, 190, this->darkMode ? this->bgColorDark : this->bgColorLight);
	Gui::Draw_Rect(0, 215, 400, 25, this->darkMode ? this->barColorDark : this->barColorLight);
	if (Config::UseBars == true) {
		GFX::DrawSprite(sprites_top_screen_top_idx, 0, 0);
		GFX::DrawSprite(sprites_top_screen_bot_idx, 0, 215);
	}
}

void UniStoreV2::DrawBaseBottom(void) const {
	Gui::ScreenDraw(Bottom);
	Gui::Draw_Rect(0, 0, 320, 25, this->darkMode ? this->barColorDark : this->barColorLight);
	Gui::Draw_Rect(0, 25, 320, 190, this->darkMode ? this->bgColorDark : this->bgColorLight);
	Gui::Draw_Rect(0, 215, 320, 25, this->darkMode ? this->barColorDark : this->barColorLight);
	if (Config::UseBars == true) {
		GFX::DrawSprite(sprites_top_screen_top_idx, 0, 0);
		GFX::DrawSprite(sprites_top_screen_bot_idx, 0, 215);
	}
}

// Draw a box.
void UniStoreV2::drawBox(float xPos, float yPos, bool selected) const {
	static constexpr int w	= 1;
	const u32 tempColor = this->darkMode ? this->outlineColorDark : this->outlineColorLight;
	const u32 outlineColor = selected ? tempColor : C2D_Color32(0, 0, 0, 255);
	C2D_DrawRectSolid(xPos, yPos, 0.5, 100, 50, this->darkMode ? this->boxColorDark : this->boxColorLight);

	// Grid part.
	C2D_DrawRectSolid(xPos, yPos, 0.5, 100, w, outlineColor); // top
	C2D_DrawRectSolid(xPos, yPos + w, 0.5, w, 50 - 2 * w, outlineColor); // left
	C2D_DrawRectSolid(xPos + 100 - w, yPos + w, 0.5, w, 50 - 2 * w, outlineColor); // right
	C2D_DrawRectSolid(xPos, yPos + 50 - w, 0.5, 100, w, outlineColor); // bottom
}

void UniStoreV2::DrawPage(void) const {
	if (this->storePage == 0) {
		this->DrawBoxes();
	} else {
		for (int i = 0, i2 = 0 + (this->storePage * STORE_ENTRIES); i2 < STORE_ENTRIES + (this->storePage * STORE_ENTRIES) && i2 < (int)this->DBJson.at("storeContent").size(); i2++, i++) {
			if (i == this->selectedBox) {
				this->drawBox(this->StoreBoxes[i].x, this->StoreBoxes[i].y, true);
			} else {
				this->drawBox(this->StoreBoxes[i].x, this->StoreBoxes[i].y, false);
			}

			if (this->sheetLoaded) {
				if (this->DBJson.at("storeContent").at(i + (this->storePage * STORE_ENTRIES)).at("info").contains("icon_index") || (int)this->DBJson.at("storeContent").at(i + (this->storePage * STORE_ENTRIES)).at("info").at("icon_index") < (int)C2D_SpriteSheetCount(this->sheet)) {
					Gui::DrawSprite(this->sheet, this->DBJson.at("storeContent").at(i + (this->storePage * STORE_ENTRIES)).at("info").at("icon_index"), this->StoreBoxes[i].x+26, this->StoreBoxes[i].y+1);
				} else {
					GFX::DrawSprite(sprites_noIcon_idx, this->StoreBoxes[i].x+26, this->StoreBoxes[i].y+1);
				}
			} else {
				GFX::DrawSprite(sprites_noIcon_idx, this->StoreBoxes[i].x+26, this->StoreBoxes[i].y+1);
			}
		}
	}
}

void UniStoreV2::DrawBoxes(void) const {
	for (int i = 0; i < STORE_ENTRIES && i < (int)this->DBJson.at("storeContent").size(); i++) {
		if (i == this->selectedBox) {
			this->drawBox(this->StoreBoxes[i].x, this->StoreBoxes[i].y, true);
		} else {
			this->drawBox(this->StoreBoxes[i].x, this->StoreBoxes[i].y, false);
		}

		if (this->sheetLoaded) {
			if (this->DBJson.at("storeContent").at(i).at("info").contains("icon_index") || (int)this->DBJson.at("storeContent").at(i).at("info").at("icon_index") <= (int)C2D_SpriteSheetCount(this->sheet)) {
				Gui::DrawSprite(this->sheet, this->DBJson.at("storeContent").at(i).at("info").at("icon_index"), this->StoreBoxes[i].x+26, this->StoreBoxes[i].y+1);
			} else {
				GFX::DrawSprite(sprites_noIcon_idx, this->StoreBoxes[i].x+26, this->StoreBoxes[i].y+1);
			}
		} else {
			GFX::DrawSprite(sprites_noIcon_idx, this->StoreBoxes[i].x+26, this->StoreBoxes[i].y+1);
		}
	}
}

// Parse the objects from a script.
void UniStoreV2::parseObjects(int selection) {
	this->objects.clear();
	for(auto it = this->DBJson.at("storeContent").at(selection).begin(); it != this->DBJson.at("storeContent").at(selection).end(); it++) {
		if (it.key() != "info") {
			this->objects.push_back(it.key());
		}
	}
}

void UniStoreV2::displaySelectedEntry(int selection) const {
	this->DrawBaseTop();

	Gui::DrawStringCentered(0, 218, 0.7f, this->returnTextColor(), std::to_string(this->downloadPage + 1) + " | " + std::to_string(1 + (this->objects.size() / DOWNLOAD_ENTRIES)));

	if (this->DBJson.at("storeContent").at(selection).at("info").contains("title")) {
		if (Config::UseBars == true) {
			Gui::DrawStringCentered(0, 0, 0.7f, this->returnTextColor(), (std::string)this->DBJson.at("storeContent").at(selection).at("info").at("title"), 400);
		} else {
			Gui::DrawStringCentered(0, 2, 0.7f, this->returnTextColor(), (std::string)this->DBJson.at("storeContent").at(selection).at("info").at("title"), 400);
		}
	} else {
		if (Config::UseBars == true) {
			Gui::DrawStringCentered(0, 0, 0.7f, this->returnTextColor(), "?", 400);
		} else {
			Gui::DrawStringCentered(0, 2, 0.7f, this->returnTextColor(), "?", 400);
		}
	}

	if (this->DBJson.at("storeContent").at(selection).at("info").contains("author")) {
		Gui::DrawStringCentered(0, 40, 0.6f, this->returnTextColor(), "Author: " + (std::string)this->DBJson.at("storeContent").at(selection).at("info").at("author"), 400);
	} else {
		Gui::DrawStringCentered(0, 40, 0.6f, this->returnTextColor(), "Author: ?", 400);
	}

	if (this->DBJson.at("storeContent").at(selection).at("info").contains("version")) {
		Gui::DrawStringCentered(0, 60, 0.6f, this->returnTextColor(), "Version: " + (std::string)this->DBJson.at("storeContent").at(selection).at("info").at("version"), 400);
	} else {
		Gui::DrawStringCentered(0, 60, 0.6f, this->returnTextColor(), "Version: ?", 400);
	}

	if (this->DBJson.at("storeContent").at(selection).at("info").contains("category")) {
		Gui::DrawStringCentered(0, 80, 0.6f, this->returnTextColor(), "Category: " + (std::string)this->DBJson.at("storeContent").at(selection).at("info").at("category"), 400);
	} else {
		Gui::DrawStringCentered(0, 80, 0.6f, this->returnTextColor(), "Category: ?", 400);
	}

	if (this->DBJson.at("storeContent").at(selection).at("info").contains("console")) {
		Gui::DrawStringCentered(0, 100, 0.6f, this->returnTextColor(), "System: " + (std::string)this->DBJson.at("storeContent").at(selection).at("info").at("console"), 400);
	} else {
		Gui::DrawStringCentered(0, 100, 0.6f, this->returnTextColor(), "System: ?", 400);
	}

	if (this->DBJson.at("storeContent").at(selection).at("info").contains("last_updated")) {
		Gui::DrawStringCentered(0, 120, 0.6f, this->returnTextColor(), "Last updated: " + (std::string)this->DBJson.at("storeContent").at(selection).at("info").at("last_updated"), 400);
	} else {
		Gui::DrawStringCentered(0, 120, 0.6f, this->returnTextColor(), "Last updated: ?", 400);
	}

	if (this->DBJson.at("storeContent").at(selection).at("info").contains("description")) {
		Gui::DrawStringCentered(0, 140, 0.5f, this->returnTextColor(), "Description: " + (std::string)this->DBJson.at("storeContent").at(selection).at("info").at("description"), 400);
	} else {
		Gui::DrawStringCentered(0, 140, 0.5f, this->returnTextColor(), "Description: ?", 400);
	}

	this->DrawBaseBottom();

	if (this->objects.size() > 0) {
		for (int i = 0, i2 = 0 + (this->downloadPage * DOWNLOAD_ENTRIES); i2 < DOWNLOAD_ENTRIES + (this->downloadPage * DOWNLOAD_ENTRIES) && i2 < (int)this->objects.size(); i2++, i++) {
			Gui::Draw_Rect(downloadBoxes[i].x, downloadBoxes[i].y, downloadBoxes[i].w, downloadBoxes[i].h, this->darkMode ? this->barColorDark : this->barColorLight);
			Gui::DrawStringCentered(0, downloadBoxes[i].y+4, 0.5f, this->returnTextColor(), this->objects[i + (this->downloadPage * DOWNLOAD_ENTRIES)], 280);
		}
	}
}


void UniStoreV2::Draw(void) const {
	if (this->mode == 0) {
		this->DrawBaseTop();

		if (Config::UseBars == true) {
			Gui::DrawStringCentered(0, 0, 0.7f, this->returnTextColor(), (std::string)this->DBJson.at("storeInfo").at("title"), 400);
		} else {
			Gui::DrawStringCentered(0, 2, 0.7f, this->returnTextColor(), (std::string)this->DBJson.at("storeInfo").at("title"), 400);
		}

		this->DrawPage();

		Gui::DrawStringCentered(0, 218, 0.7f, this->returnTextColor(), std::to_string(this->storePage + 1) + " | " + std::to_string(1 + (this->DBJson.at("storeContent").size() / STORE_ENTRIES)));

		if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha));
		this->DrawBaseBottom();

		if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha));
	} else {
		this->displaySelectedEntry(this->selection);
	}
}

void UniStoreV2::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (this->mode == 0) {
		if (hDown & KEY_B) {
			Gui::screenBack(true);
			return;
		}

		if (hDown & KEY_RIGHT) {
			if ((this->storePage * STORE_ENTRIES) + this->selectedBox + 1 < (int)this->DBJson.at("storeContent").size()) {
				if (this->selectedBox < 8 + (this->storePage * STORE_ENTRIES))	this->selectedBox++;
			}
		}

		if (hDown & KEY_LEFT) {
			if (this->selectedBox > 0)	this->selectedBox--;
		}

		if (hDown & KEY_UP) {
			if (this->selectedBox > 2 + (this->storePage * STORE_ENTRIES))	this->selectedBox -= 3;
		}

		if (hDown & KEY_DOWN) {
			if ((this->storePage * STORE_ENTRIES) + this->selectedBox + 3 < (int)this->DBJson.at("storeContent").size()) {
				if (this->selectedBox < 6)	this->selectedBox += 3;
			}
		}

		if (hDown & KEY_R) {
			if (STORE_ENTRIES + (this->storePage * STORE_ENTRIES) < (int)this->DBJson.at("storeContent").size()) {
				this->selectedBox = 0;
				this->storePage++;
			}
		}

		if (hDown & KEY_L) {
			if (this->storePage > 0) {
				this->selectedBox = 0;
				this->storePage--;
			}
		}


		if (hDown & KEY_A) {
			if (this->selectedBox + (this->storePage * STORE_ENTRIES) < (int)this->DBJson.at("storeContent").size()) {
				this->selection = this->selectedBox + (this->storePage * STORE_ENTRIES);
				this->parseObjects(this->selection);
				this->canDisplay = true;
				this->mode = 1;
			}
		}

	} else if (this->mode == 1) {
		if (hDown & KEY_TOUCH) {
			if (this->objects.size() > 0) {
				for (int i = 0, i2 = 0 + (this->downloadPage * DOWNLOAD_ENTRIES); i2 < DOWNLOAD_ENTRIES + (this->downloadPage * DOWNLOAD_ENTRIES) && i2 < (int)this->objects.size(); i2++, i++) {
					if (touching(touch, downloadBoxes[i])) {
						if (Msg::promptMsg("Do you like to execute:\n" + this->objects[i + (this->downloadPage * DOWNLOAD_ENTRIES)]))	runFunctions(this->objects[i + (this->downloadPage * DOWNLOAD_ENTRIES)]);
					}
				}
			}
		}

		if (hDown & KEY_A) {
			if (this->objects.size() > 0) {
				if ((int)this->objects.size() >= this->subSelection) {
					if (Msg::promptMsg("Do you like to execute:\n" + this->objects[this->subSelection]))	runFunctions(this->objects[this->subSelection]);
				}
			}
		}

		if (hDown & KEY_DOWN) {
			if (this->subSelection < (int)this->objects.size()-1) {
				if (this->subSelection < DOWNLOAD_ENTRIES + (this->downloadPage * DOWNLOAD_ENTRIES)) {
					this->subSelection++;
				}
			}
		}

		if (hDown & KEY_UP) {
			if (this->subSelection > 0) {
				if (this->subSelection > this->downloadPage * DOWNLOAD_ENTRIES) {
					this->subSelection--;
				}
			}
		}


		if (hDown & KEY_R) {
			if (DOWNLOAD_ENTRIES + (this->downloadPage * DOWNLOAD_ENTRIES) < (int)this->objects.size()) {
				this->downloadPage++;
				this->subSelection = 5 * this->downloadPage;
			}
		}

		if (hDown & KEY_L) {
			if (this->downloadPage > 0) {
				this->downloadPage--;
			}
		}

		if (hDown & KEY_B) {
			this->downloadPage = 0; // Reset page to 0.
			this->subSelection = 0;
			this->mode = 0;
		}
	}

	// Allow switching in both modes.
	if (hDown & KEY_Y) {
		if (this->darkMode)	this->darkMode = false;
		else				this->darkMode = true;
	}
}


// Execute | run the script.
Result UniStoreV2::runFunctions(std::string entry) {
	Result ret = NONE; // No Error as of yet.
	for(int i=0;i<(int)this->DBJson.at("storeContent").at(this->selection).at(entry).size();i++) {
		if (ret == NONE) {
			std::string type = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("type");

			if (type == "deleteFile") {
				bool missing = false;
				std::string file, message;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("file"))	file = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("file");
				else	missing = true;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	message = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
				if(!missing)	ret = ScriptHelper::removeFile(file, message);
				else	ret = SYNTAX_ERROR;

			} else if(type == "downloadFile") {
				bool missing = false;
				std::string file, output, message;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("file"))	file = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("file");
				else	missing = true;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("output"))	output = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("output");
				else	missing = true;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	message = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
				if(!missing)	ret = ScriptHelper::downloadFile(file, output, message);
				else	ret = SYNTAX_ERROR;
		
			} else if(type == "downloadRelease") {
				bool missing = false, includePrereleases = false, showVersions = false;
				std::string repo, file, output, message;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("repo"))	repo = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("repo");
				else	missing = true;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("file"))	file = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("file");
				else	missing = true;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("output"))	output = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("output");
				else	missing = true;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("includePrereleases") && this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("includePrereleases").is_boolean())
					includePrereleases = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("includePrereleases");
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("showVersions") && this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("showVersions").is_boolean())
					showVersions = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("showVersions");
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	message = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
				if(!missing)	ret = ScriptHelper::downloadRelease(repo, file, output, includePrereleases, showVersions, message);

			} else if(type == "extractFile") {
				bool missing = false;
				std::string file, input, output, message;
				if(this->DBJson.at("storeContent").at("storeContent").at(this->selection).at(entry).at(i).contains("file"))	file = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("file");
				else	missing = true;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("input"))	input = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("input");
				else	missing = true;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("output"))	output = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("output");
				else	missing = true;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	message = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
				if(!missing)	ScriptHelper::extractFile(file, input, output, message);
				else	ret = SYNTAX_ERROR;

			} else if(type == "installCia") {
				bool missing = false, updateSelf = false;
				std::string file, message;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("file"))	file = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("file");
				else	missing = true;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("updateSelf") && this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("updateSelf").is_boolean()) {
					updateSelf = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("updateSelf");
				}
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	message = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
				if(!missing)	ScriptHelper::installFile(file, updateSelf, message);
				else	ret = SYNTAX_ERROR;

			} else if (type == "mkdir") {
				bool missing = false;
				std::string directory, message;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("directory"))	directory = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("directory");
				else	missing = true;
				if(!missing)	makeDirs(directory.c_str());
				else	ret = SYNTAX_ERROR;

			} else if (type == "rmdir") {
				bool missing = false;
				std::string directory, message, promptmsg;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("directory"))	directory = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("directory");
				else	missing = true;
				promptmsg = Lang::get("DELETE_PROMPT") + "\n" + directory;
				if(!missing) {
					if(access(directory.c_str(), F_OK) != 0 ) {
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
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("file"))	file = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("file");
				else	missing = true;
				if(!missing)	ScriptHelper::createFile(file.c_str());
				else	ret = SYNTAX_ERROR;

			} else if (type == "timeMsg") {
				bool missing = false;
				std::string message;
				int seconds;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	message = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
				else	missing = true;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("seconds") && this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("seconds").is_number())
				seconds = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("seconds");
				else	missing = true;
				if(!missing)	ScriptHelper::displayTimeMsg(message, seconds);
				else	ret = SYNTAX_ERROR;

			} else if (type == "saveConfig") {
				Config::save();

			} else if (type == "bootTitle") {
				std::string TitleID = "";
				std::string message = "";
				bool isNAND = false, missing = false;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("TitleID"))	TitleID = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("TitleID");
				else	missing = true;
				if (this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("NAND") && this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("NAND").is_boolean())	isNAND = this->DBJson.at(this->selection).at(entry).at(i).at("NAND");
				else	missing = true;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	message = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
				else	missing = true;
				if(!missing)	ScriptHelper::bootTitle(TitleID, isNAND, message);
				else	ret = SYNTAX_ERROR;

			} else if (type == "promptMessage") {
				std::string Message = "";
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	Message = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
				ret = ScriptHelper::prompt(Message);
				
			} else if (type == "copy") {
				std::string Message = "", source = "", destination = "";
				bool missing = false;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("source"))	source = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("source");
				else	missing = true;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("destination"))	destination = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("destination");
				else	missing = true;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	Message = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
				if (!missing)	ret = ScriptHelper::copyFile(source, destination, Message);
				else	ret = SYNTAX_ERROR;

			} else if (type == "move") {
				std::string Message = "", oldFile = "", newFile = "";
				bool missing = false;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("old"))	oldFile = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("old");
				else	missing = true;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("new"))	newFile = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("new");
				else	missing = true;
				if(this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	Message = this->DBJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
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