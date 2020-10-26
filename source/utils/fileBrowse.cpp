/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 Universal-Team
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

#include "fileBrowse.hpp"
#include "json.hpp"
#include "structs.hpp"

#include <3ds.h>
#include <cstring>
#include <functional>
#include <unistd.h>

extern bool touching(touchPosition touch, Structs::ButtonPos button);
extern touchPosition touch;

bool nameEndsWith(const std::string &name, const std::vector<std::string> &extensionList) {
	if (name.substr(0, 2) == "._") return false;

	if (name.size() == 0) return false;

	if (extensionList.size() == 0) return true;

	for(int i = 0; i < (int)extensionList.size(); i++) {
		const std::string ext = extensionList.at(i);
		if (strcasecmp(name.c_str() + name.size() - ext.size(), ext.c_str()) == 0) return true;
	}

	return false;
}

bool dirEntryPredicate(const DirEntry &lhs, const DirEntry &rhs) {
	if (!lhs.isDirectory && rhs.isDirectory) return false;
	if (lhs.isDirectory && !rhs.isDirectory) return true;

	return strcasecmp(lhs.name.c_str(), rhs.name.c_str()) < 0;
}

void getDirectoryContents(std::vector<DirEntry> &dirContents, const std::vector<std::string> &extensionList) {
	struct stat st;

	dirContents.clear();

	DIR *pdir = opendir(".");

	if (pdir != nullptr) {
		while(true) {
			DirEntry dirEntry;

			struct dirent *pent = readdir(pdir);
			if (pent == NULL) break;

			stat(pent->d_name, &st);
			dirEntry.name = pent->d_name;
			dirEntry.isDirectory = (st.st_mode & S_IFDIR) ? true : false;

			if (dirEntry.name.compare(".") != 0 && (dirEntry.isDirectory || nameEndsWith(dirEntry.name, extensionList))) {
				dirContents.push_back(dirEntry);
			}
		}

		closedir(pdir);
	}

	sort(dirContents.begin(), dirContents.end(), dirEntryPredicate);
}

void getDirectoryContents(std::vector<DirEntry> &dirContents) {
	getDirectoryContents(dirContents, {});
}

std::vector<std::string> getContents(const std::string &name, const std::vector<std::string> &extensionList) {
	std::vector<std::string> dirContents;
	DIR* pdir = opendir(name.c_str());
	struct dirent *pent;

	while ((pent = readdir(pdir)) != NULL) {
		if (nameEndsWith(pent->d_name, extensionList)) dirContents.push_back(pent->d_name);
	}

	closedir(pdir);
	return dirContents;
}

/*
	Directory exist?
*/
bool returnIfExist(const std::string &path, const std::vector<std::string> &extensionList) {
	std::vector<DirEntry> dirContents;

	chdir(path.c_str());
	std::vector<DirEntry> dirContentsTemp;
	getDirectoryContents(dirContentsTemp, extensionList);

	for(uint i = 0; i < dirContentsTemp.size(); i++) {
		dirContents.push_back(dirContentsTemp[i]);
	}

	if (dirContents.size() == 0) return false;

	return true;
}

/*
	Return UniStore info.
*/
UniStoreInfo GetInfo(const std::string &file, const std::string &fileName) {
	UniStoreInfo Temp = { "", "", "", fileName, "", 0, -1 }; // Title, Author, URL, FileName, Desc, Version, Entries.
	nlohmann::json JSON = nullptr;

	FILE *temp = fopen(file.c_str(), "rt");
	JSON = nlohmann::json::parse(temp, nullptr, false);
	fclose(temp);

	if (!JSON.contains("storeInfo")) return Temp; // storeInfo does not exist.

	if (JSON["storeInfo"].contains("title") && JSON["storeInfo"]["title"].is_string()) {
		Temp.Title = JSON["storeInfo"]["title"];
	}

	if (JSON["storeInfo"].contains("author") && JSON["storeInfo"]["author"].is_string()) {
		Temp.Author = JSON["storeInfo"]["author"];
	}

	if (JSON["storeInfo"].contains("url") && JSON["storeInfo"]["url"].is_string()) {
		Temp.URL = JSON["storeInfo"]["url"];
	}

	if (JSON["storeInfo"].contains("description") && JSON["storeInfo"]["description"].is_string()) {
		Temp.Description = JSON["storeInfo"]["description"];
	}

	if (JSON["storeInfo"].contains("version") && JSON["storeInfo"]["version"].is_number()) {
		Temp.Version = JSON["storeInfo"]["version"];
	}

	if (JSON.contains("storeContent")) Temp.StoreSize = JSON["storeContent"].size();

	return Temp;
}

