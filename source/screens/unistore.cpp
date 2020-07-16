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

#include "download.hpp"
#include "fileBrowse.hpp"
#include "formatting.hpp"
#include "keyboard.hpp"
#include "mainMenu.hpp"
#include "unistore.hpp"
#include "unistore_v1.hpp"
#include "unistore_v2.hpp"

#include <unistd.h>

extern void notImplemented(void);
extern bool touching(touchPosition touch, Structs::ButtonPos button);
extern bool changesMade;
bool specialHandling = false;
bool didAutoboot = false;
extern std::unique_ptr<Config> config;

UniStore::UniStore(bool doAutoboot, std::string file) {
	this->doAutoboot = doAutoboot;
	this->autobootFile = file;
}

// Autoboot stuff.
void UniStore::autobootLogic() {
	if (this->doAutoboot) {
		if (!didAutoboot) {
			specialHandling = true; // Special back handling.
			if (ScriptHelper::checkIfValid(this->autobootFile, 1) == true) {
				storeInfo.push_back(parseStoreInfo(this->autobootFile));

				// Update if WiFi found and wanted.
				if (checkWifiStatus()) {
					if (Msg::promptMsg(Lang::get("WOULD_YOU_LIKE_UPDATE"))) {
						if (storeInfo[0].url != "" && storeInfo[0].url != "MISSING: storeInfo.url" &&
							storeInfo[0].file != "" && storeInfo[0].file != "MISSING: storeInfo.file") {
								ScriptHelper::downloadFile(storeInfo[0].url, storeInfo[0].file, Lang::get("UPDATING"));
							}

						if (storeInfo[0].sheetURL != "" && storeInfo[0].sheetURL != "MISSING: storeInfo.sheetURL" &&
							storeInfo[0].storeSheet != "" && storeInfo[0].storeSheet != "MISSING: storeInfo.sheet") {
								ScriptHelper::downloadFile(storeInfo[0].sheetURL, storeInfo[0].storeSheet, Lang::get("UPDATING"));
						}
					}
				}

				currentStoreFile = this->autobootFile;
				Msg::DisplayMsg(Lang::get("PREPARE_STORE"));
				JSON = openStoreFile();
				displayInformations = handleIfDisplayText();
				const std::string sheetURL = storeInfo[0].storeSheet;
				if (storeInfo[0].version == 0 || storeInfo[0].version == 1) {
					Gui::setScreen(std::make_unique<UniStoreV1>(JSON, sheetURL, displayInformations), config->screenFade(), true);
				} else if (storeInfo[0].version == 2) {
					Gui::setScreen(std::make_unique<UniStoreV2>(JSON, sheetURL, currentStoreFile), config->screenFade(), true);
				} else {
					Msg::DisplayWarnMsg(Lang::get("UNISTORE_NOT_SUPPORTED"));
				}
			} else {
				specialHandling = true; // Special back handling.
				// Display Warn or so?
			}
		}
	}
}

// Parse informations like URL, Title, Author, Description.
StoreInfo UniStore::parseStoreInfo(std::string fileName) {
	FILE* file = fopen(fileName.c_str(), "rt");
	if (!file) {
		printf("File not found.\n");
		return {"", ""};
	}

	nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
	fclose(file);

	StoreInfo info;
	info.title = ScriptHelper::getString(json, "storeInfo", "title");
	info.author = ScriptHelper::getString(json, "storeInfo", "author");
	info.description = ScriptHelper::getString(json, "storeInfo", "description");
	info.url = ScriptHelper::getString(json, "storeInfo", "url");
	info.file = ScriptHelper::getString(json, "storeInfo", "file");
	info.storeSheet = ScriptHelper::getString(json, "storeInfo", "sheet");
	info.sheetURL = ScriptHelper::getString(json, "storeInfo", "sheetURL");
	info.version = ScriptHelper::getNum(json, "storeInfo", "version");
	return info;
}

// Return a parsed UniStore file.
nlohmann::json UniStore::openStoreFile() {
	FILE* file = fopen(currentStoreFile.c_str(), "rt");
	nlohmann::json jsonFile;
	if (file)	jsonFile = nlohmann::json::parse(file, nullptr, false);
	fclose(file);
	return jsonFile;
}

