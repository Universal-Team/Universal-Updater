/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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
#include "gui.hpp"
#include "json.hpp"

#include <string>
#include <unistd.h>

int Config::lang; // Current Language.
int Config::Color1;
int Config::Color2;
int Config::Color3;
int Config::TxtColor;
int Config::SelectedColor;
int Config::UnselectedColor;
int Config::viewMode;
int Config::progressbarColor;
std::string Config::ScriptPath;
std::string Config::MusicPath;
bool Config::Logging;
bool Config::UseBars;
std::string Config::StorePath;
int Config::LangPath;
bool Config::GodMode = false;
int Config::autoboot = 0;
std::string Config::AutobootFile = "";
int Config::outdated;
int Config::uptodate;
int Config::notFound;
int Config::future;
int Config::Button;
nlohmann::json configJson;

void Config::load() {
	FILE* file = fopen("sdmc:/3ds/Universal-Updater/Settings.json", "r");
	if(file) {
		configJson = nlohmann::json::parse(file, nullptr, false);

		if(!configJson.contains("BARCOLOR")) {
			Color1 = BarColor;
		} else {
			Color1 = getInt("BARCOLOR");
		}

		if(!configJson.contains("TOPBGCOLOR")) {
			Color2 = TopBGColor;
		} else {
			Color2 = getInt("TOPBGCOLOR");
		}

		if(!configJson.contains("BOTTOMBGCOLOR")) {
			Color3 = BottomBGColor;
		} else {
			Color3 = getInt("BOTTOMBGCOLOR");
		}

		if(!configJson.contains("TEXTCOLOR")) {
			TxtColor = WHITE;
		} else {
			TxtColor = getInt("TEXTCOLOR");
		}

		if(!configJson.contains("UNSELECTEDCOLOR")) {
			UnselectedColor = UnselectedColordefault;
		} else {
			UnselectedColor = getInt("UNSELECTEDCOLOR");
		}

		if(!configJson.contains("SELECTEDCOLOR")) {
			SelectedColor = SelectedColordefault;
		} else {
			SelectedColor = getInt("SELECTEDCOLOR");
		}

		if(!configJson.contains("SCRIPTPATH")) {
			ScriptPath = SCRIPTS_PATH;
		} else {
			ScriptPath = getString("SCRIPTPATH");
		}

		if(!configJson.contains("LANGPATH")) {
			LangPath = 0;
		} else {
			LangPath = getInt("LANGPATH");
		}

		if(!configJson.contains("LANGUAGE")) {
			lang = 2;
		} else {
			lang = getInt("LANGUAGE");
		}

		if(!configJson.contains("VIEWMODE")) {
			viewMode = 0;
		} else {
			viewMode = getInt("VIEWMODE");
		}

		if(!configJson.contains("PROGRESSBARCOLOR")) {
			progressbarColor = WHITE;
		} else {
			progressbarColor = getInt("PROGRESSBARCOLOR");
		}

		if(!configJson.contains("MUSICPATH")) {
			MusicPath = MUSIC_PATH;
		} else {
			MusicPath = getString("MUSICPATH");
		}

		if(!configJson.contains("LOGGING")) {
			Logging = false;
		} else {
			Logging = getBool("LOGGING");
		}

		if(!configJson.contains("BARS")) {
			UseBars = true;
		} else {
			UseBars = getBool("BARS");
		}

		if(!configJson.contains("STOREPATH")) {
			StorePath = STORE_PATH;
		} else {
			StorePath = getString("STOREPATH");
		}

		if(!configJson.contains("AUTOBOOT")) {
			autoboot = 0;
		} else {
			autoboot = getInt("AUTOBOOT");
		}

		if(!configJson.contains("AUTOBOOT_FILE")) {
			AutobootFile = "";
		} else {
			AutobootFile = getString("AUTOBOOT_FILE");
		}

		if(!configJson.contains("OUTDATED")) {
			outdated = C2D_Color32(0xfb, 0x5b, 0x5b, 255);
		} else {
			outdated = getInt("OUTDATED");
		}

		if(!configJson.contains("UPTODATE")) {
			uptodate = C2D_Color32(0xa5, 0xdd, 0x81, 255);
		} else {
			uptodate = getInt("UPTODATE");
		}

		if(!configJson.contains("NOTFOUND")) {
			notFound = C2D_Color32(255, 128, 0, 255);
		} else {
			notFound = getInt("NOTFOUND");
		}

		if(!configJson.contains("FUTURE")) {
			future = C2D_Color32(255, 255, 0, 255);
		} else {
			future = getInt("FUTURE");
		}

		if(!configJson.contains("BUTTON")) {
			Button = C2D_Color32(0, 0, 50, 255);
		} else {
			Button = getInt("BUTTON");
		}

		fclose(file);
	} else {
		Color1 = BarColor;
		Color2 = TopBGColor;
		Color3 = BottomBGColor;
		TxtColor = WHITE;
		SelectedColor = SelectedColordefault;
		UnselectedColor = UnselectedColordefault;
		ScriptPath = SCRIPTS_PATH;
		LangPath = 0;
		lang = 2;
		viewMode = 0;
		progressbarColor = WHITE;
		MusicPath = MUSIC_PATH;
		Logging = false;
		UseBars = true;
		StorePath = STORE_PATH;
		autoboot = 0;
		AutobootFile = "";
		outdated = C2D_Color32(0xfb, 0x5b, 0x5b, 255);
		uptodate = C2D_Color32(0xa5, 0xdd, 0x81, 255);
		notFound = C2D_Color32(255, 128, 0, 255);
		future	 = C2D_Color32(255, 255, 0, 255);
		Button 	 = C2D_Color32(0, 0, 50, 255);
	}
}

