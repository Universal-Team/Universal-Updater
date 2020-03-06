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
#include "formatting.hpp"
#include "json.hpp"
#include "keyboard.hpp"
#include "mainMenu.hpp"
#include "scriptHelper.hpp"
#include "unistore.hpp"

#include <algorithm>
#include <regex>
#include <unistd.h>

extern bool touching(touchPosition touch, Structs::ButtonPos button);
extern u32 getColor(std::string colorString);
nlohmann::json appStoreJson;
std::string currentStoreFile;
extern bool isScriptSelected;

extern u32 barColor;
extern u32 bgTopColor;
extern u32 bgBottomColor;
extern u32 TextColor;
extern u32 progressBar;
extern u32 selected;
extern u32 unselected;
extern int AutobootWhat;
bool changeBackState = false;

C2D_SpriteSheet appStoreSheet;

struct storeInfo2 {
	std::string title;
	std::string author;
	std::string description;
	std::string url;
	std::string file;
	std::string storeSheet;
	std::string sheetURL;
};

storeInfo2 SI;

// Parse informations like URL, Title, Author, Description.
storeInfo2 parseStoreInfo(std::string fileName) {
	FILE* file = fopen(fileName.c_str(), "rt");
	if(!file) {
		printf("File not found\n");
		return {"", ""};
	}
	nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
	fclose(file);

	storeInfo2 info;
	info.title = ScriptHelper::getString(json, "storeInfo", "title");
	info.author = ScriptHelper::getString(json, "storeInfo", "author");
	info.description = ScriptHelper::getString(json, "storeInfo", "description");
	info.url = ScriptHelper::getString(json, "storeInfo", "url");
	info.file = ScriptHelper::getString(json, "storeInfo", "file");
	info.storeSheet = ScriptHelper::getString(json, "storeInfo", "sheet");
	info.sheetURL = ScriptHelper::getString(json, "storeInfo", "sheetURL");
	return info;
}

nlohmann::json openStoreFile() {
	FILE* file = fopen(currentStoreFile.c_str(), "rt");
	nlohmann::json jsonFile;
	if(file)	jsonFile = nlohmann::json::parse(file, nullptr, false);
	fclose(file);
	return jsonFile;
}

std::vector<storeInfo2> storeInfo; // Store selection.
std::vector<std::string> appStoreList; // Actual store. ;P
std::vector<std::string> descLines;
std::string storeDesc = "";

// Parse the Objects.
std::vector<std::string> parseStoreObjects(std::string storeName) {
	FILE* file = fopen(storeName.c_str(), "rt");
	if(!file) {
		printf("File not found\n");
		return {{""}};
	}
	nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
	fclose(file);

	std::vector<std::string> objs;
	for(auto it = json.begin();it != json.end(); it++) {
		if(it.key() != "storeInfo") {
			objs.push_back(it.key());
		}
	}
	return objs;
}

bool sheetHasLoaded = false;
// Sheet / Icon stuff.
void loadStoreSheet(int pos) {
	if (sheetHasLoaded == false) {
		appStoreSheet = C2D_SpriteSheetLoad(storeInfo[pos].storeSheet.c_str());
		sheetHasLoaded = true;
	}
}

void loadStoreAutobootSheet(const std::string &sheet) {
	if (sheetHasLoaded == false) {
		appStoreSheet = C2D_SpriteSheetLoad(sheet.c_str());
		sheetHasLoaded = true;
	}
}

void freeSheet() {
	if (sheetHasLoaded == true) {
		C2D_SpriteSheetFree(appStoreSheet);
		sheetHasLoaded = false;
	}
}

void drawBlend(int key, int x, int y) {
	C2D_ImageTint tint;
	C2D_SetImageTint(&tint, C2D_TopLeft, C2D_Color32(0, 0, 0, 180), 0.5);
	C2D_SetImageTint(&tint, C2D_TopRight, C2D_Color32(0, 0, 0, 180), 0.5);
	C2D_SetImageTint(&tint, C2D_BotLeft, C2D_Color32(0, 0, 0, 180), 0.5);
	C2D_SetImageTint(&tint, C2D_BotRight, C2D_Color32(0, 0, 0, 180), 0.5);
	C2D_DrawImageAt(C2D_SpriteSheetGetImage(appStoreSheet, key), x, y, 0.5f, &tint);
}

void drawNormal(int key, int x, int y) {
	C2D_DrawImageAt(C2D_SpriteSheetGetImage(appStoreSheet, key), x, y, 0.5f, NULL);
}

void UniStore::descript() {
	if (storeInfo[selection].description != "" || storeInfo[selection].description != "MISSING: storeInfo.description") {
		storeDesc = storeInfo[selection].description;
	} else storeDesc = "";
}

void loadStoreDesc(void) {
	descLines.clear();
	while(storeDesc.find('\n') != storeDesc.npos) {
		descLines.push_back(storeDesc.substr(0, storeDesc.find('\n')));
		storeDesc = storeDesc.substr(storeDesc.find('\n')+1);
	}
	descLines.push_back(storeDesc.substr(0, storeDesc.find('\n')));
}

// Store colors.
void loadStoreColors(nlohmann::json &json) {
	u32 colorTemp;
	colorTemp = getColor(ScriptHelper::getString(json, "storeInfo", "barColor"));
	barColor = colorTemp == 0 ? Config::Color1 : colorTemp;

	colorTemp = getColor(ScriptHelper::getString(json, "storeInfo", "bgTopColor"));
	bgTopColor = colorTemp == 0 ? Config::Color2 : colorTemp;

	colorTemp = getColor(ScriptHelper::getString(json, "storeInfo", "bgBottomColor"));
	bgBottomColor = colorTemp == 0 ? Config::Color3 : colorTemp;

	colorTemp = getColor(ScriptHelper::getString(json, "storeInfo", "textColor"));
	TextColor = colorTemp == 0 ? Config::TxtColor : colorTemp;

	colorTemp = getColor(ScriptHelper::getString(json, "storeInfo", "selectedColor"));
	selected = colorTemp == 0 ? Config::SelectedColor : colorTemp;

	colorTemp = getColor(ScriptHelper::getString(json, "storeInfo", "unselectedColor"));
	unselected = colorTemp == 0 ? Config::UnselectedColor : colorTemp;

	colorTemp = getColor(ScriptHelper::getString(json, "storeInfo", "progressbarColor"));
	progressBar = colorTemp == 0 ? Config::progressbarColor : colorTemp;
}

