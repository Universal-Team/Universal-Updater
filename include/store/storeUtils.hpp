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

#ifndef _UNIVERSAL_UPDATER_STORE_UTILS_HPP
#define _UNIVERSAL_UPDATER_STORE_UTILS_HPP

#include "common.hpp"
#include "store.hpp"
#include "storeEntry.hpp"
#include <vector>

enum class SortType : uint8_t {
	TITLE,
	AUTHOR,
	LAST_UPDATED
};

namespace StoreUtils {
	/* Grid. */
	void DrawGrid(const std::unique_ptr<Store> &store, const std::vector<std::unique_ptr<StoreEntry>> &entries);
	void GridLogic(std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, int &currentMode, int &lastMode, bool &fetch, int &smallDelay);

	/* Top List. */
	void DrawList(const std::unique_ptr<Store> &store, const std::vector<std::unique_ptr<StoreEntry>> &entries);
	void ListLogic(std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, int &currentMode, int &lastMode, bool &fetch, int &smallDelay);

	/* Entry Info. */
	void DrawEntryInfo(const std::unique_ptr<Store> &store, const std::unique_ptr<StoreEntry> &entry);
	void EntryHandle(bool &showMark, bool &fetch, bool &sFetch, int &mode);

	/* Side Menu. */
	void DrawSideMenu(int currentMenu);
	void SideMenuHandle(int &currentMenu, bool &fetch, int &lastMenu);

	/* Download Entries. */
	void DrawDownList(const std::unique_ptr<Store> &store, const std::vector<std::string> &entries, bool fetch, const std::unique_ptr<StoreEntry> &entry, const std::vector<std::string> &sizes);
	void DownloadHandle(const std::unique_ptr<Store> &store, const std::unique_ptr<StoreEntry> &entry, const std::vector<std::string> &entries, int &currentMenu, std::unique_ptr<Meta> &meta, const int &lastMode, int &smallDelay);

	/* Search + Favorite Menu. */
	void DrawSearchMenu(const std::vector<bool> &searchIncludes, const std::string &searchResult, int marks, bool updateFilter);
	void SearchHandle(std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, std::vector<bool> &searchIncludes, std::unique_ptr<Meta> &meta, std::string &searchResult, int &marks, bool &updateFilter, bool ascending, SortType sorttype);

	/* Mark Menu. */
	void DisplayMarkBox(int marks);
	void MarkHandle(std::unique_ptr<StoreEntry> &entry, const std::unique_ptr<Store> &store, bool &showMark, std::unique_ptr<Meta> &meta);

	/* Credits. */
	void DrawCredits();

	/* Screenshot menu. */
	void DrawScreenshotMenu(const C2D_Image &img, const int sIndex, const bool sFetch, const int screenshotSize, const std::string &name, const int zoom, const bool canDisplay);
	void ScreenshotMenu(C2D_Image &img, int &sIndex, bool &sFetch, int &storeMode, const int screenshotSize, int &zoom, bool &canDisplay);

	/* Settings. */
	void DrawSettings(int page, int selection, int sPos);
	void SettingsHandle(int &page, bool &dspSettings, int &storeMode, int &selection, std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, std::unique_ptr<Meta> &meta, int &sPos);

	/* Sorting. */
	void DrawSorting(bool asc, SortType st);
	void SortHandle(std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, bool &asc, SortType &st);

	bool compareTitleDescending(const std::unique_ptr<StoreEntry> &a, const std::unique_ptr<StoreEntry> &b);
	bool compareTitleAscending(const std::unique_ptr<StoreEntry> &a, const std::unique_ptr<StoreEntry> &b);

	bool compareAuthorDescending(const std::unique_ptr<StoreEntry> &a, const std::unique_ptr<StoreEntry> &b);
	bool compareAuthorAscending(const std::unique_ptr<StoreEntry> &a, const std::unique_ptr<StoreEntry> &b);

	bool compareUpdateDescending(const std::unique_ptr<StoreEntry> &a, const std::unique_ptr<StoreEntry> &b);
	bool compareUpdateAscending(const std::unique_ptr<StoreEntry> &a, const std::unique_ptr<StoreEntry> &b);

	void SortEntries(bool Ascending, SortType sorttype, std::vector<std::unique_ptr<StoreEntry>> &entries);

	void search(std::vector<std::unique_ptr<StoreEntry>> &entries, const std::string &query, bool title, bool author, bool category, bool console, int selectedMarks, bool updateAvl);

	void FilterUpdateAvailable(std::vector<std::unique_ptr<StoreEntry>> &entries);

	void ResetAll(const std::unique_ptr<Store> &store, const std::unique_ptr<Meta> &meta, std::vector<std::unique_ptr<StoreEntry>> &entries);
};

#endif