// Do the description, if found.
void UniStore::descript() {
	if (storeInfo[Selection].description != "" || storeInfo[Selection].description != "MISSING: storeInfo.description") {
		storeDesc = storeInfo[Selection].description;
	} else storeDesc = "";
}

// Load the store description.
void UniStore::loadStoreDesc(void) {
	descLines.clear();
	while(storeDesc.find('\n') != storeDesc.npos) {
		descLines.push_back(storeDesc.substr(0, storeDesc.find('\n')));
		storeDesc = storeDesc.substr(storeDesc.find('\n')+1);
	}

	descLines.push_back(storeDesc.substr(0, storeDesc.find('\n')));
}

void UniStore::DrawSubMenu(void) const {
	GFX::DrawTop();
	Gui::DrawStringCentered(0, config->useBars() ? 0 : 2, 0.7f, config->textColor(), Lang::get("UNISTORE_SUBMENU"), 400);
	GFX::DrawSprite(sprites_uniStore_HD_idx, 140, 50, 0.2, 0.2);
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	GFX::DrawBottom();
	GFX::DrawArrow(0, 218, 0, 1);

	GFX::DrawButton(subPos[0].x, subPos[0].y,Lang::get("STORE_LIST"));
	GFX::DrawButton(subPos[1].x, subPos[1].y, Lang::get("STORE_SEARCH"));
	GFX::DrawButton(subPos[2].x, subPos[2].y, Lang::get("CHANGE_STOREPATH"));

	// Selector.
	Animation::Button(subPos[Selection].x, subPos[Selection].y, .060);
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
}


// First Screen -> Storelist.
void UniStore::DrawStoreList(void) const {
	std::string line1;
	std::string line2;
	std::string storeAmount = std::to_string(Selection +1) + " | " + std::to_string(storeInfo.size());
	GFX::DrawTop();
	Gui::DrawStringCentered(0, config->useBars() ? 0 : 2, 0.7f, config->textColor(), storeInfo[Selection].title, 400);
	Gui::DrawString(5, config->useBars() ? 220 : 218, 0.6f, config->textColor(), Lang::get("UNISTORE_VERSION") + std::to_string(storeInfo[Selection].version), 400);
	Gui::DrawString(397-Gui::GetStringWidth(0.6f, storeAmount), (config->useBars() ? 239 : 237)-Gui::GetStringHeight(0.6f, storeAmount), 0.6f, config->textColor(), storeAmount);

	for(uint i = 0; i < descLines.size(); i++) {
		Gui::DrawStringCentered(0, 120-((descLines.size()*20)/2)+i*20, 0.6f, config->textColor(), descLines[i], 400);
	}

	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	GFX::DrawBottom();
	GFX::DrawArrow(295, -1);
	GFX::DrawArrow(315, 240, 180.0);
	GFX::DrawArrow(0, 218, 0, 1);
	GFX::DrawSpriteBlend(sprites_dropdown_idx, arrowPos[3].x, arrowPos[3].y);

	if (config->viewMode() == 0) {
		for(int i = 0; i < ENTRIES_PER_SCREEN && i < (int)storeInfo.size(); i++) {
			Gui::Draw_Rect(0, 40+(i*57), 320, 45, config->unselectedColor());
			line1 = storeInfo[screenPos + i].title;
			line2 = storeInfo[screenPos + i].author;
			if (screenPos + i == Selection) {
				if (!dropDownMenu) {
					Gui::drawAnimatedSelector(0, 40+(i*57), 320, 45, .060, TRANSPARENT, config->selectedColor());
				}
			}

			Gui::DrawStringCentered(0, 38+(i*57), 0.7f, config->textColor(), line1, 320);
			Gui::DrawStringCentered(0, 62+(i*57), 0.7f, config->textColor(), line2, 320);
		}
	} else if (config->viewMode() == 1) {
		for(int i = 0; i < ENTRIES_PER_LIST && i < (int)storeInfo.size(); i++) {
			Gui::Draw_Rect(0, (i+1)*27, 320, 25, config->unselectedColor());
			line1 = storeInfo[screenPosList + i].title;
			if (screenPosList + i == Selection) {
				if (!dropDownMenu) {
					Gui::drawAnimatedSelector(0, (i+1)*27, 320, 25, .060, TRANSPARENT, config->selectedColor());
				}
			}

			Gui::DrawStringCentered(0, ((i+1)*27)+1, 0.7f, config->textColor(), line1, 320);
		}
	}

	// DropDown Menu.
	if (dropDownMenu) {
		// Draw Operation Box.
		Gui::Draw_Rect(0, 25, 140, 130, config->barColor());
		for (int i = 0; i < 3; i++) {
			if (dropSelection == i) {
				Gui::drawAnimatedSelector(dropPos2[i].x, dropPos2[i].y, dropPos2[i].w, dropPos2[i].h, .090, TRANSPARENT, config->selectedColor());
			} else {
				Gui::Draw_Rect(dropPos2[i].x, dropPos2[i].y, dropPos2[i].w, dropPos2[i].h, config->unselectedColor());
			}
		}

		// Draw Dropdown Icons.
		GFX::DrawSpriteBlend(sprites_delete_idx, dropPos[0].x, dropPos[0].y);
		GFX::DrawSpriteBlend(sprites_update_idx, dropPos[1].x, dropPos[1].y);
		GFX::DrawSpriteBlend(sprites_view_idx, dropPos[2].x, dropPos[2].y);
		// Dropdown Text.
		Gui::DrawString(dropPos[0].x+30, dropPos[0].y+5, 0.4f, config->textColor(), Lang::get("DELETE_DDM"), 100);
		Gui::DrawString(dropPos[1].x+30, dropPos[1].y+5, 0.4f, config->textColor(), Lang::get("UPDATE_DDM"), 100);
		Gui::DrawString(dropPos[2].x+30, dropPos[2].y+5, 0.4f, config->textColor(), Lang::get("VIEW_DDM"), 100);
	}

	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
}

