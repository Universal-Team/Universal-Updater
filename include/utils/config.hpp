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

#include "rapidjson/document.h"
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

	/* Language. */
	std::string language() const { return this->v_language; };
	void language(const std::string &v) { this->v_language = v; this->changesMade = true; };

	/* Last Store. */
	std::string lastStore() const { return this->v_lastStore; };
	void lastStore(const std::string &v) { this->v_lastStore = v; this->changesMade = true; };

	/* Using Top List. */
	bool list() const { return this->v_list; };
	void list(bool v) { this->v_list = v; this->changesMade = true; };

	/* Sort Method. */
	SortType sortBy() const { return this->v_sortBy; };
	void sortBy(SortType v) { this->v_sortBy = v; this->changesMade = true; };

	bool sortAscending() const { return this->v_sortAscending; };
	void sortAscending(bool v) { this->v_sortAscending = v; this->changesMade = true; };

	/* Auto update on boot. */
	bool autoupdate() const { return this->v_autoUpdate; };
	void autoupdate(bool v) { this->v_autoUpdate = v; this->changesMade = true; };

	bool _3dsxInFolder() const { return this->v_3dsxInFolder; };
	void _3dsxInFolder(bool v) { this->v_3dsxInFolder = v; this->changesMade = true; };

	std::string _3dsxPath() const { return this->v_3dsxPath; };
	void _3dsxPath(const std::string &v) { this->v_3dsxPath = v; this->changesMade = true; };

	std::string ndsPath() const { return this->v_ndsPath; };
	void ndsPath(const std::string &v) { this->v_ndsPath = v; this->changesMade = true; };

	std::string archPath() const { return this->v_archivePath; };
	void archPath(const std::string &v) { this->v_archivePath = v; this->changesMade = true; };

	std::string firmPath() const { return this->v_firmPath; };
	void firmPath(const std::string &v) { this->v_firmPath = v; this->changesMade = true; };

	/* Fetching old metadata. */
	bool metadata() const { return this->v_metadata; };
	void metadata(bool v) { this->v_metadata = v; this->changesMade = true; };

	/* U-U Update check on startup. */
	bool updatecheck() const { return this->v_updateCheck; };
	void updatecheck(bool v) { this->v_updateCheck = v; this->changesMade = true; };

	/* Check for git/release on startup. */
	bool updategit() const { return this->v_updateGit; };
	void updategit(bool v) { this->v_updateGit = v; this->changesMade = true; };

	/* U-U Update check on startup. */
	bool usebg() const { return this->v_showBg; };
	void usebg(bool v) { this->v_showBg = v; this->changesMade = true; };

	/* If using custom Font. */
	bool customfont() const { return this->v_customFont; };
	void customfont(bool v) { this->v_customFont = v; this->changesMade = true; };

	/* The shortcut path. */
	std::string shortcut() const { return this->v_shortcutPath; };
	void shortcut(const std::string &v) { this->v_shortcutPath = v; this->changesMade = true; };

	/* If displaying changelog. */
	bool changelog() const { return this->v_changelog; };
	void changelog(bool v) { this->v_changelog = v; this->changesMade = true; };

	/* The active Theme. */
	std::string theme() const { return this->v_theme; };
	void theme(const std::string &v) { this->v_theme = v; this->changesMade = true; };

	/* If accent color should be used. */
	bool useAccentColor() const { return this->v_useAccentColor; };
	void useAccentColor(bool v) { this->v_useAccentColor = v; };

	/* Saved prompts. */
	PromptValue savedPrompt(const std::string &name) const {
		// Count of a std::map always returns either 0 or 1
		if (this->v_savedPrompts.count(name) == 0) return PromptValue::Unset;
		return this->v_savedPrompts.at(name) ? PromptValue::Yes : PromptValue::No;
	}
	void savePrompt(const std::string &name, bool v) { this->v_savedPrompts[name] = v; this->changesMade = true; }

	std::string proxyStr() const { return this->v_proxy; };
	std::string proxyUrl() const { return this->v_proxyUrl; };
	void proxyUrl(const std::string &v);
private:
	const std::string &sysLang() const;

	bool changesMade = false;

	std::map<std::string, bool> v_savedPrompts;

	std::string v_language = this->sysLang(),
				v_lastStore = "universal-db.unistore",
				v_3dsxPath = "sdmc:/3ds",
				v_ndsPath = "sdmc:",
				v_archivePath = "sdmc:",
				v_shortcutPath = "sdmc:/3ds/Universal-Updater/shortcuts",
				v_firmPath = "sdmc:/luma/payloads",
				v_theme = "Default",
				v_proxy = "<DEFAULT>",
				v_proxyUrl = "";

	bool v_list = false,
		v_autoUpdate = true,
		v_metadata = true,
		v_updateCheck = true,
		v_updateGit = false,
		v_showBg = true,
		v_customFont = false,
		v_changelog = true,
		v_3dsxInFolder = false,
		v_useAccentColor = true,
		v_sortAscending = false;

	SortType v_sortBy = SortType::POPULARITY;
};

#endif