UniStore::UniStore() {
	// Autobooting UniStore. ;)
	if (AutobootWhat == 1) {
		// If store isn't found, go to MainMenu.
		if (access(Config::AutobootFile.c_str(), F_OK) != 0) {
			AutobootWhat = 0;
			changeBackState = true;
			Gui::setScreen(std::make_unique<MainMenu>());
		}

		// If WiFi enabled & File exist, update store.
		if (ScriptHelper::checkIfValid(Config::AutobootFile, 1) == true) {
			SI = parseStoreInfo(Config::AutobootFile);
			if (checkWifiStatus()) {
				if (Msg::promptMsg(Lang::get("WOULD_YOU_LIKE_UPDATE"))) {
					if(SI.url != "" && SI.url != "MISSING: storeInfo.url" && SI.file != "" && SI.file != "MISSING: storeInfo.file") {
						ScriptHelper::downloadFile(SI.url, SI.file, Lang::get("UPDATING"));
					}
					if(SI.sheetURL != "" && SI.sheetURL != "MISSING: storeInfo.sheetURL" && SI.storeSheet != "" && SI.storeSheet != "MISSING: storeInfo.sheet") {
						ScriptHelper::downloadFile(SI.sheetURL, SI.storeSheet, Lang::get("UPDATING"));
					}
				}
			}
		} else {
			AutobootWhat = 0;
			changeBackState = true;
			Gui::setScreen(std::make_unique<MainMenu>());
		}

		if (ScriptHelper::checkIfValid(Config::AutobootFile, 1) == true) {
			currentStoreFile = Config::AutobootFile;
			Msg::DisplayMsg(Lang::get("PREPARE_STORE"));
			if (SI.storeSheet != "" || SI.storeSheet != "MISSING: storeInfo.sheet") {
				if(access(SI.storeSheet.c_str(), F_OK) != -1 ) {
					loadStoreAutobootSheet(SI.storeSheet);
				}
			}
			appStoreJson = openStoreFile();
			appStoreList = parseStoreObjects(currentStoreFile);
			loadStoreColors(appStoreJson);
			selectedOptionAppStore = 0;
			displayInformations = handleIfDisplayText();
			isScriptSelected = true;
			mode = 2;
			AutobootWhat = 0;
			changeBackState = true;
		} else {
			AutobootWhat = 0;
			changeBackState = true;
			Gui::setScreen(std::make_unique<MainMenu>());
		}
	}
}

extern void notImplemented(void);

void UniStore::DrawSubMenu(void) const {
	GFX::DrawTop();
	if (Config::UseBars == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, Lang::get("UNISTORE_SUBMENU"), 400);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, Lang::get("UNISTORE_SUBMENU"), 400);
	}

	GFX::DrawSprite(sprites_uniStore_HD_idx, 140, 50, 0.2, 0.2);
	GFX::DrawBottom();
	GFX::DrawArrow(0, 218, 0, 1);

	for (int i = 0; i < 3; i++) {
		Gui::Draw_Rect(subPos[i].x, subPos[i].y, subPos[i].w, subPos[i].h, Config::UnselectedColor);
		if (subSelection == i) {
			Gui::drawAnimatedSelector(subPos[i].x, subPos[i].y, subPos[i].w, subPos[i].h, .060, TRANSPARENT, Config::SelectedColor);
		}
	}

	Gui::DrawStringCentered(0, subPos[0].y+10, 0.6f, Config::TxtColor, Lang::get("STORE_LIST"), 130);
	Gui::DrawStringCentered(0, subPos[1].y+10, 0.6f, Config::TxtColor, Lang::get("STORE_SEARCH"), 130);
	Gui::DrawStringCentered(0, subPos[2].y+10, 0.6f, Config::TxtColor, Lang::get("CHANGE_STOREPATH"), 130);
}


