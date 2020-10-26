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

#include "overlay.hpp"
#include "storeUtils.hpp"

extern bool exiting;
static const std::vector<Structs::ButtonPos> mainButtons = {
	{54, 6, 262, 22},
	{54, 36, 262, 22},
	{54, 66, 262, 22},
	{54, 96, 262, 22},
	{54, 126, 262, 22},
	{54, 156, 262, 22},
	{54, 186, 262, 22},
	{54, 216, 262, 22}
};
static const std::vector<std::string> mainStrings = { "LANGUAGE", "THEME", "SELECT_UNISTORE", "EXIT_APP" };

/*
	Main Settings.
*/
static void DrawSettingsMain(const int &selection) {
	for (int i = 0; i < 4; i++) {
		GFX::drawBox(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, i == selection);
		Gui::DrawStringCentered(27, mainButtons[i].y + 4, 0.45f, C2D_Color32(255, 255, 255, 255), Lang::get(mainStrings[i]), 255);
	}
}

static void SettingsHandleMain(u32 hDown, u32 hHeld, touchPosition touch, int &page, bool &dspSettings, int &storeMode, int &selection, std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, std::unique_ptr<Meta> &meta) {
	if (hDown & KEY_B) {
		selection = 0;
		storeMode = 0;
	}

	if (hDown & KEY_DOWN) {
		if (selection < 3) selection++;
	}

	if (hDown & KEY_UP) {
		if (selection > 0) selection--;
	}

	if (hDown & KEY_A) {
		switch(selection) {
			case 0:
				Overlays::SelectLanguage();
				break;

			case 1:
				break; // Theme.

			case 2:
				Overlays::SelectStore(store, entries, meta);
				break;

			case 3:
				exiting = true;
				break;
		}
	}
}

/*
	Draw the Settings.
*/
void StoreUtils::DrawSettings(const int &page, const int &selection) {
	switch(page) {
		case 0:
			DrawSettingsMain(selection);
			break;
	}
}

/*
	Settings Handle.
*/
void StoreUtils::SettingsHandle(u32 hDown, u32 hHeld, touchPosition touch, int &page, bool &dspSettings, int &storeMode, int &selection, std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, std::unique_ptr<Meta> &meta) {
	switch(page) {
		case 0:
			SettingsHandleMain(hDown, hHeld, touch, page, dspSettings, storeMode, selection, store, entries, meta);
			break;
	}
}