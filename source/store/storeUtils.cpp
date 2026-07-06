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
#include "queueSystem.hpp"
#include "storeUtils.hpp"

std::unique_ptr<Meta> StoreUtils::meta = nullptr;
std::unique_ptr<Store> StoreUtils::store = nullptr;
std::vector<std::shared_ptr<StoreEntry>> StoreUtils::allEntries, StoreUtils::entries;

/*
	Compare Title.

	const std::shared_ptr<StoreEntry> &a: Const Reference to Entry A.
	const std::shared_ptr<StoreEntry> &b: Const Reference to Entry B.
*/
bool StoreUtils::compareTitleDescending(const std::shared_ptr<StoreEntry> &a, const std::shared_ptr<StoreEntry> &b) {
	if (a && b) return strcasecmp(a->GetTitle().c_str(), b->GetTitle().c_str()) > 0;

	return true;
}
bool StoreUtils::compareTitleAscending(const std::shared_ptr<StoreEntry> &a, const std::shared_ptr<StoreEntry> &b) {
	if (a && b) return strcasecmp(a->GetTitle().c_str(), b->GetTitle().c_str()) < 0;

	return true;
}

/*
	Compare Author.

	const std::shared_ptr<StoreEntry> &a: Const Reference to Entry A.
	const std::shared_ptr<StoreEntry> &b: Const Reference to Entry B.
*/
bool StoreUtils::compareAuthorDescending(const std::shared_ptr<StoreEntry> &a, const std::shared_ptr<StoreEntry> &b) {
	if (!a || !b) return true;

	// Sort apps by the save author by title.
	int cmp = strcasecmp(a->GetAuthor().c_str(), b->GetAuthor().c_str());
	if (cmp == 0) return compareTitleDescending(a, b);

	return cmp > 0;
}
bool StoreUtils::compareAuthorAscending(const std::shared_ptr<StoreEntry> &a, const std::shared_ptr<StoreEntry> &b) {
	if (!a || !b) return true;

	// Sort apps by the save author by title.
	int cmp = strcasecmp(a->GetAuthor().c_str(), b->GetAuthor().c_str());
	if (cmp == 0) return compareTitleAscending(a, b);

	return cmp < 0;
}

/*
	Compare Last Updated.

	const std::shared_ptr<StoreEntry> &a: Const Reference to Entry A.
	const std::shared_ptr<StoreEntry> &b: Const Reference to Entry B.
*/
bool StoreUtils::compareUpdateDescending(const std::shared_ptr<StoreEntry> &a, const std::shared_ptr<StoreEntry> &b) {
	if (a && b) return strcasecmp(a->GetLastUpdated().c_str(), b->GetLastUpdated().c_str()) > 0;

	return true;
}
bool StoreUtils::compareUpdateAscending(const std::shared_ptr<StoreEntry> &a, const std::shared_ptr<StoreEntry> &b) {
	if (a && b) return strcasecmp(b->GetLastUpdated().c_str(), a->GetLastUpdated().c_str()) > 0;

	return true;
}

/*
	Compare Stars.

	const std::shared_ptr<StoreEntry> &a: Const Reference to Entry A.
	const std::shared_ptr<StoreEntry> &b: Const Reference to Entry B.
*/
bool StoreUtils::compareStarsDescending(const std::shared_ptr<StoreEntry> &a, const std::shared_ptr<StoreEntry> &b) {
	if (!a || !b) return true;

	// Sort by title within the same star count.
	// This is intentionally backwards because
	// descending star count is high to low and
	// ascending title is A-Z.
	if (a->GetStars() == b->GetStars())
		return compareTitleAscending(a, b);

	return a->GetStars() > b->GetStars();
}
bool StoreUtils::compareStarsAscending(const std::shared_ptr<StoreEntry> &a, const std::shared_ptr<StoreEntry> &b) {
	if (!a || !b) return true;

	// Sort by title within the same star count.
	// This is intentionally backwards because
	// ascending star count is low to high and
	// descending title is Z-A.
	if (a->GetStars() == b->GetStars())
		return compareTitleDescending(a, b);

	return a->GetStars() < b->GetStars();
}

/*
	Sort the entries.

	bool Ascending: If Ascending.
	SortType sorttype: The sort type.
*/
void StoreUtils::SortEntries() {
	bool Ascending = config->sortAscending();
	switch(config->sortBy()) {
		case SortType::TITLE:
			std::sort(StoreUtils::entries.begin(), StoreUtils::entries.end(), Ascending ? StoreUtils::compareTitleAscending : StoreUtils::compareTitleDescending);
			break;

		case SortType::AUTHOR:
			std::sort(StoreUtils::entries.begin(), StoreUtils::entries.end(), Ascending ? StoreUtils::compareAuthorAscending : StoreUtils::compareAuthorDescending);
			break;

		case SortType::LAST_UPDATED:
			std::sort(StoreUtils::entries.begin(), StoreUtils::entries.end(), Ascending ? StoreUtils::compareUpdateAscending : StoreUtils::compareUpdateDescending);
			break;

		case SortType::POPULARITY:
			std::sort(StoreUtils::entries.begin(), StoreUtils::entries.end(), Ascending ? StoreUtils::compareStarsAscending : StoreUtils::compareStarsDescending);
			break;
	}
}