// First Screen -> Storelist.
void UniStore::DrawStoreList(void) const {
	std::string line1;
	std::string line2;
	std::string storeAmount = std::to_string(selection +1) + " / " + std::to_string(storeInfo.size());
	GFX::DrawTop();
	if (Config::UseBars == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, storeInfo[selection].title, 400);
		Gui::DrawString(397-Gui::GetStringWidth(0.6f, storeAmount), 239-Gui::GetStringHeight(0.6f, storeAmount), 0.6f, Config::TxtColor, storeAmount);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, storeInfo[selection].title, 400);
		Gui::DrawString(397-Gui::GetStringWidth(0.6f, storeAmount), 237-Gui::GetStringHeight(0.6f, storeAmount), 0.6f, Config::TxtColor, storeAmount);
	}
	for(uint i=0;i<descLines.size();i++) {
		Gui::DrawStringCentered(0, 120-((descLines.size()*20)/2)+i*20, 0.6f, Config::TxtColor, descLines[i], 400);
	}

	GFX::DrawBottom();
	GFX::DrawArrow(295, -1);
	GFX::DrawArrow(315, 240, 180.0);
	GFX::DrawArrow(0, 218, 0, 1);
	GFX::DrawSpriteBlend(sprites_view_idx, arrowPos[3].x, arrowPos[3].y);
	GFX::DrawSpriteBlend(sprites_search_idx, arrowPos[4].x, arrowPos[4].y);
	GFX::DrawSpriteBlend(sprites_update_idx, arrowPos[5].x, arrowPos[5].y);

	if (Config::viewMode == 0) {
		for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)storeInfo.size();i++) {
			Gui::Draw_Rect(0, 40+(i*57), 320, 45, Config::UnselectedColor);
			line1 = storeInfo[screenPos + i].title;
			line2 = storeInfo[screenPos + i].author;
			if(screenPos + i == selection) {
				Gui::drawAnimatedSelector(0, 40+(i*57), 320, 45, .060, TRANSPARENT, Config::SelectedColor);
			}
			Gui::DrawStringCentered(0, 38+(i*57), 0.7f, Config::TxtColor, line1, 320);
			Gui::DrawStringCentered(0, 62+(i*57), 0.7f, Config::TxtColor, line2, 320);
		}
	} else if (Config::viewMode == 1) {
		for(int i=0;i<ENTRIES_PER_LIST && i<(int)storeInfo.size();i++) {
			Gui::Draw_Rect(0, (i+1)*27, 320, 25, Config::UnselectedColor);
			line1 = storeInfo[screenPosList + i].title;
			if(screenPosList + i == selection) {
				Gui::drawAnimatedSelector(0, (i+1)*27, 320, 25, .060, TRANSPARENT, Config::SelectedColor);
			}
			Gui::DrawStringCentered(0, ((i+1)*27)+1, 0.7f, Config::TxtColor, line1, 320);
		}
	}
}

void UniStore::DrawStore(void) const {
	std::string entryAmount = std::to_string(selectedOptionAppStore+1) + " / " + std::to_string((int)appStoreJson.at("storeContent").size());
	std::string info;
	GFX::DrawTop();
	// Top Background.
	if (appStoreJson.at("storeInfo").contains("iconIndexTop") && sheetHasLoaded == true) {
		drawNormal(appStoreJson["storeInfo"]["iconIndexTop"], 0, 0);
	}

	// Icon.
	if (appStoreJson.at("storeContent").at(selectedOptionAppStore).at("info").contains("iconIndex") && sheetHasLoaded == true) {
		if (appStoreJson.at("storeContent").at(selectedOptionAppStore).at("info").contains("posX") && appStoreJson.at("storeContent").at(selectedOptionAppStore).at("info").contains("posY")) {
			drawNormal(appStoreJson["storeContent"][selectedOptionAppStore]["info"]["iconIndex"], appStoreJson["storeContent"][selectedOptionAppStore]["info"]["posX"], appStoreJson["storeContent"][selectedOptionAppStore]["info"]["posY"]);
		} else {
			drawNormal(appStoreJson["storeContent"][selectedOptionAppStore]["info"]["iconIndex"], 175, 155);
		}
	}

	if (displayInformations != false) {
		if (Config::UseBars == true) {
			Gui::DrawStringCentered(0, 0, 0.7f, TextColor, std::string(appStoreJson["storeInfo"]["title"]), 400);
			Gui::DrawString(397-Gui::GetStringWidth(0.6f, entryAmount), 239-Gui::GetStringHeight(0.6f, entryAmount), 0.6f, TextColor, entryAmount);
		} else {
			Gui::DrawStringCentered(0, 2, 0.7f, TextColor, std::string(appStoreJson["storeInfo"]["title"]), 400);
			Gui::DrawString(397-Gui::GetStringWidth(0.6f, entryAmount), 237-Gui::GetStringHeight(0.6f, entryAmount), 0.6f, TextColor, entryAmount);
		}

		Gui::DrawStringCentered(0, 32, 0.6f, TextColor, Lang::get("TITLE") + std::string(appStoreJson["storeContent"][selectedOptionAppStore]["info"]["title"]), 400);
		Gui::DrawStringCentered(0, 57, 0.6f, TextColor, Lang::get("AUTHOR") + std::string(appStoreJson["storeContent"][selectedOptionAppStore]["info"]["author"]), 400);
		Gui::DrawStringCentered(0, 82, 0.6f, TextColor, Lang::get("DESC") + std::string(appStoreJson["storeContent"][selectedOptionAppStore]["info"]["description"]), 400);

		if (appStoreJson["storeContent"][selectedOptionAppStore]["info"]["version"] != "") {
			Gui::DrawStringCentered(0, 107, 0.6f, TextColor, Lang::get("VERSION") + std::string(appStoreJson["storeContent"][selectedOptionAppStore]["info"]["version"]), 400);
		} else {
			Gui::DrawStringCentered(0, 107, 0.6f, TextColor, Lang::get("VERSION") + Lang::get("UNKNOWN"), 400);
		}

		if (appStoreJson["storeContent"][selectedOptionAppStore]["info"]["fileSize"] != 0) {
			Gui::DrawStringCentered(0, 132, 0.6f, TextColor, Lang::get("FILE_SIZE") + formatBytes(int64_t(appStoreJson["storeContent"][selectedOptionAppStore]["info"]["fileSize"])), 400);
		} else {
			Gui::DrawStringCentered(0, 132, 0.6f, TextColor, Lang::get("FILE_SIZE") + Lang::get("UNKNOWN"), 400);
		}
	}
	
	GFX::DrawBottom();
	// Bottom Background.
	if (appStoreJson.at("storeInfo").contains("iconIndexBottom") && sheetHasLoaded == true) {
		drawNormal(appStoreJson["storeInfo"]["iconIndexBottom"], 0, 0);
	}

	GFX::DrawArrow(295, -1);
	GFX::DrawArrow(315, 240, 180.0);
	GFX::DrawArrow(0, 218, 0, 1);
	GFX::DrawSpriteBlend(sprites_view_idx, arrowPos[3].x, arrowPos[3].y);

	if (Config::viewMode == 0) {
		for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)appStoreJson.at("storeContent").size();i++) {
			info = appStoreJson["storeContent"][screenPos2 + i]["info"]["title"];
			if(screenPos2 + i == selectedOptionAppStore) {
				if (appStoreJson.at("storeInfo").contains("buttonLarge") && sheetHasLoaded == true) {
					drawNormal(appStoreJson["storeInfo"]["buttonLarge"], 0, 40+(i*57));
				} else {
					Gui::drawAnimatedSelector(0, 40+(i*57), 320, 45, .060, TRANSPARENT, selected);
				}
			} else {
				if (appStoreJson.at("storeInfo").contains("buttonLarge") && sheetHasLoaded == true) {
					drawBlend(appStoreJson["storeInfo"]["buttonLarge"], 0, 40+(i*57));
				} else {
					Gui::Draw_Rect(0, 40+(i*57), 320, 45, unselected);
				}
			}
			Gui::DrawStringCentered(0, 50+(i*57), 0.7f, TextColor, info, 320);
		}
	} else if (Config::viewMode == 1) {
		for(int i=0;i<ENTRIES_PER_LIST && i<(int)appStoreJson.at("storeContent").size();i++) {
			info = appStoreJson["storeContent"][screenPosList2 + i]["info"]["title"];
			if(screenPosList2 + i == selectedOptionAppStore) {
				if (appStoreJson.at("storeInfo").contains("buttonSmall") && sheetHasLoaded == true) {
					drawNormal(appStoreJson["storeInfo"]["buttonSmall"], 0, (i+1)*27);
				} else {
					Gui::drawAnimatedSelector(0, (i+1)*27, 320, 25, .060, TRANSPARENT, selected);
				}
			} else {
				if (appStoreJson.at("storeInfo").contains("buttonSmall") && sheetHasLoaded == true) {
					drawBlend(appStoreJson["storeInfo"]["buttonSmall"], 0, (i+1)*27);
				} else {
					Gui::Draw_Rect(0, (i+1)*27, 320, 25, unselected);
				}
			}
			Gui::DrawStringCentered(0, ((i+1)*27)+1, 0.7f, TextColor, info, 320);
		}
	}
}

