/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

Store::Store(nlohmann::json &JS) {
	this->storeJson = JS;
	for (int i = 0; i < (int)this->storeJson.at("storeContent").size(); i++) {
		this->unsortedStore.push_back(this->getData(i));
	}

	this->sortedStore = this->unsortedStore; // Put that to sorted store as well.
	if (this->storeJson.at("storeInfo").contains("categories")) {
		this->availableCategories = this->storeJson["storeInfo"]["categories"].get<std::vector<std::string>>();
	}
}

// Here we get the data of the UniStore!
UniStoreV2Struct Store::getData(const int index) {
	UniStoreV2Struct temp = {"", "", "", "", "" ,"", -1, 0};

	if (index > (int)this->storeJson.at("storeContent").size()) return temp; // Empty.

	// Here we check.
	// Title.
	if (this->storeJson.at("storeContent").at(index).at("info").contains("title")) {
		temp.title = this->storeJson.at("storeContent").at(index).at("info").at("title");
	}

	// Author.
	if (this->storeJson.at("storeContent").at(index).at("info").contains("author")) {
		temp.author = this->storeJson.at("storeContent").at(index).at("info").at("author");
	}

	// Version.
	if (this->storeJson.at("storeContent").at(index).at("info").contains("version")) {
		temp.version = this->storeJson.at("storeContent").at(index).at("info").at("version");
	}

	if (this->storeJson.at("storeContent").at(index).at("info").contains("category")) {
		temp.category = this->storeJson.at("storeContent").at(index).at("info").at("category");
	}

	// Console.
	if (this->storeJson.at("storeContent").at(index).at("info").contains("console")) {
		temp.console = this->storeJson.at("storeContent").at(index).at("info").at("console");
	}

	// Last updated.
	if (this->storeJson.at("storeContent").at(index).at("info").contains("last_updated")) {
		temp.last_updated = this->storeJson.at("storeContent").at(index).at("info").at("last_updated");
	}

	// Icon index.
	if (this->storeJson.at("storeContent").at(index).at("info").contains("icon_index")) {
		temp.icon_index = this->storeJson.at("storeContent").at(index).at("info").at("icon_index");
	}

	// JSON index.
	temp.JSONIndex = index;
	
	return temp;
}

int Store::searchForCategory(const std::string searchResult) {
	std::vector<UniStoreV2Struct> temp;

	for (int i = 0; i < (int)this->sortedStore.size(); i++) {
		if (this->sortedStore[i].category.find(searchResult) != std::string::npos) {
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
		if (this->sortedStore[i].console.find(searchResult) != std::string::npos) {
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
		if (this->sortedStore[i].author.find(searchResult) != std::string::npos) {
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