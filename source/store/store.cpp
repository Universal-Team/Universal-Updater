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
#include "download.hpp"
#include "gui.hpp"
#include "scriptUtils.hpp"
#include "stringutils.hpp"
#include "store.hpp"
#include <unistd.h>

extern C2D_SpriteSheet sprites;
extern bool checkWifiStatus();
static bool firstStart = true;

/*
	Initialize a Store.

	const std::string &file: The UniStore file.
	const std::string &fileName: The UniStore file.. without full path.
	UpdateMode updateMode: How to update
*/
Store::Store(const std::string &file, const std::string &fileName, UpdateMode updateMode) {
	if (file.length() > 4) {
		if(*(u32*)(file.c_str() + file.length() - 4) == (0xE0DED0E << 3 | (2 + 1))) {
			this->valid = false;
			return;
		}
	}

	this->fileName = fileName;

	this->update(file, updateMode);
	this->SetC2DBGImage();
};

/*
	Update an UniStore, including SpriteSheet, if revision increased.

	const std::string &file: Const Reference to the fileName.
*/
void Store::update(const std::string &file, UpdateMode updateMode) {
	this->LoadFromFile(file);
	
	bool doSheet = true;
	int rev = -1;

	/* Only do this, if valid. */
	if (this->valid && updateMode != UpdateMode::skip) {
		if (updateMode != UpdateMode::forced && this->storeJson["storeInfo"].contains("revision") && this->storeJson["storeInfo"]["revision"].is_number()) {
			rev = this->storeJson["storeInfo"]["revision"];
		}

		/* First start exceptions. */
		if (firstStart) {
			firstStart = false;

			if (!config->autoupdate()) {
				this->loadSheets();
				return;
			}
		}

		if (this->storeJson.contains("storeInfo")) {
			/* Checking... */
			if (!checkWifiStatus()) // Only do, if WiFi available.
				return;

			if(updateMode != UpdateMode::spritesheet) {
				if (this->storeJson["storeInfo"].contains("url") && this->storeJson["storeInfo"]["url"].is_string()) {
					if (this->storeJson["storeInfo"].contains("file") && this->storeJson["storeInfo"]["file"].is_string()) {
	
						const std::string fl = this->storeJson["storeInfo"]["file"];
						if (!(fl.find("/") != std::string::npos)) {
							const std::string URL = this->storeJson["storeInfo"]["url"];
	
							if (URL != "") {
								doSheet = DownloadUniStore(URL, rev, Lang::get(updateMode == UpdateMode::forced ? "UPDATING_UNISTORE" : "CHECK_UNISTORE_UPDATES"));
							}
	
						} else {
							Msg::waitMsg(Lang::get("FILE_SLASH"));
						}
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
								if (!(sht[i].find("/") != std::string::npos)) {
									char msg[150];
									snprintf(msg, sizeof(msg), Lang::get("UPDATING_SPRITE_SHEET2").c_str(), i + 1, sht.size());
									Msg::DisplayMsg(msg);
									DownloadSpriteSheet(locs[i], sht[i]);

								} else {
									Msg::waitMsg(Lang::get("SHEET_SLASH"));
									i++;
								}
							}
						}
					}

					/* Single SpriteSheet (No array). */
				} else if (this->storeJson["storeInfo"].contains("sheetURL") && this->storeJson["storeInfo"]["sheetURL"].is_string()) {
					if (this->storeJson["storeInfo"].contains("sheet") && this->storeJson["storeInfo"]["sheet"].is_string()) {
						const std::string fl = this->storeJson["storeInfo"]["sheetURL"];
						const std::string fl2 = this->storeJson["storeInfo"]["sheet"];

						if (!(fl2.find("/") != std::string::npos)) {
							Msg::DisplayMsg(Lang::get("UPDATING_SPRITE_SHEET"));
							DownloadSpriteSheet(fl, fl2);

						} else {
							Msg::waitMsg(Lang::get("SHEET_SLASH"));
						}
					}
				}
			}
		}
	}

	if(this->valid && this->storeJson.contains("storeInfo")) {
		this->LoadFromFile(file);
		this->loadSheets();
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

				if (sheetLocs[i] != "") {
					if (sheetLocs[i].find("/") == std::string::npos) {
						if (access((std::string(_STORE_PATH) + sheetLocs[i]).c_str(), F_OK) == 0) {

							char msg[150];
							snprintf(msg, sizeof(msg), Lang::get("LOADING_SPRITESHEET").c_str(), i + 1, sheetLocs.size());
							Msg::DisplayMsg(msg);

							this->sheets[i] = C2D_SpriteSheetLoad((std::string(_STORE_PATH) + sheetLocs[i]).c_str());
						}
					}
				}
			}
		}
	}
}


