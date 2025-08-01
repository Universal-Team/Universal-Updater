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
#include "keyboard.hpp"
#include "storeUtils.hpp"
#include "structs.hpp"

extern bool exiting, QueueRuns;
extern bool touching(touchPosition touch, Structs::ButtonPos button);

static const std::vector<Structs::ButtonPos> typeButtons = {
	{ 71, 60, 104, 16 },
	{ 71, 80, 104, 16 },
	{ 71, 100, 104, 16 },
	{ 71, 120, 104, 16 },
};

static const std::vector<Structs::ButtonPos> dirButtons = {
	{ 201, 60, 104, 16 },
	{ 201, 80, 104, 16 },
};

static const std::vector<Structs::ButtonPos> viewButtons = {
	{ 71, 170, 104, 16 },
	{ 71, 190, 104, 16 }
};

static void DrawCheck(Structs::ButtonPos button, bool v) {
	GFX::DrawIcon((v ? sprites_sort_checked_idx : sprites_sort_unchecked_idx), button.x + 1, button.y, UIThemes->TextColor());
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

		case SortType::POPULARITY:
			return 3;
	}

	return 1;
}

/*
	Draw the Sort Menu.

	bool asc: The Ascending variable.
	SortType st: The SortType variable.
*/
void StoreUtils::DrawSorting(bool asc, SortType st) {
	Gui::Draw_Rect(40, 0, 280, 25, UIThemes->EntryBar());
	Gui::Draw_Rect(40, 25, 280, 1, UIThemes->EntryOutline());
	Gui::DrawStringCentered(17, 2, 0.6, UIThemes->TextColor(), Lang::get("SORTING"), 273, 0, font);

	/* Sort By. */
	Gui::DrawString(typeButtons[0].x + 1, typeButtons[0].y - 20, 0.6f, UIThemes->TextColor(), Lang::get("SORT_BY"), 90, 0, font);
	for (uint i = 0; i < 4; i++) {
		DrawCheck(typeButtons[i], i == GetType(st));
	}

	Gui::DrawString(typeButtons[0].x + 21, typeButtons[0].y + 2, 0.4f, UIThemes->TextColor(), Lang::get("TITLE"), 80, 0, font);
	Gui::DrawString(typeButtons[1].x + 21, typeButtons[1].y + 2, 0.4f, UIThemes->TextColor(), Lang::get("AUTHOR"), 80, 0, font);
	Gui::DrawString(typeButtons[2].x + 21, typeButtons[2].y + 2, 0.4f, UIThemes->TextColor(), Lang::get("LAST_UPDATED"), 80, 0, font);
	Gui::DrawString(typeButtons[3].x + 21, typeButtons[3].y + 2, 0.4f, UIThemes->TextColor(), Lang::get("POPULARITY"), 80, 0, font);

	/* Direction. */
	Gui::DrawString(dirButtons[0].x + 1, dirButtons[0].y - 20, 0.6f, UIThemes->TextColor(), Lang::get("DIRECTION"), 80, 0, font);
	DrawCheck(dirButtons[0], asc);
	DrawCheck(dirButtons[1], !asc);
	Gui::DrawString(dirButtons[0].x + 21, dirButtons[0].y + 2, 0.4f, UIThemes->TextColor(), Lang::get("ASCENDING"), 80, 0, font);
	Gui::DrawString(dirButtons[1].x + 21, dirButtons[1].y + 2, 0.4f, UIThemes->TextColor(), Lang::get("DESCENDING"), 80, 0, font);

	/* Top Style. */
	Gui::DrawString(viewButtons[0].x + 1, viewButtons[0].y - 20, 0.6f, UIThemes->TextColor(), Lang::get("TOP_STYLE"), 90, 0, font);
	DrawCheck(viewButtons[0], config->list());
	DrawCheck(viewButtons[1], !config->list());
	Gui::DrawString(viewButtons[0].x + 21, viewButtons[0].y + 2, 0.4f, UIThemes->TextColor(), Lang::get("LIST"), 90, 0, font);
	Gui::DrawString(viewButtons[1].x + 21, viewButtons[1].y + 2, 0.4f, UIThemes->TextColor(), Lang::get("GRID"), 90, 0, font);
}

/*
	Sort Handle.
	Here you can..

	- Sort your entries to..
		- Title (Ascending / Descending).
		- Author (Ascending / Descending).
		- Last Updated Date (Ascending / Descending).

	- Change the Top Style.

	bool &asc: Reference to the Ascending variable.
	SortType &st: Reference to the SortType.
*/
void StoreUtils::SortHandle(bool &asc, SortType &st) {
	if (StoreUtils::store && StoreUtils::store->GetValid() && StoreUtils::entries.size() > 0) { // Ensure, this is valid and more than 0 StoreUtils::entries exist.
		if (hDown & KEY_TOUCH) {
			/* SortType Part. */
			if (touching(touch, typeButtons[0])) {
				st = SortType::TITLE;
				StoreUtils::SortEntries(asc, st);

			} else if (touching(touch, typeButtons[1])) {
				st = SortType::AUTHOR;
				StoreUtils::SortEntries(asc, st);

			} else if (touching(touch, typeButtons[2])) {
				st = SortType::LAST_UPDATED;
				StoreUtils::SortEntries(asc, st);

			} else if (touching(touch, typeButtons[3])) {
				st = SortType::POPULARITY;
				StoreUtils::SortEntries(asc, st);

			/* Ascending | Descending Part. */
			} else if (touching(touch, dirButtons[0])) {
				asc = true;
				StoreUtils::SortEntries(asc, st);

			} else if (touching(touch, dirButtons[1])) {
				asc = false;
				StoreUtils::SortEntries(asc, st);

			} else if (touching(touch, viewButtons[0])) {
				if (config->list()) return;
				config->list(true);
				StoreUtils::store->SetEntry(0);
				StoreUtils::store->SetScreenIndx(0);
				StoreUtils::store->SetBox(0);

			} else if (touching(touch, viewButtons[1])) {
				if (!config->list()) return;
				config->list(false);
				StoreUtils::store->SetEntry(0);
				StoreUtils::store->SetScreenIndx(0);
				StoreUtils::store->SetBox(0);
			}
		}
	}

	/* Quit UU. */
	if (hDown & KEY_START && !QueueRuns)
		exiting = true;
}