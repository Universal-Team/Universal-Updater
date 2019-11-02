/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019 VoltZ, Epicpkmn11, Flame, RocketRobz, TotallyNotGuy
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

#include "gui.hpp"

#include "utils/inifile.h"
#include "utils/config.hpp"

#include <string>
#include <unistd.h>

using std::string;
using std::wstring;

static CIniFile settingsini( "sdmc:/3ds/Universal-Updater/Settings.ini" );

int Config::lang; // Current Language.
int Config::Color1;
int Config::Color2;
int Config::Color3;
int Config::TxtColor;
int Config::SelectedColor;
int Config::UnselectedColor;

void Config::loadConfig() {
	// [UI]
	Config::lang = settingsini.GetInt("UI", "LANGUAGE", 1);
	Config::Color1 = settingsini.GetInt("UI", "BARCOLOR", BarColor);
	Config::Color2 = settingsini.GetInt("UI", "TOPBGCOLOR", TopBGColor);
	Config::Color3 = settingsini.GetInt("UI", "BOTTOMBGCOLOR", BottomBGColor);
	Config::TxtColor = settingsini.GetInt("UI", "TEXTCOLOR", WHITE);
	Config::SelectedColor = settingsini.GetInt("UI", "SELECTEDCOLOR", SelectedColordefault);
	Config::UnselectedColor = settingsini.GetInt("UI", "UNSELECTEDCOLOR", UnselectedColordefault);
}

void Config::saveConfig() {
	// [UI]
	settingsini.SetInt("UI", "LANGUAGE", Config::lang);
	settingsini.SetInt("UI", "BARCOLOR", Config::Color1);
	settingsini.SetInt("UI", "TOPBGCOLOR", Config::Color2);
	settingsini.SetInt("UI", "BOTTOMBGCOLOR", Config::Color3);
	settingsini.SetInt("UI", "TEXTCOLOR", Config::TxtColor);
	settingsini.SetInt("UI", "SELECTEDCOLOR", Config::SelectedColor);
	settingsini.SetInt("UI", "UNSELECTEDCOLOR", Config::UnselectedColor);
	settingsini.SaveIniFile("sdmc:/3ds/Universal-Updater/Settings.ini");
}