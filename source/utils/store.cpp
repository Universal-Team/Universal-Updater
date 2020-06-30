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

#include "store.hpp"
#include <unistd.h>

Store::Store(nlohmann::json &JS, std::string JSONName) {
	this->storeJson = JS;
	this->updateFile = JSONName;

	if (access("sdmc:/3ds/Universal-Updater/updates.json", F_OK) != 0) {
		// We'd create the file here.
		FILE *file = fopen("sdmc:/3ds/Universal-Updater/updates.json", "w");
		this->updateJSON = nlohmann::json::parse("{}"); // So we have a valid JSON at the end.
		fwrite(this->updateJSON.dump(1, '\t').c_str(), 1, this->updateJSON.dump(1, '\t').size(), file);
		fclose(file);

		FILE *file2 = fopen("sdmc:/3ds/Universal-Updater/updates.json", "r");
		this->updateJSON = nlohmann::json::parse(file2, nullptr, false);
		fclose(file2);
	} else {
		FILE *file = fopen("sdmc:/3ds/Universal-Updater/updates.json", "r");
		this->updateJSON = nlohmann::json::parse(file, nullptr, false);
		fclose(file);
	}


	for (int i = 0; i < (int)this->storeJson.at("storeContent").size(); i++) {
		this->unsortedStore.push_back(this->getData(i));
	}

	this->sortedStore = this->unsortedStore; // Put that to sorted store as well.

	// If Categories available, push them to our vector.
	if (this->storeJson["storeInfo"].contains("categories")) {
		this->availableCategories = this->storeJson["storeInfo"]["categories"].get<std::vector<std::string>>();
	}

	// If Authors available, push them to our vector.
	if (this->storeJson["storeInfo"].contains("authors")) {
		this->availableAuthors = this->storeJson["storeInfo"]["authors"].get<std::vector<std::string>>();
	}

	// If Systems available, push them to our vector.
	if (this->storeJson["storeInfo"].contains("consoles")) {
		this->availableSystems = this->storeJson["storeInfo"]["consoles"].get<std::vector<std::string>>();
	}
}

bool Store::updateAvailable(int index) {
	if (index > (int)this->storeJson.at("storeContent").size()) return false; // out of scope.
	if (this->storeJson["storeContent"][index]["info"].contains("last_updated")) {
		const std::string updateEntry = this->storeJson["storeContent"][index]["info"]["last_updated"];
		const std::string entry = this->storeJson["storeContent"][index]["info"]["title"];

		if (this->updateJSON.contains(this->updateFile)) {
			if (this->updateJSON[this->updateFile].contains(entry)) {
				const std::string updateEntry2 = (std::string)this->updateJSON[this->updateFile][entry];
				return strcasecmp(updateEntry.c_str(), updateEntry2.c_str()) > 0;
			} else {
				return false; // Since we do not have this entry there yet.
			}
		} else { // Our update json don't have that yet.. so display available.
			return false;
		}
	} else { // Since the Store doesn't have that feature.
		return false;
	}

	return false;
}

// Here we write that to our file.
void Store::writeToFile(int index) {
	FILE *file = fopen("sdmc:/3ds/Universal-Updater/updates.json", "w");
	this->updateJSON[this->updateFile][this->sortedStore[index].title] = this->sortedStore[index].last_updated;
	const std::string dump = this->updateJSON.dump(1, '\t');
	fwrite(dump.c_str(), 1, this->updateJSON.dump(1, '\t').size(), file);
	fclose(file);

	this->sortedStore[index].updateAvailable = false;
}

// Here we get the data of the UniStore!
UniStoreV2Struct Store::getData(const int index) {
	UniStoreV2Struct temp = {"", "", "", "", "" ,"", -1, 0, false};

	if (index > (int)this->storeJson["storeContent"].size()) return temp; // Empty.

	// Here we check.
	// Title.
	if (this->storeJson["storeContent"][index]["info"].contains("title")) {
		temp.title = this->storeJson["storeContent"][index]["info"]["title"];
	}

	// Author.
	if (this->storeJson["storeContent"][index]["info"].contains("author")) {
		temp.author = this->storeJson["storeContent"][index]["info"]["author"];
	}

	// Version.
	if (this->storeJson["storeContent"][index]["info"].contains("version")) {
		temp.version = this->storeJson["storeContent"][index]["info"]["version"];
	}

	if (this->storeJson["storeContent"][index]["info"].contains("category")) {
		temp.category = this->storeJson["storeContent"][index]["info"]["category"];
	}

	// Console.
	if (this->storeJson["storeContent"][index]["info"].contains("console")) {
		temp.console = this->storeJson["storeContent"][index]["info"]["console"];
	}

	// Last updated.
	if (this->storeJson["storeContent"][index]["info"].contains("last_updated")) {
		temp.last_updated = this->storeJson["storeContent"][index]["info"]["last_updated"];
	}

	// Icon index.
	if (this->storeJson["storeContent"][index]["info"].contains("icon_index")) {
		temp.icon_index = this->storeJson["storeContent"][index]["info"]["icon_index"];
	}

	// Update available(?).
	temp.updateAvailable = this->updateAvailable(index);

	// JSON index.
	temp.JSONIndex = index;
	
	return temp;
}