void UniStore::Draw(void) const {
	if (mode == 0) {
		DrawSubMenu();
	} else if (mode == 1) {
		DrawStoreList();
	} else if (mode == 2) {
		DrawSearch();
	} else if (mode == 3) {
		DrawFullURLScreen();
	} else if (mode == 4) {
		DrawGitHubScreen();
	}
}

void UniStore::updateStore(int selectedStore) {
	if (checkWifiStatus()) {
		if (Msg::promptMsg(Lang::get("WOULD_YOU_LIKE_UPDATE"))) {
			if (storeInfo[selectedStore].url != "" && storeInfo[selectedStore].url != "MISSING: storeInfo.url" &&
			storeInfo[selectedStore].file != "" && storeInfo[selectedStore].file != "MISSING: storeInfo.file") {
				ScriptHelper::downloadFile(storeInfo[selectedStore].url, storeInfo[selectedStore].file, Lang::get("UPDATING"));
			}

			if (storeInfo[selectedStore].sheetURL != "" && storeInfo[selectedStore].sheetURL != "MISSING: storeInfo.sheetURL" &&
			storeInfo[selectedStore].storeSheet != "" && storeInfo[selectedStore].storeSheet != "MISSING: storeInfo.sheet") {
				ScriptHelper::downloadFile(storeInfo[selectedStore].sheetURL, storeInfo[selectedStore].storeSheet, Lang::get("UPDATING"));
			}

			// Refresh the list.
			Msg::DisplayMsg(Lang::get("REFRESHING_LIST"));
			dirContents.clear();
			storeInfo.clear();
			chdir(config->storePath().c_str());
			getDirectoryContents(dirContents, {"unistore"});
			for(uint i = 0; i < dirContents.size(); i++) {
				storeInfo.push_back(parseStoreInfo(dirContents[i].name));
				descript();
				loadStoreDesc();
			}
		}
	}
}

