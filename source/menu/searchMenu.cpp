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

bool detailedMode = false;

int marks = 0;
bool updateFilter = false, isAND = true;
std::string searchResult = "", authorSearchResult = "";
bool searchIncludes[] = { /* Title */ true, /* Description*/ true };
int category = -1, console = -1;

Structs::ButtonPos searchBar = { 51, 41, 262, 30 };
Structs::ButtonPos detailedSearch = { 82, 100, 205, 25 };
Structs::ButtonPos sendToQueue = { 82, 200, 205, 25 };

const std::string nameDescriptionLabels[] = { "TITLE", "DESCRIPTION" };
std::vector<Structs::ButtonPos> nameDescription = {
	{ 75, 80, 100, 10 },
	{ 175, 80, 100, 10 }
};

std::vector<favoriteMarks> markKeys {
	favoriteMarks::STAR,
	favoriteMarks::HEART,
	favoriteMarks::DIAMOND,
	favoriteMarks::CLUBS,
	favoriteMarks::SPADE
};
const std::string markLabels[] { "★", "♥", "♦", "♣", "♠" };
std::vector<Structs::ButtonPos> filters = {
	{ 82, 159, 30, 30 },
	{ 117, 159, 30, 30 },
	{ 152, 159, 30, 30 },
	{ 187, 159, 30, 30 },
	{ 222, 159, 30, 30 },
	{ 257, 159, 30, 30 }
};

const std::string andOrLabels[] = { "AND", "OR" };
std::vector<Structs::ButtonPos> andOr = {
	{ 222, 139, 30, 13 },
	{ 257, 139, 30, 13 }
};

/* For detailed mode. */
Structs::ButtonPos authorSearchBar = { 111, 140, 202, 30 };
Structs::ButtonPos categoryBtn = { 51, 200, 126, 30 };
Structs::ButtonPos consoleBtn = { 187, 200, 126, 30 };

void StoreUtils::ResetSearch() {
	marks = 0;
	updateFilter = false;
	isAND = true;
	searchResult = "";
	authorSearchResult = "";
	searchIncludes[0] = true;
	searchIncludes[1] = true;
	category = -1;
	console = -1;

	if (StoreUtils::store && StoreUtils::store->GetValid()) {
		StoreUtils::ResetAll();
		StoreUtils::SortEntries();
	}
}