void UniStore::Draw(void) const {
	if (mode == 0) {
		DrawSubMenu();
	} else if (mode == 1) {
		DrawStoreList();
	} else if (mode == 2) {
		DrawStore();
	} else if (mode == 3) {
		DrawSearch();
	} else if (mode == 4) {
		DrawFullURLScreen();
	} else if (mode == 5) {
		DrawGitHubScreen();
	}
}

void UniStore::updateStore(int selectedStore) {
	if (checkWifiStatus()) {
		if (Msg::promptMsg(Lang::get("WOULD_YOU_LIKE_UPDATE"))) {
			if(storeInfo[selectedStore].url != "" && storeInfo[selectedStore].url != "MISSING: storeInfo.url" &&
			storeInfo[selectedStore].file != "" && storeInfo[selectedStore].file != "MISSING: storeInfo.file") {
				ScriptHelper::downloadFile(storeInfo[selectedStore].url, storeInfo[selectedStore].file, Lang::get("UPDATING"));
			}
			if(storeInfo[selectedStore].sheetURL != "" && storeInfo[selectedStore].sheetURL != "MISSING: storeInfo.sheetURL" &&
			storeInfo[selectedStore].storeSheet != "" && storeInfo[selectedStore].storeSheet != "MISSING: storeInfo.sheet") {
				ScriptHelper::downloadFile(storeInfo[selectedStore].sheetURL, storeInfo[selectedStore].storeSheet, Lang::get("UPDATING"));
			}
			// Refresh the list.
			Msg::DisplayMsg(Lang::get("REFRESHING_LIST"));
			dirContents.clear();
			storeInfo.clear();
			chdir(Config::StorePath.c_str());
			getDirectoryContents(dirContents, {"unistore"});
			for(uint i=0;i<dirContents.size();i++) {
				storeInfo.push_back(parseStoreInfo(dirContents[i].name));
				descript();
				loadStoreDesc();
			}
		}
	} else {
		notConnectedMsg();
	}
}

void UniStore::refreshList() {
	if (returnIfExist(Config::StorePath, {"unistore"}) == true) {
		Msg::DisplayMsg(Lang::get("REFRESHING_LIST"));
		dirContents.clear();
		storeInfo.clear();
		chdir(Config::StorePath.c_str());
		getDirectoryContents(dirContents, {"unistore"});
		for(uint i=0;i<dirContents.size();i++) {
			storeInfo.push_back(parseStoreInfo(dirContents[i].name));
			descript();
			loadStoreDesc();
		}
		mode = 1;
	} else {
		Msg::DisplayWarnMsg(Lang::get("GET_STORES_FIRST"));
		mode = 0;
	}
}
void UniStore::SubMenuLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		if (changeBackState) {
			Gui::setScreen(std::make_unique<MainMenu>());
		} else {
			Gui::screenBack();
			return;
		}
	}

	if (hDown & KEY_UP) {
		if(subSelection > 0)	subSelection--;
	}
	if (hDown & KEY_DOWN) {
		if(subSelection < 2)	subSelection++;
	}

	if (hDown & KEY_A) {
		switch(subSelection) {
			case 0:
				if (returnIfExist(Config::StorePath, {"unistore"}) == true) {
					Msg::DisplayMsg(Lang::get("REFRESHING_LIST"));
					dirContents.clear();
					storeInfo.clear();
					chdir(Config::StorePath.c_str());
					getDirectoryContents(dirContents, {"unistore"});
					for(uint i=0;i<dirContents.size();i++) {
						storeInfo.push_back(parseStoreInfo(dirContents[i].name));
						descript();
						loadStoreDesc();
					}
					mode = 1;
				} else {
					Msg::DisplayWarnMsg(Lang::get("GET_STORES_FIRST"));
				}
				break;
			case 1:
				if (checkWifiStatus()) {
					mode = 3;
				} else {
					notConnectedMsg();
				}
				break;
			case 2:
				std::string tempStore = selectFilePath(Lang::get("SELECT_STORE_PATH"), {});
				if (tempStore != "") {
					Config::StorePath = tempStore;
				}
				break;
		}
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, subPos[0])) {
			if (returnIfExist(Config::StorePath, {"unistore"}) == true) {
				Msg::DisplayMsg(Lang::get("REFRESHING_LIST"));
				dirContents.clear();
				storeInfo.clear();
				chdir(Config::StorePath.c_str());
				getDirectoryContents(dirContents, {"unistore"});
				for(uint i=0;i<dirContents.size();i++) {
					storeInfo.push_back(parseStoreInfo(dirContents[i].name));
					descript();
					loadStoreDesc();
				}
				mode = 1;
			} else {
				Msg::DisplayWarnMsg(Lang::get("GET_STORES_FIRST"));
			}
		} else if (touching(touch, subPos[1])) {
			if (checkWifiStatus() == true) {
				mode = 3;
			} else {
				notConnectedMsg();
			}
		} else if (touching(touch, subPos[2])) {
			std::string tempStore = selectFilePath(Lang::get("SELECT_STORE_PATH"), {});
			if (tempStore != "") {
				Config::StorePath = tempStore;
			}
		}
	}
}

