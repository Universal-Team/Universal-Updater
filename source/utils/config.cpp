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
#include "config.hpp"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include <array>

using namespace rapidjson;

/*
	Detects system language and is used later to set app language to system language.
*/
const std::string &Config::sysLang() const {
	static const std::array<std::string, 12> keys = {
		"jp",    // Japanese
		"en",    // English
		"fr",    // French
		"de",    // German
		"it",    // Italian
		"es",    // Spanish
		"zh-CN", // Chinese (Simplified)
		"ko",    // Korean
		"nl",    // Dutch
		"pt",    // Portuguese
		"ru",    // Russian
		"zh-TW"  // Chinese (Traditional)
	};

	u8 language = 1;
	CFGU_GetSystemLanguage(&language);
	return language < keys.size() ? keys[language] : keys[1];
}

/*
	Set the Proxy URL.
*/
void Config::proxyUrl(const std::string &v) {
	this->changesMade = true;
	this->v_proxy = v;

	// If the proxy starts '<' (invalid char, <DEFAULT>), then try use the system proxy
	if (!v.empty() && v[0] == '<') {
		bool enabled = false;
		ACU_GetProxyEnable(&enabled);

		if (enabled) {
			u16 port;
			char host[0x100], userName[0x20], password[0x20];
			ACU_GetProxyPort(&port);
			ACU_GetProxyHost(host);
			ACU_GetProxyUserName(userName);
			ACU_GetProxyPassword(password);

			std::string userPass = "";
			if (strcmp(userName, "") != 0) {
				userPass = std::string(userName) + ":" + password + "@";
			}

			this->v_proxyUrl = std::string("http://") + userPass + host + ":" + std::to_string(port);
		} else {
			this->v_proxyUrl = "";
		}
	} else {
		this->v_proxyUrl = v;
	}
}

/*
	Constructor of the config.
*/
Config::Config() {
	FILE *file = fopen(_CONFIG_PATH, "rt");
	if (!file) return;

	Document json;
	char *readBuffer = new char[0x10000];
	FileReadStream is(file, readBuffer, 0x10000);
	json.ParseStream(is);
	delete[] readBuffer;
	fclose(file);

	// So we don't error on future configs
	if (!json.HasMember("Version") || json["Version"].GetInt() != 1) return;

	if (json.HasMember("Language") && json["Language"].IsString())                 this->language(json["Language"].GetString());
	if (json.HasMember("LastStore") && json["LastStore"].IsString())               this->lastStore(json["LastStore"].GetString());
	if (json.HasMember("_3DSX_Path") && json["_3DSX_Path"].IsString())             this->_3dsxPath(json["_3DSX_Path"].GetString());
	if (json.HasMember("NDS_Path") && json["NDS_Path"].IsString())                 this->ndsPath(json["NDS_Path"].GetString());
	if (json.HasMember("Archive_Path") && json["Archive_Path"].IsString())         this->archPath(json["Archive_Path"].GetString());
	if (json.HasMember("Shortcut_Path") && json["Shortcut_Path"].IsString())       this->shortcut(json["Shortcut_Path"].GetString());
	if (json.HasMember("Firm_Path") && json["Firm_Path"].IsString())               this->firmPath(json["Firm_Path"].GetString());
	if (json.HasMember("Active_Theme") && json["Active_Theme"].IsString())         this->theme(json["Active_Theme"].GetString());

	if (json.HasMember("List") && json["List"].IsBool())                           this->list(json["List"].GetBool());
	if (json.HasMember("AutoUpdate") && json["AutoUpdate"].IsBool())               this->autoupdate(json["AutoUpdate"].GetBool());
	if (json.HasMember("UpdateCheck") && json["UpdateCheck"].IsBool())             this->updatecheck(json["UpdateCheck"].GetBool());
	if (json.HasMember("UpdateNightly") && json["UpdateNightly"].IsBool())         this->updategit(json["UpdateNightly"].GetBool());
	if (json.HasMember("UseUniStoreBG") && json["UseUniStoreBG"].IsBool())         this->usebg(json["UseUniStoreBG"].GetBool());
	if (json.HasMember("CustomFont") && json["CustomFont"].IsBool())               this->customfont(json["CustomFont"].GetBool());
	if (json.HasMember("Display_Changelog") && json["Display_Changelog"].IsBool()) this->changelog(json["Display_Changelog"].GetBool());
	if (json.HasMember("_3DSX_InFolder") && json["_3DSX_InFolder"].IsBool())       this->_3dsxInFolder(json["_3DSX_InFolder"].GetBool());
	if (json.HasMember("UseAccentColor") && json["UseAccentColor"].IsBool())       this->useAccentColor(json["UseAccentColor"].GetBool());
	if (json.HasMember("SortAscending") && json["SortAscending"].IsBool())         this->sortAscending(json["SortAscending"].GetBool());

	if (json.HasMember("SortBy") && json["SortBy"].IsInt())                        this->sortBy((SortType)json["SortBy"].GetInt());

	this->proxyUrl((json.HasMember("ProxyURL") && json["ProxyURL"].IsString()) ? json["ProxyURL"].GetString() : "<DEFAULT>");

	if(json.HasMember("SavedPrompts") && json["SavedPrompts"].IsObject()) {
		for(const auto &prompt : json["SavedPrompts"].GetObject()) {
			if (prompt.value.IsBool()) {
				this->v_savedPrompts[prompt.name.GetString()] = prompt.value.GetBool();
			}
		}
	}
}

