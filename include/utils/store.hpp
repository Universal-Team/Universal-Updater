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

#ifndef _UNIVERSAL_UPDATER_STORE_HPP
#define _UNIVERSAL_UPDATER_STORE_HPP

#include "json.hpp"
#include <3ds.h>
#include <string>
#include <vector>

struct UniStoreV2Struct {
	std::string title;
	std::string author;
	std::string version;
	std::string category;
	std::string console;
	std::string last_updated;
	int icon_index;
	int JSONIndex;
	bool updateAvailable;
};

enum class SortType {
	TITLE,
	AUTHOR,
	LAST_UPDATED
};

class Store {
public:
	Store(nlohmann::json &JS, std::string updateJSON = "NOT_FOUND");

	void writeToFile(int index);
	void sorting(bool Ascending, SortType sorttype);

	std::string returnTitle(const int index);
	std::string returnAuthor(const int index);
	int returnIconIndex(const int index);
	int returnJSONIndex(const int index);
	int getSize();
	bool getAscending() { return this->ascending; }
	bool isUpdateAvailable(int index) { return this->sortedStore[index].updateAvailable; }

	// Searching stuff.
	int searchForEntries(const std::string searchResult);
	int searchForAuthor(const std::string searchResult);
	int searchForCategory(const std::string searchResult);
	int searchForConsole(const std::string searchResult);
	bool updateAvailable(int index);
	void reset() { this->sortedStore = this->unsortedStore; }

	const int getSortType() {
		if (this->sorttype == SortType::TITLE)	return 0;
		else if (this->sorttype == SortType::AUTHOR)	return 1;
		else if (this->sorttype == SortType::LAST_UPDATED)	return 2;
		else return -1; // Should not happen.
	}

	const std::vector<std::string> getCategories() { return this->availableCategories; }
	const std::vector<std::string> getAuthors() { return this->availableAuthors; }
	const std::vector<std::string> getSystems() { return this->availableSystems; }
private:
	std::vector<UniStoreV2Struct> sortedStore, unsortedStore;
	std::vector<std::string> availableCategories, availableAuthors, availableSystems;
	std::string updateFile;
	bool ascending = false;
	nlohmann::json storeJson, updateJSON;
	SortType sorttype = SortType::TITLE;

	UniStoreV2Struct getData(const int index);
};

#endif