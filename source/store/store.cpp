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

#include "common.hpp"
#include "gui.hpp"
#include "store.hpp"

#include <unistd.h>

extern C2D_SpriteSheet sprites;

/*
	Initialize a store.

	std::string file: The UniStore file.
*/
Store::Store(std::string file) {
	this->LoadFromFile(file);
	this->loadSheets();
}

/*
	Unload all SpriteSheets on Destructor.
*/
Store::~Store() { this->unloadSheets(); };

/*
	Unload all SpriteSheets.
*/
void Store::unloadSheets() {
	if (this->sheets.size() > 0) {
		for (int i = 0; i < (int)this->sheets.size(); i++) {
			if (this->sheets[i]) C2D_SpriteSheetFree(this->sheets[i]);
		}
	}

	this->sheets.clear();
}

/*
	Load all SpriteSheets.
*/
void Store::loadSheets() {
	if (this->storeJson["storeInfo"].contains("sheet")) {
		this->unloadSheets();

		if (this->storeJson["storeInfo"].contains("sheet")) {
			std::vector<std::string> sheetLocs = { "" };

			if (this->storeJson["storeInfo"]["sheet"].is_array()) {
				sheetLocs = this->storeJson["storeInfo"]["sheet"].get<std::vector<std::string>>();

			} else {
				sheetLocs[0] = this->storeJson["storeInfo"]["sheet"];
			}

			for (int i = 0; i < (int)sheetLocs.size(); i++) {
				this->sheets.push_back({ });
				this->sheets[i] = C2D_SpriteSheetLoad(sheetLocs[i].c_str());
			}
		}
	}
}


/*
	Load a UniStore from a file.

	std::string file: The file of the UniStore.
*/
void Store::LoadFromFile(std::string file) {
	FILE *temp = fopen(file.c_str(), "rt");
	this->storeJson = nlohmann::json::parse(temp, nullptr, false);
	fclose(temp);
}

/*
	Return the Title of the UniStore.
*/
std::string Store::GetUniStoreTitle() const {
	if (this->storeJson["storeInfo"].contains("title")) return this->storeJson["storeInfo"]["title"];

	return "";
}

/*
	Return the Title of an index.

	int index: The index.
*/
std::string Store::GetTitleEntry(int index) const {
	if (index > (int)this->storeJson["storeContent"].size() - 1) return ""; // Empty.

	if (this->storeJson["storeContent"][index]["info"].contains("title") && this->storeJson["storeContent"][index]["info"]["title"].is_string()) {
		return this->storeJson["storeContent"][index]["info"]["title"];
	}

	return "";
}

/*
	Return the Author name of an index.

	int index: The index.
*/
std::string Store::GetAuthorEntry(int index) const {
	if (index > (int)this->storeJson["storeContent"].size() - 1) return ""; // Empty.

	if (this->storeJson["storeContent"][index]["info"].contains("author") && this->storeJson["storeContent"][index]["info"]["author"].is_string()) {
		return this->storeJson["storeContent"][index]["info"]["author"];
	}

	return "";
}

/*
	Return the Description of an index.

	int index: The index.
*/
std::string Store::GetDescriptionEntry(int index) const {
	if (index > (int)this->storeJson["storeContent"].size() - 1) return ""; // Empty.

	if (this->storeJson["storeContent"][index]["info"].contains("description") && this->storeJson["storeContent"][index]["info"]["description"].is_string()) {
		return this->storeJson["storeContent"][index]["info"]["description"];
	}

	return "";
}

/*
	Return the Category of an index.

	int index: The index.
*/
std::vector<std::string> Store::GetCategoryIndex(int index) const {
	if (index > (int)this->storeJson["storeContent"].size() - 1) return { "" }; // Empty.

	if (this->storeJson["storeContent"][index]["info"].contains("category")) {
		if (this->storeJson["storeContent"][index]["info"]["category"].is_array()) {
			return this->storeJson["storeContent"][index]["info"]["category"].get<std::vector<std::string>>();

		} else if (this->storeJson["storeContent"][index]["info"]["category"].is_string()) {
			std::vector<std::string> temp;
			temp.push_back( this->storeJson["storeContent"][index]["info"]["category"] );
			return temp;
		}
	}

	return { "" };
}