void UniStore::deleteStore(int selectedStore) {
	std::string path = Config::StorePath;
	path += dirContents[selectedStore].name;
	deleteFile(path.c_str());
	// Refresh the list.
	Msg::DisplayMsg(Lang::get("REFRESHING_LIST"));
	dirContents.clear();
	storeInfo.clear();
	chdir(Config::StorePath.c_str());
	getDirectoryContents(dirContents, {"unistore"});
	for(uint i=0;i<dirContents.size();i++) {
		storeInfo.push_back(parseStoreInfo(dirContents[i].name));
		descript();
		loadStoreDesc();
	}
	if (dirContents.size() == 0) {
		dirContents.clear();
		storeInfo.clear();
		mode = 0;
	}
}


bool UniStore::handleIfDisplayText() {
	if (appStoreJson.at("storeInfo").contains("displayInformation")) {
		if (appStoreJson["storeInfo"]["displayInformation"] != true) {
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

	if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		storeInfo.clear();
		mode = 0;
	}

	if ((hHeld & KEY_DOWN && !keyRepeatDelay) || (hDown & KEY_TOUCH && touching(touch, arrowPos[1]))) {
		if (selection < (int)storeInfo.size()-1) {
			selection++;
			descript();
			loadStoreDesc();
		} else {
			selection = 0;
			descript();
			loadStoreDesc();
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
			descript();
			loadStoreDesc();
		} else {
			selection = (int)storeInfo.size()-1;
			descript();
			loadStoreDesc();
		}
		if (fastMode == true) {
			keyRepeatDelay = 3;
		} else if (fastMode == false){
			keyRepeatDelay = 6;
		}
	}

	if ((hDown & KEY_Y) || (hDown & KEY_TOUCH && touching(touch, arrowPos[5]))) {
		updateStore(selection);
	}

	if (hDown & KEY_A) {
		if (dirContents[selection].isDirectory) {
		} else if (storeInfo.size() != 0) {
			if (ScriptHelper::checkIfValid(dirContents[selection].name, 1) == true) {
				updateStore(selection);
				currentStoreFile = dirContents[selection].name;
				Msg::DisplayMsg(Lang::get("PREPARE_STORE"));
				if (storeInfo[selection].storeSheet != "" || storeInfo[selection].storeSheet != "MISSING: storeInfo.sheet") {
					if(access(storeInfo[selection].storeSheet.c_str(), F_OK) != -1 ) {
						loadStoreSheet(selection);
					}
				}
				appStoreJson = openStoreFile();
				appStoreList = parseStoreObjects(currentStoreFile);
				loadStoreColors(appStoreJson);
				selectedOptionAppStore = 0;
				displayInformations = handleIfDisplayText();
				isScriptSelected = true;
				mode = 2;
			}
		}
	}

	if (hDown & KEY_R) {
		fastMode = true;
	}

	if (hDown & KEY_L) {
		fastMode = false;
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

	if (hDown & KEY_SELECT) {
		if (Msg::promptMsg(Lang::get("DELETE_STORE"))) {
			deleteStore(selection);
		}
	}

	if (hDown & KEY_TOUCH) {
		if (Config::viewMode == 0) {
			for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)storeInfo.size();i++) {
				if(touch.py > 40+(i*57) && touch.py < 40+(i*57)+45) {
					if (ScriptHelper::checkIfValid(dirContents[screenPos + i].name, 1) == true) {
						updateStore(screenPos + i);
						currentStoreFile = dirContents[screenPos + i].name;
						Msg::DisplayMsg(Lang::get("PREPARE_STORE"));
						if (storeInfo[screenPos + i].storeSheet != "" || storeInfo[screenPos + i].storeSheet != "MISSING: storeInfo.sheet") {
							if(access(storeInfo[screenPos + i].storeSheet.c_str(), F_OK) != -1 ) {
								loadStoreSheet(screenPos + i);
							}
						}
						appStoreJson = openStoreFile();
						appStoreList = parseStoreObjects(currentStoreFile);
						loadStoreColors(appStoreJson);
						selectedOptionAppStore = 0;
						displayInformations = handleIfDisplayText();
						isScriptSelected = true;
						mode = 2;
					}
				}
			}
		} else if (Config::viewMode == 1) {
			for(int i=0;i<ENTRIES_PER_LIST && i<(int)storeInfo.size();i++) {
				if(touch.py > (i+1)*27 && touch.py < (i+2)*27) {
					if (ScriptHelper::checkIfValid(dirContents[screenPosList + i].name, 1) == true) {
						updateStore(screenPosList + i);
						currentStoreFile = dirContents[screenPosList + i].name;
						Msg::DisplayMsg(Lang::get("PREPARE_STORE"));
						if (storeInfo[screenPosList + i].storeSheet != "" || storeInfo[screenPosList + i].storeSheet != "MISSING: storeInfo.sheet") {
							if(access(storeInfo[screenPosList + i].storeSheet.c_str(), F_OK) != -1 ) {
								loadStoreSheet(screenPosList + i);
							}
						}
						appStoreJson = openStoreFile();
						appStoreList = parseStoreObjects(currentStoreFile);
						loadStoreColors(appStoreJson);
						selectedOptionAppStore = 0;
						displayInformations = handleIfDisplayText();
						isScriptSelected = true;
						mode = 2;
					}
				}
			}
		}
	}

	if (hDown & KEY_START) {
		if (Config::autoboot == 1) {
			if (Msg::promptMsg(Lang::get("DISABLE_AUTOBOOT"))) {
				Config::autoboot = 0;
				Config::AutobootFile = "";
			}
		} else {
			if (dirContents[selection].isDirectory) {
			} else if (storeInfo.size() != 0) {
				if (ScriptHelper::checkIfValid(dirContents[selection].name, 1) == true) {
					if (Msg::promptMsg(Lang::get("AUTOBOOT_STORE"))) {
						Config::AutobootFile = Config::StorePath + dirContents[selection].name;
						Config::autoboot = 1;
					}
				}
			}
		}
	}
}

