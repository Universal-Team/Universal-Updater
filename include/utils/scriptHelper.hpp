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

#ifndef SCRIPTHELPER_HPP
#define SCRIPTHELPER_HPP

#include <string>

namespace ScriptHelper {
	// Get stuff from a JSON.
	std::string getString(nlohmann::json json, const std::string &key, const std::string &key2);
	int getNum(nlohmann::json json, const std::string &key, const std::string &key2);

	// Script Functions.
	void downloadRelease(std::string repo, std::string file, std::string output, bool includePrereleases, std::string message);
	void downloadFile(std::string file, std::string output, std::string message);

	void removeFile(std::string file, std::string message);
	void installFile(std::string file, std::string message);
	void extractFile(std::string file, std::string input, std::string output, std::string message);
	Result createFile(const char * path);
	void displayTimeMsg(std::string message, int seconds);
}

#endif