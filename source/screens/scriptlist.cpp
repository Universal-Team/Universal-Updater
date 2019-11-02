/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019 VoltZ, Epicpkmn11, Flame, RocketRobz, TotallyNotGuy
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

#include "screens/mainMenu.hpp"
#include "screens/scriptlist.hpp"

#include "utils/config.hpp"
#include "utils/parse.hpp"

#include <algorithm>
#include <fstream>
#include <unistd.h>

#define ENTRIES_PER_SCREEN 3

// Information like Title and Description.
struct Info {
	std::string title;
	std::string description;
};
std::string choice;
std::string currentFile;

Info parseInfo(std::string fileName) {
	FILE* file = fopen(fileName.c_str(), "rt");
	if(!file) {
		printf("File not found\n");
		fclose(file);
		return {"", ""};
	}
	nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
	fclose(file);

	Info info;
	info.title = get(json, "info", "title");
	info.description = get(json, "info", "description");
	return info;
}

void checkForValidate(void) {
	FILE* file = fopen(currentFile.c_str(), "rt");
	nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
	fclose(file);

	std::string version;
	version = get(json, "info", "version");
	if (version != "1") {
		Gui::DisplayWarnMsg(Lang::get("INCOMPATIBLE_SCRIPT"));
	}
}


// Objects like Release or Nightly.
std::vector<std::string> parseObjects(std::string fileName) {
	FILE* file = fopen(fileName.c_str(), "rt");
	if(!file) {
		printf("File not found\n");
		fclose(file);
		return {{""}};
	}
	nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
	fclose(file);

	std::vector<std::string> objs;
	for(auto it = json.begin();it != json.end(); it++) {
		if(it.key() != "info") {
			objs.push_back(it.key());
		}
	}
	return objs;
}

void runFunctions(void) {
	FILE* file = fopen(currentFile.c_str(), "rt");
	nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
	fclose(file);
	for(int i=0;i<(int)json.at(choice).size();i++) {
		std::string type = json.at(choice).at(i).at("type");

		if(type == "deleteFile") {
			bool missing = false;
			std::string file, message;
			if(json.at(choice).at(i).contains("file"))	file = json.at(choice).at(i).at("file");
			else    missing = true;
			if(json.at(choice).at(i).contains("message"))	message = json.at(choice).at(i).at("message");
			if(!missing)	download::deleteFileList(file, message);

		} else if(type == "downloadFile") {
			bool missing = false;
			std::string file, output, message;
			if(json.at(choice).at(i).contains("file"))	file = json.at(choice).at(i).at("file");
			else	missing = true;
			if(json.at(choice).at(i).contains("output"))	output = json.at(choice).at(i).at("output");
			else	missing = true;
			if(json.at(choice).at(i).contains("message"))	message = json.at(choice).at(i).at("message");
			if(!missing)	download::downloadFile(file, output, message);
		
		} else if(type == "downloadRelease") {
			bool missing = false;
			std::string repo, file, output, message;
			if(json.at(choice).at(i).contains("repo"))	repo = json.at(choice).at(i).at("repo");
			else	missing = true;
			if(json.at(choice).at(i).contains("file"))	file = json.at(choice).at(i).at("file");
			else	missing = true;
			if(json.at(choice).at(i).contains("output"))	output = json.at(choice).at(i).at("output");
			else	missing = true;
			if(json.at(choice).at(i).contains("message"))	message = json.at(choice).at(i).at("message");
			if(!missing)	download::downloadRelease(repo, file, output, message);
			
		} else if(type == "extractFile") {
			bool missing = false;
			std::string file, input, output, message;
			if(json.at(choice).at(i).contains("file"))	file = json.at(choice).at(i).at("file");
			else	missing = true;
			if(json.at(choice).at(i).contains("input"))	file = json.at(choice).at(i).at("input");
			else	missing = true;
			if(json.at(choice).at(i).contains("output"))	output = json.at(choice).at(i).at("output");
			else	missing = true;
			if(json.at(choice).at(i).contains("message"))	message = json.at(choice).at(i).at("message");
			if(!missing)	download::extractFileList(file, input, output, message);

		} else if(type == "installCia") {
			bool missing = false;
			std::string file, message;
			if(json.at(choice).at(i).contains("file"))	file = json.at(choice).at(i).at("file");
			else    missing = true;
			if(json.at(choice).at(i).contains("message"))	message = json.at(choice).at(i).at("message");
			if(!missing)	download::installFileList(file, message);
		}
	}
	doneMsg();
}


std::vector<Info> fileInfo;
std::vector<std::string> fileInfo2;

