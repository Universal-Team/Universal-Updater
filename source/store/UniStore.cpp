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

#include "UniStore.hpp"


/*
	Initialize a UniStore.

	const std::string &FullPath: The full path to the UniStore.
	const std::string &FileName: Same as above, but just the UniStore filename.
*/
UniStore::UniStore(const std::string &FullPath, const std::string &FileName) {
	if (FullPath.length() > 4) {
		if (*(uint32_t *)(FullPath.c_str() + FullPath.length() - 4) == (0xE0DED0E << 3 | (2 + 1))) {
			this->Valid = false;
			return;
		}
	}

	this->FileName = FileName;
	this->LoadUniStore(FullPath);
};


/*
	The destructor of the UniStore class.

	This basically unloads all the Spritesheets and such that have been loaded to free up the RAM.
*/
UniStore::~UniStore() {

};


/*
	Updates a UniStore inclusive spritesheets to it's latest version.

	const std::string &File: The file of the UniStore to update.

	This function is empty, until DSi-WiFi is properly usable.
*/
void UniStore::UpdateUniStore(const std::string &File) {

};


/*
	Load a UniStore from a file.

	const std::string &File: The file of the UniStore.
*/
void UniStore::LoadUniStore(const std::string &File) {
	FILE *In = fopen(File.c_str(), "rt");

	if (!In) {
		this->Valid = false;
		return;
	}

	/* This seems to crash for some reason. */
	this->UniStoreJSON = nlohmann::json::parse(In, nullptr, false);
	fclose(In);

	if (this->UniStoreJSON.is_discarded()) {
		this->Valid = false;
		this->UniStoreJSON = { }; // Oops, invalid.
		return;
	}

	/* Check, if valid. */
	if (this->UniStoreJSON.contains("storeInfo") && this->UniStoreJSON.contains("storeContent")) {
		if (this->UniStoreJSON["storeInfo"].contains("version") && this->UniStoreJSON["storeInfo"]["version"].is_number()) {
			if (this->UniStoreJSON["storeInfo"]["version"] < 3) printf("UniStore too Old!");
			else if (this->UniStoreJSON["storeInfo"]["version"] > this->UNISTORE_VERSION) printf("UniStore too New!");

			else if (this->UniStoreJSON["storeInfo"]["version"] == 3 || this->UniStoreJSON["storeInfo"]["version"] == this->UNISTORE_VERSION) {
				this->Valid = true;
			}
		}

	} else {
		printf("UniStore invalid!");
	}
};


/*
	Load the UniStore spritesheets for the icons into RAM.

	TODO: Find a good way to handle this for 3DS and NDS cleanly.
*/
void UniStore::LoadSpriteSheets() {

};


/*
	Return the Title of the UniStore.
*/
std::string UniStore::GetUniStoreTitle() const {
	if (this->Valid) {
		if (this->UniStoreJSON["storeInfo"].contains("title")) return this->UniStoreJSON["storeInfo"]["title"];
	}

	return "";
};


/*
	Return the Title of an index.

	const int Idx: The index.
*/
std::string UniStore::GetEntryTitle(const int Idx) const {
	if (!this->Valid) return "";
	if (Idx > (int)this->UniStoreJSON["storeContent"].size() - 1) return ""; // Empty.

	if (this->UniStoreJSON["storeContent"][Idx]["info"].contains("title") && this->UniStoreJSON["storeContent"][Idx]["info"]["title"].is_string()) {
		return this->UniStoreJSON["storeContent"][Idx]["info"]["title"];
	}

	return "";
};


/*
	Return the Author name of an index.

	const int Idx: The index.
*/
std::string UniStore::GetEntryAuthor(const int Idx) const {
	if (!this->Valid) return "";
	if (Idx > (int)this->UniStoreJSON["storeContent"].size() - 1) return ""; // Empty.

	if (this->UniStoreJSON["storeContent"][Idx]["info"].contains("author") && this->UniStoreJSON["storeContent"][Idx]["info"]["author"].is_string()) {
		return this->UniStoreJSON["storeContent"][Idx]["info"]["author"];
	}

	return "";
};


