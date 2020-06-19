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
#include "keyboard.hpp"
#include "scriptHelper.hpp"
#include "unistore_v2.hpp"

#include <unistd.h>

extern u32 getColor(std::string colorString);
extern bool touching(touchPosition touch, Structs::ButtonPos button);
#define STORE_ENTRIES	 15
#define STORE_ENTRIES_LIST 3
#define DOWNLOAD_ENTRIES 5
extern bool didAutoboot;

UniStoreV2::UniStoreV2(nlohmann::json &JSON, const std::string sheetPath) {
	this->storeJson = JSON;
	this->sortedStore = std::make_unique<Store>(this->storeJson);

	if (access(sheetPath.c_str(), F_OK) != 0) {
		this->iconAmount = 0;
		this->sheetLoaded = false;
	} else {
		if (C2D_SpriteSheetLoad(sheetPath.c_str()) != nullptr) {
			this->sheet = C2D_SpriteSheetLoad(sheetPath.c_str());
			this->iconAmount = (int)C2D_SpriteSheetCount(this->sheet);
			this->sheetLoaded = true;
		} else {
			this->iconAmount = 0;
			this->sheetLoaded = false;
		}
	}

	// Get colors.
	this->barColorLight = getColor(ScriptHelper::getString(this->storeJson, "storeInfo", "barLight"));
	this->barColorDark  = getColor(ScriptHelper::getString(this->storeJson, "storeInfo", "barDark"));

	this->bgColorLight = getColor(ScriptHelper::getString(this->storeJson, "storeInfo", "bgLight"));
	this->bgColorDark  = getColor(ScriptHelper::getString(this->storeJson, "storeInfo", "bgDark"));

	this->textColorLight = getColor(ScriptHelper::getString(this->storeJson, "storeInfo", "textLight"));
	this->textColorDark  = getColor(ScriptHelper::getString(this->storeJson, "storeInfo", "textDark"));

	this->boxColorLight = getColor(ScriptHelper::getString(this->storeJson, "storeInfo", "boxLight"));
	this->boxColorDark  = getColor(ScriptHelper::getString(this->storeJson, "storeInfo", "boxDark"));

	this->outlineColorLight = getColor(ScriptHelper::getString(this->storeJson, "storeInfo", "outlineLight"));
	this->outlineColorDark  = getColor(ScriptHelper::getString(this->storeJson, "storeInfo", "outlineDark"));
}

