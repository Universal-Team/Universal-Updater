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

#include "download/download.hpp"

#include "screens/mainMenu.hpp"
#include "screens/scriptlist.hpp"

#include "utils/parse.hpp"

#include <algorithm>
#include <fstream>
#include <unistd.h>

#define ENTRIES_PER_SCREEN 3

struct Info {
	std::string title;
	std::string description;
};

Info parseInfo(std::string fileName) {
	FILE* file = fopen(fileName.c_str(), "rt");
	if(!file) {
		printf("File not found\n");
		fclose(file);
		return {"", ""};
	}
	nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
	fclose(file);

	Info info;
	info.title = get(json, "info", "title");
	info.description = get(json, "info", "description");
	return info;
}

std::vector<Info> fileInfo;

ScriptList::ScriptList() {
	dirContents.clear();
	chdir(SCRIPTS_PATH);
	getDirectoryContents(dirContents);
	for(uint i=0;i<dirContents.size();i++) {
		fileInfo.push_back(parseInfo(dirContents[i].name));
	}
}

void ScriptList::Draw(void) const {
	std::string titleinfo;
	Gui::DrawTop();
	Gui::DrawStringCentered(0, 2, 0.7f, TextColor, "Universal-Updater", 400);
	Gui::DrawBottom();
	for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)fileInfo.size();i++) {
		titleinfo = fileInfo[i].title + '\n' + fileInfo[i].description;
		Gui::Draw_Rect(0, 40+(i*57), 320, 45, TopBGColor);
		Gui::DrawString(0, 40+(i*57), 0.7f, WHITE, titleinfo, 320);
	}
}

void ScriptList::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_B) {
		Gui::screenBack();
		return;
	}
}