ScriptList::ScriptList() {
	dirContents.clear();
	chdir(SCRIPTS_PATH);
	getDirectoryContents(dirContents);
	for(uint i=0;i<dirContents.size();i++) {
		fileInfo.push_back(parseInfo(dirContents[i].name));
	}
}

void ScriptList::DrawList(void) const {
	std::string line1;
	std::string line2;
	Gui::DrawTop();
	Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, "Universal-Updater", 400);
	Gui::DrawBottom();
	for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)fileInfo.size();i++) {
		line1 = fileInfo[screenPos + i].title;
		line2 = fileInfo[screenPos + i].description;
		if(screenPos + i == selection) {
			Gui::Draw_Rect(0, 40+(i*57), 320, 45, Config::SelectedColor);
		} else { 
			Gui::Draw_Rect(0, 40+(i*57), 320, 45, Config::UnselectedColor);
		}
		Gui::DrawStringCentered(0, 38+(i*57), 0.7f, Config::TxtColor, line1, 320);
		Gui::DrawStringCentered(0, 62+(i*57), 0.7f, Config::TxtColor, line2, 320);
	}
}


void ScriptList::Draw(void) const {
	if (mode == 0){
		DrawList();
	} else if (mode == 1) {
		DrawSingleObject();
	}
}

void ScriptList::DrawSingleObject(void) const {
	std::string info;
	Gui::DrawTop();
	Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, "Universal-Updater", 400);
	Gui::DrawBottom();
	for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)fileInfo2.size();i++) {
		info = fileInfo2[screenPos2 + i];
		if(screenPos2 + i == selection2) {
			Gui::Draw_Rect(0, 40+(i*57), 320, 45, Config::SelectedColor);
		} else { 
			Gui::Draw_Rect(0, 40+(i*57), 320, 45, Config::UnselectedColor);
		}
		Gui::DrawStringCentered(0, 50+(i*57), 0.7f, Config::TxtColor, info, 320);
	}
}

void ScriptList::ListSelection(u32 hDown, u32 hHeld) {
	if (keyRepeatDelay)	keyRepeatDelay--;

	if (hDown & KEY_B) {
		fileInfo.clear();
		Gui::screenBack();
		return;
	}

	if (hHeld & KEY_DOWN && !keyRepeatDelay) {
		if (selection < (int)fileInfo.size()-1) {
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
			selection = (int)fileInfo.size()-1;
		}
		if (fastMode == true) {
			keyRepeatDelay = 3;
		} else if (fastMode == false){
			keyRepeatDelay = 6;
		}
	}

	if (hDown & KEY_A) {
		if (fileInfo.size() == 0) {
			Gui::DisplayWarnMsg(Lang::get("WHAT_DO_YOU_TRY"));
		} else {
			currentFile = dirContents[selection].name;
			checkForValidate();
			fileInfo2 = parseObjects(currentFile);
			selection = 0;
			mode = 1;
		}
	}

	if(selection < screenPos) {
		screenPos = selection;
	} else if (selection > screenPos + ENTRIES_PER_SCREEN - 1) {
		screenPos = selection - ENTRIES_PER_SCREEN + 1;
	}
}

void ScriptList::SelectFunction(u32 hDown, u32 hHeld) {
	if (keyRepeatDelay)	keyRepeatDelay--;
	if (hHeld & KEY_DOWN && !keyRepeatDelay) {
		if (selection2 < (int)fileInfo2.size()-1) {
			selection2++;
		} else {
			selection2 = 0;
		}
		if (fastMode == true) {
			keyRepeatDelay = 3;
		} else if (fastMode == false){
			keyRepeatDelay = 6;
		}
	}
	if (hHeld & KEY_UP && !keyRepeatDelay) {
		if (selection2 > 0) {
			selection2--;
		} else {
			selection2 = (int)fileInfo2.size()-1;
		}
		if (fastMode == true) {
			keyRepeatDelay = 3;
		} else if (fastMode == false){
			keyRepeatDelay = 6;
		}
	}
	if (hDown & KEY_A) {
		if (fileInfo2.size() == 0) {
			Gui::DisplayWarnMsg(Lang::get("WHAT_DO_YOU_TRY"));
		} else {
			choice = fileInfo2[selection2];
			runFunctions();
		}
	}

	if (hDown & KEY_B) {
		selection2 = 0;
		fileInfo2.clear();
		mode = 0;
	}

	if(selection2 < screenPos2) {
		screenPos2 = selection2;
	} else if (selection2 > screenPos2 + ENTRIES_PER_SCREEN - 1) {
		screenPos2 = selection2 - ENTRIES_PER_SCREEN + 1;
	}
}


void ScriptList::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (mode == 0) {
		ListSelection(hDown, hHeld);
	} else if (mode == 1) {
		SelectFunction(hDown, hHeld);
	}
}