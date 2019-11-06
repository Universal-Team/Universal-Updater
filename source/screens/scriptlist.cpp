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

#include <algorithm>
#include <fstream>
#include <regex>
#include <unistd.h>

#define ENTRIES_PER_SCREEN 3

bool isScriptSelected = false;

// Information like Title and Author.
struct Info {
	std::string title;
	std::string author;
};

std::string choice;
std::string currentFile;
std::string selectedTitle;
std::string Desc = "";
nlohmann::json jsonFile;

std::string get(nlohmann::json json, const std::string &key, const std::string &key2) {
	if(!json.contains(key))	return "MISSING: " + key;
	if(!json.at(key).is_object())	return "NOT OBJECT: " + key;

	if(!json.at(key).contains(key2))	return "MISSING: " + key + "." + key2;
	if(!json.at(key).at(key2).is_string())	return "NOT STRING: " + key + "." + key2;

	return json.at(key).at(key2).get_ref<const std::string&>();
}

int getNum(nlohmann::json json, const std::string &key, const std::string &key2) {
	if(!json.contains(key))	return 0;
	if(!json.at(key).is_object())	return 0;

	if(!json.at(key).contains(key2))	return 0;
	if(!json.at(key).at(key2).is_number())	return 0;
	return json.at(key).at(key2).get_ref<const int64_t&>();
}

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
	info.author = get(json, "info", "author");
	return info;
}

void checkForValidate(void) {
	FILE* file = fopen(currentFile.c_str(), "rt");
	nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
	fclose(file);

	std::string version;
	version = get(json, "info", "version");
	int ver = getNum(json, "info", "version");
	if (ver < SCRIPT_VERSION || ver > SCRIPT_VERSION) {
		Gui::DisplayWarnMsg(Lang::get("INCOMPATIBLE_SCRIPT"));
	}
}

nlohmann::json openScriptFile() {
	FILE* file = fopen(currentFile.c_str(), "rt");
	nlohmann::json jsonFile;
	if(file)    jsonFile = nlohmann::json::parse(file, nullptr, false);
	fclose(file);
	return jsonFile;
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

std::string Description(nlohmann::json &json) {
	std::string out = "";
	if (json.at("info").contains("description")) out = json.at("info").at("description");
	else out = "";
	return out;
}

// Because we need `#include <fstream>`.
Result createFile(const char * path) {
	std::ofstream ofstream;
	ofstream.open(path, std::ofstream::out | std::ofstream::app);
	ofstream.close();
	return 0;
}

void runFunctions(nlohmann::json &json) {
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
			bool missing = false, includePrereleases = false;
			std::string repo, file, output, message;
			if(json.at(choice).at(i).contains("repo"))	repo = json.at(choice).at(i).at("repo");
			else	missing = true;
			if(json.at(choice).at(i).contains("file"))	file = json.at(choice).at(i).at("file");
			else	missing = true;
			if(json.at(choice).at(i).contains("output"))	output = json.at(choice).at(i).at("output");
			else	missing = true;
			if(json.at(choice).at(i).contains("includePrereleases") && json.at(choice).at(i).at("includePrereleases").is_boolean())
				includePrereleases = json.at(choice).at(i).at("includePrereleases");
			if(json.at(choice).at(i).contains("message"))	message = json.at(choice).at(i).at("message");
			if(!missing)	download::downloadRelease(repo, file, output, includePrereleases, message);
			
		} else if(type == "extractFile") {
			bool missing = false;
			std::string file, input, output, message;
			if(json.at(choice).at(i).contains("file"))	file = json.at(choice).at(i).at("file");
			else	missing = true;
			if(json.at(choice).at(i).contains("input"))	input = json.at(choice).at(i).at("input");
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

		} else if (type == "mkdir") {
			bool missing = false;
			std::string directory, message;
			if(json.at(choice).at(i).contains("directory"))	directory = json.at(choice).at(i).at("directory");
			else	missing = true;
			if(!missing)	makeDirs(directory.c_str());

		} else if (type == "rmdir") {
			bool missing = false;
			std::string directory, message, promptmsg;
			if(json.at(choice).at(i).contains("directory"))	directory = json.at(choice).at(i).at("directory");
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
			if(json.at(choice).at(i).contains("file"))	file = json.at(choice).at(i).at("file");
			else	missing = true;
			if(!missing)	createFile(file.c_str());
		}
	}
	doneMsg();
}


