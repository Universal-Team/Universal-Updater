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
#include "json.hpp"
#include "scriptUtils.hpp"
#include <string>
#include <unistd.h>

/*
	Detects system language and is used later to set app language to system language.
*/
void Config::sysLang() {
	u8 language = 1;
	CFGU_GetSystemLanguage(&language);

	switch(language) {
		case 0:
			this->language("jp"); // Japanese
			break;

		case 1:
			this->language("en"); // English
			break;

		case 2:
			this->language("fr"); // French
			break;

		case 3:
			this->language("de"); // German
			break;

		case 4:
			this->language("it"); // Italian
			break;

		case 5:
			this->language("es"); // Spanish
			break;

		case 6:
			this->language("zh-CN"); // Chinese (Simplified)
			break;
		
		// case 7:
		// 	this->language("ko"); // Korean
		// 	break;
		
		// case 8:
		// 	this->language("nl"); // Dutch
		// 	break;

		case 9:
			this->language("pt"); // Portuguese
			break;

		case 10:
			this->language("ru"); // Russian
			break;

		case 11:
			this->language("zh-TW"); // Chinese (Traditional)
			break;
			
		default:
			this->language("en"); // Fall back to English if missing
			break;
	}
}

/*
	In case it doesn't exist.
*/
void Config::initialize() {
	FILE *temp = fopen("sdmc:/3ds/Universal-Updater/Config.json", "w");
	char tmp[2] = { '{', '}' };
	fwrite(tmp, sizeof(tmp), 1, temp);
	fclose(temp);
}

/*
	Constructor of the config.
*/
Config::Config() {
	if (access("sdmc:/3ds/Universal-Updater/Config.json", F_OK) != 0) {
		this->initialize();
	}

	FILE *file = fopen("sdmc:/3ds/Universal-Updater/Config.json", "r");
	this->json = nlohmann::json::parse(file, nullptr, false);
	fclose(file);

	/* Let us create a new one. */
	if (!this->json.contains("Version")) this->initialize();

	if (this->json.contains("LastStore")) this->lastStore(this->getString("LastStore"));
	if (this->json.contains("List")) this->list(this->getBool("List"));
	if (this->json.contains("AutoUpdate")) this->autoupdate(this->getBool("AutoUpdate"));
	if (this->json.contains("_3DSX_Path")) this->_3dsxPath(this->getString("_3DSX_Path"));
	if (this->json.contains("_3DSX_InFolder")) this->_3dsxInFolder(this->getBool("_3DSX_InFolder"));
	if (this->json.contains("NDS_Path")) this->ndsPath(this->getString("NDS_Path"));
	if (this->json.contains("Archive_Path")) this->archPath(this->getString("Archive_Path"));
	if (this->json.contains("Firm_Path")) this->firmPath(this->getString("Firm_Path"));
	if (this->json.contains("MetaData")) this->metadata(this->getBool("MetaData"));
	if (this->json.contains("UpdateCheck")) this->updatecheck(this->getBool("UpdateCheck"));
	if (this->json.contains("UseBG")) this->usebg(this->getBool("UseBG"));
	if (this->json.contains("CustomFont")) this->customfont(this->getBool("CustomFont"));
	if (this->json.contains("DownloadedFont")) this->downloadedFont(this->getString("DownloadedFont"));
	if (this->json.contains("Shortcut_Path")) this->shortcut(this->getString("Shortcut_Path"));
	if (this->json.contains("Display_Changelog")) this->changelog(this->getBool("Display_Changelog"));
	if (this->json.contains("Active_Theme")) this->theme(this->getInt("Active_Theme"));
	if (this->json.contains("Prompt")) this->prompt(this->getBool("Prompt"));

	if (!this->json.contains("Language")) {
		this->sysLang();
		if((this->language() == "zh-CN" || this->language() == "zh-TW") && (access("sdmc:/3ds/Universal-Updater/font.bcfnt", F_OK) != 0 || this->downloadedFont() != this->language())) {
			ScriptUtils::downloadFile("https://github.com/Universal-Team/extras/raw/master/files/" + this->language() + ".bcfnt", "sdmc:/3ds/Universal-Updater/font.bcfnt", "Downloading compatible font...", true);
			this->downloadedFont(this->language());
			this->customfont(true);
		}
	} else this->language(this->getString("Language"));

	this->changesMade = false; // No changes made yet.
}

/* Write to config if changesMade. */
void Config::save() {
	if (this->changesMade) {
		FILE *file = fopen("sdmc:/3ds/Universal-Updater/Config.json", "w");

		/* Set values. */
		this->setString("Language", this->language());
		this->setInt("Version", 1);
		this->setString("LastStore", this->lastStore());
		this->setBool("List", this->list());
		this->setBool("AutoUpdate", this->autoupdate());
		this->setString("_3DSX_Path", this->_3dsxPath());
		this->setBool("_3DSX_InFolder", this->_3dsxInFolder());
		this->setString("NDS_Path", this->ndsPath());
		this->setString("Archive_Path", this->archPath());
		this->setString("Firm_Path", this->firmPath());
		this->setBool("MetaData", this->metadata());
		this->setBool("UpdateCheck", this->updatecheck());
		this->setBool("UseBG", this->usebg());
		this->setBool("CustomFont", this->customfont());
		this->setString("DownloadedFont", this->downloadedFont());
		this->setString("Shortcut_Path", this->shortcut());
		this->setBool("Display_Changelog", this->changelog());
		this->setInt("Active_Theme", this->theme());
		this->setBool("Prompt", this->prompt());

		/* Write changes to file. */
		const std::string dump = this->json.dump(1, '\t');
		fwrite(dump.c_str(), 1, this->json.dump(1, '\t').size(), file);
		fclose(file);
	}
}

/* Helper functions. */
bool Config::getBool(const std::string &key) {
	if (this->json.is_discarded()) return false;
	if (!this->json.contains(key)) return false;

	return this->json.at(key).get_ref<const bool &>();
}
void Config::setBool(const std::string &key, bool v) {
	if (this->json.is_discarded()) return;
	this->json[key] = v;
};

int Config::getInt(const std::string &key) {
	if (this->json.is_discarded()) return 0;
	if (!this->json.contains(key)) return 0;

	return this->json.at(key).get_ref<const int64_t &>();
}
void Config::setInt(const std::string &key, int v) {
	if (this->json.is_discarded()) return;
	this->json[key] = v;
};

std::string Config::getString(const std::string &key) {
	if (this->json.is_discarded()) return "";
	if (!this->json.contains(key)) return "";

	return this->json.at(key).get_ref<const std::string &>();
}
void Config::setString(const std::string &key, const std::string &v) {
	if (this->json.is_discarded()) return;
	this->json[key] = v;
};
