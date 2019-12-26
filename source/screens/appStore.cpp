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

#include "screens/appStore.hpp"

#include "utils/config.hpp"
#include "utils/fileBrowse.h"
#include "utils/formatting.hpp"
#include "utils/json.hpp"
#include "utils/scriptHelper.hpp"

extern bool touching(touchPosition touch, Structs::ButtonPos button);
#define ENTRIES_PER_SCREEN 3
#define ENTRIES_PER_LIST 7

#define AppStoreFile "sdmc:/3ds/Universal-Updater/Appstore.json"

// JSON file for App Store.
nlohmann::json appStoreJson;

// Parse the Objects.
std::vector<std::string> parseObjects2() {
	FILE* file = fopen(AppStoreFile, "rt");
	if(!file) {
		printf("File not found\n");
		return {{""}};
	}
	appStoreJson = nlohmann::json::parse(file, nullptr, false);
	fclose(file);

	std::vector<std::string> objs;
	for(auto it = appStoreJson.begin();it != appStoreJson.end(); it++) {
		if(it.key() != "storeInfo") {
			objs.push_back(it.key());
		}
	}
	return objs;
}

std::vector<std::string> appStoreList;

// Download and Pull the data of the App Store JSON.
AppStore::AppStore() {
	DisplayMsg(Lang::get("PULLING_APPSTORE"));
	downloadToFile("https://raw.githubusercontent.com/SuperSaiyajinStackZ/UniStore/master/stores/example-store.json?raw=true", AppStoreFile);
	appStoreList = parseObjects2();
	selectedOptionAppStore = appStoreList[0];
}


void AppStore::Draw(void) const {
	std::string entryAmount = std::to_string(selection+1) + " / " + std::to_string(appStoreList.size());
	std::string info;
	Gui::DrawTop();
	if (Config::UseBars == true) {
		Gui::sprite(sprites_top_screen_top_idx, 0, 0);
		Gui::sprite(sprites_top_screen_bot_idx, 0, 215);
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, std::string(appStoreJson["storeInfo"]["title"]), 400);
		Gui::DrawString(397-Gui::GetStringWidth(0.6f, entryAmount), 239-Gui::GetStringHeight(0.6f, entryAmount), 0.6f, Config::TxtColor, entryAmount);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, std::string(appStoreJson["storeInfo"]["title"]), 400);
		Gui::DrawString(397-Gui::GetStringWidth(0.6f, entryAmount), 237-Gui::GetStringHeight(0.6f, entryAmount), 0.6f, Config::TxtColor, entryAmount);
	}

	Gui::DrawStringCentered(0, 35, 0.6f, Config::TxtColor, Lang::get("AUTHOR") + std::string(appStoreJson[selectedOptionAppStore]["info"]["author"]), 400);
	Gui::DrawStringCentered(0, 65, 0.6f, Config::TxtColor, Lang::get("DESC") + std::string(appStoreJson[selectedOptionAppStore]["info"]["description"]), 400);
	Gui::DrawStringCentered(0, 95, 0.6f, Config::TxtColor, Lang::get("VERSION") + std::string(appStoreJson[selectedOptionAppStore]["info"]["version"]), 400);
	Gui::DrawStringCentered(0, 125, 0.6f, Config::TxtColor, Lang::get("FILE_SIZE") + formatBytes(int64_t(appStoreJson[selectedOptionAppStore]["info"]["fileSize"])), 400);

	Gui::DrawBottom();
	Gui::DrawArrow(295, -1);
	Gui::DrawArrow(315, 240, 180.0);
	Gui::DrawArrow(0, 218, 0, 1);
	Gui::spriteBlend(sprites_view_idx, arrowPos[3].x, arrowPos[3].y);

	if (Config::viewMode == 0) {
		for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)appStoreList.size();i++) {
			info = appStoreList[screenPos + i];
			if(screenPos + i == selection) {
				Gui::Draw_Rect(0, 40+(i*57), 320, 45, C2D_Color32(120, 192, 216, 255));
			} else { 
				Gui::Draw_Rect(0, 40+(i*57), 320, 45, C2D_Color32(77, 118, 132, 255));
			}
			Gui::DrawStringCentered(0, 50+(i*57), 0.7f, WHITE, info, 320);
		}
	} else if (Config::viewMode == 1) {
		for(int i=0;i<ENTRIES_PER_LIST && i<(int)appStoreList.size();i++) {
			info = appStoreList[screenPosList + i];
			if(screenPosList + i == selection) {
				Gui::Draw_Rect(0, (i+1)*27, 320, 25, Config::SelectedColor);
			} else {
				Gui::Draw_Rect(0, (i+1)*27, 320, 25, Config::UnselectedColor);
			}
			Gui::DrawStringCentered(0, ((i+1)*27)+1, 0.7f, Config::TxtColor, info, 320);
		}
	}
}

