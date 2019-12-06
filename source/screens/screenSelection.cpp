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
#include "screens/screenSelection.hpp"
#include "screens/scriptBrowse.hpp"
#include "screens/scriptCreator.hpp"
#include "screens/scriptlist.hpp"
#include "screens/settings.hpp"
#include "screens/tinyDB.hpp"

#include "utils/config.hpp"

extern bool exiting;
extern bool touching(touchPosition touch, Structs::ButtonPos button);
extern bool checkWifiStatus(void);
extern int fadealpha;
extern bool fadein;
extern void notImplemented(void);
#define MAX_SCREENS 5

// This is for the Script Creator, so no one can access it for now, until it is stable or so.
bool isTesting = true;

void ScreenSelection::Draw(void) const {
	Gui::DrawTop();
    Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, Lang::get("SCREEN_SELECT"), 400);
	// Draw Boxes for the Screen Selection.
	Gui::Draw_Rect(30, 50, 100, 60, Config::Color1 & C2D_Color32(255, 255, 255, 120));
	Gui::Draw_Rect(150, 50, 100, 60, Config::Color1 & C2D_Color32(255, 255, 255, 120));
	Gui::Draw_Rect(270, 50, 100, 60, Config::Color1 & C2D_Color32(255, 255, 255, 120));
	Gui::Draw_Rect(30, 140, 100, 60, Config::Color1 & C2D_Color32(255, 255, 255, 120));
	Gui::Draw_Rect(150, 140, 100, 60, Config::Color1 & C2D_Color32(255, 255, 255, 120));
	Gui::Draw_Rect(270, 140, 100, 60, Config::Color1 & C2D_Color32(255, 255, 255, 120));

	// Draw Selection.
	if (selectedScreen == 0) {
		Gui::Draw_Rect(30, 50, 100, 60, Config::Color1);
	} else if (selectedScreen == 1) {
		Gui::Draw_Rect(150, 50, 100, 60, Config::Color1);
	} else if (selectedScreen == 2) {
		Gui::Draw_Rect(270, 50, 100, 60, Config::Color1);
	} else if (selectedScreen == 3) {
		Gui::Draw_Rect(30, 140, 100, 60, Config::Color1);
	} else if (selectedScreen == 4) {
		Gui::Draw_Rect(150, 140, 100, 60, Config::Color1);
	} else if (selectedScreen == 5) {
		Gui::Draw_Rect(270, 140, 100, 60, Config::Color1);
	}

	// Draw Text.
	Gui::DrawString((400-Gui::GetStringWidth(0.6f, Lang::get("SCRIPTLIST")))/2-70-50, 70, 0.6f, Config::TxtColor, Lang::get("SCRIPTLIST"), 90);
	Gui::DrawString((400-Gui::GetStringWidth(0.6f, Lang::get("GET_SCRIPTS")))/2-50+50, 70, 0.6f, Config::TxtColor, Lang::get("GET_SCRIPTS"), 90);
	Gui::DrawString((400-Gui::GetStringWidth(0.6f, "TinyDB"))/2+70+50, 70, 0.6f, Config::TxtColor, "TinyDB", 90);

	Gui::DrawString((400-Gui::GetStringWidth(0.6f, Lang::get("SCRIPTCREATOR")))/2-70-50, 160, 0.6f, Config::TxtColor, Lang::get("SCRIPTCREATOR"), 90);
	Gui::DrawString((400-Gui::GetStringWidth(0.6f, Lang::get("SETTINGS")))/2-50+50, 160, 0.6f, Config::TxtColor, Lang::get("SETTINGS"), 90);
	Gui::DrawString((400-Gui::GetStringWidth(0.6f, "FTP"))/2+70+50, 160, 0.6f, Config::TxtColor, "FTP", 90);

    // Draw Version.
	std::string currentVersion = Lang::get("CURRENT_VERSION");
	currentVersion += V_STRING;
	Gui::DrawString(395-Gui::GetStringWidth(0.72f, currentVersion), 218, 0.72f, Config::TxtColor, currentVersion, 400);
    
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(0, 0, 0, fadealpha)); // Fade in out effect

    // Bottom Screen.
	Gui::DrawBottom();
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(0, 0, 0, fadealpha)); // Fade in out effect
}


void ScreenSelection::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_A) {
		if (selectedScreen == 0) {
			Screen::set(std::make_unique<ScriptList>());
		} else if (selectedScreen == 1) {
            if (checkWifiStatus() == true) {
			    Screen::set(std::make_unique<ScriptBrowse>());
            } else {
                notConnectedMsg();
            }
		} else if (selectedScreen == 2) {
            if (checkWifiStatus() == true) {
			    Screen::set(std::make_unique<TinyDB>());
            } else {
                notConnectedMsg();
            }
		} else if (selectedScreen == 3) {
			if (isTesting == true) {
                Screen::set(std::make_unique<ScriptCreator>());
            } else {
                notImplemented();
            }
		} else if (selectedScreen == 4) {
            Screen::set(std::make_unique<Settings>());
        } else if (selectedScreen == 5) {
            if (checkWifiStatus() == true) {
                Screen::set(std::make_unique<FTPScreen>());
            } else {
                notConnectedMsg();
            }
        }
	}

	if (hDown & KEY_RIGHT || hDown & KEY_R) {
		if (selectedScreen < MAX_SCREENS)	selectedScreen++;
	}

	if (hDown & KEY_LEFT || hDown & KEY_L) {
		if (selectedScreen > 0)	selectedScreen--;
	}

    if (hDown & KEY_DOWN) {
        if (selectedScreen == 0 || selectedScreen == 1 || selectedScreen == 2) {
            selectedScreen += 3;
        }
    }

    if (hDown & KEY_UP) {
        if (selectedScreen == 3 || selectedScreen == 4 || selectedScreen == 5) {
            selectedScreen -= 3;
        }
    }

    if (hDown & KEY_START) {
        exiting = true;
    }
}