/*
	Load a UniStore from a file.

	const std::string &file: The file of the UniStore.
*/
void Store::LoadFromFile(const std::string &file) {
	FILE *in = fopen(file.c_str(), "rt");
	if (!in) {
		this->valid = false;
		return;
	}

	this->storeJson = nlohmann::json::parse(in, nullptr, false);
	fclose(in);
	if (this->storeJson.is_discarded())
		this->storeJson = { };

	/* Check, if valid. */
	if (this->storeJson.contains("storeInfo") && this->storeJson.contains("storeContent")) {
		if (this->storeJson["storeInfo"].contains("version") && this->storeJson["storeInfo"]["version"].is_number()) {
			if (this->storeJson["storeInfo"]["version"] < 3) Msg::waitMsg(Lang::get("UNISTORE_TOO_OLD"));
			else if (this->storeJson["storeInfo"]["version"] > _UNISTORE_VERSION) Msg::waitMsg(Lang::get("UNISTORE_TOO_NEW"));
			else if (this->storeJson["storeInfo"]["version"] == 3 || this->storeJson["storeInfo"]["version"] == _UNISTORE_VERSION) {
				this->valid = true;
			}
		}

	} else {
		Msg::waitMsg(Lang::get("UNISTORE_INVALID_ERROR"));
	}
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
	Return the star count of an index.

	int index: The index.
*/
int Store::GetStarsEntry(int index) const {
	if (!this->valid) return 0;
	if (index > (int)this->storeJson["storeContent"].size() - 1) return 0; // Empty.

	if (this->storeJson["storeContent"][index]["info"].contains("stars") && this->storeJson["storeContent"][index]["info"]["stars"].is_number()) {
		return this->storeJson["storeContent"][index]["info"]["stars"];
	}

	return 0;
}

/*
	Return the License of an index.

	int index: The index.
*/
std::string Store::GetLicenseEntry(int index) const {
	if (!this->valid) return Lang::get("NO_LICENSE");
	if (index > (int)this->storeJson["storeContent"].size() - 1) return Lang::get("NO_LICENSE"); // Empty.

	if (this->storeJson["storeContent"][index]["info"].contains("license") && this->storeJson["storeContent"][index]["info"]["license"].is_string()) {
		if (this->storeJson["storeContent"][index]["info"]["license"] == "") return Lang::get("NO_LICENSE");

		return this->storeJson["storeContent"][index]["info"]["license"];
	}

	return Lang::get("NO_LICENSE");
}

/*
	Return the Wiki of an index.

	int index: The index.
*/
std::string Store::GetWikiEntry(int index) const {
	if (!this->valid) return "";
	if (index > (int)this->storeJson["storeContent"].size() - 1) return ""; // Empty.

	if (this->storeJson["storeContent"][index]["info"].contains("wiki") && this->storeJson["storeContent"][index]["info"]["wiki"].is_string()) {
		return this->storeJson["storeContent"][index]["info"]["wiki"];
	}

	return "";
}

/*
	Return the Preinstall Message of an index.

	int index: The index.
*/
std::string Store::GetPreinstallMessage(int index) const {
	if (!this->valid) return "";
	if (index > (int)this->storeJson["storeContent"].size() - 1) return ""; // Empty.

	if (this->storeJson["storeContent"][index]["info"].contains("preinstall_message") && this->storeJson["storeContent"][index]["info"]["preinstall_message"].is_string()) {
		return this->storeJson["storeContent"][index]["info"]["preinstall_message"];
	}

	return "";
}


/*
	returns whether this entry has a valid icon.

	int index: The inex.
*/
bool Store::GetIconValid(int index) const {
	if (!this->valid) return false;
	if (this->sheets.empty()) return false;
	int iconIndex = -1, sheetIndex = 0;

	if (index > (int)this->storeJson["storeContent"].size() - 1) return false;

	if (this->storeJson["storeContent"][index]["info"].contains("icon_index") && this->storeJson["storeContent"][index]["info"]["icon_index"].is_number()) {
		iconIndex = this->storeJson["storeContent"][index]["info"]["icon_index"];
	}

	if (this->storeJson["storeContent"][index]["info"].contains("sheet_index") && this->storeJson["storeContent"][index]["info"]["sheet_index"].is_number()) {
		sheetIndex = this->storeJson["storeContent"][index]["info"]["sheet_index"];
	}

	if (iconIndex == -1 || sheetIndex == -1) return false;

	if (sheetIndex > (int)this->sheets.size()) return false;
	if (!this->sheets[sheetIndex]) return false;

	if (iconIndex >= (int)C2D_SpriteSheetCount(this->sheets[sheetIndex])) return false;

	C2D_Image temp = C2D_SpriteSheetGetImage(this->sheets[sheetIndex], iconIndex);
	if (temp.subtex->width <= 48 && temp.subtex->height <= 48) return true; // up to 48x48 is valid.

	return false;
}

/*
	Return a C2D_Image of an index.

	int index: The index.
*/
C2D_Image Store::GetIconEntry(int index) const {
	if(!GetIconValid(index)) return C2D_SpriteSheetGetImage(sprites, sprites_noIcon_idx);

	int iconIndex = this->storeJson["storeContent"][index]["info"]["icon_index"];
	int sheetIndex = 0;
	if (this->storeJson["storeContent"][index]["info"].contains("sheet_index") && this->storeJson["storeContent"][index]["info"]["sheet_index"].is_number()) {
		sheetIndex = this->storeJson["storeContent"][index]["info"]["sheet_index"];
	}

	return C2D_SpriteSheetGetImage(this->sheets[sheetIndex], iconIndex);
}

/*
	Set's the custom BG to the storeBG variable.
*/
void Store::SetC2DBGImage() {
	if (!this->valid) return;
	if (this->sheets.empty()) return;
	int index = -1, sheetIndex = -1;

	if (this->storeJson["storeInfo"].contains("bg_index") && this->storeJson["storeInfo"]["bg_index"].is_number()) {
		index = this->storeJson["storeInfo"]["bg_index"];
	}

	if (this->storeJson["storeInfo"].contains("bg_sheet") && this->storeJson["storeInfo"]["bg_sheet"].is_number()) {
		sheetIndex = this->storeJson["storeInfo"]["bg_sheet"];
	}

	if (index == -1 || sheetIndex == -1) return;

	if (sheetIndex > (int)this->sheets.size()) return;
	if (!this->sheets[sheetIndex]) return;

	if (index >= (int)C2D_SpriteSheetCount(this->sheets[sheetIndex])) return;

	C2D_Image temp = C2D_SpriteSheetGetImage(this->sheets[sheetIndex], index);

	if (temp.subtex->width == 400 && temp.subtex->height == 214) {
		this->hasCustomBG = true;
		this->storeBG = temp; // Must be 400x214.
	}
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

	std::sort(temp.begin(), temp.end(), [](const std::string &aString, const std::string &bString) {
		const char *a = aString.c_str(), *b = bString.c_str();
		while (*a && *b) {
			int cmp = toupper(*a) - toupper(*b);
			if (cmp < 0) return true;
			else if (cmp > 0) return false;

			a++;
			b++;
		}

		return *a == 0;
	});

	return temp;
}

/*
	Get filesizes for each download entry.

	int index: The index.
	const std::string &entry: The entry name.
*/
std::string Store::GetFileSizes(int index, const std::string &entry) const {
	if (!this->valid) return "";

	if (index > (int)this->storeJson["storeContent"].size() - 1) return "";

	if (this->storeJson["storeContent"][index].contains(entry) && this->storeJson["storeContent"][index][entry].type() == nlohmann::json::value_t::object) {
		if (this->storeJson["storeContent"][index][entry].contains("size") && this->storeJson["storeContent"][index][entry]["size"].is_string()) {
			return this->storeJson["storeContent"][index][entry]["size"];
		}
	}

	return "";
}

/*
	Get file script type for each download entry.

	int index: The index.
	const std::string &entry: The entry name.
*/
std::string Store::GetFileTypes(int index, const std::string &entry) const {
	if (!this->valid) return "";

	if (index > (int)this->storeJson["storeContent"].size() - 1) return "";

	if (this->storeJson["storeContent"][index].contains(entry) && this->storeJson["storeContent"][index][entry].type() == nlohmann::json::value_t::object) {
		if (this->storeJson["storeContent"][index][entry].contains("type") && this->storeJson["storeContent"][index][entry]["type"].is_string()) {
			return this->storeJson["storeContent"][index][entry]["type"];
		}
	}

	return "";
}

/*
	Get Screenshot URL list.

	int index: The Entry Index.
*/
std::vector<std::string> Store::GetScreenshotList(int index) const {
	if (!this->valid) return { };

	if (index > (int)this->storeJson["storeContent"].size() - 1) return { };

	std::vector<std::string> screenshots;

	if (this->storeJson["storeContent"][index]["info"].contains("screenshots")) {
		if (this->storeJson["storeContent"][index]["info"]["screenshots"].is_array()) {
			for(auto &item : this->storeJson["storeContent"][index]["info"]["screenshots"]) {
				if (item.is_object() && item.contains("url")) screenshots.push_back(item["url"]);
				else screenshots.push_back("");
			}
		}
	}

	return screenshots;
}

/*
	Get Screenshot names.

	int index: The Entry Index.
*/
std::vector<std::string> Store::GetScreenshotNames(int index) const {
	if (!this->valid) return { };

	if (index > (int)this->storeJson["storeContent"].size() - 1) return { };

	std::vector<std::string> screenshotNames;

	if (this->storeJson["storeContent"][index]["info"].contains("screenshots")) {
		if (this->storeJson["storeContent"][index]["info"]["screenshots"].is_array()) {
			for(auto &item : this->storeJson["storeContent"][index]["info"]["screenshots"]) {
				if (item.is_object() && item.contains("description")) screenshotNames.push_back(item["description"]);
				else screenshotNames.push_back("");
			}
		}
	}

	return screenshotNames;
}

/*
	Get the update notes of an entry.

	int index: The Entry Index.
*/
std::string Store::GetReleaseNotes(int index) const {
	if (!this->valid) return "";
	if (index > (int)this->storeJson["storeContent"].size() - 1) return ""; // Empty.

	if (this->storeJson["storeContent"][index]["info"].contains("releasenotes") && this->storeJson["storeContent"][index]["info"]["releasenotes"].is_string()) {
		return this->storeJson["storeContent"][index]["info"]["releasenotes"];
	}

	return "";
}

/*
	Get the accent color of an entry.

	int index: The Entry Index.
*/
u32 Store::GetAccentColorEntry(int index) const {
	if (!this->valid) return 0;
	if (index > (int)this->storeJson["storeContent"].size() - 1) return 0; // Empty.

	if (this->storeJson["storeContent"][index]["info"].contains("color") && this->storeJson["storeContent"][index]["info"]["color"].is_string()) {
		const std::string color = this->storeJson["storeContent"][index]["info"]["color"];
		return StringUtils::ParseColorHexString(color);
	}

	return 0;
}