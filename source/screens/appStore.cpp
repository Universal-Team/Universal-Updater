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

#include "keyboard.hpp"

#include "download/download.hpp"

#include "screens/appStore.hpp"

#include "utils/config.hpp"
#include "utils/fileBrowse.h"
#include "utils/formatting.hpp"
#include "utils/json.hpp"
#include "utils/scriptHelper.hpp"

#include <unistd.h>

extern bool touching(touchPosition touch, Structs::ButtonPos button);
nlohmann::json appStoreJson;
std::string currentStoreFile;
extern bool isScriptSelected;

struct storeInfo2 {
	std::string title;
	std::string author;
	std::string description;
	std::string url;
};

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
	return info;
}

nlohmann::json openStoreFile() {
	FILE* file = fopen(currentStoreFile.c_str(), "rt");
	nlohmann::json jsonFile;
	if(file)	jsonFile = nlohmann::json::parse(file, nullptr, false);
	fclose(file);
	return jsonFile;
}

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

std::vector<storeInfo2> storeInfo; // Store selection.
std::vector<std::string> appStoreList; // Actual store. ;P
std::vector<std::string> descLines;
std::string storeDesc = "";

void loadStoreDesc(void) {
	descLines.clear();
	while(storeDesc.find('\n') != storeDesc.npos) {
		descLines.push_back(storeDesc.substr(0, storeDesc.find('\n')));
		storeDesc = storeDesc.substr(storeDesc.find('\n')+1);
	}
	descLines.push_back(storeDesc.substr(0, storeDesc.find('\n')));
}

AppStore::AppStore() {
	dirContents.clear();
	chdir("sdmc:/3ds/Universal-Updater/stores/");
	getDirectoryContents(dirContents, {"unistore"});
	for(uint i=0;i<dirContents.size();i++) {
		storeInfo.push_back(parseStoreInfo(dirContents[i].name));
	}
}

// First Screen -> Storelist.
void AppStore::DrawStoreList(void) const {
	std::string line1;
	std::string line2;
	std::string storeAmount = std::to_string(selection +1) + " / " + std::to_string(storeInfo.size());
	Gui::DrawTop();
	if (Config::UseBars == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, "Universal-Updater", 400);
		Gui::DrawString(397-Gui::GetStringWidth(0.6f, storeAmount), 239-Gui::GetStringHeight(0.6f, storeAmount), 0.6f, Config::TxtColor, storeAmount);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, "Universal-Updater", 400);
		Gui::DrawString(397-Gui::GetStringWidth(0.6f, storeAmount), 237-Gui::GetStringHeight(0.6f, storeAmount), 0.6f, Config::TxtColor, storeAmount);
	}
	Gui::DrawStringCentered(0, 80, 0.7f, Config::TxtColor, Lang::get("TITLE") + std::string(storeInfo[selection].title), 400);
	Gui::DrawStringCentered(0, 100, 0.7f, Config::TxtColor, Lang::get("AUTHOR") + std::string(storeInfo[selection].author), 400);
	Gui::DrawStringCentered(0, 120, 0.6f, Config::TxtColor, std::string(storeInfo[selection].description), 400);

	Gui::DrawBottom();
	Gui::DrawArrow(295, -1);
	Gui::DrawArrow(315, 240, 180.0);
	Gui::DrawArrow(0, 218, 0, 1);
	Gui::spriteBlend(sprites_view_idx, arrowPos[3].x, arrowPos[3].y);

	if (Config::viewMode == 0) {
		for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)storeInfo.size();i++) {
			line1 = storeInfo[screenPos + i].title;
			line2 = storeInfo[screenPos + i].author;
			if(screenPos + i == selection) {
				Gui::Draw_Rect(0, 40+(i*57), 320, 45, Config::SelectedColor);
			} else { 
				Gui::Draw_Rect(0, 40+(i*57), 320, 45, Config::UnselectedColor);
			}
			Gui::DrawStringCentered(0, 38+(i*57), 0.7f, Config::TxtColor, line1, 320);
			Gui::DrawStringCentered(0, 62+(i*57), 0.7f, Config::TxtColor, line2, 320);
		}
	} else if (Config::viewMode == 1) {
		for(int i=0;i<ENTRIES_PER_LIST && i<(int)storeInfo.size();i++) {
			line1 = storeInfo[screenPosList + i].title;
			if(screenPosList + i == selection) {
				Gui::Draw_Rect(0, (i+1)*27, 320, 25, Config::SelectedColor);
			} else { 
				Gui::Draw_Rect(0, (i+1)*27, 320, 25, Config::UnselectedColor);
			}
			Gui::DrawStringCentered(0, ((i+1)*27)+1, 0.7f, Config::TxtColor, line1, 320);
		}
	}
}

