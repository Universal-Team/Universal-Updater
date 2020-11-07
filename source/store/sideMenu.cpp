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
	{ 0, 0, 48, 48 },
	{ 0, 48, 48, 48 },
	{ 0, 96, 48, 48 },
	{ 0, 144, 48, 48 },
	{ 0, 192, 48, 48 }
};

/*
	Draw the Side Menu part.

	int currentMenu: The current Store Mode / Menu.
*/
void StoreUtils::DrawSideMenu(int currentMenu) {
	for (int i = 0; i < 5; i++) {
		if (i == currentMenu) {
			Gui::Draw_Rect(sidePos[i].x, sidePos[i].y, sidePos[i].w, sidePos[i].h, SIDEBAR_SELECTED_COLOR);

		} else {
			Gui::Draw_Rect(sidePos[i].x, sidePos[i].y, sidePos[i].w, sidePos[i].h, SIDEBAR_UNSELECTED_COLOR);
		}
	}

	GFX::DrawSprite(sprites_info_idx, sidePos[0].x + 4, sidePos[0].y + 4);
	GFX::DrawSprite(sprites_download_idx, sidePos[1].x + 4, sidePos[1].y + 4);
	GFX::DrawSprite(sprites_search_idx, sidePos[2].x + 4, sidePos[2].y + 4);
	GFX::DrawSprite(sprites_sort_idx, sidePos[3].x + 4, sidePos[3].y + 4);
	GFX::DrawSprite(sprites_settings_idx, sidePos[4].x + 4, sidePos[4].y + 4);

	Gui::Draw_Rect(48, 0, 1, 240, BAR_OUTL_COLOR);
}

/*
	Side Menu Handle.
	Here you can..

	- Switch between the Menus through the sidebar.

	int &currentMenu: Reference to the Store Mode / Menu.
	bool &fetch: Reference of the download fetch variable.. so we know, if we need to fetch the download entries.
	int &lastMenu: Reference to the last menu.
*/
void StoreUtils::SideMenuHandle(int &currentMenu, bool &fetch, int &lastMenu) {
	if (hDown & KEY_TOUCH) {
		for (int i = 0; i < 5; i++) {
			if (touching(touch, sidePos[i])) {
				lastMenu = currentMenu;
				if (i == 1) fetch = true; // Fetch download list, if 1.
				currentMenu = i;
				break;
			}
		}
	}

	if (hRepeat & KEY_R) {
		if (currentMenu < 4) {
			lastMenu = currentMenu;
			if (currentMenu + 1 == 1) fetch = true; // Fetch download list, if 1.
			currentMenu++;
		}
	}

	if (hRepeat & KEY_L) {
		if (currentMenu > 0) {
			lastMenu = currentMenu;
			if (currentMenu - 1 == 1) fetch = true; // Fetch download list, if 1.
			currentMenu--;
		}
	}
}