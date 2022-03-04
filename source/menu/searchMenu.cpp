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
static const std::vector<Structs::ButtonPos> SearchMenu = {
	{ 51, 41, 262, 30 }, // Search bar.

	/* Includes. */
	{ 85, 101, 50, 10 },
	{ 85, 117, 50, 10 },
	{ 167, 101, 50, 10 },
	{ 167, 117, 50, 10 },

	/* Filters. */
	{ 82, 159, 30, 30 },
	{ 117, 159, 30, 30 },
	{ 152, 159, 30, 30 },
	{ 187, 159, 30, 30 },
	{ 222, 159, 30, 30 },
	{ 257, 159, 30, 30 },

	/* Send to Queue. */
	{ 82, 200, 205, 25 },

	/* AND / OR. */
	{ 222, 139, 30, 13 },
	{ 257, 139, 30, 13 }
};

/*
	Draw the Search + Filter Menu.

	const std::vector<bool> &searchIncludes: Const Reference to the searchIncludes.
	const std::string &searchResult: Const Reference to the searchResult.
	int marks: The filter mark flags.
	bool updateFilter: The update filter.
	isAND: isAND for the AND / OR mode.
*/
void StoreUtils::DrawSearchMenu(const std::vector<bool> &searchIncludes, const std::string &searchResult, int marks, bool updateFilter, bool isAND) {
	Gui::Draw_Rect(40, 0, 280, 25, UIThemes->EntryBar());
	Gui::Draw_Rect(40, 25, 280, 1, UIThemes->EntryOutline());
	Gui::DrawStringCentered(21, 2, 0.6, UIThemes->TextColor(), Lang::get("SEARCH_FILTERS"), 269, 0, font);

	Gui::Draw_Rect(50, 40, 264, SearchMenu[0].h + 2, UIThemes->SearchBarOutline());
	Gui::Draw_Rect(SearchMenu[0].x, SearchMenu[0].y, SearchMenu[0].w, SearchMenu[0].h, UIThemes->SearchBar());

	Gui::DrawStringCentered(24, 46, 0.6, UIThemes->TextColor(), searchResult, 265, 0, font);

	/* Checkboxes. */
	for (int i = 0; i < 4; i++) {
		GFX::DrawCheckbox(SearchMenu[i + 1].x, SearchMenu[i + 1].y, searchIncludes[i]);
	}

	Gui::DrawString(84, 81, 0.5, UIThemes->TextColor(), Lang::get("INCLUDE_IN_RESULTS"), 265, 0, font);

	Gui::DrawString(SearchMenu[1].x + 18, SearchMenu[1].y + 1, 0.4, UIThemes->TextColor(), Lang::get("TITLE"), 90, 0, font);
	Gui::DrawString(SearchMenu[2].x + 18, SearchMenu[2].y + 1, 0.4, UIThemes->TextColor(), Lang::get("AUTHOR"), 90, 0, font);

	Gui::DrawString(SearchMenu[3].x + 18, SearchMenu[3].y + 1, 0.4, UIThemes->TextColor(), Lang::get("CATEGORY"), 90, 0, font);
	Gui::DrawString(SearchMenu[4].x + 18, SearchMenu[4].y + 1, 0.4, UIThemes->TextColor(), Lang::get("CONSOLE"), 90, 0, font);

	/* Filters. */
	Gui::DrawString(84, SearchMenu[5].y - 20, 0.5f, UIThemes->TextColor(), Lang::get("FILTER_TO"), 265, 0, font);

	Gui::Draw_Rect(SearchMenu[5].x, SearchMenu[5].y, SearchMenu[5].w, SearchMenu[5].h, ((marks & favoriteMarks::STAR) ?
		UIThemes->SideBarUnselected() : UIThemes->BoxInside()));

	Gui::Draw_Rect(SearchMenu[6].x, SearchMenu[6].y, SearchMenu[6].w, SearchMenu[6].h, ((marks & favoriteMarks::HEART) ?
		UIThemes->SideBarUnselected() : UIThemes->BoxInside()));

	Gui::Draw_Rect(SearchMenu[7].x, SearchMenu[7].y, SearchMenu[7].w, SearchMenu[7].h, ((marks & favoriteMarks::DIAMOND) ?
		UIThemes->SideBarUnselected() : UIThemes->BoxInside()));

	Gui::Draw_Rect(SearchMenu[8].x, SearchMenu[8].y, SearchMenu[8].w, SearchMenu[8].h, ((marks & favoriteMarks::CLUBS) ?
		UIThemes->SideBarUnselected() : UIThemes->BoxInside()));

	Gui::Draw_Rect(SearchMenu[9].x, SearchMenu[9].y, SearchMenu[9].w, SearchMenu[9].h, ((marks & favoriteMarks::SPADE) ?
		UIThemes->SideBarUnselected() : UIThemes->BoxInside()));

	Gui::Draw_Rect(SearchMenu[10].x, SearchMenu[10].y, SearchMenu[10].w, SearchMenu[10].h, (updateFilter ?
		UIThemes->SideBarUnselected() : UIThemes->BoxInside()));

	Gui::DrawString(SearchMenu[5].x + 9, SearchMenu[5].y + 7, 0.5f, UIThemes->TextColor(), "★", 0, 0, font);
	Gui::DrawString(SearchMenu[6].x + 9, SearchMenu[6].y + 7, 0.5f, UIThemes->TextColor(), "♥", 0, 0, font);
	Gui::DrawString(SearchMenu[7].x + 9, SearchMenu[7].y + 7, 0.5f, UIThemes->TextColor(), "♦", 0, 0, font);
	Gui::DrawString(SearchMenu[8].x + 9, SearchMenu[8].y + 7, 0.5f, UIThemes->TextColor(), "♣", 0, 0, font);
	Gui::DrawString(SearchMenu[9].x + 9, SearchMenu[9].y + 7, 0.5f, UIThemes->TextColor(), "♠", 0, 0, font);
	GFX::DrawIcon(sprites_update_filter_idx, SearchMenu[10].x + 8, SearchMenu[10].y + 8, UIThemes->TextColor());

	Gui::Draw_Rect(SearchMenu[11].x, SearchMenu[11].y, SearchMenu[11].w, SearchMenu[11].h, UIThemes->MarkUnselected());
	Gui::DrawStringCentered(23, SearchMenu[11].y + 6, 0.45f, UIThemes->TextColor(), Lang::get("SELECTION_QUEUE"), 200, 0, font);

	/* AND / OR. */
	Gui::Draw_Rect(SearchMenu[12].x, SearchMenu[12].y, SearchMenu[12].w, SearchMenu[12].h, (isAND ? UIThemes->MarkSelected() : UIThemes->MarkUnselected()));
	Gui::DrawString(SearchMenu[12].x + 4, SearchMenu[12].y, 0.4f, UIThemes->TextColor(), "AND", 0, 0, font);

	Gui::Draw_Rect(SearchMenu[13].x, SearchMenu[13].y, SearchMenu[13].w, SearchMenu[13].h, (!isAND ? UIThemes->MarkSelected() : UIThemes->MarkUnselected()));
	Gui::DrawString(SearchMenu[13].x + 8, SearchMenu[13].y, 0.4f, UIThemes->TextColor(), "OR", 0, 0, font);
}

