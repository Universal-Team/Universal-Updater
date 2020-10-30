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
static const Structs::ButtonPos btn = { 53, 215, 20, 20 };

/*
	Draw the Entry Info part.

	const std::unique_ptr<Store> &store: Const Reference to the Store class.
	const std::unique_ptr<StoreEntry> &entry: Const Reference to the current StoreEntry.
*/
void StoreUtils::DrawEntryInfo(const std::unique_ptr<Store> &store, const std::unique_ptr<StoreEntry> &entry) {
	if (store && entry) { // Ensure, store & entry is not a nullptr.
		Gui::Draw_Rect(48, 0, 272, 36, ENTRY_BAR_COLOR);
		Gui::Draw_Rect(48, 36, 272, 1, ENTRY_BAR_OUTL_COLOR);

		Gui::DrawStringCentered(25, 0, 0.6, TEXT_COLOR, entry->GetTitle(), 265);
		Gui::DrawStringCentered(25, 20, 0.4, TEXT_COLOR, entry->GetAuthor(), 265);

		if (entry->GetDescription() != "") {
			/* "\n\n" breaks C2D_WordWrap, so check here. */
			if (!(entry->GetDescription().find("\n\n") != std::string::npos)) {
				Gui::DrawStringCentered(25, 50, 0.4, TEXT_COLOR, entry->GetDescription(), 240, 0, nullptr, C2D_WordWrap);

			} else {
				Gui::DrawStringCentered(25, 50, 0.4, TEXT_COLOR, entry->GetDescription(), 240, 0);
			}
		}

		Gui::DrawString(61, 130, 0.45, TEXT_COLOR, Lang::get("VERSION") + ": " + entry->GetVersion(), 240);
		Gui::DrawString(61, 145, 0.45, TEXT_COLOR, Lang::get("CATEGORY") + ": " + entry->GetCategory(), 240);
		Gui::DrawString(61, 160, 0.45, TEXT_COLOR, Lang::get("CONSOLE") + ": " + entry->GetConsole(), 240);
		Gui::DrawString(61, 175, 0.45, TEXT_COLOR, Lang::get("LAST_UPDATED") + ": " + entry->GetLastUpdated(), 240);
		Gui::DrawString(61, 190, 0.45, TEXT_COLOR, Lang::get("LICENSE") + ": " + entry->GetLicense(), 240);

		GFX::drawBox(btn.x, btn.y, btn.w, btn.h, false);
		Gui::DrawString(btn.x + 3, btn.y, 0.6f, TEXT_COLOR, "★");
	}
}

/*
	The EntryInfo handle.
	Here you can..

	- Go to the download list, by pressing `A`.
	- Show the MarkMenu with START.

	u32 hDown: The hidKeysDown() variable.
	u32 hHeld: The hidKeysHeld() variable.
	touchPosition touch: The TouchPosition variable.
	bool &showMark: Reference to showMark.. to show the mark menu.
	bool &fetch: Reference to fetch, so we know, if we need to fetch, when accessing download list.
*/
void StoreUtils::EntryHandle(u32 hDown, u32 hHeld, touchPosition touch, bool &showMark, bool &fetch) {
	if ((hDown & KEY_START) || (hDown & KEY_TOUCH && touching(touch, btn))) showMark = true;
}