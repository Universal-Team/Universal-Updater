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

#include "common.hpp"
#include "config.hpp"
#include "fileBrowse.hpp"
#include "gfx.hpp"
#include "gui.hpp"
#include "screenCommon.hpp"
#include "structs.hpp"

#include <3ds.h>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include <algorithm>
#include <functional>
#include <array>
#include <iostream>

int file_count = 0;

extern std::unique_ptr<Config> config;
extern uint selectedFile;
extern int keyRepeatDelay;
extern bool dirChanged;
std::vector<DirEntry> dirContents;

extern bool touching(touchPosition touch, Structs::ButtonPos button);
extern touchPosition touch;

const std::vector<Structs::ButtonPos> buttonPositions = {
	{295, 0, 25, 25}, // Arrow Up.
	{295, 215, 25, 25}, // Arrow Down.
	{15, 220, 50, 15}, // Open.
	{80, 220, 50, 15}, // Select.
	{145, 220, 50, 15}, // Refresh.
	{210, 220, 50, 15}, // Back.
	{0, 0, 25, 25}  // ViewMode Change.
};

off_t getFileSize(const char *fileName) {
	FILE* fp = fopen(fileName, "rb");
	off_t fsize = 0;
	if (fp) {
		fseek(fp, 0, SEEK_END);
		fsize = ftell(fp);			// Get source file's size
		fseek(fp, 0, SEEK_SET);
	}

	fclose(fp);

	return fsize;
}

bool nameEndsWith(const std::string& name, const std::vector<std::string> extensionList) {
	if (name.substr(0, 2) == "._") return false;

	if (name.size() == 0) return false;

	if (extensionList.size() == 0) return true;

	for(int i = 0; i < (int)extensionList.size(); i++) {
		const std::string ext = extensionList.at(i);
		if (strcasecmp(name.c_str() + name.size() - ext.size(), ext.c_str()) == 0) return true;
	}

	return false;
}

bool dirEntryPredicate(const DirEntry& lhs, const DirEntry& rhs) {
	if (!lhs.isDirectory && rhs.isDirectory) {
		return false;
	}

	if (lhs.isDirectory && !rhs.isDirectory) {
		return true;
	}

	return strcasecmp(lhs.name.c_str(), rhs.name.c_str()) < 0;
}

