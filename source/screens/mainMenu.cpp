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

#include "config.hpp"
#include "credits.hpp"
#include "mainMenu.hpp"
#include "scriptHelper.hpp"
#include "scriptlist.hpp"
#include "settings.hpp"
#include "unistore.hpp"

extern std::unique_ptr<Config> config;
extern bool exiting;
extern bool touching(touchPosition touch, Structs::ButtonPos button);
extern int fadealpha;
extern bool fadein;
extern u32 TextColor;

void MainMenu::Draw(void) const {
	GFX::DrawTop();
	Gui::DrawStringCentered(0, config->useBars() ? 0 : 2, 0.7f, config->textColor(), "Universal-Updater", 400);
	Gui::DrawString(397-Gui::GetStringWidth(0.5f, V_STRING), (config->useBars() ? 239 : 237)-Gui::GetStringHeight(0.5f, V_STRING), 0.5f, config->textColor(), V_STRING);
	GFX::DrawSprite(sprites_universal_updater_idx, 110, 30);
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	GFX::DrawBottom();
	GFX::DrawArrow(0, 218, 0, 1);

	GFX::DrawButton(mainButtons[0].x, mainButtons[0].y, "UniStore");
	GFX::DrawButton(mainButtons[1].x, mainButtons[1].y, Lang::get("SCRIPTS"));
	GFX::DrawButton(mainButtons[2].x, mainButtons[2].y, Lang::get("SETTINGS"));
	GFX::DrawButton(mainButtons[3].x, mainButtons[3].y, Lang::get("CREDITS"));
	// Selector.
	Animation::Button(mainButtons[Selection].x, mainButtons[Selection].y, .060);

	// Draw UniStore Icon. ;P
	//GFX::DrawSprite(sprites_uniStore_idx, 10, 65);
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
}

void MainMenu::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if ((hDown & KEY_START) || (hDown & KEY_TOUCH && touching(touch, mainButtons[4]))) {
		fadeout = true;
		fadecolor = 0;
		exiting = true;
	}

	// Navigation.
	if (hDown & KEY_UP) {
		if (Selection > 1)	Selection -= 2;
	} else if (hDown & KEY_DOWN) {
		if (Selection < 3 && Selection != 2 && Selection != 3)	Selection += 2;
	} else if (hDown & KEY_LEFT) {
		if (Selection%2) Selection--;
	} else if (hDown & KEY_RIGHT) {
		if (!(Selection%2)) Selection++;
	}

	if (hDown & KEY_A) {
		switch(Selection) {
			case 0:
				Gui::setScreen(std::make_unique<UniStore>(false, "NOT_USED"), config->screenFade(), true);
				break;
			case 1:
				Gui::setScreen(std::make_unique<ScriptList>(), config->screenFade(), true);
				break;
			case 2:
				Gui::setScreen(std::make_unique<Settings>(), config->screenFade(), true);
				break;
			case 3:
				Gui::setScreen(std::make_unique<Credits>(), config->screenFade(), true);
				break;
		}
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, mainButtons[0])) {
			Gui::setScreen(std::make_unique<UniStore>(false, "NOT_USED"), config->screenFade(), true);
		} else if (touching(touch, mainButtons[1])) {
			Gui::setScreen(std::make_unique<ScriptList>(), config->screenFade(), true);
		} else if (touching(touch, mainButtons[2])) {
			Gui::setScreen(std::make_unique<Settings>(), config->screenFade(), true);
		} else if (touching(touch, mainButtons[3])) {
			Gui::setScreen(std::make_unique<Credits>(), config->screenFade(), true);
		}
	}
}