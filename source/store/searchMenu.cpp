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
	{ 55, 5, 258, 30 }, // Search bar.

	/* Includes. */
	{ 85, 84, 10, 10 },
	{ 85, 100, 10, 10 },
	{ 167, 84, 10, 10 },
	{ 167, 100, 10, 10 },

	/* Filters. */
	{ 82, 170, 30, 30 },
	{ 117, 170, 30, 30 },
	{ 152, 170, 30, 30 },
	{ 187, 170, 30, 30 },
	{ 222, 170, 30, 30 },
	{ 257, 170, 30, 30 }
};

/*
	Draw the Search + Filter Menu.

	const std::vector<bool> &searchIncludes: Const Reference to the searchIncludes.
	const std::string &searchResult: Const Reference to the searchResult.
	const int &marks: Const Reference to the filter mark flags.
	const bool &updateFilter: Const Reference to the update filter.
*/
void StoreUtils::DrawSearchMenu(const std::vector<bool> &searchIncludes, const std::string &searchResult, const int &marks, const bool &updateFilter) {
	Gui::Draw_Rect(54, 4, 260, SearchMenu[0].h + 2, SEARCH_BAR_OUTL_COLOR);
	Gui::Draw_Rect(SearchMenu[0].x, SearchMenu[0].y, SearchMenu[0].w, SearchMenu[0].h, SEARCH_BAR_COLOR);

	Gui::DrawStringCentered(28, 10, 0.6, TEXT_COLOR, searchResult, 265);

	/* Checkboxes. */
	for (int i = 0; i < 4; i++) {
		GFX::DrawCheckbox(SearchMenu[i + 1].x, SearchMenu[i + 1].y, searchIncludes[i]);
	}

	Gui::DrawString(84, 60, 0.5, TEXT_COLOR, Lang::get("INCLUDE_IN_RESULTS"));

	Gui::DrawString(SearchMenu[1].x + 18, SearchMenu[1].y + 1, 0.4, TEXT_COLOR, Lang::get("TITLE"));
	Gui::DrawString(SearchMenu[2].x + 18, SearchMenu[2].y + 1, 0.4, TEXT_COLOR, Lang::get("AUTHOR"));

	Gui::DrawString(SearchMenu[3].x + 18, SearchMenu[3].y + 1, 0.4, TEXT_COLOR, Lang::get("CATEGORY"));
	Gui::DrawString(SearchMenu[4].x + 18, SearchMenu[4].y + 1, 0.4, TEXT_COLOR, Lang::get("CONSOLE"));

	/* Filters. */
	Gui::DrawString(84, 150, 0.5, TEXT_COLOR, Lang::get("FILTER_TO"));

	GFX::drawBox(SearchMenu[5].x, SearchMenu[5].y, SearchMenu[5].w, SearchMenu[5].h, marks & favoriteMarks::STAR);
	GFX::drawBox(SearchMenu[6].x, SearchMenu[6].y, SearchMenu[6].w, SearchMenu[6].h, marks & favoriteMarks::HEART);
	GFX::drawBox(SearchMenu[7].x, SearchMenu[7].y, SearchMenu[7].w, SearchMenu[7].h, marks & favoriteMarks::DIAMOND);
	GFX::drawBox(SearchMenu[8].x, SearchMenu[8].y, SearchMenu[8].w, SearchMenu[8].h, marks & favoriteMarks::CLUBS);
	GFX::drawBox(SearchMenu[9].x, SearchMenu[9].y, SearchMenu[9].w, SearchMenu[9].h, marks & favoriteMarks::SPADE);
	GFX::drawBox(SearchMenu[10].x, SearchMenu[10].y, SearchMenu[10].w, SearchMenu[10].h, updateFilter);
	GFX::DrawSprite(sprites_update_filter_idx, SearchMenu[10].x + 8, SearchMenu[10].y + 8);

	Gui::DrawString(SearchMenu[5].x + 8, SearchMenu[5].y + 8, 0.5, TEXT_COLOR, "★");
	Gui::DrawString(SearchMenu[6].x + 8, SearchMenu[6].y + 8, 0.5, TEXT_COLOR, "♥");
	Gui::DrawString(SearchMenu[7].x + 8, SearchMenu[7].y + 8, 0.5, TEXT_COLOR, "♦");
	Gui::DrawString(SearchMenu[8].x + 8, SearchMenu[8].y + 8, 0.5, TEXT_COLOR, "♣");
	Gui::DrawString(SearchMenu[9].x + 8, SearchMenu[9].y + 8, 0.5, TEXT_COLOR, "♠");
}

/*
	Search + Filter Handle.
	Here you can..

	- Filter your apps for the marks.
	- Search the UniStore.
	- Include stuff into the search.

	u32 hDown: The hidKeysDown() variable.
	u32 hHeld: The hidKeysHeld() variable.
	touchPosition touch: The TouchPosition variable.
	std::unique_ptr<Store> &store: Reference to the Store class.
	std::vector<std::unique_ptr<StoreEntry>> &entries: Reference to the Store Entries.
	std::vector<bool> &searchIncludes: Reference to the searchIncludes.
	std::unique_ptr<Meta> &meta: Reference to the Meta class.
	std::string &searchResult: Reference to the searchResult.
	int &marks: Reference to the mark flags.
	bool &updateFilter: Reference to the update filter.
*/
void StoreUtils::SearchHandle(u32 hDown, u32 hHeld, touchPosition touch, std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, std::vector<bool> &searchIncludes, std::unique_ptr<Meta> &meta, std::string &searchResult, int &marks, bool &updateFilter, bool ascending, SortType sorttype) {
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
				didTouch = true;
			} else if (touching(touch, SearchMenu[6])) {
				marks = marks ^ favoriteMarks::HEART;
				didTouch = true;
			} else if (touching(touch, SearchMenu[7])) {
				marks = marks ^ favoriteMarks::DIAMOND;
				didTouch = true;
			} else if (touching(touch, SearchMenu[8])) {
				marks = marks ^ favoriteMarks::CLUBS;
				didTouch = true;
			} else if (touching(touch, SearchMenu[9])) {
				marks = marks ^ favoriteMarks::SPADE;
				didTouch = true;
			} else if (touching(touch, SearchMenu[10])) {
				updateFilter = !updateFilter;
				didTouch = true;
			}
		}

		if (didTouch) {
			if (store && store->GetValid()) { // Only search, when valid.
				StoreUtils::ResetAll(store, meta, entries);
				StoreUtils::search(entries, searchResult, searchIncludes[0], searchIncludes[1], searchIncludes[2], searchIncludes[3], marks, updateFilter);
				store->SetScreenIndx(0);
				store->SetEntry(0);
				store->SetBox(0);

				StoreUtils::SortEntries(ascending, sorttype, entries);
			}
		}
	}

	/* Reset all. */
	if (hDown & KEY_X) {
		marks = 0;
		updateFilter = false;
		for(uint i = 0; i < searchIncludes.size(); i++)
			searchIncludes[i] = false;
		searchResult = "";

		if (store && store->GetValid()) {
			StoreUtils::ResetAll(store, meta, entries);
			StoreUtils::SortEntries(ascending, sorttype, entries);
		}
	}
}