void AppStore::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (keyRepeatDelay)	keyRepeatDelay--;
	if (hDown & KEY_B) {
		Screen::back();
		return;
	}

	if (hDown & KEY_R) {
		fastMode = true;
	}

	if (hDown & KEY_L) {
		fastMode = false;
	}

	// Go one entry up.
	if (hDown & KEY_TOUCH && touching(touch, arrowPos[0])) {
		if (selection > 0) {
			selection--;
			selectedOptionAppStore = appStoreList[selection];
		} else {
			selection = (int)appStoreList.size()-1;
			selectedOptionAppStore = appStoreList[selection];
		}
	}

	// Go one entry down.
	if (hDown & KEY_TOUCH && touching(touch, arrowPos[1])) {
		if (selection < (int)appStoreList.size()-1) {
			selection++;
			selectedOptionAppStore = appStoreList[selection];
		} else {
			selection = 0;
			selectedOptionAppStore = appStoreList[selection];
		}
	}

	// Go back.
	if (hDown & KEY_TOUCH && touching(touch, arrowPos[2])) {
		Screen::back();
		return;
	}

	// Go one entry up.
	if (hHeld & KEY_UP && !keyRepeatDelay) {
		if (selection > 0) {
			selection--;
			selectedOptionAppStore = appStoreList[selection];
		} else {
			selection = (int)appStoreList.size()-1;
			selectedOptionAppStore = appStoreList[selection];
		}
		if (fastMode == true) {
			keyRepeatDelay = 3;
		} else if (fastMode == false){
			keyRepeatDelay = 6;
		}
	}

	// Go one entry down.
	if (hHeld & KEY_DOWN && !keyRepeatDelay) {
		if (selection < (int)appStoreList.size()-1) {
			selection++;
			selectedOptionAppStore = appStoreList[selection];
		} else {
			selection = 0;
			selectedOptionAppStore = appStoreList[selection];
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
			for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)appStoreList.size();i++) {
				if(touch.py > 40+(i*57) && touch.py < 40+(i*57)+45) {
					selection = screenPos + i;
					selectedOptionAppStore = appStoreList[screenPos + i];
					execute();
				}
			}
		} else if (Config::viewMode == 1) {
			for(int i=0;i<ENTRIES_PER_LIST && i<(int)appStoreList.size();i++) {
				if(touch.py > (i+1)*27 && touch.py < (i+2)*27) {
					selection = screenPosList + i;
					selectedOptionAppStore = appStoreList[screenPosList + i];
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

	// Switch ViewMode.
	if (hDown & KEY_X || hDown & KEY_TOUCH && touching(touch, arrowPos[3])) {
		if (Config::viewMode == 0) {
			Config::viewMode = 1;
		} else {
			Config::viewMode = 0;
		}
	}
}

// Execute Entry.
void AppStore::execute() {
	for(int i=0;i<(int)appStoreJson.at(selectedOptionAppStore).at("script").size();i++) {
		std::string type = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("type");
		if(type == "deleteFile") {
			bool missing = false;
			std::string file, message;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("file"))	file = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("file");
			else	missing = true;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("message"))	message = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("message");
			if(!missing)	ScriptHelper::removeFile(file, message);

		} else if(type == "downloadFile") {
			bool missing = false;
			std::string file, output, message;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("file"))	file = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("file");
			else	missing = true;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("output"))	output = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("output");
			else	missing = true;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("message"))	message = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("message");
			if(!missing)	ScriptHelper::downloadFile(file, output, message);

		} else if(type == "installCia") {
			bool missing = false;
			std::string file, message;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("file"))	file = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("file");
			else	missing = true;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("message"))	message = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("message");
			if(!missing)	ScriptHelper::installFile(file, message);
		}
	}
	doneMsg();
}