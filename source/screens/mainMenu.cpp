/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

#include "screens/ftpScreen.hpp"
#include "screens/mainMenu.hpp"
#include "screens/scriptlist.hpp"
#include "screens/settings.hpp"
#include "screens/unistore.hpp"

#include "utils/config.hpp"

extern bool exiting;
extern bool touching(touchPosition touch, Structs::ButtonPos button);
extern int fadealpha;
extern bool fadein;

void MainMenu::Draw(void) const {
	Gui::DrawTop();

	if (Config::UseBars == true) {
		Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, "Universal-Updater", 400);
		Gui::DrawString(397-Gui::GetStringWidth(0.5f, V_STRING), 239-Gui::GetStringHeight(0.5f, V_STRING), 0.5f, Config::TxtColor, V_STRING);
	} else {
		Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, "Universal-Updater", 400);
		Gui::DrawString(397-Gui::GetStringWidth(0.5f, V_STRING), 237-Gui::GetStringHeight(0.5f, V_STRING), 0.5f, Config::TxtColor, V_STRING);
	}

	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(0, 0, 0, fadealpha)); // Fade in out effect
	Gui::DrawBottom();
	Gui::DrawArrow(0, 218, 0, 1);

	for (int i = 0; i < 4; i++) {
		Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, Config::UnselectedColor);
		if (Selection == i) {
			Gui::drawAnimatedSelector(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, .060, Config::SelectedColor);
		}
	}

	Gui::DrawStringCentered(-80+10, mainButtons[0].y+12, 0.6f, Config::TxtColor, "UniStore", 130);
	Gui::DrawStringCentered(80, mainButtons[1].y+12, 0.6f, Config::TxtColor, Lang::get("SCRIPTS"), 130);
	Gui::DrawStringCentered(-80, mainButtons[2].y+12, 0.6f, Config::TxtColor, Lang::get("SETTINGS"), 130);
	Gui::DrawStringCentered(80, mainButtons[3].y+12, 0.6f, Config::TxtColor, "FTP", 130);

	// Draw UniStore Icon. ;P
	Gui::sprite(sprites_uniStore_idx, 10, 65);
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(0, 0, 0, fadealpha)); // Fade in out effect
}

void MainMenu::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if ((hDown & KEY_START) || (hDown & KEY_TOUCH && touching(touch, mainButtons[4]))) {
		exiting = true;
	}

	// Navigation.
	if(hDown & KEY_UP) {
		if(Selection > 1)	Selection -= 2;
	} else if(hDown & KEY_DOWN) {
		if(Selection < 3 && Selection != 2 && Selection != 3)	Selection += 2;
	} else if (hDown & KEY_LEFT) {
		if (Selection%2) Selection--;
	} else if (hDown & KEY_RIGHT) {
		if (!(Selection%2)) Selection++;
	}

	if (hDown & KEY_A) {
		switch(Selection) {
			case 0:
				Screen::set(std::make_unique<UniStore>());
				break;
			case 1:
				Screen::set(std::make_unique<ScriptList>());
				break;
			case 2:
				Screen::set(std::make_unique<Settings>());
				break;
			case 3:
				Screen::set(std::make_unique<FTPScreen>());
				break;
		}
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, mainButtons[0])) {
			Screen::set(std::make_unique<UniStore>());
		} else if (touching(touch, mainButtons[1])) {
			Screen::set(std::make_unique<ScriptList>());
		} else if (touching(touch, mainButtons[2])) {
			Screen::set(std::make_unique<Settings>());
		} else if (touching(touch, mainButtons[3])) {
			Screen::set(std::make_unique<FTPScreen>());
		}
	}
}