void UniStore::refreshList() {
	if (returnIfExist(config->storePath(), {"unistore"}) == true) {
		Msg::DisplayMsg(Lang::get("REFRESHING_LIST"));
		dirContents.clear();
		storeInfo.clear();
		chdir(config->storePath().c_str());
		getDirectoryContents(dirContents, {"unistore"});
		for(uint i = 0; i < dirContents.size(); i++) {
			storeInfo.push_back(parseStoreInfo(dirContents[i].name));
			descript();
			loadStoreDesc();
		}
		Selection = 0;
		mode = 1;
	} else {
		Msg::DisplayWarnMsg(Lang::get("GET_STORES_FIRST"));
		Selection = 0;
		mode = 0;
	}
}
void UniStore::SubMenuLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		if (specialHandling) {
			Gui::setScreen(std::make_unique<MainMenu>(), config->screenFade(), true);
		} else {
			Gui::screenBack(config->screenFade());
			return;
		}
	}

	if (hDown & KEY_UP) {
		if(Selection > 0)	Selection--;
	}
	if (hDown & KEY_DOWN) {
		if(Selection < 2)	Selection++;
	}

	if (hDown & KEY_A) {
		switch(Selection) {
			case 0:
				if (returnIfExist(config->storePath(), {"unistore"}) == true) {
					Msg::DisplayMsg(Lang::get("REFRESHING_LIST"));
					dirContents.clear();
					storeInfo.clear();
					chdir(config->storePath().c_str());
					getDirectoryContents(dirContents, {"unistore"});
					for(uint i = 0; i < dirContents.size(); i++) {
						storeInfo.push_back(parseStoreInfo(dirContents[i].name));
						descript();
						loadStoreDesc();
					}

					Selection = 0;
					mode = 1;
				} else {
					Msg::DisplayWarnMsg(Lang::get("GET_STORES_FIRST"));
				}
				break;
			case 1:
				if (checkWifiStatus()) {
					Selection = 0;
					mode = 2;
				} else {
					notConnectedMsg();
				}
				break;
			case 2:
				std::string tempStore = selectFilePath(Lang::get("SELECT_STORE_PATH"), config->storePath(), {});
				if (tempStore != "") {
					config->storePath(tempStore);
					changesMade = true;
				}
				break;
		}
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, subPos[0])) {
			if (returnIfExist(config->storePath(), {"unistore"}) == true) {
				Msg::DisplayMsg(Lang::get("REFRESHING_LIST"));
				dirContents.clear();
				storeInfo.clear();
				chdir(config->storePath().c_str());
				getDirectoryContents(dirContents, {"unistore"});
				for(uint i = 0; i < dirContents.size(); i++) {
					storeInfo.push_back(parseStoreInfo(dirContents[i].name));
					descript();
					loadStoreDesc();
				}

				Selection = 0;
				mode = 1;
			} else {
				Msg::DisplayWarnMsg(Lang::get("GET_STORES_FIRST"));
			}
		} else if (touching(touch, subPos[1])) {
			if (checkWifiStatus() == true) {
				Selection = 0;
				mode = 2;
			} else {
				notConnectedMsg();
			}
		} else if (touching(touch, subPos[2])) {
			std::string tempStore = selectFilePath(Lang::get("SELECT_STORE_PATH"), config->storePath(), {});
			if (tempStore != "") {
				config->storePath(tempStore);
				changesMade = true;
			}
		}
	}
}

void UniStore::deleteStore(int selectedStore) {
	std::string path = config->storePath();
	path += dirContents[selectedStore].name;
	deleteFile(path.c_str());
	// Refresh the list.
	Msg::DisplayMsg(Lang::get("REFRESHING_LIST"));
	Selection = 0;
	dirContents.clear();
	storeInfo.clear();
	chdir(config->storePath().c_str());
	getDirectoryContents(dirContents, {"unistore"});
	for(uint i = 0; i < dirContents.size(); i++) {
		storeInfo.push_back(parseStoreInfo(dirContents[i].name));
		descript();
		loadStoreDesc();
	}

	if (dirContents.size() == 0) {
		dirContents.clear();
		storeInfo.clear();
		Selection = 0;
		mode = 0;
	}
}


bool UniStore::handleIfDisplayText() {
	if (JSON.at("storeInfo").contains("displayInformation")) {
		if (JSON["storeInfo"]["displayInformation"] != true) {
			return false;
		} else {
			return true;
		}
	} else {
		return true;
	}
}


