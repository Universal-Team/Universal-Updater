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

#ifndef _UNIVERSAL_UPDATER_FILE_BROWSE_HPP
#define _UNIVERSAL_UPDATER_FILE_BROWSE_HPP

#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <vector>

struct DirEntry {
	std::string name;
	std::string path;
	bool isDirectory;
};

/*
	UniStore Info struct.
*/
struct UniStoreInfo {
	std::string Title;
	std::string Author;
	std::string URL;
	std::string File; // Used to check, if File does NOT contain a slash or so.
	std::string FileName;
	std::string Description;
	int Version;
	int Revision;
	int StoreSize;
};

bool nameEndsWith(const std::string &name, const std::vector<std::string> &extensionList);
void getDirectoryContents(std::vector<DirEntry> &dirContents, const std::vector<std::string> &extensionList);
void getDirectoryContents(std::vector<DirEntry> &dirContents);

std::vector<UniStoreInfo> GetUniStoreInfo(const std::string &path);

void dirCopy(DirEntry *entry, const char *destinationPath, const char *sourcePath);
int fcopy(const char *sourcePath, const char *destinationPath);

#endif