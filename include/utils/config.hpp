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
	void addMissingThings();

	// Bar Color.
	u32 barColor() { return this->v_barColor; }
	void barColor(u32 v) { this->v_barColor = v; if (!this->changesMade)	this->changesMade = true; }
	// Top BG Color.
	u32 topBG() { return this->v_topBG; }
	void topBG(u32 v) { this->v_topBG = v; if (!this->changesMade)	this->changesMade = true; }
	// Bottom BG Color.
	u32 bottomBG() { return this->v_bottomBG; }
	void bottomBG(u32 v) { this->v_bottomBG = v; if (!this->changesMade)	this->changesMade = true; }
	// Text Color.
	u32 textColor() { return this->v_textColor; }
	void textColor(u32 v) { this->v_textColor = v; if (!this->changesMade)	this->changesMade = true; }
	// Button Color.
	u32 buttonColor() { return this->v_buttonColor; }
	void buttonColor(u32 v) { this->v_buttonColor = v; if (!this->changesMade)	this->changesMade = true; }
	// Selected Color.
	u32 selectedColor() { return this->v_selectedColor; }
	void selectedColor(u32 v) { this->v_selectedColor = v; if (!this->changesMade)	this->changesMade = true; }
	// Unselected Color.
	u32 unselectedColor() { return this->v_unselectedColor; }
	void unselectedColor(u32 v) { this->v_unselectedColor = v; if (!this->changesMade)	this->changesMade = true; }
	// Script Path.
	std::string scriptPath() { return this->v_scriptPath; }
	void scriptPath(std::string v) { this->v_scriptPath = v; if (!this->changesMade)	this->changesMade = true; }
	// Lang Path.
	int langPath() { return this->v_langPath; }
	void langPath(int v) { this->v_langPath = v; if (!this->changesMade)	this->changesMade = true; }
	// View Mode.
	int viewMode() { return this->v_viewMode; }
	void viewMode(int v) { this->v_viewMode = v; if (!this->changesMade)	this->changesMade = true; }
	// Progressbar Color.
	u32 progressbarColor() { return this->v_progressbarColor; }
	void progressbarColor(u32 v) { this->v_progressbarColor = v; if (!this->changesMade)	this->changesMade = true; }
	// Music Path.
	std::string musicPath() { return this->v_musicPath; }
	void musicPath(std::string v) { this->v_musicPath = v; if (!this->changesMade)	this->changesMade = true; }
	// Logging.
	bool logging() { return this->v_logging; }
	void logging(bool v) { this->v_logging = v; if (!this->changesMade)	this->changesMade = true; }
	// Use bars.
	bool useBars() { return this->v_useBars; }
	void useBars(bool v) { this->v_useBars = v; if (!this->changesMade)	this->changesMade = true; }
	// Autoboot.
	int autoboot() { return this->v_autoboot; }
	void autoboot(int v) { this->v_autoboot = v; if (!this->changesMade)	this->changesMade = true; }
	// Store Path.
	std::string storePath() { return this->v_storePath; }
	void storePath(std::string v) { this->v_storePath = v; if (!this->changesMade)	this->changesMade = true; }
	// Autoboto file.
	std::string autobootFile() { return this->v_autobootFile; }
	void autobootFile(std::string v) { this->v_autobootFile = v; if (!this->changesMade)	this->changesMade = true; }
	// Outdated Script Color.
	u32 outdatedColor() { return this->v_outdatedColor; }
	void outdatedColor(u32 v) { this->v_outdatedColor = v; if (!this->changesMade)	this->changesMade = true; }
	// Uptodate Script Color.
	u32 uptodateColor() { return this->v_uptodateColor; }
	void uptodateColor(u32 v) { this->v_uptodateColor = v; if (!this->changesMade)	this->changesMade = true; }
	// Not found Script Color.
	u32 notfoundColor() { return this->v_notfoundColor; }
	void notfoundColor(u32 v) { this->v_notfoundColor = v; if (!this->changesMade)	this->changesMade = true; }
	// Future Script Color.
	u32 futureColor() { return this->v_futureColor; }
	void futureColor(u32 v) { this->v_futureColor = v; if (!this->changesMade)	this->changesMade = true; }
	// Keydelay.
	int keyDelay() { return this->v_keyDelay; }
	void keyDelay(int v) { this->v_keyDelay = v; if (!this->changesMade)	this->changesMade = true; }
	// Screen Fade.
	bool screenFade() { return this->v_screenFade; }
	void screenFade(bool v) { this->v_screenFade = v; if (!this->changesMade)	this->changesMade = true; }
	// Progressbar Display.
	bool progressDisplay() { return this->v_progressDisplay; }
	void progressDisplay(bool v) { this->v_progressDisplay = v; if (!this->changesMade)	this->changesMade = true; }
	// Language.
	std::string language() { return this->v_language; }
	void language(std::string v) { this->v_language = v; if (!this->changesMade)	this->changesMade = true; }
	// First startup.
	bool firstStartup() { return this->v_firstStartup; }
	void firstStartup(bool v) { this->v_firstStartup = v; if (!this->changesMade)	this->changesMade = true; }
	// Use ScriptColors.
	bool useScriptColor() { return this->v_useScriptColor; }
	void useScriptColor(bool v) { this->v_useScriptColor = v; if (!this->changesMade)	this->changesMade = true; }
	// Show Downloadspeed.
	bool showSpeed() { return this->v_showSpeed; }
	void showSpeed(bool v) { this->v_showSpeed = v; if (!this->changesMade)	this->changesMade = true; }

	// Variables.
	std::string _3dsxpath() { return this->v_3dsx_install_path; }
	void _3dsxpath(std::string v) { this->v_3dsx_install_path = v; if (!this->changesMade)	this->changesMade = true; }
	std::string ndspath() { return this->v_nds_install_path; }
	void ndspath(std::string v) { this->v_nds_install_path = v; if (!this->changesMade)	this->changesMade = true; }
	std::string archivepath() { return this->v_archive_path; }
	void archivepath(std::string v) { this->v_archive_path = v; if (!this->changesMade)	this->changesMade = true; }
	
	// Mainly helper.
	bool getBool(const std::string &key);
	void setBool(const std::string &key, bool v);
	int getInt(const std::string &key);
	void setInt(const std::string &key, int v);
	std::string getString(const std::string &key);
	void setString(const std::string &key, const std::string &v);
private:
	nlohmann::json json; // Our private JSON file.
	bool changesMade = false, initialChanges = false;
	int configVersion = 2;

	// Color variables and more.
	u32 v_barColor, v_topBG, v_bottomBG, v_textColor, v_buttonColor, v_selectedColor, v_unselectedColor, v_progressbarColor,
		v_outdatedColor, v_uptodateColor, v_notfoundColor, v_futureColor;
	std::string v_scriptPath, v_musicPath, v_storePath, v_autobootFile, v_language;
	int v_langPath, v_viewMode, v_autoboot, v_keyDelay;
	bool v_logging, v_useBars, v_screenFade, v_progressDisplay, v_firstStartup, v_useScriptColor, v_showSpeed;

	// Some variables.
	std::string v_3dsx_install_path, v_nds_install_path, v_archive_path;
};

#endif