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

#include "fileBrowse.hpp"
#include "files.hpp"
#include "json.hpp"
#include "structs.hpp"
#include <3ds.h>
#include <cstring>
#include <functional>
#include <unistd.h>

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

/*
	Return UniStore info.

	const std::string &file: Const Reference to the path of the file.
	const std::string &fieName: Const Reference to the filename, without path.
*/
UniStoreInfo GetInfo(const std::string &file, const std::string &fileName) {
	UniStoreInfo Temp = { "", "", "", "", fileName, "", -1, -1, -1 }; // Title, Author, URL, File (to check if no slash exist), FileName, Desc, Version, Revision, entries.

	if (fileName.length() > 4) {
		if(*(u32*)(fileName.c_str() + fileName.length() - 4) == (1886349435 & ~(1 << 3))) return Temp;
	}

	nlohmann::json JSON;
	FILE *temp = fopen(file.c_str(), "rt");
	if(temp) {
		JSON = nlohmann::json::parse(temp, nullptr, false);
		fclose(temp);
	}
	if (JSON.is_discarded())
		JSON = { };


	if (!JSON.contains("storeInfo")) return Temp; // storeInfo does not exist.

	if (JSON["storeInfo"].contains("title") && JSON["storeInfo"]["title"].is_string()) {
		Temp.Title = JSON["storeInfo"]["title"];
	}

	if (JSON["storeInfo"].contains("file") && JSON["storeInfo"]["file"].is_string()) {
		Temp.File = JSON["storeInfo"]["file"];
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

	if (JSON["storeInfo"].contains("revision") && JSON["storeInfo"]["revision"].is_number()) {
		Temp.Revision = JSON["storeInfo"]["revision"];
	}

	if (JSON.contains("storeContent")) Temp.StoreSize = JSON["storeContent"].size();

	return Temp;
}

/*
	Return UniStore info vector.

	const std::string &path: Const Reference to the path, where to check.
*/
std::vector<UniStoreInfo> GetUniStoreInfo(const std::string &path) {
	std::vector<UniStoreInfo> info;
	std::vector<DirEntry> dirContents;

	if (access(path.c_str(), F_OK) != 0) return {}; // Folder does not exist.

	chdir(path.c_str());
	getDirectoryContents(dirContents, { "unistore" });

	for(uint i = 0; i < dirContents.size(); i++) {
		/* Make sure to ONLY push .unistores, and no folders. Avoids crashes in that case too. */
		if ((path + dirContents[i].name).find(".unistore") != std::string::npos) {
			info.push_back( GetInfo(path + dirContents[i].name, dirContents[i].name) );
		}
	}

	return info;
}

#define copyBufSize 0x8000
u32 copyBuf[copyBufSize];

/*
	Copy a directory.

	DirEntry &entry: A DirEntry reference.
	const char *destinationPath: Pointer to the destination path.
	const char *sourcePath: Pointer to the source path.
*/
void dirCopy(const DirEntry &entry, const char *destinationPath, const char *sourcePath) {
	std::vector<DirEntry> dirContents;
	dirContents.clear();
	if (entry.isDirectory)
		chdir((sourcePath + ("/" + entry.name)).c_str());
	getDirectoryContents(dirContents);
	if (((int)dirContents.size()) == 1)
		mkdir((destinationPath + ("/" + entry.name)).c_str(), 0777);
	if (((int)dirContents.size()) != 1)
		fcopy((sourcePath + ("/" + entry.name)).c_str(), (destinationPath + ("/" + entry.name)).c_str());
}

u32 copyOffset = 0, copySize = 0;
/*
	The copy operation.

	const char *destinationPath: Pointer to the destination path.
	const char *sourcePath: Pointer to the source path.
*/
int fcopy(const char *sourcePath, const char *destinationPath) {
	copyOffset = 0, copySize = 0;

	DIR *isDir = opendir(sourcePath);

	if (isDir != NULL) {
		closedir(isDir);

		/* Source path is a directory. */
		chdir(sourcePath);
		std::vector<DirEntry> dirContents;
		getDirectoryContents(dirContents);
		mkdir(destinationPath, 0777);

		for(int i = 1; i < ((int)dirContents.size()); i++) {
			chdir(sourcePath);
			dirCopy(dirContents[i], destinationPath, sourcePath);
		}

		chdir(destinationPath);
		chdir("..");
		return 1;

	} else {
		closedir(isDir);

		/* Source path is a file. */
		FILE *sourceFile = fopen(sourcePath, "rb");
		copySize = 0, copyOffset = 0;

		if (sourceFile) {
			fseek(sourceFile, 0, SEEK_END);
			copySize = ftell(sourceFile); // Get source file's size.
			fseek(sourceFile, 0, SEEK_SET);
		} else {
			return -1;
		}

		if(getAvailableSpace() < copySize) {
			fclose(sourceFile);
			return -1;
		}

		FILE *destinationFile = fopen(destinationPath, "wb");
		if (!destinationFile) {
			fclose(sourceFile);
			return -1;
		}

		while(1) {
			/* Copy file to destination path. */
			int numr = fread(copyBuf, sizeof(u32), copyBufSize, sourceFile);
			int written = fwrite(copyBuf, sizeof(u32), numr, destinationFile);

			if(written != numr) {
				fclose(sourceFile);
				fclose(destinationFile);

				return -1;
			}

			copyOffset += copyBufSize * sizeof(u32);

			if (copyOffset > copySize) {
				fclose(sourceFile);
				fclose(destinationFile);

				return 1;
			}
		}

		return -1;
	}
}