void AppStore::DrawStore(void) const {
	std::string entryAmount = std::to_string(selection2+1) + " / " + std::to_string(appStoreList.size());
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
			info = appStoreList[screenPos2 + i];
			if(screenPos2 + i == selection2) {
				Gui::Draw_Rect(0, 40+(i*57), 320, 45, C2D_Color32(120, 192, 216, 255));
			} else { 
				Gui::Draw_Rect(0, 40+(i*57), 320, 45, C2D_Color32(77, 118, 132, 255));
			}
			Gui::DrawStringCentered(0, 50+(i*57), 0.7f, WHITE, info, 320);
		}
	} else if (Config::viewMode == 1) {
		for(int i=0;i<ENTRIES_PER_LIST && i<(int)appStoreList.size();i++) {
			info = appStoreList[screenPosList2 + i];
			if(screenPosList2 + i == selection2) {
				Gui::Draw_Rect(0, (i+1)*27, 320, 25, Config::SelectedColor);
			} else {
				Gui::Draw_Rect(0, (i+1)*27, 320, 25, Config::UnselectedColor);
			}
			Gui::DrawStringCentered(0, ((i+1)*27)+1, 0.7f, Config::TxtColor, info, 320);
		}
	}
}

void AppStore::Draw(void) const {
	if (mode == 0) {
		DrawStoreList();
	} else if (mode == 1) {
		DrawStore();
	}
}