/*
	Find a query from a vector.

	const std::vector<std::string> &items: Const Reference to the vector strings / items.
	const std::string &query: Const Reference to the query.
*/
static bool findInVector(const std::vector<std::string> &items, const std::string &query) {
	for(const std::string &item : items) {
		if (item == query) return true;
	}

	return false;
}

/*
	Search for stuff of the store.

	const std::string &titleQuery: The title to search for, "" to ignore.
	const std::string &descQuery: The description to search for, "" to ignore.
	const std::string &authorQuery: The author to search for, "" to ignore.
	const std::string &category: The category to search for, "" to ignore.
	const std::string &console: The console to search for, "" to ignore.
	int selectedMarks: The selected mark flags.
	bool updateAvl: if available updates should be an included flag.
	bool installed: if the app being installed should be an included flag.
	bool isAND: if using AND or OR mode.
	bool isNOT: if the selection should be inverted.
*/
void StoreUtils::search(std::string titleQuery, std::string descQuery, std::string authorQuery, std::string category, std::string console, int selectedMarks, bool updateAvl, bool installed, bool isAND, bool isNOT) {
	titleQuery = StringUtils::toUpperCase(titleQuery);
	descQuery = StringUtils::toUpperCase(descQuery);
	authorQuery = StringUtils::toUpperCase(authorQuery);
	bool skipTextQuery = titleQuery.empty() && descQuery.empty() && authorQuery.empty();

	for (auto it = StoreUtils::entries.begin(); it != StoreUtils::entries.end(); ++it) {
		bool textOK = (
			skipTextQuery
			|| (!titleQuery.empty() && StringUtils::toUpperCase((*it)->GetTitle()).find(titleQuery) != std::string::npos)
			|| (!descQuery.empty() && StringUtils::toUpperCase((*it)->GetDescription()).find(descQuery) != std::string::npos)
			|| (!authorQuery.empty() && StringUtils::toUpperCase((*it)->GetAuthor()).find(authorQuery) != std::string::npos)
		);

		bool categoryOK = category.empty() || findInVector((*it)->GetCategories(), category);
		bool consoleOK = console.empty() || findInVector((*it)->GetConsoles(), console);

		bool filtersOK = selectedMarks == 0 && !updateAvl && !installed; // true if no filters selected
		if (!filtersOK) { // otherwise check the filters
			if(isAND) {
				filtersOK = isNOT ^ (
					(((*it)->GetMarks() & selectedMarks) == selectedMarks)
					&& (!updateAvl || (*it)->GetUpdateAvl())
					&& (!installed || (*it)->GetInstalled())
				);
			} else {
				filtersOK = isNOT ^ (
					(((*it)->GetMarks() & selectedMarks))
					|| (updateAvl && (*it)->GetUpdateAvl())
					|| (installed && (*it)->GetInstalled())
				);
			}
		}

		// If anything didn't pass, filter out
		if (!(textOK && categoryOK && consoleOK && filtersOK)) {
			it = StoreUtils::entries.erase(it);
			--it;
		}
	}
}

/* Reset everything of the store and clear + fetch the entries again. */
void StoreUtils::ResetEntries() {
	if (StoreUtils::store) {
		StoreUtils::entries.clear();

		if (StoreUtils::store->GetValid()) {
			for (const std::shared_ptr<StoreEntry> &entry : StoreUtils::allEntries) {
				StoreUtils::entries.emplace_back(entry);
			}

			StoreUtils::store->SetBox(0);
			StoreUtils::store->SetEntry(0);
			StoreUtils::store->SetScreenIndx(0);
		}
	}
}

/* Re-check all Entries for available updates and installed status. */
void StoreUtils::RefreshInstalledApps(const std::string &nameFilter) {
	for (const std::shared_ptr<StoreEntry> &entry : StoreUtils::allEntries) {
		if (entry && (nameFilter == "" || entry->GetTitle() == nameFilter)) {
			entry->SetUpdateAvl(StoreUtils::meta->UpdateAvailable(entry->GetUniStore(), entry->GetTitle(), entry->GetLastUpdated()));
			entry->SetInstalled(entry->CheckInstalled());
			for (Script &script : entry->GetScripts()) {
				script.SetInstalled(StoreUtils::meta->GetInstalled(entry->GetUniStore(), entry->GetTitle(), script.GetName()));
			}
		}
	}
}

/*
	Add all update-able entries to the queue.
*/
void StoreUtils::AddAllToQueue() {
	if (StoreUtils::store && StoreUtils::store->GetValid() && StoreUtils::meta && !StoreUtils::entries.empty()) { // Ensure all is valid.
		for (const std::shared_ptr<StoreEntry> &entry : StoreUtils::entries) {
			if (!entry) continue; // Ensure pointer is valid.
			for(size_t i = 0; i < entry->GetScripts().size(); i++) {
				if(entry->GetScript(i).IsInstalled()) {
					QueueSystem::AddToQueue(entry, i);
				}
			}
		}
	}
}