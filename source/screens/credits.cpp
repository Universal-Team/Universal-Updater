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

#include "screens/credits.hpp"

#include "utils/config.hpp"

extern bool touching(touchPosition touch, Structs::ButtonPos button);

void Credits::Draw(void) const {
	std::string title = "Universal-Updater - ";
	title += Lang::get("CREDITS");
	Gui::DrawTop();
	if (creditsPage != 4) {
		if (Config::UseBars == true) {
			Gui::DrawStringCentered(0, 0, 0.7f, Config::TxtColor, title, 400);
		} else {
			Gui::DrawStringCentered(0, 2, 0.7f, Config::TxtColor, title, 400);
		}
		Gui::DrawStringCentered(0, 40, 0.8f, Config::TxtColor, Lang::get("DEVELOPED_BY"), 400);
		Gui::DrawStringCentered(0, 70, 0.8f, Config::TxtColor, Lang::get("MAIN_DEV"), 400);
		Gui::sprite(sprites_stackZ_idx, 150, 95);
		std::string currentVersion = Lang::get("CURRENT_VERSION");
		currentVersion += V_STRING;
		Gui::DrawString(395-Gui::GetStringWidth(0.70f, currentVersion), 219, 0.70f, Config::TxtColor, currentVersion, 400);
	} else {
		Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(0, 0, 0, 190));
		Gui::sprite(sprites_discord_idx, 115, 35);
	}

	Gui::DrawBottom();

	if (creditsPage != 4) {
		Gui::DrawArrow(0, 218, 0, 1);
		Gui::DrawArrow(318, 240, 180.0, 1);
	}

	if (creditsPage == 1) {
		Gui::DrawStringCentered(0, -2, 0.7f, Config::TxtColor, Lang::get("TRANSLATORS"), 320);
		Gui::DrawString(5, 30, 0.6f, Config::TxtColor, "- _mapple²\n- antoine62\n- Chips\n- David Pires\n- Flame\n- lemonnade0\n- Pk11\n- Roby Spia\n- StackZ\n- YoSoy");
		Gui::DrawString(180, 30, 0.6f, Config::TxtColor, "Русский\nFrançais\nPortuguês\nPortuguês\nBruh\nLietuvių\n日本語\nItaliano\nDeutsch, English\nEspañol");
	} else if (creditsPage == 2) {
		Gui::DrawStringCentered(0, -2, 0.7f, Config::TxtColor, "Universal-Team", 320);
		Gui::DrawStringCentered(0, 35, 0.7f, Config::TxtColor, "DeadPhoenix");
		Gui::DrawStringCentered(0, 65, 0.7f, Config::TxtColor, "Flame");
		Gui::DrawStringCentered(0, 95, 0.7f, Config::TxtColor, "Pk11");
		Gui::DrawStringCentered(0, 125, 0.7f, Config::TxtColor, "RocketRobz");
		Gui::DrawStringCentered(0, 155, 0.7f, Config::TxtColor, "StackZ");
		Gui::DrawStringCentered(0, 185, 0.7f, Config::TxtColor, "TotallyNotGuy");
	} else if (creditsPage == 3) {
		Gui::DrawStringCentered(0, -2, 0.7f, Config::TxtColor, Lang::get("SCRIPTCREATORS"), 320);
		Gui::DrawString(5, 27, 0.55f, Config::TxtColor, "- DualBladedKirito\n\n- Glazed_Belmont\n\n- Pk11\n\n- StackZ\n\n- The Conceptionist\n\n- YoSoy");
		Gui::DrawString(180, 27, 0.55f, Config::TxtColor, "1\n\n1\n\n1\n\n5\n\n10\n\n1/2");
	} else if (creditsPage == 4) {
		Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(0, 0, 0, 190));
		Gui::DrawStringCentered(0, -2, 0.55f, Config::TxtColor, Lang::get("LINK"), 320);
		Gui::DrawArrow(0, 218, 0, 1);
	}
}

void Credits::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if ((hDown & KEY_LEFT || hDown & KEY_L) || (hDown & KEY_TOUCH && touching(touch, arrowPos[0]))) {
		if (creditsPage == 1) {
			Screen::back();
			return;
		} else if (creditsPage > 1) {
			creditsPage--;
		}
	}
		
	if ((hDown & KEY_R || hDown & KEY_RIGHT) || (hDown & KEY_TOUCH && touching(touch, arrowPos[1]))) {
		if (creditsPage < 4)	creditsPage++;
	}

	if (hDown & KEY_B) {
		Screen::back();
		return;
	}
}