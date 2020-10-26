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

#include "storeUtils.hpp"

/*
	Compare Title.
*/
bool StoreUtils::compareTitleDescending(const std::unique_ptr<StoreEntry> &a, const std::unique_ptr<StoreEntry> &b) {
	return strcasecmp(a->GetTitle().c_str(), b->GetTitle().c_str()) > 0;
}
bool StoreUtils::compareTitleAscending(const std::unique_ptr<StoreEntry> &a, const std::unique_ptr<StoreEntry> &b) {
	return strcasecmp(b->GetTitle().c_str(), a->GetTitle().c_str()) > 0;
}

/*
	Compare Author.
*/
bool StoreUtils::compareAuthorDescending(const std::unique_ptr<StoreEntry> &a, const std::unique_ptr<StoreEntry> &b) {
	return strcasecmp(a->GetAuthor().c_str(), b->GetAuthor().c_str()) > 0;
}
bool StoreUtils::compareAuthorAscending(const std::unique_ptr<StoreEntry> &a, const std::unique_ptr<StoreEntry> &b) {
	return strcasecmp(b->GetAuthor().c_str(), a->GetAuthor().c_str()) > 0;
}

/*
	Compare Last Updated.
*/
bool StoreUtils::compareUpdateDescending(const std::unique_ptr<StoreEntry> &a, const std::unique_ptr<StoreEntry> &b) {
	return strcasecmp(a->GetLastUpdated().c_str(), b->GetLastUpdated().c_str()) > 0;
}
bool StoreUtils::compareUpdateAscending(const std::unique_ptr<StoreEntry> &a, const std::unique_ptr<StoreEntry> &b) {
	return strcasecmp(b->GetLastUpdated().c_str(), a->GetLastUpdated().c_str()) > 0;
}

/*
	Sort the entries.

	bool Ascending: Is Ascending?
	SortType sorttype: The sort type.
	std::vector<std::unique_ptr<StoreEntry>> &entries: Reference to the Entries, which should be sorted.
*/
void StoreUtils::SortEntries(bool Ascending, SortType sorttype, std::vector<std::unique_ptr<StoreEntry>> &entries) {
	switch(sorttype) {
		case SortType::TITLE:
			Ascending ? std::sort(entries.begin(), entries.end(), StoreUtils::compareTitleAscending) : std::sort(entries.begin(), entries.end(), StoreUtils::compareTitleDescending);
			break;

		case SortType::AUTHOR:
			Ascending ? std::sort(entries.begin(), entries.end(), StoreUtils::compareAuthorAscending) : std::sort(entries.begin(), entries.end(), StoreUtils::compareAuthorDescending);
			break;

		case SortType::LAST_UPDATED:
			Ascending ? std::sort(entries.begin(), entries.end(), StoreUtils::compareUpdateAscending) : std::sort(entries.begin(), entries.end(), StoreUtils::compareUpdateDescending);
			break;
	}
}

/*
	Find a query from a vector.
*/
static bool findInVector(const std::vector<std::string> &items, const std::string &query) {
	for(const std::string &item : items) {
		if (item.find(query) != std::string::npos) return true;
	}

	return false;
}

/*
	Search for stuff of the store.

	std::vector<std::unique_ptr<StoreEntry>> &entries: Reference to the entries.
	const std::string &query: What should be searched?
	bool title: Include titles?
	bool author: Include authors?
	bool category: Include categories?
	bool console: Include consoles?
	int selectedMarks: The selected marks.
*/

void StoreUtils::search(std::vector<std::unique_ptr<StoreEntry>> &entries, const std::string &query, bool title, bool author, bool category, bool console, int selectedMarks) {
	for (auto it = entries.begin(); it != entries.end(); ++it) {
		if (selectedMarks != 0) { // if not 0, do filter.
			if ((*it)->GetMarks() & selectedMarks) {
				if (!((title && (*it)->GetTitle().find(query) != std::string::npos)
				|| (author && (*it)->GetAuthor().find(query) != std::string::npos)
				|| (category && findInVector((*it)->GetCategoryFull(), query))
				|| (console && findInVector((*it)->GetConsoleFull(), query)))) {
					entries.erase(it);
					--it;
				}

			} else {
				entries.erase(it);
				--it;
			}

		} else { // Else without filter.
			if (!((title && (*it)->GetTitle().find(query) != std::string::npos)
			|| (author && (*it)->GetAuthor().find(query) != std::string::npos)
			|| (category && findInVector((*it)->GetCategoryFull(), query))
			|| (console && findInVector((*it)->GetConsoleFull(), query)))) {
				entries.erase(it);
				--it;
			}
		}
	}
}

/*
	Reset everything of the store and clear + fetch the Entries again.

	std::unique_ptr<Store> &store: Reference to the Store.
	std::unique_ptr<Meta> &meta: Reference to the meta.
	std::vector<std::unique_ptr<StoreEntry>> &entries: Reference to the entries.
*/
void StoreUtils::ResetAll(std::unique_ptr<Store> &store, std::unique_ptr<Meta> &meta, std::vector<std::unique_ptr<StoreEntry>> &entries) {
	if (store && store->GetValid()) {
		entries.clear();

		for (int i = 0; i < store->GetStoreSize(); i++) {
			entries.push_back( std::make_unique<StoreEntry>(store, meta, i) );
		}

		store->SetBox(0);
		store->SetEntry(0);
		store->SetScreenIndx(0);
	}
}