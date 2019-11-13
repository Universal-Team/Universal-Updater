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

#include "screens/tinyDB.hpp"

#include "utils/config.hpp"
#include "utils/fileBrowse.h"
#include "utils/json.hpp"

#define ENTRIES_PER_SCREEN 3
#define ENTRIES_PER_LIST 7

// JSON file for TinyDB.
nlohmann::json tinyDBJson;
std::string selectedOption;

#define tinyDBFile "sdmc:/3ds/Universal-Updater/TinyDB.json"

extern std::string get(nlohmann::json json, const std::string &key, const std::string &key2);
std::string maxEntries;

// Parse the Objects.
std::vector<std::string> parseObjects() {
	FILE* file = fopen(tinyDBFile, "rt");
	if(!file) {
		printf("File not found\n");
		fclose(file);
		return {{""}};
	}
	tinyDBJson = nlohmann::json::parse(file, nullptr, false);
	fclose(file);

	std::vector<std::string> objs;
	for(auto it = tinyDBJson.begin();it != tinyDBJson.end(); it++) {
		if(it.key() != "info") {
			objs.push_back(it.key());
		}
	}
	return objs;
}

std::vector<std::string> tinyDBList;

// adapted from GM9i's byte parsing.
std::string parseBytes(int bytes) {
    char out[32];
    if(bytes == 1)
        snprintf(out, sizeof(out), "%d Byte", bytes);
    else if(bytes < 1024)
        snprintf(out, sizeof(out), "%d Bytes", bytes);
    else if(bytes < 1024 * 1024)
        snprintf(out, sizeof(out), "%.1f KB", (float)bytes / 1024);
    else if (bytes < 1024 * 1024 * 1024)
        snprintf(out, sizeof(out), "%.1f MB", (float)bytes / 1024 / 1024);
    else
        snprintf(out, sizeof(out), "%.1f GB", (float)bytes / 1024 / 1024 / 1024);

    return out;
}

TinyDB::TinyDB() {
	DisplayMsg(Lang::get("TINYDB_DOWNLOADING"));
	downloadToFile("https://tinydb.eiphax.tech/api/universal-updater.json?raw=true", tinyDBFile);
    tinyDBList = parseObjects();
	selectedOption = tinyDBList[0];
}

// To-Do.
void TinyDB::Draw(void) const {
    std::string info;
    Gui::ScreenDraw(top);
	Gui::Draw_Rect(0, 0, 400, 30, C2D_Color32(63, 81, 181, 255));
	Gui::Draw_Rect(0, 30, 400, 180, C2D_Color32(140, 140, 140, 255));
	Gui::Draw_Rect(0, 210, 400, 30, C2D_Color32(63, 81, 181, 255));

	Gui::DrawStringCentered(0, 35, 0.6f, Config::TxtColor, Lang::get("AUTHOR") + std::string(tinyDBJson[selectedOption]["info"]["author"]), 400);
	Gui::DrawStringCentered(0, 65, 0.6f, Config::TxtColor, Lang::get("DESC") + std::string(tinyDBJson[selectedOption]["info"]["description"]), 400);
	Gui::DrawStringCentered(0, 95, 0.6f, Config::TxtColor, Lang::get("RELEASE_TAG") + std::string(tinyDBJson[selectedOption]["info"]["releaseTag"]), 400);
	Gui::DrawStringCentered(0, 125, 0.6f, Config::TxtColor, Lang::get("RELEASE_ID") + std::string(tinyDBJson[selectedOption]["info"]["releaseId"]), 400);
	Gui::DrawStringCentered(0, 155, 0.6f, Config::TxtColor, Lang::get("TITLE_ID") + std::string(tinyDBJson[selectedOption]["info"]["titleid"]), 400);
	Gui::DrawStringCentered(0, 185, 0.6f, Config::TxtColor, Lang::get("FILE_SIZE") + parseBytes(int64_t(tinyDBJson[selectedOption]["info"]["fileSize"])), 400);
    Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, "TinyDB", 400);
    std::string entryAmount = std::to_string(selection+1) + " / " + std::to_string(tinyDBList.size());
    Gui::DrawString(397-Gui::GetStringWidth(0.6f, entryAmount), 237-Gui::GetStringHeight(0.6f, entryAmount), 0.6f, Config::TxtColor, entryAmount);


    Gui::ScreenDraw(bottom);
	Gui::Draw_Rect(0, 0, 320, 30, C2D_Color32(63, 81, 181, 255));
	Gui::Draw_Rect(0, 30, 320, 180, C2D_Color32(140, 140, 140, 255));
	Gui::Draw_Rect(0, 210, 320, 30, C2D_Color32(63, 81, 181, 255));

    // Search Icon.
	Gui::sprite(sprites_search_idx, -3, 0);
	Gui::DrawString(7.5, 1.5, 0.72f, BLACK, "\uE003");

    if (listMode == 0) {
    	for(int i=0;i<ENTRIES_PER_SCREEN && i<(int)tinyDBList.size();i++) {
	    	info = tinyDBList[screenPos + i];
		    if(screenPos + i == selection) {
			    Gui::Draw_Rect(0, 40+(i*57), 320, 45, C2D_Color32(120, 192, 216, 255));
	    	} else { 
		    	Gui::Draw_Rect(0, 40+(i*57), 320, 45, C2D_Color32(77, 118, 132, 255));
		    }
    		Gui::DrawStringCentered(0, 50+(i*57), 0.7f, WHITE, info, 320);
    	}
	} else if (listMode == 1) {
		for(int i=0;i<ENTRIES_PER_LIST && i<(int)tinyDBList.size();i++) {
			info = tinyDBList[screenPosList + i];
			if(screenPosList + i == selection) {
				Gui::Draw_Rect(0, 30+(i*25), 320, 30, Config::SelectedColor);
			}
			Gui::DrawStringCentered(0, 35+(i*25), 0.7f, Config::TxtColor, info, 320);
		}
	}
}