void getDirectoryContents(std::vector<DirEntry>& dirContents, const std::vector<std::string> extensionList) {
	struct stat st;

	dirContents.clear();

	DIR *pdir = opendir(".");

	if (pdir == NULL) {
		Msg::DisplayMsg("Unable to open the directory.");
		for(int i = 0; i < 120; i++) gspWaitForVBlank();
	} else {
		while(true) {
			DirEntry dirEntry;

			struct dirent* pent = readdir(pdir);
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

void getDirectoryContents(std::vector<DirEntry>& dirContents) {
	getDirectoryContents(dirContents, {});
}

std::vector<std::string> getContents(const std::string &name, const std::vector<std::string> &extensionList) {
	std::vector<std::string> dirContents;
	DIR* pdir = opendir(name.c_str());
	struct dirent *pent;
	while ((pent = readdir(pdir)) != NULL) {
		if (nameEndsWith(pent->d_name, extensionList))
			dirContents.push_back(pent->d_name);
	}

	closedir(pdir);
	return dirContents;
}

// Directory exist?
bool returnIfExist(const std::string &path, const std::vector<std::string> &extensionList) {
	dirContents.clear();
	chdir(path.c_str());
	std::vector<DirEntry> dirContentsTemp;
	getDirectoryContents(dirContentsTemp, extensionList);
	for(uint i = 0; i < dirContentsTemp.size(); i++) {
		dirContents.push_back(dirContentsTemp[i]);
	}

	if (dirContents.size() == 0) {
		return false;
	}

	return true;
}

// returns a Path or file to 'std::string'.
// selectText is the Text which is displayed on the bottom bar of the top screen.
// selectionMode is how you select it. 1 -> Path, 2 -> File.
std::string selectFilePath(std::string selectText, std::string initialPath, const std::vector<std::string> &extensionList, int selectionMode) {
	uint selectedFile = 0;
	std::string selectedPath = "";
	int keyRepeatDelay = 4;
	bool dirChanged = true;
	bool fastMode = false;
	uint screenPos = 0;
	uint screenPosList = 0;
	std::vector<DirEntry> dirContents;
	std::string dirs;

	// Initial dir change.
	dirContents.clear();
	chdir(initialPath.c_str());
	std::vector<DirEntry> dirContentsTemp;
	getDirectoryContents(dirContentsTemp, extensionList);
	for(uint i = 0; i < dirContentsTemp.size(); i++) {
		dirContents.push_back(dirContentsTemp[i]);
	}
	selectedFile = 0;

	while (1) {
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(Top, BLACK);
		C2D_TargetClear(Bottom, BLACK);
		GFX::DrawTop();
		char path[PATH_MAX];
		getcwd(path, PATH_MAX);
		Gui::DrawString((400-(Gui::GetStringWidth(0.60f, path)))/2, config->useBars() ? 0 : 2, 0.60f, config->textColor(), path, 390);
		Gui::DrawStringCentered(0, config->useBars() ? 220 : 218, 0.60f, config->textColor(), selectText, 390);
		GFX::DrawBottom();
		if (config->viewMode() == 0) {
			for(int i = 0; i < ENTRIES_PER_SCREEN && i < (int)dirContents.size(); i++) {
				Gui::Draw_Rect(0, 40+(i*57), 320, 45, config->unselectedColor());
				dirs = dirContents[screenPos + i].name;
				if (screenPos + i == selectedFile) {
					Gui::drawAnimatedSelector(0, 40+(i*57), 320, 45, .060, TRANSPARENT, config->selectedColor());
				}
				Gui::DrawStringCentered(0, 50+(i*57), 0.7f, config->textColor(), dirs, 320);
			}
		} else if (config->viewMode() == 1) {
			for(int i = 0; i < ENTRIES_PER_LIST && i < (int)dirContents.size(); i++) {
				Gui::Draw_Rect(0, (i+1)*27, 320, 25, config->unselectedColor());
				dirs = dirContents[screenPosList + i].name;
				if (screenPosList + i == selectedFile) {
					Gui::drawAnimatedSelector(0, (i+1)*27, 320, 25, .060, TRANSPARENT, config->selectedColor());
				}
				Gui::DrawStringCentered(0, ((i+1)*27)+1, 0.7f, config->textColor(), dirs, 320);
			}
		}

		Gui::DrawStringCentered(0, config->useBars() ? 0 : 2, 0.45f, config->textColor(), Lang::get("FILEBROWSE_MSG"), 260);
		GFX::DrawArrow(295, -1);
		GFX::DrawArrow(315, 240, 180.0);
		GFX::DrawSpriteBlend(sprites_view_idx, buttonPositions[6].x, buttonPositions[6].y);
		
		Gui::Draw_Rect(buttonPositions[2].x, buttonPositions[2].y, buttonPositions[2].w, buttonPositions[2].h, C2D_Color32(0, 0, 0, 190));
		Gui::Draw_Rect(buttonPositions[3].x, buttonPositions[3].y, buttonPositions[3].w, buttonPositions[3].h, C2D_Color32(0, 0, 0, 190));
		Gui::Draw_Rect(buttonPositions[4].x, buttonPositions[4].y, buttonPositions[4].w, buttonPositions[4].h, C2D_Color32(0, 0, 0, 190));
		Gui::Draw_Rect(buttonPositions[5].x, buttonPositions[5].y, buttonPositions[5].w, buttonPositions[5].h, C2D_Color32(0, 0, 0, 190));

		Gui::DrawStringCentered(-120, 222, 0.4, config->textColor(), Lang::get("OPEN"), 40);
		Gui::DrawStringCentered(-55, 222, 0.4, config->textColor(), Lang::get("SELECT"), 40);
		Gui::DrawStringCentered(10, 222, 0.4, config->textColor(), Lang::get("REFRESH"), 40);
		Gui::DrawStringCentered(75, 222, 0.4, config->textColor(), Lang::get("BACK"), 40);
		C3D_FrameEnd(0);

		// The input part.
		hidScanInput();
		hidTouchRead(&touch);

		if (keyRepeatDelay)	keyRepeatDelay--;

		if (dirChanged) {
			dirContents.clear();
			std::vector<DirEntry> dirContentsTemp;
			getDirectoryContents(dirContentsTemp, extensionList);
			for(uint i = 0; i < dirContentsTemp.size(); i++) {
				dirContents.push_back(dirContentsTemp[i]);
			}
			dirChanged = false;
		}

		if ((hidKeysDown() & KEY_SELECT) || (hidKeysDown() & KEY_TOUCH && touching(touch, buttonPositions[4]))) {
			dirChanged = true;
		}

		if ((hidKeysDown() & KEY_A) || (hidKeysDown() & KEY_TOUCH && touching(touch, buttonPositions[2]))) {
			if (dirContents.size() != 0) {
				if (dirContents[selectedFile].isDirectory) {
					chdir(dirContents[selectedFile].name.c_str());
					selectedFile = 0;
					dirChanged = true;
				}
			}
		}

		if (hidKeysDown() & KEY_TOUCH && touching(touch, buttonPositions[0])) {
			if (selectedFile > 0)	selectedFile--;
		}

		if (hidKeysDown() & KEY_TOUCH && touching(touch, buttonPositions[1])) {
			if (selectedFile < dirContents.size()-1)	selectedFile++;
		}

		if (hidKeysHeld() & KEY_UP) {
			if (selectedFile > 0 && !keyRepeatDelay) {
				selectedFile--;
				if (fastMode == true) {
					keyRepeatDelay = 3;
				} else if (fastMode == false){
					keyRepeatDelay = 6;
				}
			}
		}
		if (hidKeysHeld() & KEY_DOWN && !keyRepeatDelay) {
			if (selectedFile < dirContents.size()-1) {
				selectedFile++;
				if (fastMode == true) {
					keyRepeatDelay = 3;
				} else if (fastMode == false){
					keyRepeatDelay = 6;
				}
			}
		}

		if ((hidKeysDown() & KEY_B) || (hidKeysDown() & KEY_TOUCH && touching(touch, buttonPositions[5]))) {
			char path[PATH_MAX];
			getcwd(path, PATH_MAX);
			if (strcmp(path, "sdmc:/") == 0 || strcmp(path, "/") == 0) {
				return "";
			} else {
				chdir("..");
				selectedFile = 0;
				dirChanged = true;
			}
		}

		if ((hidKeysDown() & KEY_X) || (hidKeysDown() & KEY_TOUCH && touching(touch, buttonPositions[3]))) {
			char path[PATH_MAX];
			getcwd(path, PATH_MAX);
			selectedPath = path;
			if (selectionMode == 2) {
				selectedPath += dirContents[selectedFile].name;
			}
			return selectedPath;
		}

		if (hidKeysDown() & KEY_R) {
			fastMode = true;
		}

		if (hidKeysDown() & KEY_L) {
			fastMode = false;
		}
		// Switch ViewMode.
		if ((hidKeysDown() & KEY_Y) || (hidKeysDown() & KEY_TOUCH && touching(touch, buttonPositions[6]))) {
			if (config->viewMode() == 0) {
				config->viewMode(1);
			} else {
				config->viewMode(0);
			}
		}

		if (config->viewMode() == 0) {
			if (selectedFile < screenPos) {
				screenPos = selectedFile;
			} else if (selectedFile > screenPos + ENTRIES_PER_SCREEN - 1) {
				screenPos = selectedFile - ENTRIES_PER_SCREEN + 1;
			}
		} else if (config->viewMode() == 1) {
			if (selectedFile < screenPosList) {
				screenPosList = selectedFile;
			} else if (selectedFile > screenPosList + ENTRIES_PER_LIST - 1) {
				screenPosList = selectedFile - ENTRIES_PER_LIST + 1;
			}
		}
	}
}

#define copyBufSize 0x8000

u32 copyBuf[copyBufSize];

void dirCopy(DirEntry* entry, int i, const char *destinationPath, const char *sourcePath) {
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

		// Source path is a directory
		chdir(sourcePath);
		std::vector<DirEntry> dirContents;
		getDirectoryContents(dirContents);
		DirEntry* entry = &dirContents.at(1);

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

		// Source path is a file
		FILE* sourceFile = fopen(sourcePath, "rb");
		off_t fsize = 0;
		if (sourceFile) {
			fseek(sourceFile, 0, SEEK_END);
			fsize = ftell(sourceFile);			// Get source file's size
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
				// Cancel copying
				fclose(sourceFile);
				fclose(destinationFile);
				return -1;
				break;
			}
			
			printf("\x1b[16;0H");
			printf("Progress:\n");
			printf("%i/%i Bytes					   ", (int)offset, (int)fsize);

			// Copy file to destination path
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