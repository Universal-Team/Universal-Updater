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

#include "BrowseData.hpp"
#include "DownloadFile.hpp"
#include "DownloadUtils.hpp"
#include "UniStore.hpp"
#include "UniversalUpdater.hpp"
#include <unistd.h>


bool UniStore::FirstStart = true;

/*
	Return UniStore info.

	const std::string &File: Const Reference to the path of the file.
	const std::string &FName: Const Reference to the filename, without path.
*/
UniStore::Info UniStore::GetInfo(const std::string &File, const std::string &FName) {
	UniStore::Info Temp = { "", "", "", "", FName, "", -1, -1, -1 }; // Title, Author, URL, File (to check if no slash exist), FileName, Desc, Version, Revision, entries.

	if (FName.length() > 4) {
		if (*(uint32_t *)(FName.c_str() + FName.length() - 4) == (1886349435 & ~(1 << 3))) return Temp;
	}

	nlohmann::json JSON;

	FILE *Tmp = fopen(File.c_str(), "rt");

	if (Tmp) {
		JSON = nlohmann::json::parse(Tmp, nullptr, false);
		fclose(Tmp);
	}

	if (JSON.is_discarded()) JSON = { };

	if (!JSON.contains("storeInfo")) return Temp; // storeInfo does not exist.

	/* Fetch Info. */
	if (JSON["storeInfo"].contains("title") && JSON["storeInfo"]["title"].is_string()) Temp.Title = JSON["storeInfo"]["title"];
	if (JSON["storeInfo"].contains("file") && JSON["storeInfo"]["file"].is_string()) Temp.File = JSON["storeInfo"]["file"];
	if (JSON["storeInfo"].contains("author") && JSON["storeInfo"]["author"].is_string()) Temp.Author = JSON["storeInfo"]["author"];
	if (JSON["storeInfo"].contains("url") && JSON["storeInfo"]["url"].is_string()) Temp.URL = JSON["storeInfo"]["url"];
	if (JSON["storeInfo"].contains("description") && JSON["storeInfo"]["description"].is_string()) Temp.Description = JSON["storeInfo"]["description"];
	if (JSON["storeInfo"].contains("version") && JSON["storeInfo"]["version"].is_number()) Temp.Version = JSON["storeInfo"]["version"];
	if (JSON["storeInfo"].contains("revision") && JSON["storeInfo"]["revision"].is_number()) Temp.Revision = JSON["storeInfo"]["revision"];
	if (JSON.contains("storeContent")) Temp.StoreSize = JSON["storeContent"].size();

	return Temp;
};


/*
	Return UniStore info vector.

	const std::string &Path: Const Reference to the path, where to check.
*/
std::vector<UniStore::Info> UniStore::GetUniStoreInfo(const std::string &Path) {
	std::vector<UniStore::Info> _Info;
	std::vector<BrowseData::DirEntry> DirContents;

	if (access(Path.c_str(), F_OK) != 0) return { }; // Folder does not exist.

	std::unique_ptr<BrowseData> BData = std::make_unique<BrowseData>(Path, std::vector<std::string>({ "unistore" }));
	DirContents = BData->GetDirEntries();

	for(uint Idx = 0; Idx < DirContents.size(); Idx++) {
		/* Make sure to ONLY push .unistores, and no folders. Avoids crashes in that case too. */
		if ((Path + DirContents[Idx].Name).find(".unistore") != std::string::npos) _Info.push_back( UniStore::GetInfo(Path + DirContents[Idx].Name, DirContents[Idx].Name) );
	}

	return _Info;
};





/*
	Initialize a UniStore.

	const std::string &FullPath: The full path to the UniStore.
	const std::string &FileName: Same as above, but just the UniStore filename.
	const bool DidDownload: If it got downloaded a bit ago, so we don't update at all and directly load it.
*/
UniStore::UniStore(const std::string &FullPath, const std::string &FileName, const bool DidDownload) {
	if (FullPath.length() > 4) {
		if (*(uint32_t *)(FullPath.c_str() + FullPath.length() - 4) == (0xE0DED0E << 3 | (2 + 1))) {
			this->Valid = false;
			return;
		}
	}

	this->FileName = FileName;
	if (!DidDownload) this->UpdateUniStore(FullPath);
	else this->LoadUniStore(FullPath, true);
};


/*
	The destructor of the UniStore class.

	This basically unloads all the Spritesheets and such that have been loaded to free up the RAM.
*/
UniStore::~UniStore() { UU::App->GData->UnloadUniStoreSheets(); };


