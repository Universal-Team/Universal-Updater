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

#ifndef SCRIPTCREATOR_HPP
#define SCRIPTCREATOR_HPP

#include "common.hpp"
#include "json.hpp"
#include "structs.hpp"

#include <vector>

class ScriptCreator : public Screen {
public:
	void Draw(void) const override;
	void Logic(u32 hDown, u32 hHeld, touchPosition touch) override;

private:
	std::string entryName = "";
	
	// Screen draws.
	void DrawSubMenu(void) const;
	void DrawScriptScreen(void) const;

	void SubMenuLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void scriptLogic(u32 hDown, u32 hHeld, touchPosition touch);

	// Selection + Mode.
	int Selection = 0;
	int mode = 0;
	int page = 0;

	// Functions.
	void openJson(std::string fileName);
	void createNewJson(std::string fileName);
	void save();
	void setInfoStuff(void);

	// Creating Functions. -> Page 1.
	void createDownloadRelease();
	void createDownloadFile();
	void createDeleteFile();
	void createExtractFile();
	void createInstallCia();
	void createMkDir();

	// Creating Functions. -> Page 2.
	void createRmDir();
	void createMkFile();
	void createTimeMsg();
	void createSaveConfig();
	void createBootTitle();
	void createPromptMessage();

	// Creating Functions. -> Page 3.
	void createCopy();
	void createMove();

	// 
	void setBool(const std::string &object, const std::string &key, bool v);
	void setBool2(const std::string &object, const std::string &key, const std::string &key2, bool v);

	void setInt(const std::string &object, const std::string &key, int v);
	void setInt2(const std::string &object, const std::string &key, const std::string &key2, int v);

	void setString(const std::string &object, const std::string &key, const std::string &v);
	void setString2(const std::string &object, const std::string &key, const std::string &key2, const std::string &v);

	void createEntry(const std::string &Entryname);

	std::string jsonFileName;
	nlohmann::json editScript;
	// Main Pos.
	const std::vector<Structs::ButtonPos> mainButtons = {
		{90, 40, 140, 35}, // New Script.
		{90, 100, 140, 35}  // Existing Script.
	};

	// Creator Button Pos.
	const std::vector<Structs::ButtonPos> creatorButtons = {
		{10, 40, 140, 35},
		{170, 40, 140, 35},
		{10, 100, 140, 35},
		{170, 100, 140, 35},
		{10, 160, 140, 35},
		{170, 160, 140, 35}
	};

};

#endif