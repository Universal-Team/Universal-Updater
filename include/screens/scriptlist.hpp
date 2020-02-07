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

#ifndef SCRIPTLIST_HPP
#define SCRIPTLIST_HPP

#include "common.hpp"
#include "fileBrowse.hpp"
#include "structs.hpp"

#include <vector>

class ScriptList : public Screen
{
public:
	void Draw(void) const override;
	void Logic(u32 hDown, u32 hHeld, touchPosition touch) override;
private:
	void DrawSubMenu(void) const;
	void DrawList(void) const;
	void DrawSingleObject(void) const;

	void SubMenuLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void ListSelection(u32 hDown, u32 hHeld, touchPosition touch);
	void SelectFunction(u32 hDown, u32 hHeld, touchPosition touch);

	int mode = 0;
	int SubSelection = 0;
	std::vector<DirEntry> dirContents;
	mutable int screenPos = 0;
	mutable int screenPosList = 0;
	mutable int selection = 0;
	mutable int screenPos2 = 0;
	mutable int screenPosList2 = 0;
	mutable int selection2 = 0;

	int keyRepeatDelay = 0;
	int fastMode = false;

	std::vector<Structs::ButtonPos> arrowPos = {
		{295, 0, 25, 25}, // Arrow Up.
		{295, 215, 25, 25}, // Arrow Down.
		{0, 215, 25, 25}, // Back Arrow.
		{5, 0, 25, 25}, // viewMode Change
		{45, 0, 25, 25}, // Search.
	};

	std::vector<Structs::ButtonPos> subPos = {
		{10, 70, 140, 40}, // Script list.
		{170, 70, 140, 40}, // Get Scripts.
		{10, 145, 140, 40}, // Script Creator.
		{170, 145, 140, 40}, // Script path change.
	};
};

#endif