void TinyDB::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
    if (keyRepeatDelay)	keyRepeatDelay--;
    if (hDown & KEY_B) {
        Gui::screenBack();
        return;
    }

	if (hDown & KEY_R) {
		fastMode = true;
	}

	if (hDown & KEY_L) {
		fastMode = false;
	}

	if (hHeld & KEY_UP && !keyRepeatDelay) {
		if (selection > 0) {
			selection--;
			selectedOption = tinyDBList[selection];
		} else {
			selection = (int)tinyDBList.size()-1;
			selectedOption = tinyDBList[selection];
		}
		if (fastMode == true) {
			keyRepeatDelay = 3;
		} else if (fastMode == false){
			keyRepeatDelay = 6;
		}
	}

	if (hHeld & KEY_DOWN && !keyRepeatDelay) {
		if (selection < (int)tinyDBList.size()-1) {
			selection++;
			selectedOption = tinyDBList[selection];
		} else {
			selection = 0;
			selectedOption = tinyDBList[selection];
		}
		if (fastMode == true) {
			keyRepeatDelay = 3;
		} else if (fastMode == false){
			keyRepeatDelay = 6;
		}
	}

	if (listMode == 0) {
		if(selection < screenPos) {
			screenPos = selection;
		} else if (selection > screenPos + ENTRIES_PER_SCREEN - 1) {
			screenPos = selection - ENTRIES_PER_SCREEN + 1;
		}
	} else if (listMode == 1) {
		if(selection < screenPosList) {
			screenPosList = selection;
		} else if (selection > screenPosList + ENTRIES_PER_LIST - 1) {
			screenPosList = selection - ENTRIES_PER_LIST + 1;
		}
	}

	if (hDown & KEY_X) {
		if (listMode == 0) {
			listMode = 1;
		} else {
			listMode = 0;
		}
	}

    if (hDown & KEY_A) {
        execute();
    }
}

void TinyDB::execute() {
	for(int i=0;i<(int)tinyDBJson.at(selectedOption).size();i++) {
		std::string type = tinyDBJson.at(selectedOption).at("script").at(i).at("type");
		if(type == "deleteFile") {
			bool missing = false;
			std::string file, message;
			if(tinyDBJson.at(selectedOption).at("script").at(i).contains("file"))	file = tinyDBJson.at(selectedOption).at("script").at(i).at("file");
			else    missing = true;
			if(tinyDBJson.at(selectedOption).at("script").at(i).contains("message"))	message = tinyDBJson.at(selectedOption).at("script").at(i).at("message");
			if(!missing)	download::deleteFileList(file, message);

		} else if(type == "downloadFile") {
			bool missing = false;
			std::string file, output, message;
			if(tinyDBJson.at(selectedOption).at("script").at(i).contains("file"))	file = tinyDBJson.at(selectedOption).at("script").at(i).at("file");
			else	missing = true;
			if(tinyDBJson.at(selectedOption).at("script").at(i).contains("output"))	output = tinyDBJson.at(selectedOption).at("script").at(i).at("output");
			else	missing = true;
			if(tinyDBJson.at(selectedOption).at("script").at(i).contains("message"))	message = tinyDBJson.at(selectedOption).at("script").at(i).at("message");
			if(!missing)	download::downloadFile(file, output, message);

		} else if(type == "installCia") {
			bool missing = false;
			std::string file, message;
			if(tinyDBJson.at(selectedOption).at("script").at(i).contains("file"))	file = tinyDBJson.at(selectedOption).at("script").at(i).at("file");
			else    missing = true;
			if(tinyDBJson.at(selectedOption).at("script").at(i).contains("message"))	message = tinyDBJson.at(selectedOption).at("script").at(i).at("message");
			if(!missing)	download::installFileList(file, message);
		}
	}
	doneMsg();
}