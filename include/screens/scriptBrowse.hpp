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

#ifndef SCRIPTBROWSE_HPP
#define SCRIPTBROWSE_HPP

#include "common.hpp"
#include "fileBrowse.hpp"
#include "structs.hpp"

#include <vector>

class ScriptBrowse : public Screen
{
public:
	void Draw(void) const override;
	void Logic(u32 hDown, u32 hHeld, touchPosition touch) override;
	ScriptBrowse();
private:
	// Selection.
	mutable int screenPos = 0;
	mutable int screenPosList = 0;
	mutable int Selection = 0;
	int dropSelection = 0;
	int mode = 0;
	bool dropDownMenu = false;
	bool loaded = false;

	// Draws.
	void DrawBrowse(void) const;
	void DrawGlossary(void) const;

	void DropDownLogic(u32 hDown, u32 hHeld, touchPosition touch);

	nlohmann::json infoJson;
	int maxScripts;

	// Browse stuff.
	int keyRepeatDelay = 0;
	int fastMode = false;
	std::vector<DirEntry> dirContents;
	void refresh();
	void downloadAll();

	// Button | Icon struct.
	std::vector<Structs::ButtonPos> arrowPos = {
		{295, 0, 25, 25}, // Arrow Up.
		{295, 215, 25, 25}, // Arrow Down.
		{0, 215, 25, 25}, // Back Arrow.
		{5, 0, 25, 25} // Dropdown Menu.
	};

	// DropDownMenu.
	std::vector<Structs::ButtonPos> dropPos = {
		{5, 30, 25, 25}, // Download All.
		{5, 70, 25, 25}, // Refresh.
		{5, 110, 25, 25} // ViewMode.
	};
	std::vector<Structs::ButtonPos> dropPos2 = {
		{0, 28, 140, 30}, // Download All.
		{0, 68, 140, 30}, // Refresh.
		{0, 108, 140, 30} // ViewMode.
	};
};

#endif