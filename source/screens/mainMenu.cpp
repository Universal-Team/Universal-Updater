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

#include "screens/ftpScreen.hpp"
#include "screens/mainMenu.hpp"
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

u64 current = 0;
u64 total = 100;

// This is for the Script Creator, so no one can access it for now, until it is stable or so.
bool isTesting = false;

void MainMenu::Draw(void) const {
	if (mode == 0) {
		DrawMainMenu();
	} else if (mode == 1) {
		DisplayTestbar();
	}
}

void MainMenu::DrawMainMenu(void) const {
	Gui::DrawTop();
	Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, "Universal-Updater", 400);
	Gui::DrawString(397-Gui::GetStringWidth(0.5f, V_STRING), 237-Gui::GetStringHeight(0.5f, V_STRING), 0.5f, Config::TxtColor, V_STRING);
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(0, 0, 0, fadealpha)); // Fade in out effect
	Gui::DrawBottom();

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

void MainMenu::DisplayTestbar(void) const {
	Gui::DrawTop();
	Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, "Downloading <Filename> . <Extension>...", 400);
	std::string progressDownload = std::to_string(current);
	progressDownload += " KB / ";
	progressDownload += std::to_string(total);
	progressDownload += " KB downloaded.";
	Gui::DrawStringCentered(0, 80, 0.6f, Config::TxtColor, progressDownload, 400);
	Gui::Draw_Rect(30, 120, 340, 30, BLACK);
	Gui::Draw_Rect(31, 121, (((double)current/(double)total) * 338.0), 28, WHITE);
	Gui::DrawBottom();
}

void MainMenu::ProgressBarLogic(u32 hDown, u32 hHeld) {
	if (keyRepeatDelay)	keyRepeatDelay--;
	if (hHeld & KEY_LEFT && !keyRepeatDelay) {
		if (current > 0) {
			current--;
		} else {
			current = total;
		}
	}

	if (hHeld & KEY_RIGHT && !keyRepeatDelay) {
		if (current < total) {
			current++;
		} else {
			current = 0;
		}
	}

	if (hDown & KEY_SELECT) {
		mode = 0;
	}
}


void MainMenu::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (mode == 1) {
		ProgressBarLogic(hDown, hHeld);
	} else {
		if (hDown & KEY_SELECT) {
			mode = 1;
		}

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
					Gui::setScreen(std::make_unique<ScriptList>());
					break;
				case 1:
					if (checkWifiStatus() == true) {
						Gui::setScreen(std::make_unique<ScriptBrowse>());
					} else {
						notConnectedMsg();
					}
					break;
				case 2:
					if (checkWifiStatus() == true) {
						Gui::setScreen(std::make_unique<TinyDB>());
					} else {
						notConnectedMsg();
					}
					break;
				case 3:
					if (isTesting == true) {
						Gui::setScreen(std::make_unique<ScriptCreator>());
					}
					break;
					case 4:
				Gui::setScreen(std::make_unique<Settings>());
					break;
				case 5:
					if (checkWifiStatus() == true) {
						Gui::setScreen(std::make_unique<FTPScreen>());
					} else {
						notConnectedMsg();
					}
					break;
			}
		}

		if (hDown & KEY_X) {
				if (checkWifiStatus() == true) {
			Gui::setScreen(std::make_unique<FTPScreen>());
			}
		}

		if (hDown & KEY_TOUCH) {
			if (touching(touch, mainButtons[0])) {
				Gui::setScreen(std::make_unique<ScriptList>());
			} else if (touching(touch, mainButtons[1])) {
				if (checkWifiStatus() == true) {
					Gui::setScreen(std::make_unique<ScriptBrowse>());
				} else {
					notConnectedMsg();
				}

	 		} else if (touching(touch, mainButtons[2])) {
				if (checkWifiStatus() == true) {
					Gui::setScreen(std::make_unique<TinyDB>());
				} else {
					notConnectedMsg();
				}
			} else if (touching(touch, mainButtons[3])) {
				if (isTesting == true) {
					Gui::setScreen(std::make_unique<ScriptCreator>());
				}
			} else if (touching(touch, mainButtons[4])) {
				Gui::setScreen(std::make_unique<Settings>());
			} else if (touching(touch, mainButtons[5])) {
				if (checkWifiStatus() == true) {
					Gui::setScreen(std::make_unique<FTPScreen>());
				} else {
					notConnectedMsg();
				}
			}
		}
	}
}