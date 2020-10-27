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
extern bool touching(touchPosition touch, Structs::ButtonPos button);
static const std::vector<Structs::ButtonPos> mainButtons = {
	{ 54, 4, 262, 22 },
	{ 54, 34, 262, 22 },
	{ 54, 64, 262, 22 },
	{ 54, 94, 262, 22 },
	{ 54, 124, 262, 22 },
	{ 54, 154, 262, 22 },
	{ 54, 184, 262, 22 },
	{ 54, 214, 262, 22 }
};

static const std::string autoupdate() { return (config->autoupdate() ? "DISABLE_AUTOUPDATE_UNISTORE" : "ENABLE_AUTOUPDATE_UNISTORE"); };

static const std::vector<std::string> mainStrings = { "LANGUAGE", "SELECT_UNISTORE", "TOGGLE_STYLE", "CREDITS", "EXIT_APP" };

/*
	Main Settings.
*/
static void DrawSettingsMain(const int &selection) {
	for (int i = 0; i < 6; i++) {
		GFX::drawBox(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, i == selection);
	}

	Gui::DrawStringCentered(54 - 160 + (262 / 2), mainButtons[0].y + 4, 0.45f, TEXT_COLOR, Lang::get(mainStrings[0]), 260);
	Gui::DrawStringCentered(54 - 160 + (262 / 2), mainButtons[1].y + 4, 0.45f, TEXT_COLOR, Lang::get(mainStrings[1]), 260);
	Gui::DrawStringCentered(54 - 160 + (262 / 2), mainButtons[2].y + 4, 0.45f, TEXT_COLOR, Lang::get(mainStrings[2]), 260);
	Gui::DrawStringCentered(54 - 160 + (262 / 2), mainButtons[3].y + 4, 0.45f, TEXT_COLOR, Lang::get(autoupdate()), 260);
	Gui::DrawStringCentered(54 - 160 + (262 / 2), mainButtons[4].y + 4, 0.45f, TEXT_COLOR, Lang::get(mainStrings[3]), 260);
	Gui::DrawStringCentered(54 - 160 + (262 / 2), mainButtons[5].y + 4, 0.45f, TEXT_COLOR, Lang::get(mainStrings[4]), 260);
}

static void SettingsHandleMain(u32 hDown, u32 hHeld, touchPosition touch, int &page, bool &dspSettings, int &storeMode, int &selection, std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, std::unique_ptr<Meta> &meta) {
	u32 hRepeat = hidKeysDownRepeat();

	if (hDown & KEY_B) {
		selection = 0;
		storeMode = 0;
	}

	if (hRepeat & KEY_DOWN) {
		if (selection < 5) selection++;
	}

	if (hRepeat & KEY_UP) {
		if (selection > 0) selection--;
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, mainButtons[0])) {
			Overlays::SelectLanguage();

		} else if (touching(touch, mainButtons[1])) {
			Overlays::SelectStore(store, entries, meta);

		} else if (touching(touch, mainButtons[2])) {
			config->list(!config->list());
			store->SetEntry(0);
			store->SetScreenIndx(0);
			store->SetBox(0);

		} else if (touching(touch, mainButtons[3])) {
			config->autoupdate(!config->autoupdate());

		} else if (touching(touch, mainButtons[4])) {
			Overlays::ShowCredits();

		} else if (touching(touch, mainButtons[5])) {
			exiting = true;
		}
	}

	if (hDown & KEY_A) {
		switch(selection) {
			case 0:
				Overlays::SelectLanguage();
				break;

			case 1:
				Overlays::SelectStore(store, entries, meta);
				break;

			case 2:
				config->list(!config->list());
				store->SetEntry(0);
				store->SetScreenIndx(0);
				store->SetBox(0);
				break;

			case 3:
				config->autoupdate(!config->autoupdate());
				break;

			case 4:
				Overlays::ShowCredits();
				break;

			case 5:
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