void UniStore::StoreLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (keyRepeatDelay)	keyRepeatDelay--;

	if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		refreshList();
		appStoreList.clear();
		isScriptSelected = false;
		selection2 = 0;
		if (sheetHasLoaded == true) {
			freeSheet();
		}
	}

	if (hDown & KEY_R) {
		fastMode = true;
	}

	if (hDown & KEY_L) {
		fastMode = false;
	}

	// Go one entry up.
	if ((hHeld & KEY_UP && !keyRepeatDelay) || (hDown & KEY_TOUCH && touching(touch, arrowPos[0]))) {
		if (selectedOptionAppStore > 0) {
			selectedOptionAppStore--;
		} else {
			selectedOptionAppStore = (int)appStoreJson.at("storeContent").size()-1;
		}
		if (fastMode == true) {
			keyRepeatDelay = 3;
		} else if (fastMode == false){
			keyRepeatDelay = 6;
		}
	}

	// Go one entry down.
	if ((hHeld & KEY_DOWN && !keyRepeatDelay) || (hDown & KEY_TOUCH && touching(touch, arrowPos[1]))) {
		if (selectedOptionAppStore < (int)appStoreJson.at("storeContent").size()-1) {
			selectedOptionAppStore++;
		} else {
			selectedOptionAppStore = 0;
		}
		if (fastMode == true) {
			keyRepeatDelay = 3;
		} else if (fastMode == false){
			keyRepeatDelay = 6;
		}
	}

	// Execute touched Entry.
	if (hDown & KEY_TOUCH) {
		if (Config::viewMode == 0) {
			for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)appStoreJson.at("storeContent").size();i++) {
				if(touch.py > 40+(i*57) && touch.py < 40+(i*57)+45) {
					selectedOptionAppStore = screenPos2 + i;
					execute();
				}
			}
		} else if (Config::viewMode == 1) {
			for(int i=0;i<ENTRIES_PER_LIST && i<(int)appStoreJson.at("storeContent").size();i++) {
				if(touch.py > (i+1)*27 && touch.py < (i+2)*27) {
					selectedOptionAppStore = screenPosList2 + i;
					execute();
				}
			}
		}
	}
	// Execute that Entry.
	if (hDown & KEY_A) {
		execute();
	}

	if (Config::viewMode == 0) {
		if(selectedOptionAppStore < screenPos2) {
			screenPos2 = selectedOptionAppStore;
		} else if (selectedOptionAppStore > screenPos2 + ENTRIES_PER_SCREEN - 1) {
			screenPos2 = selectedOptionAppStore - ENTRIES_PER_SCREEN + 1;
		}
	} else if (Config::viewMode == 1) {
		if(selectedOptionAppStore < screenPosList2) {
			screenPosList2 = selectedOptionAppStore;
		} else if (selectedOptionAppStore > screenPosList2 + ENTRIES_PER_LIST - 1) {
			screenPosList2 = selectedOptionAppStore - ENTRIES_PER_LIST + 1;
		}
	}
}

void UniStore::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (mode == 0) {
		SubMenuLogic(hDown, hHeld, touch);
	} else if (mode == 1) {
		StoreSelectionLogic(hDown, hHeld, touch);
	} else if (mode == 2) {
		StoreLogic(hDown, hHeld, touch);
	} else if (mode == 3) {
		SearchLogic(hDown, hHeld, touch);
	} else if (mode == 4) {
		FullURLLogic(hDown, hHeld, touch);
	} else if (mode == 5) {
		GitHubLogic(hDown, hHeld, touch);
	}

	// Switch ViewMode.
	if (((mode != 0 || mode != 3) && (hDown & KEY_X)) || ((mode != 0 || mode != 3) && (hDown & KEY_TOUCH && touching(touch, arrowPos[3])))) {
		if (Config::viewMode == 0) {
			Config::viewMode = 1;
		} else {
			Config::viewMode = 0;
		}
	}
}

