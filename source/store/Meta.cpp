// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#include "Meta.hpp"

#include "Platform.hpp"
#include "UniversalUpdater.hpp"
#include "UniStore.hpp"

#include <unistd.h>


/*
	The Constructor of the Meta.

	Includes MetaData file creation, if non existent.
*/
Meta::Meta() {
	if (access(_META_PATH, F_OK) != 0) {
		FILE *Temp = fopen(_META_PATH, "w");

		if (Temp) {
			char TMP[2] = { '{', '}' };
			fwrite(TMP, sizeof(TMP), 1, Temp);
			fclose(Temp);
		}
	}

	FILE *Temp = fopen(_META_PATH, "rt");

	if (Temp) {
		this->MetaDataJSON = nlohmann::json::parse(Temp, nullptr, false);
		fclose(Temp);
	}

	if (this->MetaDataJSON.is_discarded()) this->MetaDataJSON = { };

	if (UU::App->CData->MetaData()) {
		UU::App->MSData->DisplayWaitMsg("Importing old MetaData...");
		this->ImportMetadata();
		UU::App->CData->MetaData(false);
	}
};


/*
	Import the old Metadata of the 'updates.json' file.
*/
void Meta::ImportMetadata() {
	if (access(_OLD_UPDATE_PATH, F_OK) != 0) {
		return; // Not found.
	}


	nlohmann::json OldJSON;
	FILE *Old = fopen(_OLD_UPDATE_PATH, "rt");

	if (Old) {
		OldJSON = nlohmann::json::parse(Old, nullptr, false);
		fclose(Old);
	}

	if (OldJSON.is_discarded()) OldJSON = { };

	std::vector<UniStore::Info> Info = UniStore::GetUniStoreInfo(_STORE_PATH); // Fetch UniStores.

	for (size_t Idx = 0; Idx < Info.size(); Idx++) {
		if (Info[Idx].Title != "" && OldJSON.contains(Info[Idx].File)) {
			for(auto It = OldJSON[Info[Idx].File].begin(); It != OldJSON[Info[Idx].File].end(); ++It) {
				this->SetUpdated(Info[Idx].Title, It.key().c_str(), It.value().get<std::string>());
			}
		}
	}
};


/*
	Get Last Updated.

	const std::string &UniStoreName: The UniStore name.
	const std::string &Entry: The Entry name.
*/
std::string Meta::GetUpdated(const std::string &UniStoreName, const std::string &Entry) const {
	if (!this->MetaDataJSON.contains(UniStoreName)) return ""; // UniStore Name does not exist.

	if (!this->MetaDataJSON[UniStoreName].contains(Entry)) return ""; // Entry does not exist.

	if (!this->MetaDataJSON[UniStoreName][Entry].contains("updated")) return ""; // updated does not exist.

	if (this->MetaDataJSON[UniStoreName][Entry]["updated"].is_string()) return this->MetaDataJSON[UniStoreName][Entry]["updated"];
	return "";
};


/*
	Get the marks.

	const std::string &UniStoreName: The UniStore name.
	const std::string &Entry: The Entry name.
*/
int Meta::GetMarks(const std::string &UniStoreName, const std::string &Entry) const {
	int temp = 0;

	if (!this->MetaDataJSON.contains(UniStoreName)) return temp; // UniStore Name does not exist.

	if (!this->MetaDataJSON[UniStoreName].contains(Entry)) return temp; // Entry does not exist.

	if (!this->MetaDataJSON[UniStoreName][Entry].contains("marks")) return temp; // marks does not exist.

	if (this->MetaDataJSON[UniStoreName][Entry]["marks"].is_number()) return this->MetaDataJSON[UniStoreName][Entry]["marks"];
	return temp;
};


/*
	Return, if update available.

	const std::string &UniStoreName: The UniStore name.
	const std::string &Entry: The Entry name.
	const std::string &Updated: Compare for the update.
*/
bool Meta::UpdateAvailable(const std::string &UniStoreName, const std::string &Entry, const std::string &Updated) const {
	if (this->GetUpdated(UniStoreName, Entry) != "" && Updated != "") {
		return strcasecmp(Updated.c_str(), this->GetUpdated(UniStoreName, Entry).c_str()) > 0;
	}

	return false;
};


/*
	Return all the installed downloads.

	const std::string &UniStoreName: The UniStore name.
	const std::string &Entry: The Entry name.
*/
std::vector<std::string> Meta::GetInstalled(const std::string &UniStoreName, const std::string &Entry) const {
	if (!this->MetaDataJSON.contains(UniStoreName)) return { }; // UniStore Name does not exist.

	if (!this->MetaDataJSON[UniStoreName].contains(Entry)) return { }; // Entry does not exist.

	if (!this->MetaDataJSON[UniStoreName][Entry].contains("installed")) return { }; // marks does not exist.

	if (this->MetaDataJSON[UniStoreName][Entry]["installed"].is_array()) return this->MetaDataJSON[UniStoreName][Entry]["installed"];
	return { };
}


/*
	The save call.

	Write to file.. called on destructor.
*/
void Meta::SaveCall() {
	FILE *File = fopen(_META_PATH, "w");

	if (File) {
		const std::string Dump = this->MetaDataJSON.dump(1, '\t');
		fwrite(Dump.c_str(), 1, Dump.size(), File);
		fclose(File);
	}
};