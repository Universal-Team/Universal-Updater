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

static const std::vector<Structs::ButtonPos> buttons = {
	{ 75, 60, 100, 16 },
	{ 75, 80, 100, 16 },
	{ 75, 100, 100, 16 },

	{ 205, 60, 100, 16 },
	{ 205, 80, 100, 16 },

	{ 75, 170, 100, 16 },
	{ 75, 190, 100, 16 }
};

static void DrawCheck(int pos, bool v) {
	GFX::DrawSprite((v ? sprites_sort_checked_idx : sprites_sort_unchecked_idx), buttons[pos].x + 5, buttons[pos].y);
}

/*
	Return SortType as an uint8_t.

	SortType st: The SortType variable.
*/
static const uint8_t GetType(SortType st) {
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

	bool asc: The Ascending variable.
	SortType st: The SortType variable.
*/
void StoreUtils::DrawSorting(bool asc, SortType st) {
	Gui::Draw_Rect(48, 0, 272, 25, ENTRY_BAR_COLOR);
	Gui::Draw_Rect(48, 25, 272, 1, ENTRY_BAR_OUTL_COLOR);
	Gui::DrawStringCentered(25, 2, 0.6, TEXT_COLOR, Lang::get("SORTING"), 265, 0, font);

	/* Sort By. */
	Gui::DrawString(buttons[0].x + 5, buttons[0].y - 20, 0.6f, TEXT_COLOR, Lang::get("SORT_BY"), 90, 0, font);
	for (int i = 0; i < 3; i++) {
		DrawCheck(i, i == GetType(st));
	}

	Gui::DrawString(buttons[0].x + 25, buttons[0].y + 2, 0.4f, TEXT_COLOR, Lang::get("TITLE"), 80, 0, font);
	Gui::DrawString(buttons[1].x + 25, buttons[1].y + 2, 0.4f, TEXT_COLOR, Lang::get("AUTHOR"), 80, 0, font);
	Gui::DrawString(buttons[2].x + 25, buttons[2].y + 2, 0.4f, TEXT_COLOR, Lang::get("LAST_UPDATED"), 80, 0, font);

	/* Direction. */
	Gui::DrawString(buttons[3].x + 5, buttons[3].y - 20, 0.6f, TEXT_COLOR, Lang::get("DIRECTION"), 80, 0, font);
	DrawCheck(3, asc);
	DrawCheck(4, !asc);
	Gui::DrawString(buttons[3].x + 25, buttons[3].y + 2, 0.4f, TEXT_COLOR, Lang::get("ASCENDING"), 80, 0, font);
	Gui::DrawString(buttons[4].x + 25, buttons[4].y + 2, 0.4f, TEXT_COLOR, Lang::get("DESCENDING"), 80, 0, font);

	/* Top Style. */
	Gui::DrawString(buttons[5].x + 5, buttons[5].y - 20, 0.6f, TEXT_COLOR, Lang::get("TOP_STYLE"), 90, 0, font);
	DrawCheck(5, config->list());
	DrawCheck(6, !config->list());
	Gui::DrawString(buttons[5].x + 25, buttons[5].y + 2, 0.4f, TEXT_COLOR, Lang::get("LIST"), 90, 0, font);
	Gui::DrawString(buttons[6].x + 25, buttons[6].y + 2, 0.4f, TEXT_COLOR, Lang::get("GRID"), 90, 0, font);
}

/*
	Sort Handle.
	Here you can..

	- Sort your Entries to..
		- Title (Ascending / Descending).
		- Author (Ascending / Descending).
		- Last Updated Date (Ascending / Descending).

	- Change the Top Style.

	std::unique_ptr<Store> &store: Reference to the Store class.
	std::vector<std::unique_ptr<StoreEntry>> &entries: Reference to the StoreEntries.
	bool &asc: Reference to the Ascending variable.
	SortType &st: Reference to the SortType.
*/
void StoreUtils::SortHandle(std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, bool &asc, SortType &st) {
	if (store && store->GetValid() && entries.size() > 0) { // Ensure, this is valid and more than 0 entries exist.
		if (hDown & KEY_TOUCH) {
			/* SortType Part. */
			if (touching(touch, buttons[0])) {
				st = SortType::TITLE;
				StoreUtils::SortEntries(asc, st, entries);

			} else if (touching(touch, buttons[1])) {
				st = SortType::AUTHOR;
				StoreUtils::SortEntries(asc, st, entries);

			} else if (touching(touch, buttons[2])) {
				st = SortType::LAST_UPDATED;
				StoreUtils::SortEntries(asc, st, entries);

			/* Ascending | Descending Part. */
			} else if (touching(touch, buttons[3])) {
				asc = true;
				StoreUtils::SortEntries(asc, st, entries);

			} else if (touching(touch, buttons[4])) {
				asc = false;
				StoreUtils::SortEntries(asc, st, entries);

			} else if (touching(touch, buttons[5])) {
				if (config->list()) return;
				config->list(true);
				store->SetEntry(0);
				store->SetScreenIndx(0);
				store->SetBox(0);

			} else if (touching(touch, buttons[6])) {
				if (!config->list()) return;
				config->list(false);
				store->SetEntry(0);
				store->SetScreenIndx(0);
				store->SetBox(0);
			}
		}
	}
}