// Execute Entry.
void UniStore::execute() {
	for(int i=0;i<(int)appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").size();i++) {
		std::string type = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("type");
		if(type == "deleteFile") {
			bool missing = false;
			std::string file, message;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("file"))	file = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("file");
			else	missing = true;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("message"))	message = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("message");
			if(!missing)	ScriptHelper::removeFile(file, message);

		} else if(type == "downloadFile") {
			bool missing = false;
			std::string file, output, message;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("file"))	file = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("file");
			else	missing = true;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("output"))	output = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("output");
			else	missing = true;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("message"))	message = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("message");
			if(!missing)	ScriptHelper::downloadFile(file, output, message);

		} else if(type == "downloadRelease") {
			bool missing = false, includePrereleases = false;
			std::string repo, file, output, message;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("repo"))	repo = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("repo");
			else	missing = true;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("file"))	file = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("file");
			else	missing = true;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("output"))	output = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("output");
			else	missing = true;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("includePrereleases") && appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("includePrereleases").is_boolean())
				includePrereleases = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("includePrereleases");
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("message"))	message = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("message");
			if(!missing)	ScriptHelper::downloadRelease(repo, file, output, includePrereleases, message);

		} else if(type == "extractFile") {
			bool missing = false;
			std::string file, input, output, message;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("file"))	file = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("file");
			else	missing = true;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("input"))	input = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("input");
			else	missing = true;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("output"))	output = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("output");
			else	missing = true;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("message"))	message = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("message");
			if(!missing)	ScriptHelper::extractFile(file, input, output, message);

		} else if(type == "installCia") {
			bool missing = false;
			std::string file, message;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("file"))	file = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("file");
			else	missing = true;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("message"))	message = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("message");
			if(!missing)	ScriptHelper::installFile(file, message);
	
		} else if (type == "mkdir") {
			bool missing = false;
			std::string directory, message;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("directory"))	directory = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("directory");
			else	missing = true;
			if(!missing)	makeDirs(directory.c_str());

		} else if (type == "rmdir") {
			bool missing = false;
			std::string directory, message, promptmsg;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("directory"))	directory = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("directory");
			else	missing = true;
			promptmsg = Lang::get("DELETE_PROMPT") + "\n" + directory;
			if(!missing) {
				if (Msg::promptMsg(promptmsg)) {
					removeDirRecursive(directory.c_str());
				}
			}

		} else if (type == "mkfile") {
			bool missing = false;
			std::string file;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("file"))	file = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("file");
			else	missing = true;
			if(!missing)	ScriptHelper::createFile(file.c_str());

		} else if (type == "timeMsg") {
			bool missing = false;
			std::string message;
			int seconds;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("message"))	message = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("message");
			else	missing = true;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("seconds") && appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("seconds").is_number())
			seconds = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("seconds");
			else	missing = true;
			if(!missing)	ScriptHelper::displayTimeMsg(message, seconds);
		} else if (type == "saveConfig") {
			Config::save();
		} else if (type == "notImplemented") {
			notImplemented();
		} else if (type == "deleteTitle") {
			std::string TitleID = "";
			std::string message = "";
			bool isNAND = false, missing = false;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("TitleID"))	TitleID = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("TitleID");
			else	missing = true;
			if (appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("NAND") && appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("NAND").is_boolean())	isNAND = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("NAND");
			else	missing = true;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("message"))	message = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("message");
			else	missing = true;
			if(!missing)	ScriptHelper::deleteTitle(TitleID, isNAND, message);
		} else if (type == "bootTitle") {
			std::string TitleID = "";
			std::string message = "";
			bool isNAND = false, missing = false;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("TitleID"))	TitleID = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("TitleID");
			else	missing = true;
			if (appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("NAND") && appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("NAND").is_boolean())	isNAND = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("NAND");
			else	missing = true;
			if(appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).contains("message"))	message = appStoreJson.at("storeContent").at(selectedOptionAppStore).at("script").at(i).at("message");
			else	missing = true;
			if(!missing)	ScriptHelper::bootTitle(TitleID, isNAND, message);
		}
	}
	doneMsg();
}

void UniStore::DrawSearch(void) const {
	GFX::DrawTop();
	if (Config::UseBars == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, Lang::get("UNISTORE_SEARCH"), 400);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, Lang::get("UNISTORE_SEARCH"), 400);
	}

	GFX::DrawSprite(sprites_uniStore_HD_idx, 140, 50, 0.2, 0.2);
	GFX::DrawBottom();
	GFX::DrawArrow(0, 218, 0, 1);

	for (int i = 0; i < 3; i++) {
		Gui::Draw_Rect(URLBtn[i].x, URLBtn[i].y, URLBtn[i].w, URLBtn[i].h, Config::UnselectedColor);
		if (searchSelection == i) {
			Gui::drawAnimatedSelector(URLBtn[i].x, URLBtn[i].y, URLBtn[i].w, URLBtn[i].h, .060, TRANSPARENT, Config::SelectedColor);
		}
	}

	Gui::DrawStringCentered(-80, URLBtn[0].y+12, 0.6f, Config::TxtColor, Lang::get("FULL_URL"), 130);
	Gui::DrawStringCentered(80, URLBtn[1].y+12, 0.6f, Config::TxtColor, Lang::get("GITHUB"), 130);
	Gui::DrawStringCentered(-80, URLBtn[2].y+12, 0.6f, Config::TxtColor, "TinyDB", 130);
}

