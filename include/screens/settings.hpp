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
#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "screens/screen.hpp"
#include "screens/screenCommon.hpp"

#include "utils/structs.hpp"

#include <vector>

class Settings : public Screen
{
public:
	void Draw(void) const override;
	void Logic(u32 hDown, u32 hHeld, touchPosition touch) override;
private:
	void DrawLanguageSelection(void) const;
	void DrawColorChanging(void) const;
	void LanguageSelection(u32 hDown, touchPosition touch);
	void colorChanging(u32 hDown, touchPosition touch);

	std::vector<Structs::ButtonPos> langBlocks = {
		{37, 52, 20, 20, -1},
		{37, 92, 20, 20, -1},
		{37, 132, 20, 20, -1},
		{37, 172, 20, 20, -1},
		{177, 52, 20, 20, -1},
		{177, 92, 20, 20, -1},
		{177, 132, 20, 20, -1},
		{177, 172, 20, 20, -1},
		{293, 213, 27, 27, -1},
	};

	std::vector<Structs::ButtonPos> buttons = {
		{10, 85, 95, 41, -1},
		{115, 85, 95, 41, -1},
		{220, 85, 95, 41, -1},
	};

};

#endif