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
#include "overlay.hpp"
#include <unistd.h>

extern bool touching(touchPosition touch, Structs::ButtonPos button);
static const std::vector<Structs::ButtonPos> mainButtons = {
	{ 10, 4, 300, 22 },
	{ 10, 34, 300, 22 },
	{ 10, 64, 300, 22 },
	{ 10, 94, 300, 22 },
	{ 10, 124, 300, 22 },
	{ 10, 154, 300, 22 },
	{ 10, 184, 300, 22 },
	{ 54, 214, 300, 22 }
};

/*
	Select a Directory.
*/
std::string Overlays::SelectDir(const std::string &oldDir, const std::string &msg) {
	std::string currentPath = oldDir;
	bool dirChanged = true;
	int selection = 0, sPos = 0;

	std::vector<DirEntry> dirContents;

	if (dirChanged) {
		dirChanged = false;

		dirContents.clear();
		chdir(oldDir.c_str());
		std::vector<DirEntry> dirContentsTemp;
		getDirectoryContents(dirContentsTemp);

		for(uint i = 0; i < dirContentsTemp.size(); i++) {
			dirContents.push_back(dirContentsTemp[i]);
		}
	}

	while(1) {
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(Top, TRANSPARENT);
		C2D_TargetClear(Bottom, TRANSPARENT);

		GFX::DrawTop();
		Gui::DrawStringCentered(0, 1, 0.7f, TEXT_COLOR, msg, 380);

		Gui::Draw_Rect(0, 215, 400, 25, BAR_COLOR);
		Gui::Draw_Rect(0, 214, 400, 1, BAR_OUTL_COLOR);
		Gui::DrawStringCentered(0, 217, 0.6f, TEXT_COLOR, currentPath, 390);

		GFX::DrawBottom();
		if (dirContents.size() > 0) {
			for(int i = 0; i < 8 && i < (int)dirContents.size(); i++) {
				GFX::drawBox(10, mainButtons[i].y, 300, 22, sPos + i == selection);
				Gui::DrawStringCentered(10 - 160 + (300 / 2), mainButtons[i].y + 4, 0.45f, TEXT_COLOR, dirContents[sPos + i].name, 295);
			}
		}

		C3D_FrameEnd(0);

		if (dirChanged) {
			dirChanged = false;

			selection = 0;
			sPos = 0;
			dirContents.clear();
			std::vector<DirEntry> dirContentsTemp;
			getDirectoryContents(dirContentsTemp);

			for(uint i = 0; i < dirContentsTemp.size(); i++) {
				dirContents.push_back(dirContentsTemp[i]);
			}
		}


		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);
		u32 hRepeat = hidKeysDownRepeat();

		if (dirContents.size() > 0) {
			if (hRepeat & KEY_DOWN) {
				if (selection < (int)dirContents.size() - 1) selection++;
			}

			if (hRepeat & KEY_UP) {
				if (selection > 0) selection--;
			}

			if (hidKeysDown() & KEY_A) {
				if (dirContents[selection].isDirectory) {

					chdir(dirContents[selection].name.c_str());

					char path[PATH_MAX];
					getcwd(path, PATH_MAX);
					currentPath = path;

					dirChanged = true;
				}
			}

			if (hidKeysDown() & KEY_TOUCH) {
				for (int i = 0; i < 7; i++) {
					if (touching(touch, mainButtons[i])) {
						if (i + sPos < (int)dirContents.size()) {
							if (dirContents[i + sPos].isDirectory) {
								chdir(dirContents[i + sPos].name.c_str());

								char path[PATH_MAX];
								getcwd(path, PATH_MAX);
								currentPath = path;

								dirChanged = true;
								break;
							}
						}
					}
				}
			}

			if (selection < sPos) sPos = selection;
			else if (selection > sPos + 8 - 1) sPos = selection - 8 + 1;
		}

		if (hidKeysDown() & KEY_X) {
			if (currentPath.size() > 0 && currentPath.back() == '/') currentPath.pop_back(); // Pop back the "/".
			return currentPath;
		}

		if (hidKeysDown() & KEY_B) {
			char path[PATH_MAX];
			getcwd(path, PATH_MAX);

			if (strcmp(path, "sdmc:/") == 0 || strcmp(path, "/") == 0) return "";
			else {
				chdir("..");
				getcwd(path, PATH_MAX);
				currentPath = path;
				dirChanged = true;
			}
		}
	}
}