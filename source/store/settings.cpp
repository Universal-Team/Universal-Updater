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
	{ 54, 32, 262, 22 },
	{ 54, 62, 262, 22 },
	{ 54, 92, 262, 22 },
	{ 54, 122, 262, 22 },
	{ 54, 152, 262, 22 },
	{ 54, 182, 262, 22 },
	{ 54, 212, 262, 22 }
};

static const std::vector<Structs::ButtonPos> toggleAbles = {
	{ 52, 6, 24, 24 }, // Back arrow.
	{ 288, 64, 24, 24 },
	{ 288, 140, 24, 24 }
};

static const Structs::ButtonPos back = { 52, 0, 24, 24 }; // Back arrow for directory.


static const std::vector<std::string> mainStrings = { "LANGUAGE", "SELECT_UNISTORE", "AUTO_UPDATE_SETTINGS_BTN", "GUI_SETTINGS_BTN", "DIRECTORY_SETTINGS_BTN", "CREDITS", "EXIT_APP" };
static const std::vector<std::string> dirStrings = { "CHANGE_3DSX_PATH", "CHANGE_NDS_PATH", "CHANGE_ARCHIVE_PATH" };

/*
	Main Settings.

	const int &selection: Const Reference to the Settings Selection.
*/
static void DrawSettingsMain(const int &selection) {
	Gui::Draw_Rect(48, 0, 272, 25, ENTRY_BAR_COLOR);
	Gui::Draw_Rect(48, 25, 272, 1, ENTRY_BAR_OUTL_COLOR);
	Gui::DrawStringCentered(25, 2, 0.6, TEXT_COLOR, Lang::get("SETTINGS"), 265);

	for (int i = 0; i < 7; i++) {
		if (i == selection) GFX::DrawBox(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, false);
		Gui::DrawStringCentered(30, mainButtons[i].y + 4, 0.45f, TEXT_COLOR, Lang::get(mainStrings[i]), 255);
	}
}

/*
	Directory Change Draw.

	const int &selection: Const Reference to the Settings Selection.
*/
static void DrawSettingsDir(const int &selection) {
	Gui::Draw_Rect(48, 0, 272, 25, ENTRY_BAR_COLOR);
	Gui::Draw_Rect(48, 25, 272, 1, ENTRY_BAR_OUTL_COLOR);
	GFX::DrawSprite(sprites_arrow_idx, back.x, back.y);
	Gui::DrawStringCentered(32, 2, 0.6, TEXT_COLOR, Lang::get("DIRECTORY_SETTINGS"), 240);

	for (int i = 0; i < 3; i++) {
		if (i == selection) GFX::DrawBox(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, false);
		Gui::DrawStringCentered(30, mainButtons[i].y + 4, 0.45f, TEXT_COLOR, Lang::get(dirStrings[i]), 255);
	}
}

/*
	Draw Auto-Update Settings page.
*/
static void DrawAutoUpdate(const int &selection) {
	Gui::Draw_Rect(48, 0, 272, 36, ENTRY_BAR_COLOR);
	Gui::Draw_Rect(48, 36, 272, 1, ENTRY_BAR_OUTL_COLOR);
	GFX::DrawSprite(sprites_arrow_idx, 52, 6);

	Gui::DrawStringCentered(32, 7, 0.6, TEXT_COLOR, Lang::get("AUTO_UPDATE_SETTINGS"), 240);

	/* Toggle Boxes. */
	Gui::Draw_Rect(48, 64, 273, 24, (selection == 0 ? SIDEBAR_UNSELECTED_COLOR : BOX_INSIDE_COLOR));
	Gui::DrawString(55, 68, 0.5f, TEXT_COLOR, Lang::get("AUTO_UPDATE_UNISTORE"), 210);
	GFX::DrawToggle(288, 64, config->autoupdate());
	Gui::DrawString(55, 95, 0.4f, TEXT_COLOR, Lang::get("AUTO_UPDATE_UNISTORE_DESC"), 265, 0, nullptr, C2D_WordWrap);

	Gui::Draw_Rect(48, 140, 273, 24, (selection == 1 ? SIDEBAR_UNSELECTED_COLOR : BOX_INSIDE_COLOR));
	Gui::DrawString(55, 144, 0.5f, TEXT_COLOR, Lang::get("AUTO_UPDATE_UU"), 210);
	GFX::DrawToggle(288, 140, config->updatecheck());
	Gui::DrawString(55, 171, 0.4f, TEXT_COLOR, Lang::get("AUTO_UPDATE_UU_DESC"), 265, 0, nullptr, C2D_WordWrap);
}

static void DrawGUISettings(const int &selection) {
	Gui::Draw_Rect(48, 0, 272, 36, ENTRY_BAR_COLOR);
	Gui::Draw_Rect(48, 36, 272, 1, ENTRY_BAR_OUTL_COLOR);
	GFX::DrawSprite(sprites_arrow_idx, 52, 6);

	Gui::DrawStringCentered(32, 7, 0.6, TEXT_COLOR, Lang::get("GUI_SETTINGS"), 240);

	Gui::Draw_Rect(48, 64, 273, 24, (selection == 0 ? SIDEBAR_UNSELECTED_COLOR : BOX_INSIDE_COLOR));
	Gui::DrawString(55, 68, 0.5f, TEXT_COLOR, Lang::get("UNISTORE_BG"), 210);
	GFX::DrawToggle(288, 64, config->usebg());
	Gui::DrawString(55, 95, 0.4f, TEXT_COLOR, Lang::get("UNISTORE_BG_DESC"), 265, 0, nullptr, C2D_WordWrap);
}