/* Write to config if changesMade. */
void Config::save() {
	if (!this->changesMade) return;
	this->changesMade = false;

	Document json;
	json.SetObject();
	Document::AllocatorType &a = json.GetAllocator();

	json.AddMember("Version", Value(1), a);

	json.AddMember("Language",      Value().SetString(this->language().c_str(),  this->language().size(), a),  a);
	json.AddMember("LastStore",     Value().SetString(this->lastStore().c_str(), this->lastStore().size(), a), a);
	json.AddMember("_3DSX_Path",    Value().SetString(this->_3dsxPath().c_str(), this->_3dsxPath().size(), a), a);
	json.AddMember("NDS_Path",      Value().SetString(this->ndsPath().c_str(),   this->ndsPath().size(), a),   a);
	json.AddMember("Archive_Path",  Value().SetString(this->archPath().c_str(),  this->archPath().size(), a),  a);
	json.AddMember("Shortcut_Path", Value().SetString(this->shortcut().c_str(),  this->shortcut().size(), a),  a);
	json.AddMember("Firm_Path",     Value().SetString(this->firmPath().c_str(),  this->firmPath().size(), a),  a);
	json.AddMember("Active_Theme",  Value().SetString(this->theme().c_str(),     this->theme().size(), a),     a);
	json.AddMember("ProxyURL",      Value().SetString(this->proxyStr().c_str(),  this->proxyStr().size(), a),  a);

	json.AddMember("List",              Value().SetBool(this->list()),           a);
	json.AddMember("AutoUpdate",        Value().SetBool(this->autoupdate()),     a);
	json.AddMember("SortAscending",     Value().SetBool(this->sortAscending()),  a);
	json.AddMember("_3DSX_InFolder",    Value().SetBool(this->_3dsxInFolder()),  a);
	json.AddMember("UpdateCheck",       Value().SetBool(this->updatecheck()),    a);
	json.AddMember("UpdateNightly",     Value().SetBool(this->updategit()),      a);
	json.AddMember("UseUniStoreBG",     Value().SetBool(this->usebg()),          a);
	json.AddMember("UseAccentColor",    Value().SetBool(this->useAccentColor()), a);
	json.AddMember("CustomFont",        Value().SetBool(this->customfont()),     a);
	json.AddMember("Display_Changelog", Value().SetBool(this->changelog()),      a);

	json.AddMember("SortBy", Value((int)this->sortBy()), a);

	json.AddMember(Value("SavedPrompts"), Value(kObjectType), a);
	for(const auto &prompt : this->v_savedPrompts) {
		json["SavedPrompts"].AddMember(Value(prompt.first.c_str(), prompt.first.size()), Value().SetBool(prompt.second), a);
	}

	FILE *out = fopen(_CONFIG_PATH, "w");
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
