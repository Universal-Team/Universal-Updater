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
#include "download.hpp"
#include "gui.hpp"
#include "scriptUtils.hpp"
#include "store.hpp"

#include <unistd.h>

extern C2D_SpriteSheet sprites;
extern bool checkWifiStatus();
static bool firstStart = true;

/*
	Initialize a store.

	std::string file: The UniStore file.
*/
Store::Store(std::string file) { this->update(file); };

void Store::update(const std::string &file) {
	bool doSheet = false;
	this->LoadFromFile(file);

	int rev = -1;
	if (this->valid) {
		if (this->storeJson["storeInfo"].contains("revision") && this->storeJson["storeInfo"]["revision"].is_number()) {
			rev = this->storeJson["storeInfo"]["revision"];
		}

		if (firstStart) {
			firstStart = false;

			if (!config->autoupdate()) {
				this->loadSheets();
				return;
			}
		}

		if (this->storeJson.contains("storeInfo")) {
			/* Checking... */
			if (checkWifiStatus()) { // Only do, if WiFi available.
				if (this->storeJson["storeInfo"].contains("url") && this->storeJson["storeInfo"]["url"].is_string()) {
					if (this->storeJson["storeInfo"].contains("file") && this->storeJson["storeInfo"]["file"].is_string()) {
						const std::string URL = this->storeJson["storeInfo"]["url"];

						if (URL != "") {
							std::string tmp = "";
							doSheet = DownloadUniStore(URL, rev, tmp);
						}
					}
				}

				if (doSheet) {
					/* SpriteSheet Array. */
					if (this->storeJson["storeInfo"].contains("sheetURL") && this->storeJson["storeInfo"]["sheetURL"].is_array()) {
						if (this->storeJson["storeInfo"].contains("sheet") && this->storeJson["storeInfo"]["sheet"].is_array()) {
							const std::vector<std::string> locs = this->storeJson["storeInfo"]["sheetURL"].get<std::vector<std::string>>();
							const std::vector<std::string> sht = this->storeJson["storeInfo"]["sheet"].get<std::vector<std::string>>();

							if (locs.size() == sht.size()) {
								for (int i = 0; i < (int)sht.size(); i++) {
									char msg[150];
									snprintf(msg, sizeof(msg), Lang::get("UPDATING_SPRITE_SHEET2").c_str(), i + 1, sht.size());
									Msg::DisplayMsg(msg);
									DownloadSpriteSheet(locs[i], sht[i]);
								}
							}
						}

						/* Single SpriteSheet (No array). */
					} else if (this->storeJson["storeInfo"].contains("sheetURL") && this->storeJson["storeInfo"]["sheetURL"].is_string()) {
						if (this->storeJson["storeInfo"].contains("sheet") && this->storeJson["storeInfo"]["sheet"].is_string()) {
							const std::string fl = this->storeJson["storeInfo"]["sheetURL"];
							const std::string fl2 = this->storeJson["storeInfo"]["sheet"];
							Msg::DisplayMsg(Lang::get("UPDATING_SPRITE_SHEET"));
							DownloadSpriteSheet(fl, fl2);
						}
					}
				}
			}

			this->LoadFromFile(file);
			this->loadSheets();
		}
	}
}

/*
	Unload all SpriteSheets on Destructor.
*/
Store::~Store() { this->unloadSheets(); };

/*
	Unload all SpriteSheets.
*/
void Store::unloadSheets() {
	if (this->valid) {
		if (this->sheets.size() > 0) {
			for (int i = 0; i < (int)this->sheets.size(); i++) {
				if (this->sheets[i]) C2D_SpriteSheetFree(this->sheets[i]);
			}
		}

		this->sheets.clear();
	}
}

/*
	Load all SpriteSheets.
*/
void Store::loadSheets() {
	if (this->valid) {
		if (this->storeJson["storeInfo"].contains("sheet")) {
			this->unloadSheets();

			std::vector<std::string> sheetLocs = { "" };

			if (this->storeJson["storeInfo"]["sheet"].is_array()) {
				sheetLocs = this->storeJson["storeInfo"]["sheet"].get<std::vector<std::string>>();

			} else if (this->storeJson["storeInfo"]["sheet"].is_string()) {
				sheetLocs[0] = this->storeJson["storeInfo"]["sheet"];

			} else {
				return;
			}

			for (int i = 0; i < (int)sheetLocs.size(); i++) {
				this->sheets.push_back({ });

				if (access((std::string("sdmc:/3ds/Universal-Updater/stores/") + sheetLocs[i]).c_str(), F_OK) == 0) {
					this->sheets[i] = C2D_SpriteSheetLoad((std::string("sdmc:/3ds/Universal-Updater/stores/") + sheetLocs[i]).c_str());
				}
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

	if (this->storeJson.contains("storeInfo") && this->storeJson.contains("storeContent")) this->valid = true;
}

/*
	Return the Title of the UniStore.
*/
std::string Store::GetUniStoreTitle() const {
	if (this->valid) {
		if (this->storeJson["storeInfo"].contains("title")) return this->storeJson["storeInfo"]["title"];
	}

	return "";
}

/*
	Return the Title of an index.

	int index: The index.
*/
std::string Store::GetTitleEntry(int index) const {
	if (!this->valid) return "";
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
	if (!this->valid) return "";
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
	if (!this->valid) return "";
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
	if (!this->valid) return { "" };
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
	if (!this->valid) return "";
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
	if (!this->valid) return { "" };
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
	if (!this->valid) return "";
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
	if (!this->valid) return "";
	if (index > (int)this->storeJson["storeContent"].size() - 1) return ""; // Empty.

	if (this->storeJson["storeContent"][index]["info"].contains("license") && this->storeJson["storeContent"][index]["info"]["license"].is_string()) {
		return this->storeJson["storeContent"][index]["info"]["license"];
	}

	return "";
}

/*
	Return a C2D_Image of an index.

	int index: The index.
*/
C2D_Image Store::GetIconEntry(int index) const {
	if (!this->valid) return C2D_SpriteSheetGetImage(sprites, sprites_noIcon_idx);
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
	if (!this->sheets[sheetIndex]) return C2D_SpriteSheetGetImage(sprites, sprites_noIcon_idx);

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
	if (!this->valid) return { "" };
	std::vector<std::string> temp;

	if (index > (int)this->storeJson["storeContent"].size() - 1) return temp;

	for(auto it = this->storeJson.at("storeContent").at(index).begin(); it != this->storeJson.at("storeContent").at(index).end(); it++) {
		if (it.key() != "info") temp.push_back(it.key());
	}

	return temp;
}