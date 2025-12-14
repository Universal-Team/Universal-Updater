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

#ifndef _UNIVERSAL_UPDATER_CONFIG_HPP
#define _UNIVERSAL_UPDATER_CONFIG_HPP

#include "json.hpp"
#include "store/storeUtils.hpp"

#include <3ds.h>
#include <string>

enum class PromptValue {
	Unset,
	Yes,
	No
};

class Config {
public:
	Config();
	void save();
	void initialize();
	void sysLang();

	/* Language. */
	std::string language() const { return this->v_language; };
	void language(const std::string &v) { this->v_language = v; if (!this->changesMade) this->changesMade = true; };

	/* Last Store. */
	std::string lastStore() const { return this->v_lastStore; };
	void lastStore(const std::string &v) { this->v_lastStore = v; if (!this->changesMade) this->changesMade = true; };

	/* Using Top List. */
	bool list() const { return this->v_list; };
	void list(bool v) { this->v_list = v; if (!this->changesMade) this->changesMade = true; };

	/* Sort Method. */
	SortType sortBy() const { return this->v_sortBy; };
	void sortBy(SortType v) { this->v_sortBy = v; if (!this->changesMade) this->changesMade = true; };

	bool sortAscending() const { return this->v_sortAscending; };
	void sortAscending(bool v) { this->v_sortAscending = v; if (!this->changesMade) this->changesMade = true; };

	/* Auto update on boot. */
	bool autoupdate() const { return this->v_autoUpdate; };
	void autoupdate(bool v) { this->v_autoUpdate = v; if (!this->changesMade) this->changesMade = true; };

	bool _3dsxInFolder() const { return this->v_3dsxInFolder; };
	void _3dsxInFolder(bool v) { this->v_3dsxInFolder = v; if (!this->changesMade) this->changesMade = true; };

	std::string _3dsxPath() const { return this->v_3dsxPath; };
	void _3dsxPath(const std::string &v) { this->v_3dsxPath = v; if (!this->changesMade) this->changesMade = true; };

	std::string ndsPath() const { return this->v_ndsPath; };
	void ndsPath(const std::string &v) { this->v_ndsPath = v; if (!this->changesMade) this->changesMade = true; };

	std::string archPath() const { return this->v_archivePath; };
	void archPath(const std::string &v) { this->v_archivePath = v; if (!this->changesMade) this->changesMade = true; };

	std::string firmPath() const { return this->v_firmPath; };
	void firmPath(const std::string &v) { this->v_firmPath = v; if (!this->changesMade) this->changesMade = true; };

	/* Fetching old metadata. */
	bool metadata() const { return this->v_metadata; };
	void metadata(bool v) { this->v_metadata = v; if (!this->changesMade) this->changesMade = true; };

	/* U-U Update check on startup. */
	bool updatecheck() const { return this->v_updateCheck; };
	void updatecheck(bool v) { this->v_updateCheck = v; if (!this->changesMade) this->changesMade = true; };

	/* Check for nightly/release on startup. */
	bool updatenightly() const { return this->v_updateNightly; };
	void updatenightly(bool v) { this->v_updateNightly = v; if (!this->changesMade) this->changesMade = true; };

	/* U-U Update check on startup. */
	bool usebg() const { return this->v_showBg; };
	void usebg(bool v) { this->v_showBg = v; if (!this->changesMade) this->changesMade = true; };

	/* If using custom Font. */
	bool customfont() const { return this->v_customFont; };
	void customfont(bool v) { this->v_customFont = v; if (!this->changesMade) this->changesMade = true; };

	/* The shortcut path. */
	std::string shortcut() const { return this->v_shortcutPath; };
	void shortcut(const std::string &v) { this->v_shortcutPath = v; if (!this->changesMade) this->changesMade = true; };

	/* If displaying changelog. */
	bool changelog() const { return this->v_changelog; };
	void changelog(bool v) { this->v_changelog = v; if (!this->changesMade) this->changesMade = true; };

	/* The active Theme. */
	std::string theme() const { return this->v_theme; };
	void theme(const std::string &v) { this->v_theme = v; if (!this->changesMade) this->changesMade = true; };

	/* If accent color should be used. */
	bool useAccentColor() const { return this->v_useAccentColor; };
	void useAccentColor(bool v) { this->v_useAccentColor = v; };

	/* If showing prompt if action failed / succeeded. */
	bool prompt() const { return this->v_prompt; };
	void prompt(bool v) { this->v_prompt = v; if (!this->changesMade) this->changesMade = true; };

	PromptValue savedPrompt(const std::string &name);
	void savePrompt(const std::string &name, bool v);
private:
	/* Mainly helper. */
	bool getBool(const std::string &key);
	void setBool(const std::string &key, bool v);
	int getInt(const std::string &key);
	void setInt(const std::string &key, int v);
	std::string getString(const std::string &key);
	void setString(const std::string &key, const std::string &v);

	nlohmann::json json;
	bool changesMade = false;

	std::vector<std::pair<std::string, bool>> v_savedPrompts;

	std::string v_language = "en", v_lastStore = "universal-db.unistore",
				v_3dsxPath = "sdmc:/3ds", v_ndsPath = "sdmc:", v_archivePath = "sdmc:",
				v_shortcutPath = "sdmc:/3ds/Universal-Updater/shortcuts", v_firmPath = "sdmc:/luma/payloads", v_theme = "Default";

	bool v_list = false, v_autoUpdate = true, v_metadata = true, v_updateCheck = true, v_updateNightly = false,
		v_showBg = false, v_customFont = false, v_changelog = true, v_prompt = true, v_3dsxInFolder = false,
		v_useAccentColor = true, v_sortAscending = false;

	SortType v_sortBy = SortType::LAST_UPDATED;
};

#endif