/*
	Return the Description of an index.
	const int Idx: The index.
*/
std::string UniStore::GetEntryDescription(const int Idx) const {
	if (!this->Valid) return "";
	if (Idx > (int)this->UniStoreJSON["storeContent"].size() - 1) return ""; // Empty.

	if (this->UniStoreJSON["storeContent"][Idx]["info"].contains("description") && this->UniStoreJSON["storeContent"][Idx]["info"]["description"].is_string()) {
		return this->UniStoreJSON["storeContent"][Idx]["info"]["description"];
	}

	return "";
};


/*
	Return the Categories of an index.

	const int Idx: The index.
*/
std::vector<std::string> UniStore::GetEntryCategories(const int Idx) const {
	if (!this->Valid) return { "" };
	if (Idx > (int)this->UniStoreJSON["storeContent"].size() - 1) return { "" }; // Empty.

	if (this->UniStoreJSON["storeContent"][Idx]["info"].contains("category")) {
		if (this->UniStoreJSON["storeContent"][Idx]["info"]["category"].is_array()) {
			return this->UniStoreJSON["storeContent"][Idx]["info"]["category"].get<std::vector<std::string>>();

		} else if (this->UniStoreJSON["storeContent"][Idx]["info"]["category"].is_string()) {
			std::vector<std::string> Temp;
			Temp.push_back( this->UniStoreJSON["storeContent"][Idx]["info"]["category"] );

			return Temp;
		}
	}

	return { "" };
};


/*
	Return the Version of an index.

	const int Idx: The index.
*/
std::string UniStore::GetEntryVersion(const int Idx) const {
	if (!this->Valid) return "";
	if (Idx > (int)this->UniStoreJSON["storeContent"].size() - 1) return ""; // Empty.

	if (this->UniStoreJSON["storeContent"][Idx]["info"].contains("version") && this->UniStoreJSON["storeContent"][Idx]["info"]["version"].is_string()) {
		return this->UniStoreJSON["storeContent"][Idx]["info"]["version"];
	}

	return "";
};


/*
	Return the Consoles of an index.

	const int Idx: The index.
*/
std::vector<std::string> UniStore::GetEntryConsoles(const int Idx) const {
	if (!this->Valid) return { "" };
	if (Idx > (int)this->UniStoreJSON["storeContent"].size() - 1) return { "" }; // Empty.

	if (this->UniStoreJSON["storeContent"][Idx]["info"].contains("console")) {
		if (this->UniStoreJSON["storeContent"][Idx]["info"]["console"].is_array()) {
			return this->UniStoreJSON["storeContent"][Idx]["info"]["console"].get<std::vector<std::string>>();

		} else if (this->UniStoreJSON["storeContent"][Idx]["info"]["console"].is_string()) {
			std::vector<std::string> Temp;
			Temp.push_back( this->UniStoreJSON["storeContent"][Idx]["info"]["console"] );

			return Temp;
		}
	}

	return { "" };
};


/*
	Return the Last Updated date of an index.

	const int Idx: The index.
*/
std::string UniStore::GetEntryLastUpdated(const int Idx) const {
	if (!this->Valid) return "";
	if (Idx > (int)this->UniStoreJSON["storeContent"].size() - 1) return ""; // Empty.

	if (this->UniStoreJSON["storeContent"][Idx]["info"].contains("last_updated") && this->UniStoreJSON["storeContent"][Idx]["info"]["last_updated"].is_string()) {
		return this->UniStoreJSON["storeContent"][Idx]["info"]["last_updated"];
	}

	return "";
};


/*
	Return the License of an index.

	const int Idx: The index.
*/
std::string UniStore::GetEntryLicense(const int Idx) const {
	if (!this->Valid) return "";
	if (Idx > (int)this->UniStoreJSON["storeContent"].size() - 1) return ""; // Empty.

	if (this->UniStoreJSON["storeContent"][Idx]["info"].contains("license") && this->UniStoreJSON["storeContent"][Idx]["info"]["license"].is_string()) {
		return this->UniStoreJSON["storeContent"][Idx]["info"]["license"];
	}

	return "";
};


