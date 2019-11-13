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
#ifndef SCRIPTLIST_HPP
#define SCRIPTLIST_HPP

#include "screens/screen.hpp"
#include "screens/screenCommon.hpp"

#include "utils/fileBrowse.h"

class ScriptList : public Screen
{
public:
	void Draw(void) const override;
	void Logic(u32 hDown, u32 hHeld, touchPosition touch) override;
	ScriptList();

private:
	void DrawList(void) const;
	void DrawSingleObject(void) const;

	void ListSelection(u32 hDown, u32 hHeld);
	void SelectFunction(u32 hDown, u32 hHeld);

	int mode = 0;
	std::vector<DirEntry> dirContents;
	mutable int screenPos = 0;
	mutable int screenPosList = 0;
	mutable int selection = 0;
	mutable int screenPos2 = 0;
	mutable int screenPosList2 = 0;
	mutable int selection2 = 0;

	int keyRepeatDelay = 0;
	int fastMode = false;
};

Result createFile(const char * path);
#endif