/*
	Draw the Search + Filter Menu.

	const std::vector<bool> &searchIncludes: Const Reference to the searchIncludes.
	const std::string &searchResult: Const Reference to the searchResult.
	int marks: The filter mark flags.
	bool updateFilter: The update filter.
	isAND: isAND for the AND / OR mode.
*/
void StoreUtils::DrawSearchMenu() {
	Gui::Draw_Rect(40, 0, 280, 25, UIThemes->EntryBar());
	Gui::Draw_Rect(40, 25, 280, 1, UIThemes->EntryOutline());
	Gui::DrawStringCentered(21, 2, 0.6, UIThemes->TextColor(), Lang::get("SEARCH_FILTERS"), 269, 0, font);

	Gui::Draw_Rect(searchBar.x - 1, searchBar.y - 1, searchBar.w + 2, searchBar.h + 2, UIThemes->SearchBarOutline());
	Gui::Draw_Rect(searchBar.x, searchBar.y, searchBar.w, searchBar.h, UIThemes->SearchBar());

	Gui::DrawStringCentered(24, 46, 0.6, UIThemes->TextColor(), searchResult, 265, 0, font);

	/* Checkboxes. */
	for (size_t i = 0; i < nameDescription.size(); i++) {
		GFX::DrawCheckbox(nameDescription[i].x, nameDescription[i].y, searchIncludes[i]);
		Gui::DrawString(nameDescription[i].x + 18, nameDescription[i].y + 1, 0.4, UIThemes->TextColor(), Lang::get(nameDescriptionLabels[i]), 90, 0, font);
	}

	Gui::Draw_Rect(detailedSearch.x, detailedSearch.y, detailedSearch.w, detailedSearch.h, detailedMode ? UIThemes->MarkSelected() : UIThemes->MarkUnselected());
	Gui::DrawStringCentered(23, detailedSearch.y + 6, 0.45f, UIThemes->TextColor(), Lang::get("DETAILED_SEARCH"), 200, 0, font);

	if(detailedMode) {
		Gui::DrawString(51, authorSearchBar.y + 7, 0.6, UIThemes->TextColor(), Lang::get("AUTHOR"), 50, 0, font);
		Gui::Draw_Rect(authorSearchBar.x - 1, authorSearchBar.y - 1, authorSearchBar.w + 2, authorSearchBar.h + 2, UIThemes->SearchBarOutline());
		Gui::Draw_Rect(authorSearchBar.x, authorSearchBar.y, authorSearchBar.w, authorSearchBar.h, UIThemes->SearchBar());
		Gui::DrawStringCentered(authorSearchBar.x - 160 + authorSearchBar.w / 2, authorSearchBar.y + 5, 0.6, UIThemes->TextColor(), authorSearchResult, authorSearchBar.w - 10, 0, font);

		Gui::DrawStringCentered(categoryBtn.x - 160 + categoryBtn.w / 2, categoryBtn.y - 20, 0.6, UIThemes->TextColor(), Lang::get("CATEGORY"), categoryBtn.w, 0, font);
		Gui::Draw_Rect(categoryBtn.x, categoryBtn.y, categoryBtn.w, categoryBtn.h, category == -1 ? UIThemes->MarkUnselected() : UIThemes->MarkSelected());
		if (category > -1 && store) Gui::DrawStringCentered((categoryBtn.x - 160 + categoryBtn.w / 2), categoryBtn.y + 5, 0.6, UIThemes->TextColor(), StoreUtils::store->GetCategories()[category], categoryBtn.w - 10, 0, font);

		Gui::DrawStringCentered(consoleBtn.x - 160 + consoleBtn.w / 2, consoleBtn.y - 20, 0.6, UIThemes->TextColor(), Lang::get("CONSOLE"), consoleBtn.w, 0, font);
		Gui::Draw_Rect(consoleBtn.x, consoleBtn.y, consoleBtn.w, consoleBtn.h, console == -1 ? UIThemes->MarkUnselected() : UIThemes->MarkSelected());
		if (console > -1 && store) Gui::DrawStringCentered((consoleBtn.x - 160 + consoleBtn.w / 2), consoleBtn.y + 5, 0.6, UIThemes->TextColor(), StoreUtils::store->GetConsoles()[console], consoleBtn.w - 10, 0, font);
	} else {
		/* Filters. */
		Gui::DrawString(84, filters[0].y - 20, 0.5f, UIThemes->TextColor(), Lang::get("FILTER_TO"), 265, 0, font);
	
		for (size_t i = 0; i < markKeys.size(); i++) {
			Gui::Draw_Rect(filters[i].x, filters[i].y, filters[i].w, filters[i].h, ((marks & markKeys[i]) ?
				UIThemes->SideBarUnselected() : UIThemes->BoxInside()));
			Gui::DrawString(filters[i].x + 9, filters[i].y + 7, 0.5f, UIThemes->TextColor(), markLabels[i], 0, 0, font);
		}
		Gui::Draw_Rect(filters[5].x, filters[5].y, filters[5].w, filters[5].h, (updateFilter ?
			UIThemes->SideBarUnselected() : UIThemes->BoxInside()));
		GFX::DrawIcon(sprites_update_filter_idx, filters[5].x + 8, filters[5].y + 8, UIThemes->TextColor());
	
		Gui::Draw_Rect(sendToQueue.x, sendToQueue.y, sendToQueue.w, sendToQueue.h, UIThemes->MarkUnselected());
		Gui::DrawStringCentered(23, sendToQueue.y + 6, 0.45f, UIThemes->TextColor(), Lang::get("SELECTION_QUEUE"), 200, 0, font);
	
		/* AND / OR. */
		for(size_t i = 0; i < andOr.size(); i++) {
			Gui::Draw_Rect(andOr[i].x, andOr[i].y, andOr[i].w, andOr[i].h, ((isAND ^ i) ? UIThemes->MarkSelected() : UIThemes->MarkUnselected()));
			Gui::DrawString(andOr[i].x + 4, andOr[i].y, 0.4f, UIThemes->TextColor(), andOrLabels[i], 0, 0, font);
		}
	}
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
void StoreUtils::SearchHandle() {
	/* Checkboxes. */
	if (hDown & KEY_TOUCH) {
		bool didTouch = false;
		for(size_t i = 0; i < nameDescription.size(); i++) {
			if (touching(touch, nameDescription[i])) {
				searchIncludes[i] = !searchIncludes[i];
				didTouch = true;
				break;
			}
		}

		/* Search bar. */
		if (touching(touch, searchBar) && StoreUtils::store) {
			searchResult = Input::setkbdString(20, Lang::get("ENTER_SEARCH"), {});
			didTouch = true;
		}

		if (touching(touch, detailedSearch)) {
			detailedMode = !detailedMode;
			// We don't need to set didTouch cause no filters are changed.
		}

		if(detailedMode) {
			if (touching(touch, authorSearchBar) && StoreUtils::store) {
				authorSearchResult = Input::setkbdString(20, Lang::get("ENTER_SEARCH"), {});
				didTouch = true;
			}

			if (touching(touch, categoryBtn) && store) {
				category++;
				if (category >= (int)store->GetCategories().size()) category = -1;
				didTouch = true;
			}

			if (touching(touch, consoleBtn) && store) {
				console++;
				if (console >= (int)store->GetConsoles().size()) console = -1;
				didTouch = true;
			}
		} else {
			/* Filters. */
			for(size_t i = 0; i < markKeys.size(); i++) {
				if (touching(touch, filters[i])) {
					marks ^= markKeys[i];
					didTouch = true;
					break;
				}
			}
			if (touching(touch, filters[5])) {
				updateFilter = !updateFilter;
				didTouch = true;
			}
	
			if (touching(touch, sendToQueue)) {
				StoreUtils::AddAllToQueue();
				didTouch = true;
			}
	
			for(size_t i = 0; i < nameDescription.size(); i++) {
				if (touching(touch, andOr[i])) {
					isAND = !i;
					didTouch = true;
				}
			}
		}

		if (didTouch) {
			if (StoreUtils::store && StoreUtils::store->GetValid()) { // Only search, when valid.
				StoreUtils::ResetAll();
				const std::string &titleQuery = searchIncludes[0] ? searchResult : "";
				const std::string &descQuery = searchIncludes[1] ? searchResult : "";
				const std::string &categoryStr = category == -1 ? "" : store->GetCategories()[category];
				const std::string &consoleStr = console == -1 ? "" : store->GetConsoles()[console];
				StoreUtils::search(titleQuery, descQuery, authorSearchResult, categoryStr, consoleStr, marks, updateFilter, isAND);
				StoreUtils::store->SetScreenIndx(0);
				StoreUtils::store->SetEntry(0);
				StoreUtils::store->SetBox(0);

				StoreUtils::SortEntries();
			}
		}
	}

	/* Reset all. */
	if (hDown & KEY_X) {
		ResetSearch();
	}

	/* Quit UU. */
	if (hDown & KEY_START && !QueueRuns)
		exiting = true;
}