UniStoreV2::~UniStoreV2() {
	// Only unload if sheet has loaded.
	if (this->sheetLoaded) {
		C2D_SpriteSheetFree(this->sheet);
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
void UniStoreV2::drawBox(float xPos, float yPos, float width, float height, bool selected) const {
	static constexpr int w	= 1;
	const u32 tempColor = this->darkMode ? this->outlineColorDark : this->outlineColorLight;
	const u32 outlineColor = selected ? tempColor : C2D_Color32(0, 0, 0, 255);
	C2D_DrawRectSolid(xPos, yPos, 0.5, width, height, this->darkMode ? this->boxColorDark : this->boxColorLight);

	// Grid part.
	C2D_DrawRectSolid(xPos, yPos, 0.5, width, w, outlineColor); // top
	C2D_DrawRectSolid(xPos, yPos + w, 0.5, w, height - 2 * w, outlineColor); // left
	C2D_DrawRectSolid(xPos + width - w, yPos + w, 0.5, w, height - 2 * w, outlineColor); // right
	C2D_DrawRectSolid(xPos, yPos + height - w, 0.5, width, w, outlineColor); // bottom
}

void UniStoreV2::DrawGrid(void) const {
	for (int i = 0, i2 = 0 + (this->storePage * STORE_ENTRIES); i2 < STORE_ENTRIES + (this->storePage * STORE_ENTRIES) && i2 < this->sortedStore->getSize(); i2++, i++) {
		if (i == this->selectedBox) {
			this->drawBox(this->StoreBoxesGrid[i].x, this->StoreBoxesGrid[i].y, 50, 50, true);
		} else {
			this->drawBox(this->StoreBoxesGrid[i].x, this->StoreBoxesGrid[i].y, 50, 50, false);
		}

		if (this->sheetLoaded) {
			if (this->sortedStore->returnIconIndex(i + (this->storePage * STORE_ENTRIES)) != -1) {
				if (this->sortedStore->returnIconIndex(i + (this->storePage * STORE_ENTRIES)) < this->iconAmount) {
					C2D_Image temp = C2D_SpriteSheetGetImage(this->sheet, this->sortedStore->returnIconIndex(i + (this->storePage * STORE_ENTRIES)));
					if (temp.subtex->width < 49 && temp.subtex->height < 49) {
						int offset = (48 - temp.subtex->width) / 2;
						int offset2 = (48 - temp.subtex->height) / 2;
						Gui::DrawSprite(this->sheet, this->sortedStore->returnIconIndex(i + (this->storePage * STORE_ENTRIES)), this->StoreBoxesGrid[i].x+1 + offset, this->StoreBoxesGrid[i].y+1 + offset2);
					} else {
						GFX::DrawSprite(sprites_noIcon_idx, this->StoreBoxesList[i].x+1, this->StoreBoxesList[i].y+1);
					}
					temp = {nullptr, nullptr};
				} else {
					GFX::DrawSprite(sprites_noIcon_idx, this->StoreBoxesGrid[i].x+26, this->StoreBoxesGrid[i].y+1);
				}
			} else {
				GFX::DrawSprite(sprites_noIcon_idx, this->StoreBoxesGrid[i].x+26, this->StoreBoxesGrid[i].y+1);
			}
		} else {
			GFX::DrawSprite(sprites_noIcon_idx, this->StoreBoxesGrid[i].x+26, this->StoreBoxesGrid[i].y+1);
		}
	}
}

void UniStoreV2::DrawList(void) const {
	for (int i = 0, i2 = 0 + (this->storePageList * STORE_ENTRIES_LIST); i2 < STORE_ENTRIES_LIST + (this->storePageList * STORE_ENTRIES_LIST) && i2 < this->sortedStore->getSize(); i2++, i++) {
		if (i == this->selectedBoxList) {
			this->drawBox(this->StoreBoxesList[i].x, this->StoreBoxesList[i].y, 360, 50, true);
		} else {
			this->drawBox(this->StoreBoxesList[i].x, this->StoreBoxesList[i].y, 360, 50, false);
		}

		if (this->sheetLoaded) {
			if (this->sortedStore->returnIconIndex(i + (this->storePageList * STORE_ENTRIES_LIST)) != -1) {
				if (this->sortedStore->returnIconIndex(i + (this->storePageList * STORE_ENTRIES_LIST)) < this->iconAmount) {
					C2D_Image temp = C2D_SpriteSheetGetImage(this->sheet, this->sortedStore->returnIconIndex(i + (this->storePageList * STORE_ENTRIES_LIST)));
					if (temp.subtex->width < 49 && temp.subtex->height < 49) {
						int offset = (48 - temp.subtex->width) / 2;
						int offset2 = (48 - temp.subtex->height) / 2;
						Gui::DrawSprite(this->sheet, this->sortedStore->returnIconIndex(i + (this->storePageList * STORE_ENTRIES_LIST)), this->StoreBoxesList[i].x+1 + offset, this->StoreBoxesList[i].y+1 + offset2);
					} else {
						GFX::DrawSprite(sprites_noIcon_idx, this->StoreBoxesList[i].x+1, this->StoreBoxesList[i].y+1);
					}
					temp = {nullptr, nullptr};
				} else {
					GFX::DrawSprite(sprites_noIcon_idx, this->StoreBoxesList[i].x+1, this->StoreBoxesList[i].y+1);
				}
			} else {
				GFX::DrawSprite(sprites_noIcon_idx, this->StoreBoxesList[i].x+1, this->StoreBoxesList[i].y+1);
			}
		} else {
			GFX::DrawSprite(sprites_noIcon_idx, this->StoreBoxesList[i].x+1, this->StoreBoxesList[i].y+1);
		}

		// Display Author & App name.
		
		Gui::DrawString(this->StoreBoxesList[i].x+55, this->StoreBoxesList[i].y+12, 0.45f, this->returnTextColor(), this->sortedStore->returnTitle(i + (this->storePageList * STORE_ENTRIES_LIST)), 300);
		Gui::DrawString(this->StoreBoxesList[i].x+55, this->StoreBoxesList[i].y+28, 0.45f, this->returnTextColor(), this->sortedStore->returnAuthor(i + (this->storePageList * STORE_ENTRIES_LIST)), 300);
	}
}

void UniStoreV2::DrawSortingMenu(void) const {
	for (int i = 2; i < (int)sortingPos.size(); i++) {
		if (i - 2 == this->sortedStore->getSortType()) {
			this->drawBox(sortingPos[i].x, sortingPos[i].y, sortingPos[i].w, sortingPos[i].h, true);
		} else {
			this->drawBox(sortingPos[i].x, sortingPos[i].y, sortingPos[i].w, sortingPos[i].h, false);
		}
	}

	Gui::DrawString(sortingPos[2].x+4, sortingPos[2].y+9, 0.55f, this->returnTextColor(), Lang::get("TITLE_BTN"));
	Gui::DrawString(sortingPos[3].x+4, sortingPos[3].y+9, 0.55f, this->returnTextColor(), Lang::get("AUTHOR_BTN"));
	Gui::DrawString(sortingPos[4].x+4, sortingPos[4].y+9, 0.55f, this->returnTextColor(), Lang::get("LAST_UPDATED_BTN"));

	if (this->sortedStore->getAscending()) {
		this->drawBox(sortingPos[0].x, sortingPos[0].y, sortingPos[0].w, sortingPos[0].h, false);
		this->drawBox(sortingPos[1].x, sortingPos[1].y, sortingPos[1].w, sortingPos[1].h, true);
	} else {
		this->drawBox(sortingPos[0].x, sortingPos[0].y, sortingPos[0].w, sortingPos[0].h, true);
		this->drawBox(sortingPos[1].x, sortingPos[1].y, sortingPos[1].w, sortingPos[1].h, false);
	}

	Gui::DrawString(sortingPos[0].x+4, sortingPos[0].y+9, 0.55f, this->returnTextColor(), Lang::get("DESCENDING"));
	Gui::DrawString(sortingPos[1].x+4, sortingPos[1].y+9, 0.55f, this->returnTextColor(), Lang::get("ASCENDING"));
}


// Parse the objects from a script.
void UniStoreV2::parseObjects(int selection) {
	this->objects.clear();
	for(auto it = this->storeJson.at("storeContent").at(selection).begin(); it != this->storeJson.at("storeContent").at(selection).end(); it++) {
		if (it.key() != "info") {
			this->objects.push_back(it.key());
		}
	}
}

void UniStoreV2::DropDownMenu(void) const {
	if (this->mode != 2) {
		// DropDown Menu.
		if (this->isDropDown) {
			// Draw Operation Box.
			Gui::Draw_Rect(5, 25, 140, 60, this->darkMode ? this->barColorDark : this->barColorLight);

			for (int i = 0; i < 2; i++) {
				Gui::Draw_Rect(dropPos[i].x, dropPos[i].y, dropPos[i].w, dropPos[i].h, this->darkMode ? this->boxColorDark : this->boxColorLight);
			}

			Gui::drawAnimatedSelector(dropPos[dropSelection].x, dropPos[dropSelection].y, dropPos[dropSelection].w, dropPos[dropSelection].h, .090, this->darkMode ? this->barColorDark : this->barColorLight, TRANSPARENT);

			// Draw Dropdown Icons.
			//GFX::DrawSpriteBlend(sprites_theme_idx, this->dropPos[0].x, this->dropPos[0].y); // Theme Icon instead.
			//GFX::DrawSpriteBlend(sprites_style_idx, this->dropPos[1].x, this->dropPos[1].y); // Style Icon instead.

			// Dropdown Text.
			Gui::DrawString(this->dropPos[0].x+30, this->dropPos[0].y+5, 0.4f, this->returnTextColor(), Lang::get("CHANGE_THEME"), 100);
			Gui::DrawString(this->dropPos[1].x+30, this->dropPos[1].y+5, 0.4f, this->returnTextColor(), Lang::get("CHANGE_STYLE"), 100);
		}
	}
}

void UniStoreV2::displaySelectedEntry(int selection) const {
	this->DrawBaseTop();

	Gui::DrawStringCentered(0, 218, 0.7f, this->returnTextColor(), std::to_string(this->downloadPage + 1) + " | " + std::to_string(1 + (this->objects.size() / DOWNLOAD_ENTRIES)));

	if (this->storeJson.at("storeContent").at(selection).at("info").contains("title")) {
		if (Config::UseBars == true) {
			Gui::DrawStringCentered(0, 0, 0.7f, this->returnTextColor(), (std::string)this->storeJson.at("storeContent").at(selection).at("info").at("title"), 400);
		} else {
			Gui::DrawStringCentered(0, 2, 0.7f, this->returnTextColor(), (std::string)this->storeJson.at("storeContent").at(selection).at("info").at("title"), 400);
		}
	} else {
		if (Config::UseBars == true) {
			Gui::DrawStringCentered(0, 0, 0.7f, this->returnTextColor(), "?", 400);
		} else {
			Gui::DrawStringCentered(0, 2, 0.7f, this->returnTextColor(), "?", 400);
		}
	}

	if (this->storeJson.at("storeContent").at(selection).at("info").contains("author")) {
		Gui::DrawStringCentered(0, 40, 0.6f, this->returnTextColor(), Lang::get("AUTHOR") + (std::string)this->storeJson.at("storeContent").at(selection).at("info").at("author"), 400);
	} else {
		Gui::DrawStringCentered(0, 40, 0.6f, this->returnTextColor(), Lang::get("AUTHOR") + "?", 400);
	}

	if (this->storeJson.at("storeContent").at(selection).at("info").contains("version")) {
		Gui::DrawStringCentered(0, 60, 0.6f, this->returnTextColor(), Lang::get("VERSION") + (std::string)this->storeJson.at("storeContent").at(selection).at("info").at("version"), 400);
	} else {
		Gui::DrawStringCentered(0, 60, 0.6f, this->returnTextColor(), Lang::get("VERSION") + "?", 400);
	}

	if (this->storeJson.at("storeContent").at(selection).at("info").contains("category")) {
		Gui::DrawStringCentered(0, 80, 0.6f, this->returnTextColor(), Lang::get("CATEGORY") + (std::string)this->storeJson.at("storeContent").at(selection).at("info").at("category"), 400);
	} else {
		Gui::DrawStringCentered(0, 80, 0.6f, this->returnTextColor(), Lang::get("CATEGORY") + "?", 400);
	}

	if (this->storeJson.at("storeContent").at(selection).at("info").contains("console")) {
		Gui::DrawStringCentered(0, 100, 0.6f, this->returnTextColor(), Lang::get("SYSTEM") + (std::string)this->storeJson.at("storeContent").at(selection).at("info").at("console"), 400);
	} else {
		Gui::DrawStringCentered(0, 100, 0.6f, this->returnTextColor(), Lang::get("SYSTEM") + "?", 400);
	}

	if (this->storeJson.at("storeContent").at(selection).at("info").contains("last_updated")) {
		Gui::DrawStringCentered(0, 120, 0.6f, this->returnTextColor(), Lang::get("LAST_UPDATED") + (std::string)this->storeJson.at("storeContent").at(selection).at("info").at("last_updated"), 400);
	} else {
		Gui::DrawStringCentered(0, 120, 0.6f, this->returnTextColor(), Lang::get("LAST_UPDATED") + "?", 400);
	}

	if (this->storeJson.at("storeContent").at(selection).at("info").contains("description")) {
		Gui::DrawStringCentered(0, 140, 0.5f, this->returnTextColor(), Lang::get("DESC") + (std::string)this->storeJson.at("storeContent").at(selection).at("info").at("description"), 400);
	} else {
		Gui::DrawStringCentered(0, 140, 0.5f, this->returnTextColor(), Lang::get("DESC") + "?", 400);
	}

	this->DrawBaseBottom();

	if (this->objects.size() > 0) {
		for (int i = 0, i2 = (this->downloadPage * DOWNLOAD_ENTRIES); i2 < DOWNLOAD_ENTRIES + (this->downloadPage * DOWNLOAD_ENTRIES) && i2 < (int)this->objects.size(); i2++, i++) {
			if (i + (this->downloadPage * DOWNLOAD_ENTRIES) == this->subSelection) {
				this->drawBox(downloadBoxes[i].x, downloadBoxes[i].y, downloadBoxes[i].w, downloadBoxes[i].h, true);
			} else {
				this->drawBox(downloadBoxes[i].x, downloadBoxes[i].y, downloadBoxes[i].w, downloadBoxes[i].h, false);
			}
			
			Gui::DrawStringCentered(0, downloadBoxes[i].y+4, 0.5f, this->returnTextColor(), this->objects[i + (this->downloadPage * DOWNLOAD_ENTRIES)], 280);
		}
	} else {
		Gui::DrawStringCentered(0, downloadBoxes[0].y+4, 0.5f, this->returnTextColor(), Lang::get("NO_DOWNLOADS_AVAILABLE"), 280);
	}
}


void UniStoreV2::Draw(void) const {
	if (this->mode == 0) {
		this->DrawBaseTop();

		if (Config::UseBars == true) {
			Gui::DrawStringCentered(0, 0, 0.6f, this->returnTextColor(), (std::string)this->storeJson.at("storeInfo").at("title"), 400);
		} else {
			Gui::DrawStringCentered(0, 2, 0.6f, this->returnTextColor(), (std::string)this->storeJson.at("storeInfo").at("title"), 400);
		}

		this->DrawGrid();

		Gui::DrawStringCentered(0, 218, 0.6f, this->returnTextColor(), std::to_string(this->storePage + 1) + " | " + std::to_string(1 + (this->sortedStore->getSize() / STORE_ENTRIES)));

		if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha));
		this->DrawBaseBottom();
		this->DrawSortingMenu();

		if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha));
	} else if (this->mode == 1) {
		this->DrawBaseTop();

		if (Config::UseBars == true) {
			Gui::DrawStringCentered(0, 0, 0.6f, this->returnTextColor(), (std::string)this->storeJson.at("storeInfo").at("title"), 400);
		} else {
			Gui::DrawStringCentered(0, 2, 0.6f, this->returnTextColor(), (std::string)this->storeJson.at("storeInfo").at("title"), 400);
		}

		this->DrawList();
		Gui::DrawStringCentered(0, 218, 0.6f, this->returnTextColor(), std::to_string(this->storePageList + 1) + " | " + std::to_string(1 + (this->sortedStore->getSize() / STORE_ENTRIES_LIST)));

		if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha));
		this->DrawBaseBottom();
		this->DrawSortingMenu();
		if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha));
	} else if (this->mode == 2) {
		this->displaySelectedEntry(this->selection);
	}

	if (this->mode != 2)	GFX::DrawSpriteBlend(sprites_dropdown_idx, iconPos[0].x, iconPos[0].y);
	this->DropDownMenu();
}

