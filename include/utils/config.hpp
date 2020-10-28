/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 Universal-Team
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

#include <3ds.h>
#include <string>

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
	void list(const bool &v) { this->v_list = v; if (!this->changesMade) this->changesMade = true; };

	/* Auto update on boot. */
	bool autoupdate() const { return this->v_autoUpdate; };
	void autoupdate(const bool &v) { this->v_autoUpdate = v; if (!this->changesMade) this->changesMade = true; };

	std::string _3dsxPath() const { return this->v_3dsxPath; };
	void _3dsxPath(const std::string &v) { this->v_3dsxPath = v; if (!this->changesMade) this->changesMade = true; };

	std::string ndsPath() const { return this->v_ndsPath; };
	void ndsPath(const std::string &v) { this->v_ndsPath = v; if (!this->changesMade) this->changesMade = true; };

	std::string archPath() const { return this->v_archivePath; };
	void archPath(const std::string &v) { this->v_archivePath = v; if (!this->changesMade) this->changesMade = true; };
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

	std::string v_language = "en", v_lastStore = "universal-db-beta.unistore",
				v_3dsxPath = "sdmc:/3ds", v_ndsPath = "sdmc:", v_archivePath = "sdmc:";
	bool v_list = false, v_autoUpdate = true;
};

#endif
