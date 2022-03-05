/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2021 Universal-Team
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

#include "common.hpp"
#include "files.hpp"
#include "storeUtils.hpp"
#include "structs.hpp"

extern bool touching(touchPosition touch, Structs::ButtonPos button);
static const Structs::ButtonPos btn = { 45, 215, 24, 24 };
static const Structs::ButtonPos sshot = { 75, 215, 24, 24 };
static const Structs::ButtonPos notes = { 105, 215, 24, 24 };
extern bool checkWifiStatus();
extern bool exiting, QueueRuns;

/*
	Draw the Entry Info part.

	const std::unique_ptr<StoreEntry> &entry: Const Reference to the current StoreEntry.
*/
void StoreUtils::DrawEntryInfo(const std::unique_ptr<StoreEntry> &entry) {
	if (StoreUtils::store && entry) { // Ensure, store & entry is not a nullptr.
		Gui::Draw_Rect(40, 0, 280, 36, UIThemes->EntryBar());
		Gui::Draw_Rect(40, 36, 280, 1, UIThemes->EntryOutline());

		Gui::DrawStringCentered(17, 0, 0.6, UIThemes->TextColor(), entry->GetTitle(), 273, 0, font);
		Gui::DrawStringCentered(17, 20, 0.4, UIThemes->TextColor(), entry->GetAuthor(), 273, 0, font);
		Gui::DrawStringCentered(17, 50, 0.4, UIThemes->TextColor(), entry->GetDescription(), 248, 0, font, C2D_WordWrap);

		Gui::DrawString(53, 130, 0.45, UIThemes->TextColor(), Lang::get("VERSION") + ": " + entry->GetVersion(), 248, 0, font);
		Gui::DrawString(53, 145, 0.45, UIThemes->TextColor(), Lang::get("CATEGORY") + ": " + entry->GetCategory(), 248, 0, font);
		Gui::DrawString(53, 160, 0.45, UIThemes->TextColor(), Lang::get("CONSOLE") + ": " + entry->GetConsole(), 248, 0, font);
		Gui::DrawString(53, 175, 0.45, UIThemes->TextColor(), Lang::get("LAST_UPDATED") + ": " + entry->GetLastUpdated(), 248, 0, font);
		Gui::DrawString(53, 190, 0.45, UIThemes->TextColor(), Lang::get("LICENSE") + ": " + entry->GetLicense(), 248, 0, font);

		GFX::DrawBox(btn.x, btn.y, btn.w, btn.h, false);
		if (!entry->GetScreenshots().empty()) GFX::DrawIcon(sprites_screenshot_idx, sshot.x, sshot.y, UIThemes->TextColor());
		if (entry->GetReleaseNotes() != "") GFX::DrawIcon(sprites_notes_idx, notes.x, notes.y, UIThemes->TextColor());
		Gui::DrawString(btn.x + 5, btn.y + 2, 0.6f, UIThemes->TextColor(), "★", 0, 0, font);
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
	int &mode: Reference to the store mode.
	const std::unique_ptr<StoreEntry> &entry: The store Entry.
*/
void StoreUtils::EntryHandle(bool &showMark, bool &fetch, bool &sFetch, int &mode, const std::unique_ptr<StoreEntry> &entry) {
	if (entry) {
		if ((hDown & KEY_SELECT) || (hDown & KEY_TOUCH && touching(touch, btn))) showMark = true;

		if ((hDown & KEY_Y) || (hDown & KEY_TOUCH && touching(touch, sshot))) {
			if (!entry->GetScreenshots().empty()) {
				if (checkWifiStatus()) {
					if (QueueRuns) {
						if (!Msg::promptMsg(Lang::get("FEATURE_SIDE_EFFECTS"))) return;
						sFetch = true;
						mode = 6;

					} else {
						sFetch = true;
						mode = 6;
					}
				}
			}
		}

		if ((hDown & KEY_X) || (hDown & KEY_TOUCH && touching(touch, notes))) {
			if (entry->GetReleaseNotes() != "") {
				ProcessReleaseNotes(entry->GetReleaseNotes());
				mode = 7;
			}
		}
	}

	/* Quit UU. */
	if (hDown & KEY_START && !QueueRuns)
		exiting = true;
}