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

#include "storeEntry.hpp"
#include "common.hpp"
#include <regex>

using namespace rapidjson;

/*
	Fetch informations on constructor.

	const std::unique_ptr<Store> &store: Const Reference to the store class.
	const std::unique_ptr<Meta> &meta: Const Reference to the meta class.
	int index: Index of the entry.
*/
StoreEntry::StoreEntry(const Value &json, const Store &store) {
	if (!json.HasMember("info") || !json["info"].IsObject()) return;
	const Value &info = json["info"];

	this->UniStore = store.GetInfo().title;

	if (info.HasMember("title") && info["title"].IsString())
		this->Title = info["title"].GetString();
	if (info.HasMember("author") && info["author"].IsString())
		this->Author = info["author"].GetString();
	if (info.HasMember("description") && info["description"].IsString())
		this->Description = info["description"].GetString();
	if (info.HasMember("license") && info["license"].IsString())
		this->License = info["license"].GetString();

	if (info.HasMember("version") && info["version"].IsString())
		this->Version = info["version"].GetString();
	if (info.HasMember("last_updated") && info["last_updated"].IsString())
		this->LastUpdated = info["last_updated"].GetString();
	if (info.HasMember("releasenotes") && info["releasenotes"].IsString())
		this->ReleaseNotes = info["releasenotes"].GetString();

	if (info.HasMember("wiki") && info["wiki"].IsString())
		this->Wiki = info["wiki"].GetString();
	if (info.HasMember("preinstall_message") && info["preinstall_message"].IsString())
		this->PreinstallMessage = info["preinstall_message"].GetString();

	if (info.HasMember("stars") && info["stars"].IsInt())
		this->Stars = info["stars"].GetInt();
	if (info.HasMember("color") && info["color"].IsString())
		this->AccentColor = StringUtils::ParseColorHexString(info["color"].GetString());

	if (info.HasMember("llm_generation") && info["llm_generation"].IsString())
		this->LlmGeneration = info["llm_generation"].GetString();

	if (this->LlmGeneration == "" || this->LlmGeneration == "unknown") {
		this->LlmGeneration = Lang::get("LLM_UNKNOWN");
	} else if (this->LlmGeneration == "yes") {
		this->LlmGeneration = Lang::get("YES");
	} else if (this->LlmGeneration == "no") {
		this->LlmGeneration = Lang::get("NO");
	} else if (this->LlmGeneration == "minor") {
		this->LlmGeneration = Lang::get("LLM_MINOR");
	}

	if (info.HasMember("installed_files")) {
		const Value &installedFiles = info["installed_files"];
		if (installedFiles.IsArray()) {
			for (const Value &val : installedFiles.GetArray()) {
				if (val.IsString()) this->InstalledFiles.push_back(val.GetString());
			}
		}
	}
	if (info.HasMember("title_ids")) {
		const Value &titleIds = info["title_ids"];
		if (titleIds.IsArray()) {
			for (const Value &val : titleIds.GetArray()) {
				if (val.IsInt()) this->TitleIds.push_back(val.GetInt());
			}
		}
	}

	if (info.HasMember("category")) {
		const Value &categories = info["category"];
		if (categories.IsArray()) {
			for (const Value &val : categories.GetArray()) {
				if (val.IsString()) this->FullCategory.push_back(val.GetString());
			}
		}
		this->Category = StringUtils::FetchStringsFromVector(this->FullCategory);
	}
	if (info.HasMember("console")) {
		const Value &consoles = info["console"];
		if (consoles.IsArray()) {
			for (const Value &val : consoles.GetArray()) {
				if (val.IsString()) this->FullConsole.push_back(val.GetString());
			}
		}
		this->Console = StringUtils::FetchStringsFromVector(this->FullConsole);
	}

	if (info.HasMember("screenshots")) {
		const Value &screenshots = info["screenshots"];
		if (screenshots.IsArray()) {
			for (const Value &screenshot : screenshots.GetArray()) {
				if (screenshot.IsObject()) {
					if (screenshot.HasMember("description") && screenshot["description"].IsString()
						&& screenshot.HasMember("url") && screenshot["url"].IsString()) {
						this->ScreenshotNames.push_back(screenshot["description"].GetString());
						this->Screenshots.push_back(screenshot["url"].GetString());
					}
				}
			}
		}
		this->Console = StringUtils::FetchStringsFromVector(this->FullConsole);
	}

	if (info.HasMember("sheet_index") && info["sheet_index"].IsInt())
		this->SheetIndex = info["sheet_index"].GetInt();

	int iconIndex = -1;
	if (info.HasMember("icon_index") && info["icon_index"].IsInt())
		iconIndex = info["icon_index"].GetInt();

	this->vHasIcon = store.GetIconValid(iconIndex, this->SheetIndex);
	this->Icon = store.GetIconEntry(iconIndex, this->SheetIndex);

	this->UpdateAvailable = StoreUtils::meta->UpdateAvailable(this->UniStore, this->Title, this->LastUpdated);
	this->Marks = StoreUtils::meta->GetMarks(this->UniStore, this->Title);
	this->MarkString = StringUtils::GetMarkString(this->Marks);

	this->Installed = CheckInstalled();

	for (const auto &item : json.GetObject()) {
		const std::string &name = item.name.GetString();
		if (name == "info") continue;

		bool installed = StoreUtils::meta->GetInstalled(this->UniStore, this->Title, name);

		if (item.value.IsArray()) {
			this->Scripts.emplace_back(name, item.value, installed);
		} else if (item.value.IsObject()) {
			std::string size, type;
			if (item.value.HasMember("size") && item.value["size"].IsString())
				size = item.value["size"].GetString();
			if (item.value.HasMember("type") && item.value["type"].IsString())
				type = item.value["type"].GetString();

			if (item.value.HasMember("script") && item.value["script"].IsArray())
				this->Scripts.emplace_back(name, item.value["script"], installed, size, type);
		}
	}

	sort(this->Scripts.begin(), this->Scripts.end(), [](const Script &lhs, const Script &rhs) {
		return StringUtils::toUpperCase(lhs.GetName()) < StringUtils::toUpperCase(rhs.GetName());
	});
}

bool StoreEntry::CheckInstalled() const {
	char temp[16];
	for (int uniqueId : this->TitleIds) {
		Result res = AM_GetTitleProductCode(MEDIATYPE_SD, 0x0004000000000000 | (uniqueId << 8), temp);
		if (R_SUCCEEDED(res)) {
			return true;
		}
	}

	for (std::string path : this->InstalledFiles) {
		if (config) {
			path = std::regex_replace(path, std::regex("%ARCHIVE_DEFAULT%"), config->archPath());
			path = std::regex_replace(path, std::regex("%3DSX%/(.*)\\.(.*)"), config->_3dsxPath() + (config->_3dsxInFolder() ? "/$1/$1.$2" : "/$1.$2"));
			path = std::regex_replace(path, std::regex("%3DSX%"), config->_3dsxPath());
			path = std::regex_replace(path, std::regex("%NDS%"), config->ndsPath());
			path = std::regex_replace(path, std::regex("%FIRM%"), config->firmPath());
		}

		if (access(path.c_str(), F_OK) == 0) {
			return true;
		}
	}

	return false;
}