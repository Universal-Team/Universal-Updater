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

#include "screens/mainMenu.hpp"
#include "screens/settings.hpp"
#include "screens/scriptlist.hpp"

#include "utils/config.hpp"

extern int mode;
extern bool exiting;
extern bool touching(touchPosition touch, Structs::ButtonPos button);

void MainMenu::Draw(void) const {
	Gui::DrawTop();
	Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, "Universal-Updater", 400);
	Gui::DrawString(395-Gui::GetStringWidth(0.72f, VERSION_STRING), 218, 0.72f, Config::TxtColor, VERSION_STRING);
	Gui::DrawBottom();

	for (int i = 0; i < 3; i++) {
		if (Selection == i) {
			Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, Config::SelectedColor);
		} else {
			Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, Config::UnselectedColor);
		}
	}

	Gui::DrawString((320-Gui::GetStringWidth(0.6f, Lang::get("SCRIPTLIST")))/2, mainButtons[0].y+10, 0.6f, Config::TxtColor, Lang::get("SCRIPTLIST"), 140);
	Gui::DrawString((320-Gui::GetStringWidth(0.6f, Lang::get("LANGUAGE")))/2, mainButtons[1].y+10, 0.6f, Config::TxtColor, Lang::get("LANGUAGE"), 140);
	Gui::DrawString((320-Gui::GetStringWidth(0.6f, Lang::get("COLORS")))/2, mainButtons[2].y+10, 0.6f, Config::TxtColor, Lang::get("COLORS"), 140);
}

void MainMenu::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_START) {
		exiting = true;
	}

	if (hDown & KEY_UP) {
		if(Selection > 0)	Selection--;
	} else if (hDown & KEY_DOWN) {
		if(Selection < 2)	Selection++;
	}

	if (hDown & KEY_A) {
		switch(Selection) {
			case 0:
				Gui::setScreen(std::make_unique<ScriptList>());
				break;
			case 1:
				mode = 0;
				Gui::setScreen(std::make_unique<Settings>());
				break;
			case 2:
				mode = 1;
				Gui::setScreen(std::make_unique<Settings>());
				break;
		}
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, mainButtons[0])) {
			Gui::setScreen(std::make_unique<ScriptList>());
		} else if (touching(touch, mainButtons[1])) {
			mode = 0;
			Gui::setScreen(std::make_unique<Settings>());
		} else if (touching(touch, mainButtons[2])) {
			mode = 1;
			Gui::setScreen(std::make_unique<Settings>());
		}
	}
}