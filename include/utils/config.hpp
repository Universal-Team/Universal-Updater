/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

namespace Config {
	extern int LangPath, lang, Color1, Color2, Color3, TxtColor, SelectedColor, UnselectedColor, viewMode, progressbarColor;
	extern std::string ScriptPath, MusicPath, StorePath;
	extern bool Logging, UseBars;

	void load();
	void save();
	void initializeNewConfig();

	bool getBool(const std::string &key);
	void setBool(const std::string &key, bool v);

	int getInt(const std::string &key);
	void setInt(const std::string &key, int v);

	std::string getString(const std::string &key);
	void setString(const std::string &key, const std::string &v);
}

#endif