/*
	Updates a UniStore inclusive spritesheets to it's latest version.

	const std::string &File: The file of the UniStore to update.
*/
void UniStore::UpdateUniStore(const std::string &File) {
	bool DoSheet = false;
	int Rev = -1;
	this->LoadUniStore(File, false);

	/* Only do this, if valid. */
	if (this->Valid) {
		if (this->UniStoreJSON["storeInfo"].contains("revision") && this->UniStoreJSON["storeInfo"]["revision"].is_number()) {
			Rev = this->UniStoreJSON["storeInfo"]["revision"];
		}

		/* First start exceptions. */
		if (UniStore::FirstStart) {
			UniStore::FirstStart = false;

			if (!UU::App->CData->AutoUpdate()) {
				this->LoadSpriteSheets();
				return;
			}
		}

		if (this->UniStoreJSON.contains("storeInfo")) {
			/* Ensure WiFi is available. */
			if (DownloadUtils::WiFiAvailable()) {
				if (this->UniStoreJSON["storeInfo"].contains("url") && this->UniStoreJSON["storeInfo"]["url"].is_string()) {
					if (this->UniStoreJSON["storeInfo"].contains("file") && this->UniStoreJSON["storeInfo"]["file"].is_string()) {
						const std::string Fl = this->UniStoreJSON["storeInfo"]["file"];
						if (!(Fl.find("/") != std::string::npos)) {
							const std::string URL = this->UniStoreJSON["storeInfo"]["url"];

							if (URL != "") {
								UU::App->MSData->DisplayWaitMsg("Updating UniStore...");
								std::unique_ptr<Action> USDL = std::make_unique<DownloadFile>(URL, _STORE_PATH + Fl);
								USDL->Handler();

								/* Check if the revision increased. */
								UniStore::Info _Info = UniStore::GetInfo(_STORE_PATH + Fl, Fl);
								if (_Info.Revision > Rev) DoSheet = true;
							}

						} else {
							UU::App->MSData->PromptMsg("Filename contains a slash which is invalid.");
						}
					}
				}

				if (DoSheet) {
					/* SpriteSheet Array. */
					if (this->UniStoreJSON["storeInfo"].contains(SHEET_URL_KEY) && this->UniStoreJSON["storeInfo"][SHEET_URL_KEY].is_array()) {
						if (this->UniStoreJSON["storeInfo"].contains(SHEET_PATH_KEY) && this->UniStoreJSON["storeInfo"][SHEET_PATH_KEY].is_array()) {
							const std::vector<std::string> Locs = this->UniStoreJSON["storeInfo"][SHEET_URL_KEY].get<std::vector<std::string>>();
							const std::vector<std::string> Sht = this->UniStoreJSON["storeInfo"][SHEET_PATH_KEY].get<std::vector<std::string>>();

							/* Loop through the array. */
							if (Locs.size() == Sht.size()) {
								for (size_t Idx = 0; Idx < Sht.size(); Idx++) {
									if (!(Sht[Idx].find("/") != std::string::npos)) {
										char Msg[150];
										snprintf(Msg, sizeof(Msg), "Updating SpriteSheet %i of %i...", Idx + 1, Sht.size());
										UU::App->MSData->DisplayWaitMsg(Msg);

										std::unique_ptr<Action> SDL = std::make_unique<DownloadFile>(Locs[Idx], _STORE_PATH + Sht[Idx]);
										SDL->Handler();

									} else {
										UU::App->MSData->PromptMsg("SpriteSheet contains a slash which is invalid.");
									}
								}
							}
						}

					/* Single SpriteSheet (No array). */
					} else if (this->UniStoreJSON["storeInfo"].contains(SHEET_URL_KEY) && this->UniStoreJSON["storeInfo"][SHEET_URL_KEY].is_string()) {
						if (this->UniStoreJSON["storeInfo"].contains(SHEET_PATH_KEY) && this->UniStoreJSON["storeInfo"][SHEET_PATH_KEY].is_string()) {
							const std::string URL = this->UniStoreJSON["storeInfo"][SHEET_URL_KEY];
							const std::string FL = this->UniStoreJSON["storeInfo"][SHEET_PATH_KEY];

							if (!(FL.find("/") != std::string::npos)) {
								UU::App->MSData->DisplayWaitMsg("Updating Spritesheet...");
								std::unique_ptr<Action> SDL = std::make_unique<DownloadFile>(URL, _STORE_PATH + FL);
								SDL->Handler();

							} else {
								UU::App->MSData->PromptMsg("SpriteSheet contains a slash which is invalid.");
							}
						}
					}
				}
			}

			this->LoadUniStore(File, true);
		}
	}
};