void UniStore::SearchLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		mode = 0;
	}

	if (hDown & KEY_RIGHT || hDown & KEY_R) {
		if (searchSelection == 0)	searchSelection = 1;
	}
	if (hDown & KEY_LEFT || hDown & KEY_L) {
		if (searchSelection == 1)	searchSelection = 0;
	}

	if (hDown & KEY_DOWN) {
		if (searchSelection == 0)	searchSelection = 2;
	}

	if (hDown & KEY_UP) {
		if (searchSelection == 2)	searchSelection = 0;
	}

	if (hDown & KEY_A) {
		if (searchSelection == 0) {
			mode = 4;
		} else if (searchSelection == 1) {
			mode = 5;
		} else if (searchSelection == 2) {
			ScriptHelper::downloadFile("https://tinydb.eiphax.tech/api/tinydb.unistore", Config::StorePath + "TinyDB.unistore", Lang::get("DOWNLOADING") + "TinyDB");
		}
	}

	if (hDown & KEY_TOUCH && touching(touch, URLBtn[0])) {
		mode = 4;
	} else if (hDown & KEY_TOUCH && touching(touch, URLBtn[1])) {
		mode = 5;
	} else if (hDown & KEY_TOUCH && touching(touch, URLBtn[2])) {
		ScriptHelper::downloadFile("https://tinydb.eiphax.tech/api/tinydb.unistore", Config::StorePath + "TinyDB.unistore", Lang::get("DOWNLOADING") + "TinyDB");
	}
}

void UniStore::DrawGitHubScreen(void) const {
	GFX::DrawTop();
	if (Config::UseBars == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, Lang::get("GITHUB"), 400);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, Lang::get("GITHUB"), 400);
	}

	GFX::DrawSprite(sprites_uniStore_HD_idx, 140, 50, 0.2, 0.2);
	GFX::DrawBottom();
	GFX::DrawArrow(0, 218, 0, 1);

	Gui::DrawStringCentered(0, 28, 0.7f, Config::TxtColor, Lang::get("OWNER_AND_REPO"), 300);
	Gui::DrawStringCentered(0, 108, 0.7f, Config::TxtColor, Lang::get("FILENAME"), 300);

	Gui::Draw_Rect(GitHubPos[0].x, GitHubPos[0].y, GitHubPos[0].w, GitHubPos[0].h, Config::Color1);
	Gui::Draw_Rect(GitHubPos[1].x, GitHubPos[1].y, GitHubPos[1].w, GitHubPos[1].h, Config::Color1);
	Gui::Draw_Rect(GitHubPos[2].x, GitHubPos[2].y, GitHubPos[2].w, GitHubPos[2].h, Config::Color1);

	Gui::DrawStringCentered(0, 185, 0.7f, Config::TxtColor, Lang::get("OK"), 40);

	Gui::DrawStringCentered(0, 57, 0.5f, Config::TxtColor, OwnerAndRepo, 250);
	Gui::DrawStringCentered(0, 137, 0.5f, Config::TxtColor, fileName, 250);
}

void UniStore::GitHubLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_TOUCH && touching(touch, GitHubPos[0])) {
		OwnerAndRepo = Input::getStringLong(Lang::get("ENTER_OWNER_AND_REPO"));
	}

	if (hDown & KEY_TOUCH && touching(touch, GitHubPos[1])) {
		fileName = Input::getStringLong(Lang::get("ENTER_FILENAME"));
	}

	if (hDown & KEY_TOUCH && touching(touch, GitHubPos[2])) {
		if (checkWifiStatus() == true) {
			std::string URL = "https://github.com/";
			URL += OwnerAndRepo;
			URL += "/raw/master/unistore/";
			URL += fileName;
			ScriptHelper::downloadFile(URL, Config::StorePath + fileName, Lang::get("DOWNLOADING") + fileName);
		} else {
			notConnectedMsg();
		}
	}

	if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		// Reset everything.
		OwnerAndRepo = "";
		fileName = "";
		mode = 3;
	}
}

void UniStore::DrawFullURLScreen(void) const {
	GFX::DrawTop();
	if (Config::UseBars == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, Lang::get("FULL_URL"), 400);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, Lang::get("FULL_URL"), 400);
	}

	GFX::DrawSprite(sprites_uniStore_HD_idx, 140, 50, 0.2, 0.2);
	GFX::DrawBottom();
	GFX::DrawArrow(0, 218, 0, 1);

	Gui::DrawStringCentered(0, 28, 0.7f, Config::TxtColor, Lang::get("FULL_URL"), 320);
	Gui::DrawStringCentered(0, 108, 0.7f, Config::TxtColor, Lang::get("FILENAME"), 320);

	Gui::Draw_Rect(GitHubPos[0].x, GitHubPos[0].y, GitHubPos[0].w, GitHubPos[0].h, Config::Color1);
	Gui::Draw_Rect(GitHubPos[1].x, GitHubPos[1].y, GitHubPos[1].w, GitHubPos[1].h, Config::Color1);
	Gui::Draw_Rect(GitHubPos[2].x, GitHubPos[2].y, GitHubPos[2].w, GitHubPos[2].h, Config::Color1);

	Gui::DrawStringCentered(0, 185, 0.7f, Config::TxtColor, Lang::get("OK"), 40);

	Gui::DrawStringCentered(0, 57, 0.45f, Config::TxtColor, FullURL, 250);
	Gui::DrawStringCentered(0, 137, 0.45f, Config::TxtColor, fileName, 250);
}

void UniStore::FullURLLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_TOUCH && touching(touch, GitHubPos[0])) {
		FullURL = Input::getStringLong(Lang::get("ENTER_FULL_URL"));
	}

	if (hDown & KEY_TOUCH && touching(touch, GitHubPos[1])) {
		fileName = Input::getStringLong(Lang::get("ENTER_FILENAME"));
	}

	if (hDown & KEY_TOUCH && touching(touch, GitHubPos[2])) {
		if (checkWifiStatus() == true) {
			ScriptHelper::downloadFile(FullURL, Config::StorePath + fileName, Lang::get("DOWNLOADING") + fileName);
		} else {
			notConnectedMsg();
		}
	}

	if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, arrowPos[2]))) {
		// Reset everything.
		FullURL = "";
		fileName = "";
		mode = 3;
	}
}