void UniStoreV2::DropLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (this->mode != 2) {
		if (hDown & KEY_DOWN) {
			if (this->dropSelection < (int)this->dropPos.size()-1) this->dropSelection++;
		}

		if (hDown & KEY_UP) {
			if (this->dropSelection > 0) this->dropSelection--;
		}

		if (hDown & KEY_A) {
			switch(this->dropSelection) {
				case 0:
					if (this->darkMode) this->darkMode = false;
					else this->darkMode = true;
					break;
				case 1:
					if (this->mode == 0) this->mode = 1;
					else this->mode = 0;
				break;
			}
			this->isDropDown = false;
		}

		if ((hDown & KEY_SELECT) || (hDown & KEY_TOUCH && touching(touch, this->iconPos[0]))) {
			this->isDropDown = false;
		}

		if (hDown & KEY_TOUCH) {
			if (touching(touch, this->dropPos[0])) {
				if (this->darkMode) this->darkMode = false;
				else this->darkMode = true;
				this->isDropDown = false;
			} else if (touching(touch, this->dropPos[1])) {
				if (this->mode == 0) this->mode = 1;
				else this->mode = 0;
				this->isDropDown = false;
			}
		}
	}
}

void UniStoreV2::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (this->isDropDown) {
		this->DropLogic(hDown, hHeld, touch);
	} else {
		if (this->mode == 0) {
			if (hDown & KEY_B) {
				if (!didAutoboot) didAutoboot = true;
				Gui::screenBack(Config::fading);
				return;
			}

			if (hDown & KEY_RIGHT) {
				// Try to go to next page.
				if (this->selectedBox == 4 || this->selectedBox == 9 || this->selectedBox == 14) {
					if (STORE_ENTRIES + (this->storePage * STORE_ENTRIES) < (int)this->sortedStore->getSize()) {
						this->selectedBox = 0;
						this->storePage++;
					}
				} else {
					if ((this->storePage * STORE_ENTRIES) + this->selectedBox + 1 < (int)this->sortedStore->getSize()) {
						if (this->selectedBox < 14 + (this->storePage * STORE_ENTRIES))	this->selectedBox++;
					}
				}
			}

			if (hDown & KEY_LEFT) {
				// Try to go to next page.
				if (this->selectedBox == 0 || this->selectedBox == 5 || this->selectedBox == 11) {
					if (this->storePage > 0) {
						this->storePage--;
					}
				} else {
					if (this->selectedBox > 0)	this->selectedBox--;
				}
			}

			if (hDown & KEY_UP) {
				if (this->selectedBox > 4 + (this->storePage * STORE_ENTRIES))	this->selectedBox -= 5;
			}

			if (hDown & KEY_DOWN) {
				if ((this->storePage * STORE_ENTRIES) + this->selectedBox + 5 < (int)this->sortedStore->getSize()) {
					if (this->selectedBox < 11)	this->selectedBox += 5;
				}
			}

			if (hDown & KEY_R) {
				if (STORE_ENTRIES + (this->storePage * STORE_ENTRIES) < this->sortedStore->getSize()) {
					// Selected box is smaller.. so we can keep it on the same position.
					if (this->selectedBox + STORE_ENTRIES + (this->storePage * STORE_ENTRIES) < this->sortedStore->getSize()) {
						this->storePage++;
					} else {
						this->selectedBox = 0;
						this->storePage++;
					}
				}
			}

			if (hDown & KEY_X) {
				std::string temp = Input::getStringLong(Lang::get("ENTER_SEARCH"));
				if (temp != "") {
					this->selectedBox = 0;
					this->storePage = 0;
					int amount = this->sortedStore->searchForEntries(temp);
					if (amount == 0) Msg::DisplayWarnMsg(Lang::get("NO_RESULTS_FOUND"));
				}
			}

			if (hDown & KEY_Y) {
				this->selectedBox = 0;
				this->storePage = 0;
				this->sortedStore->reset();
			}

			if (hDown & KEY_L) {
				if (this->storePage > 0) {
					this->storePage--;
				}
			}


			if (hDown & KEY_A) {
				if (this->sortedStore->returnJSONIndex(this->selectedBox + (this->storePage * STORE_ENTRIES)) < (int)this->storeJson.at("storeContent").size()) {
					this->selection = this->sortedStore->returnJSONIndex(this->selectedBox + (this->storePage * STORE_ENTRIES));
					this->parseObjects(this->selection);
					this->canDisplay = true;
					this->lastViewMode = this->mode;
					this->mode = 2;
				}
			}

			if (hDown & KEY_TOUCH) {
				if (touching(touch, sortingPos[0])) {
					this->sortedStore->sorting(false, SortType(this->sortedStore->getSortType()));
				} else if (touching(touch, sortingPos[1])) {
					this->sortedStore->sorting(true, SortType(this->sortedStore->getSortType()));
				} else if (touching(touch, sortingPos[2])) {
					this->sortedStore->sorting(this->sortedStore->getAscending(), SortType(0));
				} else if (touching(touch, sortingPos[3])) {
					this->sortedStore->sorting(this->sortedStore->getAscending(), SortType(1));
				} else if (touching(touch, sortingPos[4])) {
					this->sortedStore->sorting(this->sortedStore->getAscending(), SortType(2));
				}
			}

		} else if (this->mode == 1) {
			if (hDown & KEY_B) {
				if (!didAutoboot) didAutoboot = true;
				Gui::screenBack(Config::fading);
				return;
			}

			if (hDown & KEY_DOWN) {
				if ((this->storePageList * STORE_ENTRIES_LIST) + this->selectedBoxList + 1 < (int)this->sortedStore->getSize()) {
					if (this->selectedBoxList < STORE_ENTRIES_LIST-1)	this->selectedBoxList++;
				}
			}

			if (hDown & KEY_UP) {
				if (this->selectedBoxList > 0)	this->selectedBoxList--;
			}

			if (hDown & KEY_RIGHT || hDown & KEY_R) {
				if (STORE_ENTRIES_LIST + (this->storePageList * STORE_ENTRIES_LIST) < (int)this->sortedStore->getSize()) {
					if (this->selectedBoxList + STORE_ENTRIES_LIST + (this->storePageList * STORE_ENTRIES_LIST) < this->sortedStore->getSize()) {
						this->storePageList++;
					} else {
						this->selectedBoxList = 0;
						this->storePageList++;
					}
				}
			}

			if (hDown & KEY_LEFT || hDown & KEY_L) {
				if (this->storePageList > 0) {
					this->storePageList--;
				}
			}

			if (hDown & KEY_A) {
				if (this->sortedStore->returnJSONIndex(this->selectedBoxList + (this->storePageList * STORE_ENTRIES_LIST)) < (int)this->storeJson.at("storeContent").size()) {
					this->selection = this->sortedStore->returnJSONIndex(this->selectedBoxList + (this->storePageList * STORE_ENTRIES_LIST));
					this->parseObjects(this->selection);
					this->canDisplay = true;
					this->lastViewMode = this->mode;
					this->mode = 2;
				}
			}

			if (hDown & KEY_TOUCH) {
				if (touching(touch, sortingPos[0])) {
					this->sortedStore->sorting(false, SortType(this->sortedStore->getSortType()));
				} else if (touching(touch, sortingPos[1])) {
					this->sortedStore->sorting(true, SortType(this->sortedStore->getSortType()));
				} else if (touching(touch, sortingPos[2])) {
					this->sortedStore->sorting(this->sortedStore->getAscending(), SortType(0));
				} else if (touching(touch, sortingPos[3])) {
					this->sortedStore->sorting(this->sortedStore->getAscending(), SortType(1));
				} else if (touching(touch, sortingPos[4])) {
					this->sortedStore->sorting(this->sortedStore->getAscending(), SortType(2));
				}
			}

			if (hDown & KEY_X) {
				std::string temp = Input::getStringLong(Lang::get("ENTER_SEARCH"));
				if (temp != "") {
					this->selectedBoxList = 0;
					this->storePageList = 0;
					int amount = this->sortedStore->searchForEntries(temp);
					if (amount == 0) Msg::DisplayWarnMsg(Lang::get("NO_RESULTS_FOUND"));
				}
			}

			if (hDown & KEY_Y) {
				this->selectedBoxList = 0;
				this->storePageList = 0;
				this->sortedStore->reset();
			}

		} else if (this->mode == 2) {
			if (hDown & KEY_TOUCH) {
				if (this->objects.size() > 0) {
					for (int i = 0, i2 = 0 + (this->downloadPage * DOWNLOAD_ENTRIES); i2 < DOWNLOAD_ENTRIES + (this->downloadPage * DOWNLOAD_ENTRIES) && i2 < (int)this->objects.size(); i2++, i++) {
						if (touching(touch, downloadBoxes[i])) {
							if (Msg::promptMsg(Lang::get("EXECUTE_SCRIPT") + "\n" + this->objects[i + (this->downloadPage * DOWNLOAD_ENTRIES)]))	runFunctions(this->objects[i + (this->downloadPage * DOWNLOAD_ENTRIES)]);
						}
					}
				}
			}

			if (hDown & KEY_A) {
				if (this->objects.size() > 0) {
					if ((int)this->objects.size() >= this->subSelection) {
						if (Msg::promptMsg(Lang::get("EXECUTE_SCRIPT") + "\n" + this->objects[this->subSelection]))	runFunctions(this->objects[this->subSelection]);
					}
				}
			}

			if (hDown & KEY_DOWN) {
				if (this->subSelection < (int)this->objects.size()-1) {
					if (this->subSelection < DOWNLOAD_ENTRIES + (this->downloadPage * DOWNLOAD_ENTRIES)-1) {
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


			if (hDown & KEY_R || hDown & KEY_RIGHT) {
				if (DOWNLOAD_ENTRIES + (this->downloadPage * DOWNLOAD_ENTRIES) < (int)this->objects.size()) {
					this->downloadPage++;
					this->subSelection = this->downloadPage * DOWNLOAD_ENTRIES;
				}
			}

			if (hDown & KEY_L || hDown & KEY_LEFT) {
				if (this->downloadPage > 0) {
					this->downloadPage--;
					this->subSelection = this->downloadPage * DOWNLOAD_ENTRIES;
				}
			}

			if (hDown & KEY_B) {
				this->downloadPage = 0; // Reset page to 0.
				this->subSelection = 0;
				this->mode = this->lastViewMode;
			}
		}

		if ((hDown & KEY_SELECT) || (hDown & KEY_TOUCH && touching(touch, iconPos[0]))) {
			if (this->mode != 2) {
				this->dropSelection = 0;
				this->isDropDown = true;
			}
		}
	}
}


// Execute | run the script.
Result UniStoreV2::runFunctions(std::string entry) {
	Result ret = NONE; // No Error as of yet.
	for(int i=0;i<(int)this->storeJson.at("storeContent").at(this->selection).at(entry).size();i++) {
		if (ret == NONE) {
			std::string type = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("type");

			if (type == "deleteFile") {
				bool missing = false;
				std::string file, message;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("file"))	file = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("file");
				else	missing = true;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	message = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
				if(!missing)	ret = ScriptHelper::removeFile(file, message);
				else	ret = SYNTAX_ERROR;

			} else if(type == "downloadFile") {
				bool missing = false;
				std::string file, output, message;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("file"))	file = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("file");
				else	missing = true;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("output"))	output = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("output");
				else	missing = true;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	message = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
				if(!missing)	ret = ScriptHelper::downloadFile(file, output, message);
				else	ret = SYNTAX_ERROR;
		
			} else if(type == "downloadRelease") {
				bool missing = false, includePrereleases = false, showVersions = false;
				std::string repo, file, output, message;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("repo"))	repo = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("repo");
				else	missing = true;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("file"))	file = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("file");
				else	missing = true;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("output"))	output = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("output");
				else	missing = true;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("includePrereleases") && this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("includePrereleases").is_boolean())
					includePrereleases = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("includePrereleases");
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("showVersions") && this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("showVersions").is_boolean())
					showVersions = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("showVersions");
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	message = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
				if(!missing)	ret = ScriptHelper::downloadRelease(repo, file, output, includePrereleases, showVersions, message);

			} else if(type == "extractFile") {
				bool missing = false;
				std::string file, input, output, message;
				if(this->storeJson.at("storeContent").at("storeContent").at(this->selection).at(entry).at(i).contains("file"))	file = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("file");
				else	missing = true;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("input"))	input = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("input");
				else	missing = true;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("output"))	output = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("output");
				else	missing = true;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	message = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
				if(!missing)	ScriptHelper::extractFile(file, input, output, message);
				else	ret = SYNTAX_ERROR;

			} else if(type == "installCia") {
				bool missing = false, updateSelf = false;
				std::string file, message;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("file"))	file = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("file");
				else	missing = true;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("updateSelf") && this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("updateSelf").is_boolean()) {
					updateSelf = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("updateSelf");
				}
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	message = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
				if(!missing)	ScriptHelper::installFile(file, updateSelf, message);
				else	ret = SYNTAX_ERROR;

			} else if (type == "mkdir") {
				bool missing = false;
				std::string directory, message;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("directory"))	directory = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("directory");
				else	missing = true;
				if(!missing)	makeDirs(directory.c_str());
				else	ret = SYNTAX_ERROR;

			} else if (type == "rmdir") {
				bool missing = false;
				std::string directory, message, promptmsg;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("directory"))	directory = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("directory");
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
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("file"))	file = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("file");
				else	missing = true;
				if(!missing)	ScriptHelper::createFile(file.c_str());
				else	ret = SYNTAX_ERROR;

			} else if (type == "timeMsg") {
				bool missing = false;
				std::string message;
				int seconds;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	message = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
				else	missing = true;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("seconds") && this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("seconds").is_number())
				seconds = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("seconds");
				else	missing = true;
				if(!missing)	ScriptHelper::displayTimeMsg(message, seconds);
				else	ret = SYNTAX_ERROR;

			} else if (type == "saveConfig") {
				Config::save();

			} else if (type == "bootTitle") {
				std::string TitleID = "";
				std::string message = "";
				bool isNAND = false, missing = false;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("TitleID"))	TitleID = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("TitleID");
				else	missing = true;
				if (this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("NAND") && this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("NAND").is_boolean())	isNAND = this->storeJson.at(this->selection).at(entry).at(i).at("NAND");
				else	missing = true;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	message = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
				else	missing = true;
				if(!missing)	ScriptHelper::bootTitle(TitleID, isNAND, message);
				else	ret = SYNTAX_ERROR;

			} else if (type == "promptMessage") {
				std::string Message = "";
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	Message = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
				ret = ScriptHelper::prompt(Message);
				
			} else if (type == "copy") {
				std::string Message = "", source = "", destination = "";
				bool missing = false;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("source"))	source = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("source");
				else	missing = true;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("destination"))	destination = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("destination");
				else	missing = true;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	Message = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
				if (!missing)	ret = ScriptHelper::copyFile(source, destination, Message);
				else	ret = SYNTAX_ERROR;

			} else if (type == "move") {
				std::string Message = "", oldFile = "", newFile = "";
				bool missing = false;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("old"))	oldFile = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("old");
				else	missing = true;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("new"))	newFile = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("new");
				else	missing = true;
				if(this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).contains("message"))	Message = this->storeJson.at("storeContent").at(this->selection).at(entry).at(i).at("message");
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