/*
	Load a UniStore from a file.

	const std::string &File: The file of the UniStore.
*/
void UniStore::LoadUniStore(const std::string &File, const bool FullInit) {
	/* Reset UniStore related stuff. */
	this->Indexes.clear();
	this->SelectedIndex = 0;

	if (access(File.c_str(), F_OK) != 0) return;
	
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
			if (this->UniStoreJSON["storeInfo"]["version"] < 3) UU::App->MSData->PromptMsg("UniStore too old!");
			else if (this->UniStoreJSON["storeInfo"]["version"] > this->UNISTORE_VERSION) UU::App->MSData->PromptMsg("UniStore too new!");

			else if (this->UniStoreJSON["storeInfo"]["version"] == 3 || this->UniStoreJSON["storeInfo"]["version"] == this->UNISTORE_VERSION) {
				this->Valid = true;
			}
		}

	} else {
		UU::App->MSData->PromptMsg("UniStore invalid!");
	}

	if (FullInit) {
		this->ResetIndexes();
		UU::App->GData->UnloadUniStoreSheets();
		this->LoadSpriteSheets();
	}
};


/*
	Load the UniStore spritesheets for the icons into RAM.

	TODO: Find a good way to handle this for 3DS and NDS cleanly.
*/
void UniStore::LoadSpriteSheets() {
	if (this->Valid) {
		if (this->UniStoreJSON["storeInfo"].contains(SHEET_PATH_KEY)) {
			std::vector<std::string> SheetLocs = { "" };

			if (this->UniStoreJSON["storeInfo"][SHEET_PATH_KEY].is_array()) {
				SheetLocs = this->UniStoreJSON["storeInfo"][SHEET_PATH_KEY].get<std::vector<std::string>>();

			} else if (this->UniStoreJSON["storeInfo"][SHEET_PATH_KEY].is_string()) {
				SheetLocs[0] = this->UniStoreJSON["storeInfo"][SHEET_PATH_KEY];

			} else {
				return;
			}

			for (size_t Idx = 0; Idx < SheetLocs.size(); Idx++) {
				if (SheetLocs[Idx] != "") {
					if (SheetLocs[Idx].find("/") == std::string::npos) {
						UU::App->GData->LoadUniStoreSheet((std::string(_STORE_PATH) + SheetLocs[Idx]));
					}
				}
			}
		}
	}
};



void UniStore::ResetIndexes() {
	this->SelectedIndex = 0, this->ScreenIndex = 0;
	this->Indexes.clear();

	if (this->Valid) {
		/* Push valid indexes back. */
		for (size_t Idx = 0; Idx < this->UniStoreJSON["storeContent"].size(); Idx++) {
			this->Indexes.push_back(Idx);
		}
	}
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
	Return the index of the icon at the specific index.

	const int Idx: The index.
*/
int UniStore::GetEntryIcon(const int Idx) const {
	if (!this->Valid) return -1;
	if (Idx > (int)this->UniStoreJSON["storeContent"].size() - 1) return -1;

	if (this->UniStoreJSON["storeContent"][Idx]["info"].contains("icon_index") && this->UniStoreJSON["storeContent"][Idx]["info"]["icon_index"].is_number()) {
		return this->UniStoreJSON["storeContent"][Idx]["info"]["icon_index"];
	}

	return -1;
};


/*
	Return the index of the sheet at the specific index.

	const int Idx: The index.
*/
int UniStore::GetEntrySheet(const int Idx) const {
	if (!this->Valid) return -1;
	if (Idx > (int)this->UniStoreJSON["storeContent"].size() - 1) return -1;

	if (this->UniStoreJSON["storeContent"][Idx]["info"].contains("sheet_index") && this->UniStoreJSON["storeContent"][Idx]["info"]["sheet_index"].is_number()) {
		return this->UniStoreJSON["storeContent"][Idx]["info"]["sheet_index"];
	}

	return 0;
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

/*
	Return a const pointer to the script of an entry. Returns nullptr if it can't find the script.

	const int Idx: The index.
	const std::string &Script: The script.
*/
const nlohmann::json *UniStore::GetScript(const int Idx, const std::string &Script) const {
	if (!this->Valid) return nullptr;
	if (Idx > (int)this->UniStoreJSON["storeContent"].size() - 1) return nullptr;

	const nlohmann::json &Entry = this->UniStoreJSON["storeContent"][Idx];

	if (Entry.contains(Script)) {
		if (Entry[Script].is_array()) {
			return &Entry[Script];
		} else if (Entry[Script].is_object()) {
			if (Entry[Script].contains("script") && Entry[Script]["script"].is_array()) {
				return &Entry[Script]["script"];
			}
		}
	}

	return nullptr;
}