/*
	Return UniStore info vector.
*/
std::vector<UniStoreInfo> GetUniStoreInfo(const std::string &path) {
	std::vector<UniStoreInfo> info;
	std::vector<DirEntry> dirContents;

	chdir(path.c_str());

	getDirectoryContents(dirContents, { "unistore" });

	for(uint i = 0; i < dirContents.size(); i++) {
		info.push_back( GetInfo("sdmc:/3ds/Universal-Updater/stores/" + dirContents[i].name, dirContents[i].name) );
	}

	return info;
}

#define copyBufSize 0x8000
u32 copyBuf[copyBufSize];

void dirCopy(DirEntry *entry, int i, const char *destinationPath, const char *sourcePath) {
	std::vector<DirEntry> dirContents;
	dirContents.clear();
	if (entry->isDirectory)	chdir((sourcePath + ("/" + entry->name)).c_str());
	getDirectoryContents(dirContents);
	if (((int)dirContents.size()) == 1)	mkdir((destinationPath + ("/" + entry->name)).c_str(), 0777);
	if (((int)dirContents.size()) != 1)	fcopy((sourcePath + ("/" + entry->name)).c_str(), (destinationPath + ("/" + entry->name)).c_str());
}

int fcopy(const char *sourcePath, const char *destinationPath) {
	DIR *isDir = opendir(sourcePath);

	if (isDir != NULL) {
		closedir(isDir);

		/* Source path is a directory. */
		chdir(sourcePath);
		std::vector<DirEntry> dirContents;
		getDirectoryContents(dirContents);
		DirEntry *entry = &dirContents.at(1);
		mkdir(destinationPath, 0777);

		for(int i = 1; i < ((int)dirContents.size()); i++) {
			chdir(sourcePath);
			entry = &dirContents.at(i);
			dirCopy(entry, i, destinationPath, sourcePath);
		}

		chdir(destinationPath);
		chdir("..");
		return 1;

	} else {
		closedir(isDir);

		/* Source path is a file. */
		FILE *sourceFile = fopen(sourcePath, "rb");
		off_t fsize = 0;
		if (sourceFile) {
			fseek(sourceFile, 0, SEEK_END);
			fsize = ftell(sourceFile); // Get source file's size.
			fseek(sourceFile, 0, SEEK_SET);

		} else {
			fclose(sourceFile);
			return -1;
		}

		FILE* destinationFile = fopen(destinationPath, "wb");
		//if (destinationFile) {
			fseek(destinationFile, 0, SEEK_SET);
		/*} else {
			fclose(sourceFile);
			fclose(destinationFile);
			return -1;
		}*/

		off_t offset = 0;
		int numr;
		while(1) {
			scanKeys();
			if (keysHeld() & KEY_B) {
				/* Cancel copying. */
				fclose(sourceFile);
				fclose(destinationFile);
				return -1;
				break;
			}

			printf("\x1b[16;0H");
			printf("Progress:\n");
			printf("%i/%i Bytes					   ", (int)offset, (int)fsize);

			/* Copy file to destination path. */
			numr = fread(copyBuf, 2, copyBufSize, sourceFile);
			fwrite(copyBuf, 2, numr, destinationFile);
			offset += copyBufSize;

			if (offset > fsize) {
				fclose(sourceFile);
				fclose(destinationFile);

				printf("\x1b[17;0H");
				printf("%i/%i Bytes					   ", (int)fsize, (int)fsize);
				for(int i = 0; i < 30; i++) gspWaitForVBlank();

				return 1;
				break;
			}
		}

		return -1;
	}
}