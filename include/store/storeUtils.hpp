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
	void GridLogic(u32 hDown, u32 hHeld, touchPosition touch, std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries);

	/* Top List. */
	void DrawList(const std::unique_ptr<Store> &store, const std::vector<std::unique_ptr<StoreEntry>> &entries);
	void ListLogic(u32 hDown, u32 hHeld, touchPosition touch, std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries);

	/* Entry Info. */
	void DrawEntryInfo(const std::unique_ptr<Store> &store, const std::unique_ptr<StoreEntry> &entry);
	void EntryHandle(u32 hDown, u32 hHeld, touchPosition touch, bool &showMark, int &menu, bool &fetch);

	/* Side Menu. */
	void DrawSideMenu(const int &currentMenu);
	void SideMenuHandle(u32 hDown, u32 hHeld, touchPosition touch, int &currentMenu, bool &fetch);

	/* Download Entries. */
	void DrawDownList(const std::unique_ptr<Store> &store, const std::vector<std::string> &entries);
	void DownloadHandle(u32 hDown, u32 hHeld, touchPosition touch, const std::unique_ptr<Store> &store, const std::unique_ptr<StoreEntry> &entry, const std::vector<std::string> &entries, int &currentMenu, std::unique_ptr<Meta> &meta);

	/* Search + Favorite Menu. */
	void DrawSearchMenu(const std::vector<bool> &searchIncludes, const std::string &searchResult, const int &marks);
	void SearchHandle(u32 hDown, u32 hHeld, touchPosition touch, std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, std::vector<bool> &searchIncludes, std::unique_ptr<Meta> &meta, std::string &searchResult, int &marks);

	/* Mark Menu. */
	void DisplayMarkBox(const int &marks);
	void MarkHandle(u32 hDown, u32 hHeld, touchPosition touch, std::unique_ptr<StoreEntry> &entry, const std::unique_ptr<Store> &store, bool &showMark, std::unique_ptr<Meta> &meta);

	/* Credits. */
	void DrawCredits();

	/* Settings. */
	void DrawSettings(const int &page, const int &selection);
	void SettingsHandle(u32 hDown, u32 hHeld, touchPosition touch, int &page, bool &dspSettings, int &storeMode, int &selection, std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, std::unique_ptr<Meta> &meta);

	/* Sorting. */
	void DrawSorting(const bool &asc, const SortType &st);
	void SortHandle(u32 hDown, u32 hHeld, touchPosition touch, std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, bool &asc, SortType &st);

	bool compareTitleDescending(const std::unique_ptr<StoreEntry> &a, const std::unique_ptr<StoreEntry> &b);
	bool compareTitleAscending(const std::unique_ptr<StoreEntry> &a, const std::unique_ptr<StoreEntry> &b);

	bool compareAuthorDescending(const std::unique_ptr<StoreEntry> &a, const std::unique_ptr<StoreEntry> &b);
	bool compareAuthorAscending(const std::unique_ptr<StoreEntry> &a, const std::unique_ptr<StoreEntry> &b);

	bool compareUpdateDescending(const std::unique_ptr<StoreEntry> &a, const std::unique_ptr<StoreEntry> &b);
	bool compareUpdateAscending(const std::unique_ptr<StoreEntry> &a, const std::unique_ptr<StoreEntry> &b);

	void SortEntries(bool Ascending, SortType sorttype, std::vector<std::unique_ptr<StoreEntry>> &entries);

	void search(std::vector<std::unique_ptr<StoreEntry>> &entries, const std::string &query, bool title, bool author, bool category, bool console, int selectedMarks = 0);

	void ResetAll(std::unique_ptr<Store> &store, std::unique_ptr<Meta> &meta, std::vector<std::unique_ptr<StoreEntry>> &entries);
};

#endif