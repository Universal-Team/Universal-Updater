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

static const std::vector<std::string> mainStrings = { "LANGUAGE", "SELECT_UNISTORE", "TOGGLE_STYLE", "CREDITS", "CHANGE_DIRECTORIES", "EXIT_APP" };
static const std::vector<std::string> dirStrings = { "CHANGE_3DSX_PATH", "CHANGE_NDS_PATH", "CHANGE_ARCHIVE_PATH" };

/*
	Main Settings.

	const int &selection: Const Reference to the Settings Selection.
*/
static void DrawSettingsMain(const int &selection) {
	for (int i = 0; i < 7; i++) {
		GFX::drawBox(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, i == selection);
	}

	Gui::DrawStringCentered(54 - 160 + (262 / 2), mainButtons[0].y + 4, 0.45f, TEXT_COLOR, Lang::get(mainStrings[0]), 260);
	Gui::DrawStringCentered(54 - 160 + (262 / 2), mainButtons[1].y + 4, 0.45f, TEXT_COLOR, Lang::get(mainStrings[1]), 260);
	Gui::DrawStringCentered(54 - 160 + (262 / 2), mainButtons[2].y + 4, 0.45f, TEXT_COLOR, Lang::get(mainStrings[2]), 260);
	Gui::DrawStringCentered(54 - 160 + (262 / 2), mainButtons[3].y + 4, 0.45f, TEXT_COLOR, Lang::get(autoupdate()), 260);
	Gui::DrawStringCentered(54 - 160 + (262 / 2), mainButtons[4].y + 4, 0.45f, TEXT_COLOR, Lang::get(mainStrings[3]), 260);
	Gui::DrawStringCentered(54 - 160 + (262 / 2), mainButtons[5].y + 4, 0.45f, TEXT_COLOR, Lang::get(mainStrings[4]), 260);
	Gui::DrawStringCentered(54 - 160 + (262 / 2), mainButtons[6].y + 4, 0.45f, TEXT_COLOR, Lang::get(mainStrings[5]), 260);
}

/*
	Directory Change Draw.

	const int &selection: Const Reference to the Settings Selection.
*/
static void DrawSettingsDir(const int &selection) {
	for (int i = 0; i < 3; i++) {
		GFX::DrawButton(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, i == selection, Lang::get(dirStrings[i]));
	}
}

/*
	Settings Main Handle.
	Here you can..

	- Change the Language.
	- Access the UniStore Manage Handle.
	- Change the Top Grid / List style.
	- Enable UniStore auto update on boot.
	- Show the Credits.
	- Exit Universal-Updater.

	u32 hDown: The hidKeysDown() variable.
	u32 hHeld: The hidKeysHeld() variable.
	touchPosition touch: The TouchPosition variable.
	int &page: Reference to the page.
	bool &dspSettings: Reference to the display Settings.
	int &storeMode: Reference to the Store Mode.
	int &selection: Reference to the Selection.
	std::unique_ptr<Store> &store: Reference to the Store class.
	std::vector<std::unique_ptr<StoreEntry>> &entries: Reference to the StoreEntries.
	std::unique_ptr<Meta> &meta: Reference to the Meta class.
*/
static void SettingsHandleMain(u32 hDown, u32 hHeld, touchPosition touch, int &page, bool &dspSettings, int &storeMode, int &selection, std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, std::unique_ptr<Meta> &meta) {
	u32 hRepeat = hidKeysDownRepeat();

	if (hDown & KEY_B) {
		selection = 0;
		storeMode = 0;
	}

	if (hRepeat & KEY_DOWN) {
		if (selection < 6) selection++;
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
			selection = 0;
			page = 1;

		} else if (touching(touch, mainButtons[6])) {
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
				selection = 0;
				page = 1;
				break;

			case 6:
				exiting = true;
				break;
		}
	}
}

/*
	Directory Handle.
	Here you can..

	- Change the Directory of...
		- 3DSX, NDS & Archives.

	u32 hDown: The hidKeysDown() variable.
	u32 hHeld: The hidKeysHeld() variable.
	touchPosition touch: The TouchPosition variable.
	int &page: Reference to the page.
	int &selection: Reference to the Selection.
*/
static void SettingsHandleDir(u32 hDown, u32 hHeld, touchPosition touch, int &page, int &selection) {
	u32 hRepeat = hidKeysDownRepeat();

	if (hDown & KEY_B) {
		page = 0;
		selection = 5;
	}

	if (hRepeat & KEY_DOWN) {
		if (selection < 2) selection++;
	}

	if (hRepeat & KEY_UP) {
		if (selection > 0) selection--;
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, mainButtons[0])) {
			const std::string path = Overlays::SelectDir(config->_3dsxPath(), Lang::get("SELECT_DIR"));
			if (path != "") config->_3dsxPath(path);

		} else if (touching(touch, mainButtons[1])) {
			const std::string path = Overlays::SelectDir(config->ndsPath(), Lang::get("SELECT_DIR"));
			if (path != "") config->ndsPath(path);

		} else if (touching(touch, mainButtons[2])) {
			const std::string path = Overlays::SelectDir(config->archPath(), Lang::get("SELECT_DIR"));
			if (path != "") config->archPath(path);
		}
	}

	if (hDown & KEY_A) {
		std::string path = "";

		switch(selection) {
			case 0:
				path = Overlays::SelectDir(config->_3dsxPath(), Lang::get("SELECT_DIR"));
				if (path != "") config->_3dsxPath(path);
				break;

			case 1:
				path = Overlays::SelectDir(config->ndsPath(), Lang::get("SELECT_DIR"));
				if (path != "") config->ndsPath(path);
				break;

			case 2:
				path = Overlays::SelectDir(config->archPath(), Lang::get("SELECT_DIR"));
				if (path != "") config->archPath(path);
				break;
		}
	}
}

/*
	Draw the Settings.

	const int &page: Const Reference to the page.
	const int &selection: Const Reference to the selection.
*/
void StoreUtils::DrawSettings(const int &page, const int &selection) {
	switch(page) {
		case 0:
			DrawSettingsMain(selection);
			break;

		case 1:
			DrawSettingsDir(selection);
			break;
	}
}

/*
	Settings Handle.

	u32 hDown: The hidKeysDown() variable.
	u32 hHeld: The hidKeysHeld() variable.
	touchPosition touch: The TouchPosition variable.
	int &page: Reference to the page.
	bool &dspSettings: Reference to the display Settings.
	int &storeMode: Reference to the Store Mode.
	int &selection: Reference to the Selection.
	std::unique_ptr<Store> &store: Reference to the Store class.
	std::vector<std::unique_ptr<StoreEntry>> &entries: Reference to the StoreEntries.
	std::unique_ptr<Meta> &meta: Reference to the Meta class.
*/
void StoreUtils::SettingsHandle(u32 hDown, u32 hHeld, touchPosition touch, int &page, bool &dspSettings, int &storeMode, int &selection, std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, std::unique_ptr<Meta> &meta) {
	switch(page) {
		case 0:
			SettingsHandleMain(hDown, hHeld, touch, page, dspSettings, storeMode, selection, store, entries, meta);
			break;

		case 1:
			SettingsHandleDir(hDown, hHeld, touch, page, selection);
			break;
	}
}