void UniStore::StoreSelectionLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (keyRepeatDelay)	keyRepeatDelay--;

	if (dropDownMenu) {
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
					if (Msg::promptMsg(Lang::get("DELETE_STORE"))) {
						deleteStore(Selection);
					}
					break;
				case 1:
					updateStore(Selection);
					break;
				case 2:
					if (config->viewMode() == 0) {
						config->viewMode(1);
					} else {
						config->viewMode(0);
					}
					break;
			}
			dropDownMenu = false;
		}

		if (hDown & KEY_TOUCH) {
			if (touching(touch, dropPos2[0])) {
				if (Msg::promptMsg(Lang::get("DELETE_STORE"))) {
					deleteStore(Selection);
				}
				dropDownMenu = false;
			} else if (touching(touch, dropPos2[1])) {
				updateStore(Selection);
				dropDownMenu = false;
			} else if (touching(touch, dropPos2[2])) {
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
			storeInfo.clear();
			Selection = 0;
			mode = 0;
		}

		if ((hHeld & KEY_DOWN && !keyRepeatDelay) || (hDown & KEY_TOUCH && touching(touch, arrowPos[1]))) {
			if (Selection < (int)storeInfo.size()-1) {
				Selection++;
				descript();
				loadStoreDesc();
			} else {
				Selection = 0;
				descript();
				loadStoreDesc();
			}
			
			keyRepeatDelay = config->keyDelay();
		}

		if ((hHeld & KEY_UP && !keyRepeatDelay) || (hDown & KEY_TOUCH && touching(touch, arrowPos[0]))) {
			if (Selection > 0) {
				Selection--;
				descript();
				loadStoreDesc();
			} else {
				Selection = (int)storeInfo.size()-1;
				descript();
				loadStoreDesc();
			}
			
			keyRepeatDelay = config->keyDelay();
		}

		if ((hHeld & KEY_RIGHT && !keyRepeatDelay)) {
			if (config->viewMode() == 0) {
				if (Selection < (int)storeInfo.size()-1-3) {
					Selection += 3;
					descript();
					loadStoreDesc();
				} else {
					Selection = (int)storeInfo.size()-1;
					descript();
					loadStoreDesc();
				}
			} else {
				if (Selection < (int)storeInfo.size()-1-6) {
					Selection += 7;
					descript();
					loadStoreDesc();
				} else {
					Selection = (int)storeInfo.size()-1;
					descript();
					loadStoreDesc();
				}
			}
			
			keyRepeatDelay = config->keyDelay();
		}

		if ((hHeld & KEY_LEFT && !keyRepeatDelay)) {
			if (config->viewMode() == 0) {
				if (Selection > 2) {
					Selection -= 3;
					descript();
					loadStoreDesc();
				} else {
					Selection = 0;
					descript();
					loadStoreDesc();
				}
			} else {
				if (Selection > 6) {
					Selection -= 7;
					descript();
					loadStoreDesc();
				} else {
					Selection = 0;
					descript();
					loadStoreDesc();
				}
			}
			
			keyRepeatDelay = config->keyDelay();
		}

		if (hDown & KEY_A) {
			if (!dirContents[Selection].isDirectory && storeInfo.size() != 0) {
				if (ScriptHelper::checkIfValid(dirContents[Selection].name, 1) == true) {
					updateStore(Selection);
					currentStoreFile = dirContents[Selection].name;
					Msg::DisplayMsg(Lang::get("PREPARE_STORE"));
					JSON = openStoreFile();
					displayInformations = handleIfDisplayText();
					const std::string sheetURL = storeInfo[Selection].storeSheet;

					if (storeInfo[Selection].version == 0 || storeInfo[Selection].version == 1) {
						Gui::setScreen(std::make_unique<UniStoreV1>(JSON, sheetURL, displayInformations), config->screenFade(), true);
					} else if (storeInfo[Selection].version == 2) {
						Gui::setScreen(std::make_unique<UniStoreV2>(JSON, sheetURL, currentStoreFile), config->screenFade(), true);
					} else {
						Msg::DisplayWarnMsg(Lang::get("UNISTORE_NOT_SUPPORTED"));
					}
				}
			}
		}

		if (config->viewMode() == 0) {
			if(Selection < screenPos) {
				screenPos = Selection;
			} else if (Selection > screenPos + ENTRIES_PER_SCREEN - 1) {
				screenPos = Selection - ENTRIES_PER_SCREEN + 1;
			}
		} else if (config->viewMode() == 1) {
			if(Selection < screenPosList) {
				screenPosList = Selection;
			} else if (Selection > screenPosList + ENTRIES_PER_LIST - 1) {
				screenPosList = Selection - ENTRIES_PER_LIST + 1;
			}
		}

		if (hDown & KEY_TOUCH) {
			if (config->viewMode() == 0) {
				for(int i = 0; i < ENTRIES_PER_SCREEN && i < (int)storeInfo.size(); i++) {
					if (touch.py > 40+(i*57) && touch.py < 40+(i*57)+45) {
						if (ScriptHelper::checkIfValid(dirContents[screenPos + i].name, 1) == true) {
							updateStore(screenPos + i);
							currentStoreFile = dirContents[screenPos + i].name;
							Msg::DisplayMsg(Lang::get("PREPARE_STORE"));
							JSON = openStoreFile();
							displayInformations = handleIfDisplayText();
							const std::string sheetURL = storeInfo[screenPos + i].storeSheet;
							if (storeInfo[screenPos + i].version == 0 || storeInfo[screenPos + i].version == 1) {
								Gui::setScreen(std::make_unique<UniStoreV1>(JSON, sheetURL, displayInformations), config->screenFade(), true);
							} else if (storeInfo[screenPos + i].version == 2) {
								Gui::setScreen(std::make_unique<UniStoreV2>(JSON, sheetURL, currentStoreFile), config->screenFade(), true);
							} else {
								Msg::DisplayWarnMsg(Lang::get("UNISTORE_NOT_SUPPORTED"));
							}
						}
					}
				}
			} else if (config->viewMode() == 1) {
				for(int i = 0; i < ENTRIES_PER_LIST && i < (int)storeInfo.size(); i++) {
					if (touch.py > (i+1)*27 && touch.py < (i+2)*27) {
						if (ScriptHelper::checkIfValid(dirContents[screenPosList + i].name, 1) == true) {
							updateStore(screenPosList + i);
							currentStoreFile = dirContents[screenPosList + i].name;
							Msg::DisplayMsg(Lang::get("PREPARE_STORE"));

							JSON = openStoreFile();
							displayInformations = handleIfDisplayText();
							const std::string sheetURL = storeInfo[screenPosList + i].storeSheet;
							if (storeInfo[screenPosList + i].version == 0 || storeInfo[screenPosList + i].version == 1) {
								Gui::setScreen(std::make_unique<UniStoreV1>(JSON, sheetURL, displayInformations), config->screenFade(), true);
							} else if (storeInfo[screenPosList + i].version == 2) {
								Gui::setScreen(std::make_unique<UniStoreV2>(JSON, sheetURL, currentStoreFile), config->screenFade(), true);
							} else {
								Msg::DisplayWarnMsg(Lang::get("UNISTORE_NOT_SUPPORTED"));
							}
						}
					}
				}
			}
		}

		if (hDown & KEY_START) {
			if (config->autoboot() == 1) {
				if (Msg::promptMsg(Lang::get("DISABLE_AUTOBOOT"))) {
					config->autoboot(0);
					config->autobootFile("");
					changesMade = true;
				}
			} else {
				if (!dirContents[Selection].isDirectory && storeInfo.size() != 0) {
					if (ScriptHelper::checkIfValid(dirContents[Selection].name, 1) == true) {
						if (Msg::promptMsg(Lang::get("AUTOBOOT_STORE"))) {
							config->autoboot(1);
							config->autobootFile(config->storePath() + dirContents[Selection].name);
							changesMade = true;
						}
					}
				}
			}
		}
	}
}