/*
	Search + Filter Handle.
	Here you can..

	- Filter your apps for the marks.
	- Search the UniStore.
	- Include stuff into the search.

	std::vector<bool> &searchIncludes: Reference to the searchIncludes.
	std::string &searchResult: Reference to the searchResult.
	int &marks: Reference to the mark flags.
	bool &updateFilter: Reference to the update filter.
	bool &isAND: Reference to isAND boolean for AND / OR mode.
*/
void StoreUtils::SearchHandle(std::vector<bool> &searchIncludes, std::string &searchResult, int &marks, bool &updateFilter, bool ascending, SortType sorttype, bool &isAND) {
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
				if (StoreUtils::store) {
					searchResult = Input::setkbdString(20, Lang::get("ENTER_SEARCH"), {});
					didTouch = true;

				} else {
					searchResult = Input::setkbdString(20, Lang::get("ENTER_SEARCH"), {});
					didTouch = true;
				}
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

			} else if (touching(touch, SearchMenu[11])) {
				StoreUtils::AddAllToQueue();

			} else if (touching(touch, SearchMenu[12])) {
				isAND = true;
				didTouch = true;

			} else if (touching(touch, SearchMenu[13])) {
				isAND = false;
				didTouch = true;
			}
		}

		if (didTouch) {
			if (StoreUtils::store && StoreUtils::store->GetValid()) { // Only search, when valid.
				StoreUtils::ResetAll();
				StoreUtils::search(searchResult, searchIncludes[0], searchIncludes[1], searchIncludes[2], searchIncludes[3], marks, updateFilter, isAND);
				StoreUtils::store->SetScreenIndx(0);
				StoreUtils::store->SetEntry(0);
				StoreUtils::store->SetBox(0);

				StoreUtils::SortEntries(ascending, sorttype);
			}
		}
	}

	/* Reset all. */
	if (hDown & KEY_X) {
		marks = 0;
		updateFilter = false;
		for(uint i = 0; i < searchIncludes.size(); i++) searchIncludes[i] = false;
		searchResult = "";

		if (StoreUtils::store && StoreUtils::store->GetValid()) {
			StoreUtils::ResetAll();
			StoreUtils::SortEntries(ascending, sorttype);
		}
	}

	/* Quit UU. */
	if (hDown & KEY_START && !QueueRuns)
		exiting = true;
}