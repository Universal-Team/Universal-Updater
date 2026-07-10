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
#include "rapidjson/filereadstream.h"
#include "scriptUtils.hpp"
#include "stringutils.hpp"
#include "store.hpp"
#include <unistd.h>

using namespace rapidjson;

extern C2D_SpriteSheet sprites;
extern bool checkWifiStatus();


/*
	Initialize a Store.

	const std::string &file: The UniStore file.
	const std::string &fileName: The UniStore file.. without full path.
	UpdateMode updateMode: How to update
*/
Store::Store(const std::string &file, const std::string &fileName, UpdateMode updateMode, bool loadContent) {
	if (file.length() > 4) {
		if(*(u32*)(file.c_str() + file.length() - 4) == (0xE0DED0E << 3 | (2 + 1))) {
			this->info.valid = false;
			return;
		}
	}

	this->update(file, updateMode, loadContent);
};

/*
	Update an UniStore, including SpriteSheet, if revision increased.

	const std::string &file: Const Reference to the fileName.
*/
void Store::update(const std::string &file, UpdateMode updateMode, bool loadContent) {
	this->LoadFromFile(file, false);

	/* If we're not updating and don't want content, we're already done. */
	if (updateMode == UpdateMode::skip && !loadContent) return;

	bool doSheet = true;
	int rev = (updateMode == UpdateMode::forced) ? -1 : this->info.revision;

	/* Only do this, if valid. */
	if (this->info.valid && updateMode != UpdateMode::skip && checkWifiStatus()) {
		if(updateMode != UpdateMode::spritesheet) {
			const std::string fl = this->info.file;
			if (!(fl.find("/") != std::string::npos)) {
				const std::string URL = this->info.url;

				if (URL != "") {
					doSheet = DownloadUniStore(URL, rev, Lang::get(updateMode == UpdateMode::forced ? "UPDATING_UNISTORE" : "CHECK_UNISTORE_UPDATES"));
					if(doSheet) this->LoadFromFile(file, false); // Reload JSON
					if(!this->info.valid) return;
				}

			} else {
				Msg::waitMsg(Lang::get("FILE_SLASH"));
			}
		}

		if (doSheet) {
			/* SpriteSheet Array. */
			if (this->info.sheetUrls.size() == this->info.sheets.size()) {
				for (size_t i = 0; i < this->info.sheets.size(); i++) {
					if (!(this->info.sheets[i].find("/") != std::string::npos)) {
						char msg[150];
						snprintf(msg, sizeof(msg), Lang::get("UPDATING_SPRITE_SHEET2").c_str(), i + 1, this->info.sheets.size());
						Msg::DisplayMsg(msg);
						DownloadSpriteSheet(this->info.sheetUrls[i], this->info.sheets[i]);

					} else {
						Msg::waitMsg(Lang::get("SHEET_SLASH"));
						i++;
					}
				}
			}
		}
	}

	if(this->info.valid) {
		this->LoadFromFile(file, loadContent);
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
	if (this->info.valid) {
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
	if (!this->info.valid) return;
	this->unloadSheets();

	for (size_t i = 0; i < this->info.sheets.size(); i++) {
		this->sheets.push_back({ });

		if (this->info.sheets[i] != "") {
			if (this->info.sheets[i].find("/") == std::string::npos) {
				if (access((std::string(_STORE_PATH) + this->info.sheets[i]).c_str(), F_OK) == 0) {

					char msg[150];
					snprintf(msg, sizeof(msg), Lang::get("LOADING_SPRITESHEET").c_str(), i + 1, this->info.sheets.size());
					Msg::DisplayMsg(msg);

					this->sheets[i] = C2D_SpriteSheetLoad((std::string(_STORE_PATH) + this->info.sheets[i]).c_str());
				}
			}
		}
	}
}

bool Store::LoadStoreInfo(const Value &storeInfo) {
	if (!storeInfo.IsObject()) return false;

	// Ensure valid version.
	if (!storeInfo.HasMember("version")) return false;

	const Value &version = storeInfo["version"];
	if (!version.IsInt()) {
		return false;
	} else if (version.GetInt() < 3) {
		this->info.error = Lang::get("UNISTORE_TOO_OLD");
		return false;
	} else if (version.GetInt() > _UNISTORE_VERSION) {
		this->info.error = Lang::get("UNISTORE_TOO_NEW");
		return false;
	}

	if (storeInfo.HasMember("title") && storeInfo["title"].IsString())
		this->info.title = storeInfo["title"].GetString();
	else return false;

	if (storeInfo.HasMember("author") && storeInfo["author"].IsString())
		this->info.author = storeInfo["author"].GetString();

	if (storeInfo.HasMember("description") && storeInfo["description"].IsString())
		this->info.description = storeInfo["description"].GetString();

	if (storeInfo.HasMember("revision") && storeInfo["revision"].IsInt())
		this->info.revision = storeInfo["revision"].GetInt();
	else return false;

	if (storeInfo.HasMember("bg_index") && storeInfo["bg_index"].IsInt())
		this->info.bgIndex = storeInfo["bg_index"].GetInt();

	if (storeInfo.HasMember("bg_sheet") && storeInfo["bg_sheet"].IsInt())
		this->info.bgSheet = storeInfo["bg_sheet"].GetInt();

	if (storeInfo.HasMember("file") && storeInfo["file"].IsString()) {
		this->info.file = storeInfo["file"].GetString();
		if (this->info.file.find("/") != std::string::npos) {
			this->info.error = Lang::get("FILE_SLASH");
			return false;
		}
	}

	if (storeInfo.HasMember("url") && storeInfo["url"].IsString())
		this->info.url = storeInfo["url"].GetString();

	if (storeInfo.HasMember("infoURL") && storeInfo["infoURL"].IsString())
		this->info.infoUrl = storeInfo["infoURL"].GetString();

	this->info.sheets.clear();
	if (storeInfo.HasMember("sheet")) {
		const Value &sheet = storeInfo["sheet"];
		if (sheet.IsString()) {
			this->info.sheets = {sheet.GetString()};
		} else if (sheet.IsArray()) {
			for (const Value &val : sheet.GetArray()) {
				if (val.IsString()) this->info.sheets.push_back(val.GetString());
			}
		}

		for (const std::string &sheet : this->info.sheets) {
			if (sheet.find("/") != std::string::npos) {
				this->info.error = Lang::get("SHEET_SLASH");
				return false;
			}
		}
	}

	this->info.sheetUrls.clear();
	if (storeInfo.HasMember("sheetURL")) {
		const Value &sheetUrl = storeInfo["sheetURL"];
		if (sheetUrl.IsString()) {
			this->info.sheetUrls = {sheetUrl.GetString()};
		} else if (sheetUrl.IsArray()) {
			for (const Value &val : sheetUrl.GetArray()) {
				if (val.IsString()) this->info.sheetUrls.push_back(val.GetString());
			}
		}
	}

	if (this->info.sheetUrls.size() != this->info.sheets.size())
		return false;

	return true;
}

bool Store::LoadStoreContent(const Value &storeContent) {
	StoreUtils::entries.clear();
	StoreUtils::allEntries.clear();
	categories.clear();
	consoles.clear();

	for(const Value &app : storeContent.GetArray()) {
		StoreUtils::allEntries.push_back(std::make_shared<StoreEntry>(app, *this));
		StoreUtils::entries.emplace_back(StoreUtils::allEntries.back());


		for (const auto &category : StoreUtils::allEntries.back()->GetCategories()) {
			if (!category.empty() && std::find(categories.begin(), categories.end(), category) == categories.end()) {
				categories.push_back(category);
			}
		}

		for (const auto &console : StoreUtils::allEntries.back()->GetConsoles()) {
			if (!console.empty() && std::find(consoles.begin(), consoles.end(), console) == consoles.end()) {
				consoles.push_back(console);
			}
		}
	}

	sort(categories.begin(), categories.end(), [](const std::string &lhs, const std::string &rhs) {
		return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
	});

	sort(consoles.begin(), consoles.end(), [](const std::string &lhs, const std::string &rhs) {
		return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
	});

	return true;
}

/*
	Load a UniStore from a file.

	const std::string &file: The file of the UniStore.
*/
void Store::LoadFromFile(const std::string &file, bool loadContent) {
	// Set defaults
	this->info.file = file.substr(file.rfind('/') + 1);
	this->info.title = this->info.file;

	FILE *in = fopen(file.c_str(), "rt");
	if (!in) {
		this->info.valid = false;
		return;
	}

	Document json;
	char *readBuffer = new char[0x10000];
	FileReadStream is(in, readBuffer, 0x10000);
	json.ParseStream(is);
	delete[] readBuffer;
	fclose(in);

	/* Check, if valid. */
	if (!json.IsObject() || !json.HasMember("storeInfo") || !json.HasMember("storeContent")) {
		this->info.valid = false;
		this->info.error = Lang::get("UNISTORE_INVALID_ERROR");
		return;
	}

	this->info.valid = LoadStoreInfo(json["storeInfo"]);
	if (!this->info.valid) {
		if (this->info.error.empty()) this->info.error = "failed to load info";
		return;
	}

	const Value &storeContent = json["storeContent"];
	if (storeContent.IsArray()) {
		this->info.entryCount = storeContent.Size();

		if(loadContent) {
			this->loadSheets();
			this->SetC2DBGImage();

			this->info.valid = LoadStoreContent(storeContent);
			if (!this->info.valid) {
				if (this->info.error.empty()) this->info.error = "failed to load content";
				return;
			}
		}
	} else {
		this->info.valid = false;
		this->info.error = "no content";
	}
}

/*
	returns whether this entry has a valid icon.

	int index: The inex.
*/
bool Store::GetIconValid(int iconIndex, int sheetIndex) const {
	if (!this->info.valid) return false;
	if (this->sheets.empty()) return false;
	if (iconIndex < 0 || sheetIndex < 0) return false;
	if (iconIndex > this->info.entryCount) return false;
	if (sheetIndex >= (int)this->sheets.size()) return false;

	if (!this->sheets[sheetIndex]) return false;
	if (iconIndex >= (int)C2D_SpriteSheetCount(this->sheets[sheetIndex])) return false;

	return true;
	C2D_Image temp = C2D_SpriteSheetGetImage(this->sheets[sheetIndex], iconIndex);
	return (temp.subtex->width <= 48 && temp.subtex->height <= 48); // up to 48x48 is valid.
}

/*
	Return a C2D_Image of an index.

	int index: The index.
*/
C2D_Image Store::GetIconEntry(int iconIndex, int sheetIndex) const {
	if(!GetIconValid(iconIndex, sheetIndex)) return C2D_SpriteSheetGetImage(sprites, sprites_noIcon_idx);
	return C2D_SpriteSheetGetImage(this->sheets[sheetIndex], iconIndex);
}

/*
	Set's the custom BG to the storeBG variable.
*/
void Store::SetC2DBGImage() {
	if (!this->info.valid) return;
	if (this->sheets.empty()) return;
	if (this->info.bgIndex < 0 || this->info.bgSheet < 0) return;
	if (this->info.bgSheet >= (int)this->sheets.size()) return;

	if (!this->sheets[this->info.bgSheet]) return;
	if (this->info.bgIndex >= (int)C2D_SpriteSheetCount(this->sheets[this->info.bgSheet])) return;

	C2D_Image temp = C2D_SpriteSheetGetImage(this->sheets[this->info.bgSheet], this->info.bgIndex);
	if (temp.subtex->width == 400 && temp.subtex->height == 214) {
		this->hasCustomBG = true;
		this->storeBG = temp; // Must be 400x214.
	}
}