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
static const std::vector<Structs::ButtonPos> SearchMenu = {
	{55, 5, 258, 30}, // Search bar.

	/* Includes. */
	{85, 84, 10, 10},
	{85, 100, 10, 10},
	{167, 84, 10, 10},
	{167, 100, 10, 10},

	/* Filters. */
	{85, 170, 30, 30},
	{125, 170, 30, 30},
	{165, 170, 30, 30},
	{205, 170, 30, 30},
	{245, 170, 30, 30}
};

/*
	Draw the Search + Favorites Menu.
*/
void StoreUtils::DrawSearchMenu(const std::vector<bool> &searchIncludes, const std::string &searchResult, const int &marks) {
	Gui::Draw_Rect(SearchMenu[0].x, SearchMenu[0].y, SearchMenu[0].w, SearchMenu[0].h, C2D_Color32(51, 75, 102, 255));
	Gui::Draw_Rect(55, 35, 258, 1, C2D_Color32(25, 30, 53, 255));

	Gui::DrawStringCentered(28, 10, 0.6, C2D_Color32(255, 255, 255, 255), searchResult, 265);

	/* Checkboxes. */
	for (int i = 0; i < 4; i++) {
		if (searchIncludes[i]) {
			GFX::drawBox(SearchMenu[i + 1].x, SearchMenu[i + 1].y, SearchMenu[i + 1].w, SearchMenu[i + 1].h, true);

		} else {
			GFX::drawBox(SearchMenu[i + 1].x, SearchMenu[i + 1].y, SearchMenu[i + 1].w, SearchMenu[i + 1].h, false);
		}
	}

	Gui::DrawString(84, 60, 0.5, C2D_Color32(255, 255, 255, 255), Lang::get("INCLUDE_IN_RESULTS"));

	Gui::DrawString(SearchMenu[1].x + 13, SearchMenu[1].y - 2, 0.4, C2D_Color32(255, 255, 255, 255), Lang::get("TITLE"));
	Gui::DrawString(SearchMenu[2].x + 13, SearchMenu[2].y - 2, 0.4, C2D_Color32(255, 255, 255, 255), Lang::get("AUTHOR"));

	Gui::DrawString(SearchMenu[3].x + 13, SearchMenu[3].y - 2, 0.4, C2D_Color32(255, 255, 255, 255), Lang::get("CATEGORY"));
	Gui::DrawString(SearchMenu[4].x + 13, SearchMenu[4].y - 2, 0.4, C2D_Color32(255, 255, 255, 255), Lang::get("CONSOLE"));

	/* Filters. */
	Gui::DrawString(84, 150, 0.5, C2D_Color32(255, 255, 255, 255), "Filter to:");

	GFX::drawBox(SearchMenu[5].x, SearchMenu[5].y, SearchMenu[5].w, SearchMenu[5].h, marks & favoriteMarks::STAR);
	GFX::drawBox(SearchMenu[6].x, SearchMenu[6].y, SearchMenu[6].w, SearchMenu[6].h, marks & favoriteMarks::HEART);
	GFX::drawBox(SearchMenu[7].x, SearchMenu[7].y, SearchMenu[7].w, SearchMenu[7].h, marks & favoriteMarks::DIAMOND);
	GFX::drawBox(SearchMenu[8].x, SearchMenu[8].y, SearchMenu[8].w, SearchMenu[8].h, marks & favoriteMarks::CLUBS);
	GFX::drawBox(SearchMenu[9].x, SearchMenu[9].y, SearchMenu[9].w, SearchMenu[9].h, marks & favoriteMarks::SPADE);

	Gui::DrawString(SearchMenu[5].x + 8, SearchMenu[5].y + 8, 0.5, C2D_Color32(255, 255, 255, 255), "★");
	Gui::DrawString(SearchMenu[6].x + 8, SearchMenu[6].y + 8, 0.5, C2D_Color32(255, 255, 255, 255), "♥");
	Gui::DrawString(SearchMenu[7].x + 8, SearchMenu[7].y + 8, 0.5, C2D_Color32(255, 255, 255, 255), "♦");
	Gui::DrawString(SearchMenu[8].x + 8, SearchMenu[8].y + 8, 0.5, C2D_Color32(255, 255, 255, 255), "♣");
	Gui::DrawString(SearchMenu[9].x + 8, SearchMenu[9].y + 8, 0.5, C2D_Color32(255, 255, 255, 255), "♠");
}

/*
	Search Handle.
*/
void StoreUtils::SearchHandle(u32 hDown, u32 hHeld, touchPosition touch, std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, std::vector<bool> &searchIncludes, std::unique_ptr<Meta> &meta, std::string &searchResult, int &marks) {
	/* Checkboxes. */
	if (hDown & KEY_TOUCH) {
		bool didTouch = false;

		/* Includes. */
		for (int i = 0; i < 4; i++) {
			if (touching(touch, SearchMenu[i + 1])) {
				searchIncludes[i] = !searchIncludes[i];
				didTouch = true;
				break;
			}
		}

		/* Search bar. */
		if (!didTouch) {
			if (touching(touch, SearchMenu[0])) {
				searchResult = Input::setkbdString(20, Lang::get("ENTER_SEARCH"));
				didTouch = true;
			}
		}

		/* Filters. */
		if (!didTouch) {
			if (touching(touch, SearchMenu[5])) {
				marks = marks ^ favoriteMarks::STAR;

			} else if (touching(touch, SearchMenu[6])) {
				marks = marks ^ favoriteMarks::HEART;

			} else if (touching(touch, SearchMenu[7])) {
				marks = marks ^ favoriteMarks::DIAMOND;

			} else if (touching(touch, SearchMenu[8])) {
				marks = marks ^ favoriteMarks::CLUBS;

			} else if (touching(touch, SearchMenu[9])) {
				marks = marks ^ favoriteMarks::SPADE;

			}
		}
	}

	/* Start for now does a search. */
	if (hDown & KEY_START) {
		StoreUtils::search(entries, searchResult, searchIncludes[0], searchIncludes[1], searchIncludes[2], searchIncludes[3], marks);
		store->SetScreenIndx(0);
		store->SetEntry(0);
		store->SetBox(0);
	}

	/* Reset all. */
	if (hDown & KEY_SELECT) {
		StoreUtils::ResetAll(store, meta, entries);
	}
}