/*
	Settings Main Handle.
	Here you can..

	- Change the Language.
	- Access the UniStore Manage Handle.
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
	if (hDown & KEY_B) {
		selection = 0;
		storeMode = 0;
	}

	if (hRepeat & KEY_DOWN) {
		if (selection < 6) selection++;
		else selection = 0;
	}

	if (hRepeat & KEY_UP) {
		if (selection > 0) selection--;
		else selection = mainStrings.size() - 1;
	}

	if (hRepeat & KEY_RIGHT) {
		if (selection + 8 < (int)mainStrings.size()) selection += 8;
		else selection = mainStrings.size() - 1;
	}

	if (hRepeat & KEY_LEFT) {
		if (selection - 8 > 0) selection -= 8;
		else selection = 0;
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, mainButtons[0])) {
			Overlays::SelectLanguage(store);

		} else if (touching(touch, mainButtons[1])) {
			Overlays::SelectStore(store, entries, meta);

		} else if (touching(touch, mainButtons[2])) {
			selection = 0;
			page = 2;

		} else if (touching(touch, mainButtons[3])) {
			selection = 0;
			page = 3;

		} else if (touching(touch, mainButtons[4])) {
			selection = 0;
			page = 1;

 		} else if (touching(touch, mainButtons[5])) {
			Overlays::ShowCredits();

		} else if (touching(touch, mainButtons[6])) {
			exiting = true;
		}
	}

	if (hDown & KEY_A) {
		switch(selection) {
			case 0:
				Overlays::SelectLanguage(store);
				break;

			case 1:
				Overlays::SelectStore(store, entries, meta);
				break;

			case 2:
				selection = 0;
				page = 2;
				break;

			case 3:
				selection = 0;
				page = 3;
				break;

			case 4:
				selection = 0;
				page = 1;
				break;

			case 5:
				Overlays::ShowCredits();
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
static void SettingsHandleDir(u32 hDown, u32 hHeld, touchPosition touch, int &page, int &selection, const std::unique_ptr<Store> &store) {
	if (hDown & KEY_B) {
		page = 0;
		selection = 4;
	}

	if (hRepeat & KEY_DOWN) {
		if (selection < 2) selection++;
		else selection = 0;
	}

	if (hRepeat & KEY_UP) {
		if (selection > 0) selection--;
		else selection = dirStrings.size()-1;
	}

	if (hRepeat & KEY_RIGHT) {
		if (selection + 8 < (int)dirStrings.size()-1) selection += 8;
		else selection = dirStrings.size()-1;
	}

	if (hRepeat & KEY_LEFT) {
		if (selection - 8 > 0) selection -= 8;
		else selection = 0;
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, back)) {
			page = 0;
			selection = 4;

		} else if (touching(touch, mainButtons[0])) {
			const std::string path = Overlays::SelectDir(config->_3dsxPath(), Lang::get("SELECT_DIR"), store);
			if (path != "") config->_3dsxPath(path);

		} else if (touching(touch, mainButtons[1])) {
			const std::string path = Overlays::SelectDir(config->ndsPath(), Lang::get("SELECT_DIR"), store);
			if (path != "") config->ndsPath(path);

		} else if (touching(touch, mainButtons[2])) {
			const std::string path = Overlays::SelectDir(config->archPath(), Lang::get("SELECT_DIR"), store);
			if (path != "") config->archPath(path);
		}
	}

	if (hDown & KEY_A) {
		std::string path = "";

		switch(selection) {
			case 0:
				path = Overlays::SelectDir(config->_3dsxPath(), Lang::get("SELECT_DIR"), store);
				if (path != "") config->_3dsxPath(path);
				break;

			case 1:
				path = Overlays::SelectDir(config->ndsPath(), Lang::get("SELECT_DIR"), store);
				if (path != "") config->ndsPath(path);
				break;

			case 2:
				path = Overlays::SelectDir(config->archPath(), Lang::get("SELECT_DIR"), store);
				if (path != "") config->archPath(path);
				break;
		}
	}
}

static void AutoUpdateLogic(u32 hDown, u32 hHeld, touchPosition touch, int &page, int &selection) {
	if (hDown & KEY_B) {
		page = 0;
		selection = 2;
	}

	if (hRepeat & KEY_DOWN) {
		if (selection < 1) selection++;
	}

	if (hRepeat & KEY_UP) {
		if (selection > 0) selection--;
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, toggleAbles[0])) {
			page = 0;
			selection = 2;

		} else if (touching(touch, toggleAbles[1])) {
			config->autoupdate(!config->autoupdate());

		} else if (touching(touch, toggleAbles[2])) {
			config->updatecheck(!config->updatecheck());
		}
	}

	if (hDown & KEY_A) {
		switch(selection) {
			case 0:
				config->autoupdate(!config->autoupdate());
				break;

			case 1:
				config->updatecheck(!config->updatecheck());
				break;
		}
	}
}

static void GUISettingsLogic(u32 hDown, u32 hHeld, touchPosition touch, int &page, int &selection, const std::unique_ptr<Store> &store) {
	if (hDown & KEY_B) {
		page = 0;
		selection = 3;
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, toggleAbles[0])) {
			page = 0;
			selection = 3;

		} else if (touching(touch, toggleAbles[1])) {
			config->usebg(!config->usebg());
		}
	}

	if (hDown & KEY_A) {
		switch(selection) {
			case 0:
				config->usebg(!config->usebg());
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

		case 2:
			DrawAutoUpdate(selection);
			break;

		case 3:
			DrawGUISettings(selection);
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
			SettingsHandleDir(hDown, hHeld, touch, page, selection, store);
			break;

		case 2:
			AutoUpdateLogic(hDown, hHeld, touch, page, selection);
			break;

		case 3:
			GUISettingsLogic(hDown, hHeld, touch, page, selection, store);
			break;
	}
}