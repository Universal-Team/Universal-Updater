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
static const Structs::ButtonPos btn = { 53, 215, 24, 24 };
static const Structs::ButtonPos sshot = { 83, 215, 24, 24 };
static const Structs::ButtonPos notes = { 113, 215, 24, 24 };
extern bool checkWifiStatus();

/*
	Draw the Entry Info part.

	const std::unique_ptr<Store> &store: Const Reference to the Store class.
	const std::unique_ptr<StoreEntry> &entry: Const Reference to the current StoreEntry.
*/
void StoreUtils::DrawEntryInfo(const std::unique_ptr<Store> &store, const std::unique_ptr<StoreEntry> &entry) {
	if (store && entry) { // Ensure, store & entry is not a nullptr.
		Gui::Draw_Rect(48, 0, 272, 36, ENTRY_BAR_COLOR);
		Gui::Draw_Rect(48, 36, 272, 1, ENTRY_BAR_OUTL_COLOR);

		Gui::DrawStringCentered(25, 0, 0.6, TEXT_COLOR, entry->GetTitle(), 265, 0, font);
		Gui::DrawStringCentered(25, 20, 0.4, TEXT_COLOR, entry->GetAuthor(), 265, 0, font);
		Gui::DrawStringCentered(25, 50, 0.4, TEXT_COLOR, entry->GetDescription(), 240, 0, font, C2D_WordWrap);

		Gui::DrawString(61, 130, 0.45, TEXT_COLOR, Lang::get("VERSION") + ": " + entry->GetVersion(), 240, 0, font);
		Gui::DrawString(61, 145, 0.45, TEXT_COLOR, Lang::get("CATEGORY") + ": " + entry->GetCategory(), 240, 0, font);
		Gui::DrawString(61, 160, 0.45, TEXT_COLOR, Lang::get("CONSOLE") + ": " + entry->GetConsole(), 240, 0, font);
		Gui::DrawString(61, 175, 0.45, TEXT_COLOR, Lang::get("LAST_UPDATED") + ": " + entry->GetLastUpdated(), 240, 0, font);
		Gui::DrawString(61, 190, 0.45, TEXT_COLOR, Lang::get("LICENSE") + ": " + entry->GetLicense(), 240, 0, font);

		GFX::DrawBox(btn.x, btn.y, btn.w, btn.h, false);
		if (!entry->GetScreenshots().empty()) GFX::DrawSprite(sprites_screenshot_idx, sshot.x, sshot.y);
		if (entry->GetReleaseNotes() != "") GFX::DrawSprite(sprites_notes_idx, notes.x, notes.y);
		Gui::DrawString(btn.x + 5, btn.y + 2, 0.6f, TEXT_COLOR, "★", 0, 0, font);
	}
}

/*
	The EntryInfo handle.
	Here you can..

	- Go to the download list, by pressing `A`.
	- Show the MarkMenu with START.

	bool &showMark: Reference to showMark.. to show the mark menu.
	bool &fetch: Reference to fetch, so we know, if we need to fetch, when accessing download list.
	bool &sFetch: Reference to the screenshot fetch.
	int &mode: Reference to the Store mode.
	const std::unique_ptr<StoreEntry> &entry: The Store Entry.
*/
void StoreUtils::EntryHandle(bool &showMark, bool &fetch, bool &sFetch, int &mode, const std::unique_ptr<StoreEntry> &entry) {
	if (entry) {
		if ((hDown & KEY_START) || (hDown & KEY_TOUCH && touching(touch, btn))) showMark = true;

		if ((hDown & KEY_Y) || (hDown & KEY_TOUCH && touching(touch, sshot))) {
			if (!entry->GetScreenshots().empty()) {
				if (checkWifiStatus()) {
					sFetch = true;
					mode = 5;
				}
			}
		}

		if ((hDown & KEY_X) || (hDown & KEY_TOUCH && touching(touch, notes))) {
			if (entry->GetReleaseNotes() != "") mode = 6;
		}
	}
}