void AppStore::StoreSelectionLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (keyRepeatDelay)	keyRepeatDelay--;

	if (hDown & KEY_B) {
		storeInfo.clear();
		Screen::back();
		return;
	}

	if (hDown & KEY_TOUCH && touching(touch, arrowPos[0])) {
		if (selection > 0) {
			selection--;
		} else {
			selection = (int)storeInfo.size()-1;
		}
	}

	if (hDown & KEY_TOUCH && touching(touch, arrowPos[1])) {
		if (selection < (int)storeInfo.size()-1) {
			selection++;
		} else {
			selection = 0;
		}
	}

	if (hDown & KEY_TOUCH && touching(touch, arrowPos[2])) {
		storeInfo.clear();
		Screen::back();
		return;
	}

	if (hHeld & KEY_DOWN && !keyRepeatDelay) {
		if (selection < (int)storeInfo.size()-1) {
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
			selection = (int)storeInfo.size()-1;
		}
		if (fastMode == true) {
			keyRepeatDelay = 3;
		} else if (fastMode == false){
			keyRepeatDelay = 6;
		}
	}

	if (hDown & KEY_A) {
		if (dirContents[selection].isDirectory) {
		} else if (storeInfo.size() != 0) {
			if (ScriptHelper::checkIfValid(dirContents[selection].name, 1) == true) {
				currentStoreFile = dirContents[selection].name;
				appStoreJson = openStoreFile();
				appStoreList = parseStoreObjects(currentStoreFile);
				selectedOptionAppStore = appStoreList[0];
				selection = 0;
				mode = 1;
				storeInfo.clear();
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
}

void AppStore::StoreLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (keyRepeatDelay)	keyRepeatDelay--;
	if (hDown & KEY_B) {
		mode = 0;
		appStoreList.clear();
	}

	if (hDown & KEY_R) {
		fastMode = true;
	}

	if (hDown & KEY_L) {
		fastMode = false;
	}

	// Go one entry up.
	if (hDown & KEY_TOUCH && touching(touch, arrowPos[0])) {
		if (selection2 > 0) {
			selection2--;
			selectedOptionAppStore = appStoreList[selection2];
		} else {
			selection2 = (int)appStoreList.size()-1;
			selectedOptionAppStore = appStoreList[selection2];
		}
	}

	// Go one entry down.
	if (hDown & KEY_TOUCH && touching(touch, arrowPos[1])) {
		if (selection2 < (int)appStoreList.size()-1) {
			selection2++;
			selectedOptionAppStore = appStoreList[selection2];
		} else {
			selection2 = 0;
			selectedOptionAppStore = appStoreList[selection2];
		}
	}

	// Go back.
	if (hDown & KEY_TOUCH && touching(touch, arrowPos[2])) {
		mode = 0;
		appStoreList.clear();
	}

	// Go one entry up.
	if (hHeld & KEY_UP && !keyRepeatDelay) {
		if (selection2 > 0) {
			selection2--;
			selectedOptionAppStore = appStoreList[selection2];
		} else {
			selection2 = (int)appStoreList.size()-1;
			selectedOptionAppStore = appStoreList[selection2];
		}
		if (fastMode == true) {
			keyRepeatDelay = 3;
		} else if (fastMode == false){
			keyRepeatDelay = 6;
		}
	}

	// Go one entry down.
	if (hHeld & KEY_DOWN && !keyRepeatDelay) {
		if (selection2 < (int)appStoreList.size()-1) {
			selection2++;
			selectedOptionAppStore = appStoreList[selection2];
		} else {
			selection2 = 0;
			selectedOptionAppStore = appStoreList[selection2];
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
					selection2 = screenPos + i;
					selectedOptionAppStore = appStoreList[screenPos + i];
					execute();
				}
			}
		} else if (Config::viewMode == 1) {
			for(int i=0;i<ENTRIES_PER_LIST && i<(int)appStoreList.size();i++) {
				if(touch.py > (i+1)*27 && touch.py < (i+2)*27) {
					selection2 = screenPosList + i;
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
		if(selection2 < screenPos2) {
			screenPos2 = selection2;
		} else if (selection2 > screenPos2 + ENTRIES_PER_SCREEN - 1) {
			screenPos2 = selection2 - ENTRIES_PER_SCREEN + 1;
		}
	} else if (Config::viewMode == 1) {
		if(selection2 < screenPosList2) {
			screenPosList2 = selection2;
		} else if (selection2 > screenPosList2 + ENTRIES_PER_LIST - 1) {
			screenPosList2 = selection2 - ENTRIES_PER_LIST + 1;
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

void AppStore::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (mode == 0) {
		StoreSelectionLogic(hDown, hHeld, touch);
	} else if (mode == 1) {
		StoreLogic(hDown, hHeld, touch);
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

		} else if(type == "downloadRelease") {
			bool missing = false, includePrereleases = false;
			std::string repo, file, output, message;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("repo"))	repo = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("repo");
			else	missing = true;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("file"))	file = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("file");
			else	missing = true;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("output"))	output = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("output");
			else	missing = true;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("includePrereleases") && appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("includePrereleases").is_boolean())
				includePrereleases = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("includePrereleases");
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("message"))	message = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("message");
			if(!missing)	ScriptHelper::downloadRelease(repo, file, output, includePrereleases, message);

		} else if(type == "extractFile") {
			bool missing = false;
			std::string file, input, output, message;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("file"))	file = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("file");
			else	missing = true;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("input"))	input = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("input");
			else	missing = true;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("output"))	output = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("output");
			else	missing = true;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("message"))	message = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("message");
			if(!missing)	ScriptHelper::extractFile(file, input, output, message);

		} else if(type == "installCia") {
			bool missing = false;
			std::string file, message;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("file"))	file = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("file");
			else	missing = true;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("message"))	message = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("message");
			if(!missing)	ScriptHelper::installFile(file, message);
	
		} else if (type == "mkdir") {
			bool missing = false;
			std::string directory, message;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("directory"))	directory = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("directory");
			else	missing = true;
			if(!missing)	makeDirs(directory.c_str());

		} else if (type == "rmdir") {
			bool missing = false;
			std::string directory, message, promptmsg;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("directory"))	directory = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("directory");
			else	missing = true;
			promptmsg = Lang::get("DELETE_PROMPT") + "\n" + directory;
			if(!missing) {
				if (Gui::promptMsg(promptmsg)) {
					removeDirRecursive(directory.c_str());
				}
			}

		} else if (type == "mkfile") {
			bool missing = false;
			std::string file;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("file"))	file = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("file");
			else	missing = true;
			if(!missing)	ScriptHelper::createFile(file.c_str());

		} else if (type == "timeMsg") {
			bool missing = false;
			std::string message;
			int seconds;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("message"))	message = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("message");
			else	missing = true;
			if(appStoreJson.at(selectedOptionAppStore).at("script").at(i).contains("seconds") && appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("seconds").is_number())
			seconds = appStoreJson.at(selectedOptionAppStore).at("script").at(i).at("seconds");
			else	missing = true;
			if(!missing)	ScriptHelper::displayTimeMsg(message, seconds);
		} else if (type == "saveConfig") {
			Config::save();
		}
	}
	doneMsg();
}