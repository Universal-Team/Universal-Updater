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

#ifndef _UNIVERSAL_UPDATER_MAIN_SCREEN_HPP
#define _UNIVERSAL_UPDATER_MAIN_SCREEN_HPP

#include "common.hpp"
#include "store.hpp"
#include "storeEntry.hpp"
#include "storeUtils.hpp"

/*
	Modes:

	0: Entry Info.
	1: Download List.
	2: Queue.
	3: Search + Favorites.
	4: Sorting.
	5: Settings / Credits(?).
	6: Screenshot Menu.
	7: Release Notes.
*/

class MainScreen : public Screen {
public:
	MainScreen();
	void Draw(void) const override;
	void Logic(u32 hDown, u32 hHeld, touchPosition touch) override;
private:
	std::vector<std::string> dwnldList, dwnldSizes, dwnldTypes;

	bool initialized = false, fetchDown = false, showMarks = false, showSettings = false,
		 ascending = false, updateFilter = false, screenshotFetch = false, canDisplay = false, isAND = true;

	int storeMode = 0, marks = 0, markIndex = 0, sPage = 0, lMode = 0, sSelection = 0,
		lastMode = 0, smallDelay = 0, sPos = 0, screenshotIndex = 0, sSize = 0, zoom = 0, scrollIndex = 0, queueIndex = 0;

	SortType sorttype = SortType::LAST_UPDATED;

	/* Title, Author, Category, Console. */
	std::vector<bool> searchIncludes = { true, false, false, false }, installs = { };
	std::string searchResult = "", screenshotName = "";

	C2D_Image Screenshot = { nullptr, nullptr };
};

#endif