void UniStore::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	this->autobootLogic();

	if (mode == 0) {
		SubMenuLogic(hDown, hHeld, touch);
	} else if (mode == 1) {
		StoreSelectionLogic(hDown, hHeld, touch);
	} else if (mode == 2) {
		SearchLogic(hDown, hHeld, touch);
	} else if (mode == 3) {
		FullURLLogic(hDown, hHeld, touch);
	} else if (mode == 4) {
		GitHubLogic(hDown, hHeld, touch);
	}
}

void UniStore::DrawSearch(void) const {
	GFX::DrawTop();
	if (config->useBars() == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, config->textColor(), Lang::get("UNISTORE_SEARCH"), 400);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, config->textColor(), Lang::get("UNISTORE_SEARCH"), 400);
	}

	GFX::DrawSprite(sprites_uniStore_HD_idx, 140, 50, 0.2, 0.2);
	GFX::DrawBottom();
	GFX::DrawArrow(0, 218, 0, 1);

	GFX::DrawButton(URLBtn[0].x, URLBtn[0].y,Lang::get("FULL_URL"));
	GFX::DrawButton(URLBtn[1].x, URLBtn[1].y, Lang::get("GITHUB"));
	GFX::DrawButton(URLBtn[2].x, URLBtn[2].y, "TinyDB");
	GFX::DrawButton(URLBtn[3].x, URLBtn[3].y, "Universal DB");

	// Selector.
	Animation::Button(URLBtn[Selection].x, URLBtn[Selection].y, .060);
}