/*
	Return the download list of an entry.

	const int Idx: The index.
*/
std::vector<std::string> UniStore::GetDownloadList(const int Idx) const {
	if (!this->Valid) return { "" };

	std::vector<std::string> Temp;
	if (Idx > (int)this->UniStoreJSON["storeContent"].size() - 1) return Temp;

	for(auto It = this->UniStoreJSON.at("storeContent").at(Idx).begin(); It != this->UniStoreJSON.at("storeContent").at(Idx).end(); It++) {
		if (It.key() != "info") Temp.push_back(It.key());
	}

	return Temp;
};


/*
	Return filesizes for each download entry.

	const int Idx: The index.
	const std::string &Entry: The entry name.
*/
std::string UniStore::GetFileSizes(const int Idx, const std::string &Entry) const {
	if (!this->Valid) return "";

	if (Idx > (int)this->UniStoreJSON["storeContent"].size() - 1) return "";

	if (this->UniStoreJSON["storeContent"][Idx].contains(Entry) && this->UniStoreJSON["storeContent"][Idx][Entry].type() == nlohmann::json::value_t::object) {
		if (this->UniStoreJSON["storeContent"][Idx][Entry].contains("size") && this->UniStoreJSON["storeContent"][Idx][Entry]["size"].is_string()) {
			return this->UniStoreJSON["storeContent"][Idx][Entry]["size"];
		}
	}

	return "";
};


/*
	Return Screenshot URL list.

	const int Idx: The Entry Index.
*/
std::vector<std::string> UniStore::GetScreenshotList(const int Idx) const {
	if (!this->Valid) return { };

	if (Idx > (int)this->UniStoreJSON["storeContent"].size() - 1) return { };

	std::vector<std::string> Screenshots;

	if (this->UniStoreJSON["storeContent"][Idx]["info"].contains("screenshots")) {
		if (this->UniStoreJSON["storeContent"][Idx]["info"]["screenshots"].is_array()) {
			for(auto &Itm : this->UniStoreJSON["storeContent"][Idx]["info"]["screenshots"]) {
				if (Itm.is_object() && Itm.contains("url")) Screenshots.push_back(Itm["url"]);
				else Screenshots.push_back("");
			}
		}
	}

	return Screenshots;
};


/*
	Return Screenshot names.

	const int Idx: The Entry Index.
*/
std::vector<std::string> UniStore::GetScreenshotNames(const int Idx) const {
	if (!this->Valid) return { };

	if (Idx > (int)this->UniStoreJSON["storeContent"].size() - 1) return { };

	std::vector<std::string> Screenshots;

	if (this->UniStoreJSON["storeContent"][Idx]["info"].contains("screenshots")) {
		if (this->UniStoreJSON["storeContent"][Idx]["info"]["screenshots"].is_array()) {
			for(auto &Itm : this->UniStoreJSON["storeContent"][Idx]["info"]["screenshots"]) {
				if (Itm.is_object() && Itm.contains("description")) Screenshots.push_back(Itm["description"]);
				else Screenshots.push_back("");
			}
		}
	}

	return Screenshots;
};


/*
	Return the update notes of an entry.

	const int Idx: The index.
*/
std::string UniStore::GetReleaseNotes(const int Idx) const {
	if (!this->Valid) return "";
	if (Idx > (int)this->UniStoreJSON["storeContent"].size() - 1) return ""; // Empty.

	if (this->UniStoreJSON["storeContent"][Idx]["info"].contains("releasenotes") && this->UniStoreJSON["storeContent"][Idx]["info"]["releasenotes"].is_string()) {
		return this->UniStoreJSON["storeContent"][Idx]["info"]["releasenotes"];
	}

	return "";
};