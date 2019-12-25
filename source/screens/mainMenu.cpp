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

#include "download/download.hpp"

#include "screens/ftpScreen.hpp"
#include "screens/mainMenu.hpp"
#include "screens/scriptBrowse.hpp"
#include "screens/scriptCreator.hpp"
#include "screens/scriptlist.hpp"
#include "screens/settings.hpp"
#include "screens/tinyDB.hpp"

#include "utils/config.hpp"

#include <unistd.h>

extern bool exiting;
extern bool touching(touchPosition touch, Structs::ButtonPos button);
extern bool checkWifiStatus(void);
extern int fadealpha;
extern bool fadein;
extern void notImplemented(void);

// This is for the Script Creator, so no one can access it for now, until it is stable or so.
bool isTesting = false;

void MainMenu::Draw(void) const {
	Gui::DrawTop();
	Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, "Universal-Updater", 400);
	Gui::DrawString(397-Gui::GetStringWidth(0.5f, V_STRING), 239-Gui::GetStringHeight(0.5f, V_STRING), 0.5f, Config::TxtColor, V_STRING);
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(0, 0, 0, fadealpha)); // Fade in out effect
	Gui::DrawBottom();
	Gui::DrawArrow(0, 218, 0, 1);

	for (int i = 0; i < 6; i++) {
		if (Selection == i) {
			Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, Config::SelectedColor);
		} else {
			Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, Config::UnselectedColor);
		}
	}

	Gui::DrawString((320-Gui::GetStringWidth(0.6f, Lang::get("SCRIPTLIST")))/2-150+70, mainButtons[0].y+10, 0.6f, Config::TxtColor, Lang::get("SCRIPTLIST"), 140);
	Gui::DrawString((320-Gui::GetStringWidth(0.6f, Lang::get("GET_SCRIPTS")))/2+150-70, mainButtons[1].y+10, 0.6f, Config::TxtColor, Lang::get("GET_SCRIPTS"), 140);
	Gui::DrawString((320-Gui::GetStringWidth(0.6f, "TinyDB"))/2-150+70, mainButtons[2].y+10, 0.6f, Config::TxtColor, "TinyDB", 140);
	Gui::DrawString((320-Gui::GetStringWidth(0.6f, Lang::get("SCRIPTCREATOR")))/2+150-70, mainButtons[3].y+10, 0.6f, Config::TxtColor, Lang::get("SCRIPTCREATOR"), 140);
	Gui::DrawString((320-Gui::GetStringWidth(0.6f, Lang::get("SETTINGS")))/2-150+70, mainButtons[4].y+10, 0.6f, Config::TxtColor, Lang::get("SETTINGS"), 140);
	Gui::DrawString((320-Gui::GetStringWidth(0.6f, "FTP"))/2+150-70, mainButtons[5].y+10, 0.6f, Config::TxtColor, "FTP", 140);
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(0, 0, 0, fadealpha)); // Fade in out effect
}

bool MainMenu::returnScriptState() {
	dirContents.clear();
	chdir(Config::ScriptPath.c_str());
	std::vector<DirEntry> dirContentsTemp;
	getDirectoryContents(dirContentsTemp, {"json"});
	for(uint i=0;i<dirContentsTemp.size();i++) {
		dirContents.push_back(dirContentsTemp[i]);
	}

	if (dirContents.size() == 0) {
		return false;
	}
	return true;
}

void MainMenu::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_START) {
		exiting = true;
	}
	if (hDown & KEY_UP) {
		if(Selection > 1)	Selection -= 2;
	}
	if (hDown & KEY_DOWN) {
		if(Selection < 4)	Selection += 2;
	}
	if (hDown & KEY_LEFT) {
		if (Selection%2) Selection--;
	}
	if (hDown & KEY_RIGHT) {
		if (!(Selection%2)) Selection++;
	}

	if (hDown & KEY_A) {
		switch(Selection) {
			case 0:
				if (returnScriptState() == true) {
					Screen::set(std::make_unique<ScriptList>());
				} else {
					Gui::DisplayWarnMsg(Lang::get("GET_SCRIPTS_FIRST"));
				}
				break;
			case 1:
				if (checkWifiStatus() == true) {
					Screen::set(std::make_unique<ScriptBrowse>());
				} else {
					notConnectedMsg();
				}
				break;
			case 2:
				if (checkWifiStatus() == true) {
					Screen::set(std::make_unique<TinyDB>());
				} else {
					notConnectedMsg();
				}
				break;
			case 3:
				if (isTesting == true) {
					Screen::set(std::make_unique<ScriptCreator>());
				} else {
					notImplemented();
				}
				break;
			case 4:
				Screen::set(std::make_unique<Settings>());
				break;
			case 5:
				if (checkWifiStatus() == true) {
					Screen::set(std::make_unique<FTPScreen>());
				} else {
					notConnectedMsg();
				}
				break;
		}
	}

	if (hDown & KEY_X) {
		if (checkWifiStatus() == true) {
			Screen::set(std::make_unique<FTPScreen>());
		}
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, mainButtons[0])) {
			if (returnScriptState() == true) {
				Screen::set(std::make_unique<ScriptList>());
			} else {
				Gui::DisplayWarnMsg(Lang::get("GET_SCRIPTS_FIRST"));
			}
		} else if (touching(touch, mainButtons[1])) {
			if (checkWifiStatus() == true) {
				Screen::set(std::make_unique<ScriptBrowse>());
			} else {
				notConnectedMsg();
			}
		} else if (touching(touch, mainButtons[2])) {
			if (checkWifiStatus() == true) {
				Screen::set(std::make_unique<TinyDB>());
			} else {
				notConnectedMsg();
			}
		} else if (touching(touch, mainButtons[3])) {
			if (isTesting == true) {
				Screen::set(std::make_unique<ScriptCreator>());
			} else {
				notImplemented();
			}
		} else if (touching(touch, mainButtons[4])) {
			Screen::set(std::make_unique<Settings>());
		} else if (touching(touch, mainButtons[5])) {
			if (checkWifiStatus() == true) {
				Screen::set(std::make_unique<FTPScreen>());
			} else {
				notConnectedMsg();
			}
		} else if (touching(touch, mainButtons[6])) {
			exiting = true;
		}
	}
}