void UniStore::SearchLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		Selection = 0;
		mode = 0;
	}

	if (hDown & KEY_RIGHT || hDown & KEY_R) {
		if (Selection == 0)	Selection = 1;
		else if (Selection == 2) Selection = 3;
	}
	if (hDown & KEY_LEFT || hDown & KEY_L) {
		if (Selection == 1)	Selection = 0;
		else if (Selection == 3) Selection = 2;
	}

	if (hDown & KEY_DOWN) {
		if (Selection == 0)	Selection = 2;
		else if (Selection == 1) Selection = 3;
	}

	if (hDown & KEY_UP) {
		if (Selection == 2)	Selection = 0;
		else if (Selection == 3) Selection = 1;
	}

	if (hDown & KEY_A) {
		if (Selection == 0) {
			Selection = 0;
			mode = 3;
		} else if (Selection == 1) {
			Selection = 0;
			mode = 4;
		} else if (Selection == 2) {
			if (Msg::promptMsg("TinyDB might be down. This would lead to Download Failed!\nDo you still like to continue?")) {
				ScriptHelper::downloadFile("https://tinydb.eiphax.tech/api/tinydb.unistore", config->storePath() + "TinyDB.unistore", Lang::get("DOWNLOADING") + "TinyDB");
			}
		} else if (Selection == 3) {
			ScriptHelper::downloadFile("https://db.universal-team.net/unistore/universal-db.unistore", config->storePath() + "Universal-DB.unistore", Lang::get("DOWNLOADING") + "Universal DB");
		}
	}

	if (hDown & KEY_TOUCH && touching(touch, URLBtn[0])) {
		Selection = 0;
		mode = 3;
	} else if (hDown & KEY_TOUCH && touching(touch, URLBtn[1])) {
		Selection = 0;
		mode = 4;
	} else if (hDown & KEY_TOUCH && touching(touch, URLBtn[2])) {
		if (Msg::promptMsg("TinyDB might be down. This would lead to Download Failed!\nDo you still like to continue?")) {
			ScriptHelper::downloadFile("https://tinydb.eiphax.tech/api/tinydb.unistore", config->storePath() + "TinyDB.unistore", Lang::get("DOWNLOADING") + "TinyDB");
		}
	} else if (hDown & KEY_TOUCH && touching(touch, URLBtn[3])) {
		ScriptHelper::downloadFile("https://db.universal-team.net/unistore/universal-db.unistore", config->storePath() + "Universal-DB.unistore", Lang::get("DOWNLOADING") + "Universal DB");
	}
}

