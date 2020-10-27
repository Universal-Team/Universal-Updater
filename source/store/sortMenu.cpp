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

#include "keyboard.hpp"
#include "storeUtils.hpp"
#include "structs.hpp"

extern bool touching(touchPosition touch, Structs::ButtonPos button);

static const std::vector<std::string> ButtonNames = { "TITLE", "AUTHOR", "LAST_UPDATED", "ASCENDING", "DESCENDING" };
static const std::vector<Structs::ButtonPos> buttons = {
	{ 66, 46, 100, 30 },
	{ 66, 86, 100, 30 },
	{ 66, 126, 100, 30 },

	{ 204, 54, 100, 40 },
	{ 204, 111, 100, 40 }
};

/*
	Return SortType uint8_t.
*/
static const uint8_t GetType(const SortType &st) {
	switch(st) {
		case SortType::TITLE:
			return 0;

		case SortType::AUTHOR:
			return 1;

		case SortType::LAST_UPDATED:
			return 2;
	}

	return 1;
}

/*
	Draw the Sort Menu.
*/
void StoreUtils::DrawSorting(const bool &asc, const SortType &st) {
	/* Display Key. */
	Gui::DrawStringCentered(66 - 160 + (100 / 2), 15 + (30 / 2) - (Gui::GetStringHeight(0.6f, Lang::get("KEY")) / 2), 0.6f, TEXT_COLOR, Lang::get("KEY"), 100 - 4, 30 - 4);

	for (int i = 0; i < 3; i++) {
		GFX::DrawButton(buttons[i].x, buttons[i].y, buttons[i].w, buttons[i].h, i == GetType(st), Lang::get(ButtonNames[i]));
	}

	/* Direction. */
	Gui::DrawStringCentered(204 - 160 + (100 / 2), 15 + (30 / 2) - (Gui::GetStringHeight(0.6f, Lang::get("DIRECTION")) / 2), 0.6f, TEXT_COLOR, Lang::get("DIRECTION"), 100 - 4, 30 - 4);
	GFX::DrawButton(buttons[3].x, buttons[3].y, buttons[3].w, buttons[3].h, asc, Lang::get(ButtonNames[3]));
	GFX::DrawButton(buttons[4].x, buttons[4].y, buttons[4].w, buttons[4].h, !asc, Lang::get(ButtonNames[4]));
}

/*
	Sort Handle.
*/
void StoreUtils::SortHandle(u32 hDown, u32 hHeld, touchPosition touch, std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, bool &asc, SortType &st) {
	if (store && store->GetValid() && entries.size() > 0) {

		if (hDown & KEY_TOUCH) {
			if (touching(touch, buttons[0])) {
				st = SortType::TITLE;
				StoreUtils::SortEntries(asc, st, entries);

			} else if (touching(touch, buttons[1])) {
				st = SortType::AUTHOR;
				StoreUtils::SortEntries(asc, st, entries);

			} else if (touching(touch, buttons[2])) {
				st = SortType::LAST_UPDATED;
				StoreUtils::SortEntries(asc, st, entries);

			} else if (touching(touch, buttons[3])) {
				asc = true;
				StoreUtils::SortEntries(asc, st, entries);

			} else if (touching(touch, buttons[4])) {
				asc = false;
				StoreUtils::SortEntries(asc, st, entries);
			}
		}
	}
}