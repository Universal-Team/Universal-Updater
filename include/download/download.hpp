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

#pragma once

#include "utils/common.hpp"

#define APP_TITLE "Universal-Updater"
#define V_STRING "1.0.0"

enum DownloadError {
	DL_ERROR_NONE = 0,
	DL_ERROR_WRITEFILE,
	DL_ERROR_ALLOC,
	DL_ERROR_STATUSCODE,
	DL_ERROR_GIT,
};

struct ThemeEntry {
	std::string downloadUrl;
	std::string name;
	std::string path;
	std::string sdPath;
};

Result downloadToFile(std::string url, std::string path);
Result downloadFromRelease(std::string url, std::string asset, std::string path);

void displayProgressBar();

/**
 * Check Wi-Fi status.
 * @return True if Wi-Fi is connected; false if not.
 */
bool checkWifiStatus(void);

/**
 * Display "Please connect to Wi-Fi" for 2s.
 */
void notConnectedMsg(void);

/**
 * Display "Not Implemented Yet" for 2s.
 */
void notImplemented(void);

// Display the done msg.
void doneMsg(void);

/**
 * Get info from the GitHub API about a Release.
 * repo is where to get from. (Ex. "RocketRobz/TWiLightMenu")
 * item is that to get from the API. (Ex. "tag_name")
 * @return the string from the API.
 */
std::string getLatestRelease(std::string repo, std::string item);

/**
 * Get info from the GitHub API about a Commit.
 * repo is where to get from. (Ex. "RocketRobz/TWiLightMenu")
 * item is that to get from the API. (Ex. "sha")
 * @return the string from the API.
 */
std::string getLatestCommit(std::string repo, std::string item);

/**
 * Get info from the GitHub API about a Commit.
 * repo is where to get from. (Ex. "RocketRobz/TWiLightMenu")
 * array is the array the item is in. (Ex. "commit")
 * item is that to get from the API. (Ex. "message")
 * @return the string from the API.
 */
std::string getLatestCommit(std::string repo, std::string array, std::string item);

/**
 * Get a GitHub directory's contents with the GitHub API.
 * repo is where to get from. (Ex. "DS-Homebrew/twlmenu-extras")
 * path is the path within the repo (Ex. "contents/_nds/TWiLightMenu/dsimenu/themes")
 * @return the string from the API.
 */
std::vector<ThemeEntry> getThemeList(std::string repo, std::string path);

namespace download {
	void downloadRelease(std::string repo, std::string file, std::string output, std::string message);
	void downloadFile(std::string file, std::string output, std::string message);
	void deleteFileList(std::string file, std::string message);
	void installFileList(std::string file, std::string message);
	void extractFileList(std::string file, std::string input, std::string output, std::string message);
}

void downloadScripts(void);