void UniStore::DrawGitHubScreen(void) const {
	GFX::DrawTop();
	if (config->useBars() == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, config->textColor(), Lang::get("GITHUB"), 400);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, config->textColor(), Lang::get("GITHUB"), 400);
	}

	GFX::DrawSprite(sprites_uniStore_HD_idx, 140, 50, 0.2, 0.2);
	GFX::DrawBottom();
	GFX::DrawArrow(0, 218, 0, 1);

	Gui::DrawStringCentered(0, 28, 0.7f, config->textColor(), Lang::get("OWNER_AND_REPO"), 300);
	Gui::DrawStringCentered(0, 108, 0.7f, config->textColor(), Lang::get("FILENAME"), 300);

	Gui::Draw_Rect(GitHubPos[0].x, GitHubPos[0].y, GitHubPos[0].w, GitHubPos[0].h, config->barColor());
	Gui::Draw_Rect(GitHubPos[1].x, GitHubPos[1].y, GitHubPos[1].w, GitHubPos[1].h, config->barColor());
	Gui::Draw_Rect(GitHubPos[2].x, GitHubPos[2].y, GitHubPos[2].w, GitHubPos[2].h, config->barColor());

	Gui::DrawStringCentered(0, 185, 0.7f, config->textColor(), Lang::get("OK"), 40);

	Gui::DrawStringCentered(0, 57, 0.5f, config->textColor(), OwnerAndRepo, 250);
	Gui::DrawStringCentered(0, 137, 0.5f, config->textColor(), fileName, 250);
}

void UniStore::GitHubLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_TOUCH && touching(touch, GitHubPos[0])) {
		OwnerAndRepo = Input::setkbdString(150, Lang::get("ENTER_OWNER_AND_REPO"));
	}

	if (hDown & KEY_TOUCH && touching(touch, GitHubPos[1])) {
		fileName = Input::setkbdString(150, Lang::get("ENTER_FILENAME"));
	}

	if (hDown & KEY_TOUCH && touching(touch, GitHubPos[2])) {
		if (checkWifiStatus() == true) {
			std::string URL = "https://github.com/";
			URL += OwnerAndRepo;
			URL += "/raw/master/unistore/";
			URL += fileName;
			ScriptHelper::downloadFile(URL, config->storePath() + fileName, Lang::get("DOWNLOADING") + fileName);
		} else {
			notConnectedMsg();
		}
	}

	if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		// Reset everything.
		OwnerAndRepo = "";
		fileName = "";
		Selection = 0;
		mode = 2;
	}
}

void UniStore::DrawFullURLScreen(void) const {
	GFX::DrawTop();
	if (config->useBars() == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, config->textColor(), Lang::get("FULL_URL"), 400);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, config->textColor(), Lang::get("FULL_URL"), 400);
	}

	GFX::DrawSprite(sprites_uniStore_HD_idx, 140, 50, 0.2, 0.2);
	GFX::DrawBottom();
	GFX::DrawArrow(0, 218, 0, 1);

	Gui::DrawStringCentered(0, 28, 0.7f, config->textColor(), Lang::get("FULL_URL"), 320);
	Gui::DrawStringCentered(0, 108, 0.7f, config->textColor(), Lang::get("FILENAME"), 320);

	Gui::Draw_Rect(GitHubPos[0].x, GitHubPos[0].y, GitHubPos[0].w, GitHubPos[0].h, config->barColor());
	Gui::Draw_Rect(GitHubPos[1].x, GitHubPos[1].y, GitHubPos[1].w, GitHubPos[1].h, config->barColor());
	Gui::Draw_Rect(GitHubPos[2].x, GitHubPos[2].y, GitHubPos[2].w, GitHubPos[2].h, config->barColor());

	Gui::DrawStringCentered(0, 185, 0.7f, config->textColor(), Lang::get("OK"), 40);

	Gui::DrawStringCentered(0, 57, 0.45f, config->textColor(), FullURL, 250);
	Gui::DrawStringCentered(0, 137, 0.45f, config->textColor(), fileName, 250);
}

void UniStore::FullURLLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_TOUCH && touching(touch, GitHubPos[0])) {
		FullURL = Input::setkbdString(150, Lang::get("ENTER_FULL_URL"));
	}

	if (hDown & KEY_TOUCH && touching(touch, GitHubPos[1])) {
		fileName = Input::setkbdString(150, Lang::get("ENTER_FILENAME"));
	}

	if (hDown & KEY_TOUCH && touching(touch, GitHubPos[2])) {
		if (checkWifiStatus() == true) {
			ScriptHelper::downloadFile(FullURL, config->storePath() + fileName, Lang::get("DOWNLOADING") + fileName);
		} else {
			notConnectedMsg();
		}
	}

	if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		// Reset everything.
		FullURL = "";
		fileName = "";
		Selection = 0;
		mode = 2;
	}
}