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
#include "extract.hpp"
#include "gui.hpp"
#include "msg.hpp"
#include "scriptHelper.hpp"
#include "thread.hpp"

#include <fstream>

extern "C" {
    #include "cia.h"
}

extern bool showProgressBar;
extern bool progressBarType;
extern char progressBarMsg[128];
extern int filesExtracted;

extern void downloadFailed();

// Get String of the Script.
std::string ScriptHelper::getString(nlohmann::json json, const std::string &key, const std::string &key2) {
	if(!json.contains(key))	return "MISSING: " + key;
	if(!json.at(key).is_object())	return "NOT OBJECT: " + key;

	if(!json.at(key).contains(key2))	return "MISSING: " + key + "." + key2;
	if(!json.at(key).at(key2).is_string())	return "NOT STRING: " + key + "." + key2;

	return json.at(key).at(key2).get_ref<const std::string&>();
}

// Get int of the Script.
int ScriptHelper::getNum(nlohmann::json json, const std::string &key, const std::string &key2) {
	if(!json.contains(key))	return 0;
	if(!json.at(key).is_object())	return 0;

	if(!json.at(key).contains(key2))	return 0;
	if(!json.at(key).at(key2).is_number())	return 0;
	return json.at(key).at(key2).get_ref<const int64_t&>();
}

// Download from a Github Release.
void ScriptHelper::downloadRelease(std::string repo, std::string file, std::string output, bool includePrereleases, std::string message) {
	snprintf(progressBarMsg, sizeof(progressBarMsg), message.c_str());
	showProgressBar = true;
	progressBarType = 0;
	Threads::create((ThreadFunc)displayProgressBar);
	if (downloadFromRelease("https://github.com/" + repo, file, output, includePrereleases) != 0) {
		showProgressBar = false;
		downloadFailed();
		return;
	}
	showProgressBar = false;
}

// Download a File from everywhere.
void ScriptHelper::downloadFile(std::string file, std::string output, std::string message) {
	snprintf(progressBarMsg, sizeof(progressBarMsg), message.c_str());
	showProgressBar = true;
	progressBarType = 0;
	Threads::create((ThreadFunc)displayProgressBar);
	if (downloadToFile(file, output) != 0) {
		showProgressBar = false;
		downloadFailed();
		return;
	}
	showProgressBar = false;
}

// Remove a File.
void ScriptHelper::removeFile(std::string file, std::string message) {
	Msg::DisplayMsg(message);
	deleteFile(file.c_str());
}

// Install a file.
void ScriptHelper::installFile(std::string file, std::string message) {
	Msg::DisplayMsg(message);
	installCia(file.c_str());
}

// Extract Files.
void ScriptHelper::extractFile(std::string file, std::string input, std::string output, std::string message) {
	snprintf(progressBarMsg, sizeof(progressBarMsg), message.c_str());
	showProgressBar = true;
	filesExtracted = 0;
	progressBarType = 1;
	Threads::create((ThreadFunc)displayProgressBar);
	extractArchive(file, input, output);
	showProgressBar = false;
}

// Create an empty file.
Result ScriptHelper::createFile(const char * path) {
	std::ofstream ofstream;
	ofstream.open(path, std::ofstream::out | std::ofstream::app);
	ofstream.close();
	return 0;
}

// Display a Message for a specific amount of time. 
void ScriptHelper::displayTimeMsg(std::string message, int seconds) {
	Msg::DisplayMsg(message);
	for (int i = 0; i < 60*seconds; i++) {
		gspWaitForVBlank();
	}
}

bool ScriptHelper::checkIfValid(std::string scriptFile, int mode) {
	FILE* file = fopen(scriptFile.c_str(), "rt");
	if(!file) {
		printf("File not found\n");
		return false;
	}
	nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
	fclose(file);

	if (mode == 0) {
		if (!json.contains("info"))	return false;
	} else if (mode == 1) {
		if (!json.contains("storeInfo"))	return false;
	}
	
	return true;
}

void ScriptHelper::deleteTitle(const std::string TitleID, bool isNAND, std::string message) {
	if (Config::getBool("GODMODE")) {
		std::string MSG = Lang::get("DELETE_TITLE") + "\n\n";
		if (isNAND)	MSG += Lang::get("MEDIATYPE_NAND") + "\n" + TitleID;
		else		MSG += Lang::get("MEDIATYPE_SD") + "\n" + TitleID;
		u64 ID = std::stoull(TitleID, 0, 16);
		if (Msg::promptMsg(MSG)) {
			if (isNAND == true) {
				Msg::DisplayMsg(message);
				deletePrevious(ID, MEDIATYPE_NAND);
			} else {
				Msg::DisplayMsg(message);
				deletePrevious(ID, MEDIATYPE_SD);
			}
		}
	} else {
		Msg::DisplayWarnMsg(Lang::get("FUNCTION_NOT_ALLOWED"));
	}
}

void ScriptHelper::bootTitle(const std::string TitleID, bool isNAND, std::string message) {
	std::string MSG = Lang::get("BOOT_TITLE") + "\n\n";
	if (isNAND)	MSG += Lang::get("MEDIATYPE_NAND") + "\n" + TitleID;
	else		MSG += Lang::get("MEDIATYPE_SD") + "\n" + TitleID;
	u64 ID = std::stoull(TitleID, 0, 16);
	if (Msg::promptMsg(MSG)) {
		if (isNAND == true) {
			Msg::DisplayMsg(message);
			CIA_LaunchTitle(ID, MEDIATYPE_NAND);
		} else {
			Msg::DisplayMsg(message);
			CIA_LaunchTitle(ID, MEDIATYPE_SD);
		}
	}
}