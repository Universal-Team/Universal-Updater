/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2021 Universal-Team
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

#ifndef _UNIVERSAL_UPDATER_DOWNLOAD_HPP
#define _UNIVERSAL_UPDATER_DOWNLOAD_HPP

#include "common.hpp"

#define APP_TITLE "Universal-Updater"

enum DownloadError {
	DL_ERROR_NONE = 0,
	DL_ERROR_WRITEFILE,
	DL_ERROR_ALLOC,
	DL_ERROR_STATUSCODE,
	DL_ERROR_GIT,
	DL_CANCEL, // No clue if that's needed tho.
};

struct StoreList {
	std::string Title;
	std::string Author;
	std::string URL;
	std::string Description;
};

struct UUUpdate {
	bool Available = false;
	std::string Notes = "";
	std::string Version = "";
};

Result downloadToFile(const std::string &url, const std::string &path);
Result downloadFromRelease(const std::string &url, const std::string &asset, const std::string &path, bool includePrereleases);

/*
	Check Wi-Fi status.
	@return True if Wi-Fi is connected; false if not.
*/
bool checkWifiStatus(void);

/*
	Display "Please connect to Wi-Fi" for 2s.
*/
void notConnectedMsg(void);

/*
	Display "Not Implemented Yet" for 2s.
*/
void notImplemented(void);

/*
	Display the done msg.
*/
void doneMsg(void);

bool IsUpdateAvailable(const std::string &URL, int revCurrent);
bool DownloadUniStore(const std::string &URL, int currentRev, std::string &fl, bool isDownload = false, bool isUDB = false);
bool DownloadSpriteSheet(const std::string &URL, const std::string &file);
UUUpdate IsUUUpdateAvailable();
void UpdateAction();
std::vector<StoreList> FetchStores();
C2D_Image FetchScreenshot(const std::string &URL);
std::string GetChangelog();

#endif