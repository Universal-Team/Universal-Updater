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
#include "fileBrowse.hpp"
#include "meta.hpp"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"

using namespace rapidjson;

std::string Meta::emptyString;
std::vector<std::string> Meta::emptyVector;

/*
	The Constructor of the Meta.

	Includes MetaData file creation, if non existent.
*/
Meta::Meta() {
	FILE *file = fopen(_META_PATH, "r");
	if (!file) return;

	Document json;
	char *readBuffer = new char[0x10000];
	FileReadStream is(file, readBuffer, 0x10000);
	json.ParseStream(is);
	delete[] readBuffer;
	fclose(file);

	if (!json.IsObject()) return;

	for (const auto &store : json.GetObject()) {
		if (!store.value.IsObject()) continue;
		for (const auto &app : store.value.GetObject()) {
			if (!app.value.IsObject()) continue;
			std::vector<std::string> installed;
			std::string updated;
			int marks = 0;

			if (app.value.HasMember("installed") && app.value["installed"].IsArray()) {
				for (const Value &item : app.value["installed"].GetArray()) {
					if (item.IsString()) installed.push_back(item.GetString());
				}
			}

			if (app.value.HasMember("updated") && app.value["updated"].IsString())
				updated = app.value["updated"].GetString();

			if (app.value.HasMember("marks") && app.value["marks"].IsInt())
				marks = app.value["marks"].GetInt();

			this->apps[store.name.GetString()][app.name.GetString()] = {installed, updated, marks};
		}
	}
}

/*
	The save call.

	Write to file.. called on destructor.
*/
void Meta::Save() {
	if (!this->changesMade) return;
	this->changesMade = false;

	Document json;
	json.SetObject();
	Document::AllocatorType &a = json.GetAllocator();

	for (const auto &store : this->apps) {
		json.AddMember(Value(store.first.c_str(), store.first.size(), a), Value(kObjectType), a);
		for (const auto &app : store.second) {
			// If no info, skip!
			if (app.second.installed.empty() && app.second.updated.empty() && app.second.marks == 0)
				continue;

			json[store.first.c_str()].AddMember(Value(app.first.c_str(), app.first.size(), a), Value(kObjectType), a);
			Value &appJson = json[store.first.c_str()][app.first.c_str()];

			if (!app.second.installed.empty()) {
				appJson.AddMember("installed", Value(kArrayType), a);
				for (const std::string &file : app.second.installed) {
					appJson["installed"].PushBack(Value().SetString(file.c_str(), file.size(), a), a);
				}
			}

			if (!app.second.updated.empty())
				appJson.AddMember("updated", Value().SetString(app.second.updated.c_str(), app.second.updated.size(), a), a);

			if (app.second.marks != 0)
				appJson.AddMember("marks", Value(app.second.marks), a);
		}
	}

	FILE *out = fopen(_META_PATH, "w");
	if (out) {
		char *writeBuffer = new char[0x10000];
		FileWriteStream os(out, writeBuffer, 0x10000);

		PrettyWriter<FileWriteStream> writer(os);
		writer.SetIndent('\t', 1);
		json.Accept(writer);

		delete[] writeBuffer;
		fclose(out);
	}
}

bool Meta::UpdateAvailable(const std::string &unistoreName, const std::string &entry, const std::string &updated) const {
	if (!this->GetUpdated(unistoreName, entry).empty() && !updated.empty()) {
		return strcasecmp(updated.c_str(), this->GetUpdated(unistoreName, entry).c_str()) > 0;
	}
	return false;
}

/* Remove installed state from a download list entry. */
void Meta::RemoveInstalled(const std::string &unistoreName, const std::string &entry, const std::string &name) {
	std::vector<std::string> &installs = this->apps[unistoreName][entry].installed;
	if (installs.empty()) return;

	for (auto it = installs.begin(); it != installs.end(); it++) {
		if (*it == name) {
			installs.erase(it);
			this->changesMade = true;
			break;
		}
	}

	if (installs.empty()) {
		this->apps[unistoreName][entry].updated = "";
		this->changesMade = true;
	}
}