int Store::searchForCategory(const std::string searchResult) {
	std::vector<UniStoreV2Struct> temp;

	for (int i = 0; i < (int)this->sortedStore.size(); i++) {
		if (this->sortedStore[i].category == searchResult) {
			temp.push_back({this->sortedStore[i]});
		}
	}

	if (temp.size() != 0) {
		this->sortedStore = temp;
	}

	return (int)temp.size();
}

int Store::searchForConsole(const std::string searchResult) {
	std::vector<UniStoreV2Struct> temp;

	for (int i = 0; i < (int)this->sortedStore.size(); i++) {
		if (this->sortedStore[i].console == searchResult) {
			temp.push_back({this->sortedStore[i]});
		}
	}

	if (temp.size() != 0) {
		this->sortedStore = temp;
	}

	return (int)temp.size();
}

int Store::searchForAuthor(const std::string searchResult) {
	std::vector<UniStoreV2Struct> temp;

	for (int i = 0; i < (int)this->sortedStore.size(); i++) {
		if (this->sortedStore[i].author == searchResult) {
			temp.push_back({this->sortedStore[i]});
		}
	}

	if (temp.size() != 0) {
		this->sortedStore = temp;
	}

	return (int)temp.size();
}

int Store::searchForEntries(const std::string searchResult) {
	std::vector<UniStoreV2Struct> temp;

	for (int i = 0; i < (int)this->sortedStore.size(); i++) {
		if (this->sortedStore[i].title.find(searchResult) != std::string::npos) {
			temp.push_back({this->sortedStore[i]});
		}
	}

	if (temp.size() != 0) {
		this->sortedStore = temp;
	}

	return (int)temp.size();
}

// Title.
bool compareTitleDescending(const UniStoreV2Struct& a, const UniStoreV2Struct& b) {
	return strcasecmp(a.title.c_str(), b.title.c_str()) > 0;
}
bool compareTitleAscending(const UniStoreV2Struct& a, const UniStoreV2Struct& b) {
	return strcasecmp(b.title.c_str(), a.title.c_str()) > 0;
}

// Author.
bool compareAuthorDescending(const UniStoreV2Struct& a, const UniStoreV2Struct& b) {
	return strcasecmp(a.author.c_str(), b.author.c_str()) > 0;
}
bool compareAuthorAscending(const UniStoreV2Struct& a, const UniStoreV2Struct& b) {
	return strcasecmp(b.author.c_str(), a.author.c_str()) > 0;
}

// Last updated.
bool compareUpdateDescending(const UniStoreV2Struct& a, const UniStoreV2Struct& b) {
	return strcasecmp(a.last_updated.c_str(), b.last_updated.c_str()) > 0;
}
bool compareUpdateAscending(const UniStoreV2Struct& a, const UniStoreV2Struct& b) {
	return strcasecmp(b.last_updated.c_str(), a.last_updated.c_str()) > 0;
}

void Store::sorting(bool Ascending, SortType sorttype) {
	this->ascending = Ascending;
	this->sorttype = sorttype;
	switch(this->sorttype) {
		case SortType::TITLE:
			if (Ascending) {
				std::sort(this->sortedStore.begin(), this->sortedStore.end(), compareTitleAscending);
			} else {
				std::sort(this->sortedStore.begin(), this->sortedStore.end(), compareTitleDescending);
			}
			break;
		case SortType::AUTHOR:
			if (Ascending) {
				std::sort(this->sortedStore.begin(), this->sortedStore.end(), compareAuthorAscending);
			} else {
				std::sort(this->sortedStore.begin(), this->sortedStore.end(), compareAuthorDescending);
			}
			break;
		case SortType::LAST_UPDATED:
			if (Ascending) {
				std::sort(this->sortedStore.begin(), sortedStore.end(), compareUpdateAscending);
			} else {
				std::sort(this->sortedStore.begin(), sortedStore.end(), compareUpdateDescending);
			}
			break;
	}
}

// Some return stuff with checks!
std::string Store::returnTitle(const int index) {
	if (index > (int)this->sortedStore.size())	return "?"; // Out of scope.
	return this->sortedStore[index].title;
}

std::string Store::returnAuthor(const int index) {
	if (index > (int)this->sortedStore.size())	return "?"; // Out of scope.
	return this->sortedStore[index].author;
}

int Store::returnIconIndex(const int index) {
	if (index > (int)this->sortedStore.size())	return -1; // Out of scope.
	return this->sortedStore[index].icon_index;
}

int Store::returnJSONIndex(const int index) {
	if (index > (int)this->sortedStore.size())	return -1; // Out of scope.
	return this->sortedStore[index].JSONIndex;
}

int Store::getSize() { return (int)this->sortedStore.size(); }