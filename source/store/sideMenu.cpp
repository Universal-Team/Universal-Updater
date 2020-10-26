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

#include "storeUtils.hpp"
#include "structs.hpp"

extern bool touching(touchPosition touch, Structs::ButtonPos button);
static const std::vector<Structs::ButtonPos> sidePos = {
	{0, 0, 48, 48},
	{0, 48, 48, 48},
	{0, 96, 48, 48},
	{0, 144, 48, 48},
	{0, 192, 48, 48}
};

/*
	Draw the Side Menu part.
*/
void StoreUtils::DrawSideMenu(const int &currentMenu) {
	for (int i = 0; i < 5; i++) {
		if (i == currentMenu) {
			Gui::Draw_Rect(sidePos[i].x, sidePos[i].y, sidePos[i].w, sidePos[i].h, C2D_Color32(108, 130, 155, 255));

		} else {
			Gui::Draw_Rect(sidePos[i].x, sidePos[i].y, sidePos[i].w, sidePos[i].h, C2D_Color32(77, 101, 128, 255));
		}
	}

	Gui::Draw_Rect(48, 0, 1, 240, C2D_Color32(25, 30, 53, 255));
}

/*
	Side Menu Handle.
*/
void StoreUtils::SideMenuHandle(u32 hDown, u32 hHeld, touchPosition touch, int &currentMenu, bool &fetch) {
	if (hDown & KEY_TOUCH) {
		for (int i = 0; i < 5; i++) {
			if (touching(touch, sidePos[i])) {
				if (i == 1) fetch = true; // Fetch download list, if 1.
				currentMenu = i;
				break;
			}
		}
	}

	if (hDown & KEY_R) {
		if (currentMenu < 4) {
			if (currentMenu + 1 == 1) fetch = true;
			currentMenu++;
		}
	}

	if (hDown & KEY_L) {
		if (currentMenu > 0) {
			if (currentMenu - 1 == 1) fetch = true;
			currentMenu--;
		}
	}
}