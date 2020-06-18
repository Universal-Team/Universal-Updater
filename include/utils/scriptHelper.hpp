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

#ifndef SCRIPTHELPER_HPP
#define SCRIPTHELPER_HPP

#include <string>

// Information like Title and Author.
struct ScriptInfo {
	std::string title;
	std::string author;
	std::string shortDesc;
};

// Store Information.
struct StoreInfo {
	std::string title;
	std::string author;
	std::string description;
	std::string url;
	std::string file;
	std::string storeSheet;
	std::string sheetURL;
	int version;
};

enum ScriptState {
	NONE = 0,
	FAILED_DOWNLOAD,
	SCRIPT_CANCELED,
	SYNTAX_ERROR,
	COPY_ERROR,
	MOVE_ERROR,
	DELETE_ERROR,
};

namespace ScriptHelper {
	// Get stuff from a JSON.
	std::string getString(nlohmann::json json, const std::string &key, const std::string &key2);
	int getNum(nlohmann::json json, const std::string &key, const std::string &key2);

	// Script Functions.
	Result downloadRelease(std::string repo, std::string file, std::string output, bool includePrereleases, bool showVersions, std::string message);
	Result downloadFile(std::string file, std::string output, std::string message);

	Result removeFile(std::string file, std::string message);
	void installFile(std::string file, bool updateSelf, std::string message);
	void extractFile(std::string file, std::string input, std::string output, std::string message);
	Result createFile(const char * path);
	void displayTimeMsg(std::string message, int seconds);

	bool checkIfValid(std::string scriptFile, int mode = 0);

	void bootTitle(const std::string TitleID, bool isNAND, std::string message);

	Result prompt(std::string message);

	Result copyFile(std::string source, std::string destination, std::string message);
	Result renameFile(std::string oldName, std::string newName, std::string message);
}

#endif