std::vector<Info> fileInfo;
std::vector<std::string> fileInfo2;
std::vector<std::string> lines;


u32 getColor(std::string colorString) {
	if(colorString.length() < 7 || std::regex_search(colorString.substr(1), std::regex("[^0-9a-f]"))) { // invalid color
		return 0;
	}

	int r = std::stoi(colorString.substr(1, 2), nullptr, 16);
	int g = std::stoi(colorString.substr(3, 2), nullptr, 16);
	int b = std::stoi(colorString.substr(5, 2), nullptr, 16);
	return RGBA8(r, g, b, 0xFF);
}

// Color listing!
u32 barColor;
u32 bgTopColor;
u32 bgBottomColor;
u32 TextColor;
u32 selected;
u32 unselected;

void loadColors(nlohmann::json &json) {
	u32 colorTemp;
	colorTemp = getColor(get(json, "info", "barColor"));
	barColor = colorTemp == 0 ? Config::Color1 : colorTemp;

	colorTemp = getColor(get(json, "info", "bgTopColor"));
	bgTopColor = colorTemp == 0 ? Config::Color2 : colorTemp;

	colorTemp = getColor(get(json, "info", "bgBottomColor"));
	bgBottomColor = colorTemp == 0 ? Config::Color3 : colorTemp;

	colorTemp = getColor(get(json, "info", "textColor"));
	TextColor = colorTemp == 0 ? Config::TxtColor : colorTemp;

	colorTemp = getColor(get(json, "info", "selectedColor"));
	selected = colorTemp == 0 ? Config::SelectedColor : colorTemp;

	colorTemp = getColor(get(json, "info", "unselectedColor"));
	unselected = colorTemp == 0 ? Config::UnselectedColor : colorTemp;
}

ScriptList::ScriptList() {
	dirContents.clear();
	chdir(Config::ScriptPath.c_str());
	getDirectoryContents(dirContents, {"json"});
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
		line2 = fileInfo[screenPos + i].author;
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

void loadDesc(void) {
	lines.clear();
	while(Desc.find('\n') != Desc.npos) {
		lines.push_back(Desc.substr(0, Desc.find('\n')));
		Desc = Desc.substr(Desc.find('\n')+1);
	}
	lines.push_back(Desc.substr(0, Desc.find('\n')));
}

void ScriptList::DrawSingleObject(void) const {
	std::string info;
	Gui::DrawTop();
	Gui::DrawStringCentered(0, 2, 0.8f, TextColor, selectedTitle, 400);
	for(uint i=0;i<lines.size();i++) {
		Gui::DrawStringCentered(0, 120-((lines.size()*20)/2)+i*20, 0.6f, TextColor, lines[i], 400);
	}
	Gui::DrawBottom();
	for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)fileInfo2.size();i++) {
		info = fileInfo2[screenPos2 + i];
		if(screenPos2 + i == selection2) {
			Gui::Draw_Rect(0, 40+(i*57), 320, 45, selected);
		} else { 
			Gui::Draw_Rect(0, 40+(i*57), 320, 45, unselected);
		}
		Gui::DrawStringCentered(0, 50+(i*57), 0.7f, TextColor, info, 320);
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
		if (dirContents[selection].isDirectory) {
		} else if (fileInfo.size() != 0) {
			currentFile = dirContents[selection].name;
			selectedTitle = fileInfo[selection].title;
			jsonFile = openScriptFile();
			Desc = Description(jsonFile);
			checkForValidate();
			fileInfo2 = parseObjects(currentFile);
			loadColors(jsonFile);
			loadDesc();
			isScriptSelected = true;
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
		if (fileInfo2.size() != 0) {
			choice = fileInfo2[selection2];
			runFunctions(jsonFile);
		}
	}

	if (hDown & KEY_SELECT) {
		Config::Color1 = barColor;
		Config::Color2 = bgTopColor;
		Config::Color3 = bgBottomColor;
		Config::TxtColor = TextColor;
		Config::SelectedColor = selected;
		Config::UnselectedColor = unselected;
		Config::saveConfig();
	}

	if (hDown & KEY_B) {
		selection2 = 0;
		fileInfo2.clear();
		isScriptSelected = false;
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