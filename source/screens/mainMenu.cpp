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
#include "screens/scriptlist.hpp"

extern bool exiting;
extern bool touching(touchPosition touch, Structs::ButtonPos button);

void MainMenu::Draw(void) const {
	Gui::DrawTop();
	Gui::DrawStringCentered(0, 2, 0.7f, TextColor, "Universal-Updater", 400);
	Gui::DrawString(395-Gui::GetStringWidth(0.72f, VERSION_STRING), 218, 0.72f, WHITE, VERSION_STRING);
	Gui::DrawBottom();

	// Draw 2 'Buttons'.
	Gui::Draw_Rect(mainButtons[0].x, mainButtons[0].y, mainButtons[0].w, mainButtons[0].h, TopBGColor);
	Gui::DrawString((320-Gui::GetStringWidth(0.6f, Lang::get("SCRIPTLIST")))/2, mainButtons[0].y+10, 0.6f, WHITE, Lang::get("SCRIPTLIST"), 140);
	Gui::Draw_Rect(mainButtons[1].x, mainButtons[1].y, mainButtons[1].w, mainButtons[1].h, TopBGColor);
	Gui::DrawString((320-Gui::GetStringWidth(0.6f, Lang::get("SETTINGS")))/2, mainButtons[1].y+10, 0.6f, WHITE, Lang::get("SETTINGS"), 140);
}

void MainMenu::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (hDown & KEY_START) {
		exiting = true;
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, mainButtons[0])) {
			Gui::setScreen(std::make_unique<ScriptList>());
		}
	}
}