/*
	Return the Version of an index.

	int index: The index.
*/
std::string Store::GetVersionEntry(int index) const {
	if (index > (int)this->storeJson["storeContent"].size() - 1) return ""; // Empty.

	if (this->storeJson["storeContent"][index]["info"].contains("version") && this->storeJson["storeContent"][index]["info"]["version"].is_string()) {
		return this->storeJson["storeContent"][index]["info"]["version"];
	}

	return "";
}

/*
	Return the Console of an index.

	int index: The index.
*/
std::vector<std::string> Store::GetConsoleEntry(int index) const {
	if (index > (int)this->storeJson["storeContent"].size() - 1) return { "" }; // Empty.

	if (this->storeJson["storeContent"][index]["info"].contains("console")) {
		if (this->storeJson["storeContent"][index]["info"]["console"].is_array()) {
			return this->storeJson["storeContent"][index]["info"]["console"].get<std::vector<std::string>>();

		} else if (this->storeJson["storeContent"][index]["info"]["console"].is_string()) {
			std::vector<std::string> temp;
			temp.push_back( this->storeJson["storeContent"][index]["info"]["console"] );
			return temp;
		}
	}

	return { "" };
}

/*
	Return the Last updated date of an index.

	int index: The index.
*/
std::string Store::GetLastUpdatedEntry(int index) const {
	if (index > (int)this->storeJson["storeContent"].size() - 1) return ""; // Empty.

	if (this->storeJson["storeContent"][index]["info"].contains("last_updated") && this->storeJson["storeContent"][index]["info"]["last_updated"].is_string()) {
		return this->storeJson["storeContent"][index]["info"]["last_updated"];
	}

	return "";
}

/*
	Return the License of an index.

	int index: The index.
*/
std::string Store::GetLicenseEntry(int index) const {
	if (index > (int)this->storeJson["storeContent"].size() - 1) return ""; // Empty.

	if (this->storeJson["storeContent"][index]["info"].contains("license") && this->storeJson["storeContent"][index]["info"]["license"].is_string()) {
		return this->storeJson["storeContent"][index]["info"]["license"];
	}

	return "";
}

/*
	Return the size of an index.

	int index: The index.
*/
int Store::GetSizeEntry(int index) const {
	if (index > (int)this->storeJson["storeContent"].size() - 1) return 0; // Empty.

	if (this->storeJson["storeContent"][index]["info"].contains("size") && this->storeJson["storeContent"][index]["info"]["size"].is_number()) {
		return this->storeJson["storeContent"][index]["info"]["size"];
	}

	return 0;
}

/*
	Return a C2D_Image of an index.

	int index: The index.
*/
C2D_Image Store::GetIconEntry(int index) const {
	int iconIndex = -1, sheetIndex = 0;

	if (index > (int)this->storeJson["storeContent"].size() - 1) return C2D_SpriteSheetGetImage(sprites, sprites_noIcon_idx);

	if (this->storeJson["storeContent"][index]["info"].contains("icon_index") && this->storeJson["storeContent"][index]["info"]["icon_index"].is_number()) {
		iconIndex = this->storeJson["storeContent"][index]["info"]["icon_index"];
	}

	if (this->storeJson["storeContent"][index]["info"].contains("sheet_index") && this->storeJson["storeContent"][index]["info"]["sheet_index"].is_number()) {
		sheetIndex = this->storeJson["storeContent"][index]["info"]["sheet_index"];
	}

	if (iconIndex == -1) return C2D_SpriteSheetGetImage(sprites, sprites_noIcon_idx);

	if (sheetIndex > (int)this->sheets.size()) return C2D_SpriteSheetGetImage(sprites, sprites_noIcon_idx);
	if (iconIndex > (int)C2D_SpriteSheetCount(this->sheets[sheetIndex])-1) return C2D_SpriteSheetGetImage(sprites, sprites_noIcon_idx);

	C2D_Image temp = C2D_SpriteSheetGetImage(this->sheets[sheetIndex], iconIndex);
	if (temp.subtex->width < 49 && temp.subtex->height < 49) return temp; // up to 48x48 is valid.

	return C2D_SpriteSheetGetImage(sprites, sprites_noIcon_idx);
}

/*
	Return the download list of an entry.

	int index: The index.
*/
std::vector<std::string> Store::GetDownloadList(int index) const {
	std::vector<std::string> temp;

	if (index > (int)this->storeJson["storeContent"].size() - 1) return temp;

	for(auto it = this->storeJson.at("storeContent").at(index).begin(); it != this->storeJson.at("storeContent").at(index).end(); it++) {
		if (it.key() != "info") {
			temp.push_back(it.key());
		}
	}

	return temp;
}