void Config::save() {
	setInt("BARCOLOR", Color1);
	setInt("TOPBGCOLOR", Color2);
	setInt("BOTTOMBGCOLOR", Color3);
	setInt("TEXTCOLOR", TxtColor);
	setInt("SELECTEDCOLOR", SelectedColor);
	setInt("UNSELECTEDCOLOR", UnselectedColor);
	setString("SCRIPTPATH", ScriptPath);
	setInt("LANGPATH", LangPath);
	setInt("LANGUAGE", lang);
	setInt("VIEWMODE", viewMode);
	setInt("PROGRESSBARCOLOR", progressbarColor);
	setString("MUSICPATH", MusicPath);
	setBool("LOGGING", Logging);
	setBool("BARS", UseBars);
	setString("STOREPATH", StorePath);
	setInt("AUTOBOOT", autoboot);
	setString("AUTOBOOT_FILE", AutobootFile);
	setInt("OUTDATED", outdated);
	setInt("UPTODATE", uptodate);
	setInt("NOTFOUND", notFound);
	setInt("FUTURE", future);
	setInt("BUTTON", Button);

	FILE* file = fopen("sdmc:/3ds/Universal-Updater/Settings.json", "w");
	if(file)	fwrite(configJson.dump(1, '\t').c_str(), 1, configJson.dump(1, '\t').size(), file);
	fclose(file);
}

void Config::initializeNewConfig() {
	FILE* file = fopen("sdmc:/3ds/Universal-Updater/Settings.json", "r");
	if(file) configJson = nlohmann::json::parse(file, nullptr, false);

	setInt("BARCOLOR", BarColor);
	setInt("TOPBGCOLOR", TopBGColor);
	setInt("BOTTOMBGCOLOR", BottomBGColor);
	setInt("TEXTCOLOR", WHITE);
	setInt("SELECTEDCOLOR", SelectedColordefault);
	setInt("UNSELECTEDCOLOR", UnselectedColordefault);
	setString("SCRIPTPATH", SCRIPTS_PATH);
	setInt("LANGPATH", 0);
	setInt("LANGUAGE", 2);
	setInt("VIEWMODE", 0);
	setInt("PROGRESSBARCOLOR", WHITE);
	setString("MUSICPATH", MUSIC_PATH);
	setBool("LOGGING", false);
	setBool("BARS", true);
	setString("STOREPATH", STORE_PATH);
	setInt("AUTOBOOT", 0);
	setString("AUTOBOOT_FILE", "");
	setInt("OUTDATED", C2D_Color32(0xfb, 0x5b, 0x5b, 255));
	setInt("UPTODATE", C2D_Color32(0xa5, 0xdd, 0x81, 255));
	setInt("NOTFOUND", C2D_Color32(255, 128, 0, 255));
	setInt("FUTURE", C2D_Color32(255, 255, 0, 255));
	setInt("BUTTON", C2D_Color32(0, 0, 50, 255));

	if(file)	fwrite(configJson.dump(1, '\t').c_str(), 1, configJson.dump(1, '\t').size(), file);
	fclose(file);
}

bool Config::getBool(const std::string &key) {
	if(!configJson.contains(key)) {
		return false;
	}
	return configJson.at(key).get_ref<const bool&>();
}
void Config::setBool(const std::string &key, bool v) {
	configJson[key] = v;
}

int Config::getInt(const std::string &key) {
	if(!configJson.contains(key)) {
		return 0;
	}
	return configJson.at(key).get_ref<const int64_t&>();
}
void Config::setInt(const std::string &key, int v) {
	configJson[key] = v;
}

std::string Config::getString(const std::string &key) {
	if(!configJson.contains(key)) {
		return "";
	}
	return configJson.at(key).get_ref<const std::string&>();
}
void Config::setString(const std::string &key, const std::string &v) {
	configJson[key] = v;
}