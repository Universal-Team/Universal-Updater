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

#include "Common.hpp"
#include "Meta.hpp"
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

	/* TODO: fetch old MetaData from Universal-Updater v2.5.1 and below and only do it one time at first start ever -> TODO: Config. */
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
		if (Info[Idx].Title != "" && OldJSON.contains(Info[Idx].FileName)) {
			for(auto It = OldJSON[Info[Idx].FileName].begin(); It != OldJSON[Info[Idx].FileName].end(); ++It) {
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