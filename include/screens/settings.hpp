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

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "screens/screen.hpp"
#include "screens/screenCommon.hpp"

#include "utils/structs.hpp"

#include <vector>

class Settings : public screen
{
public:
	void Draw(void) const override;
	void Logic(u32 hDown, u32 hHeld, touchPosition touch) override;
private:
	// Credits stuff.
	int DisplayMode = 1;
	int delay = 120;
	bool discordText = false;

	int colorMode = 0;
	int mode = 0;
	int Selection = 0;

	void DrawSubMenu(void) const;
	void DrawLanguageSelection(void) const;
	void DrawColorChanging(void) const;
	void DrawCreditsScreen(void) const;
	void SubMenuLogic(u32 hDown, u32 hHeld, touchPosition touch);
	void LanguageSelection(u32 hDown, touchPosition touch);
	void colorChanging(u32 hDown, touchPosition touch);
	void CreditsLogic(u32 hDown, touchPosition touch);

	std::vector<Structs::ButtonPos> mainButtons = {
		{90, 40, 140, 35, -1}, // Language.
		{90, 100, 140, 35, -1}, // Colors.
		{90, 160, 140, 35, -1}, // Misc.
	};

	std::vector<Structs::ButtonPos> langBlocks = {
		{37, 32, 20, 20, -1},
		{37, 72, 20, 20, -1},
		{37, 112, 20, 20, -1},
		{37, 152, 20, 20, -1},
		{37, 188, 20, 20, -1},
		{177, 32, 20, 20, -1},
		{177, 72, 20, 20, -1},
		{177, 112, 20, 20, -1},
		{177, 152, 20, 20, -1},
		{177, 188, 20, 20, -1},
	};

	std::vector<Structs::ButtonPos> buttons = {
		{10, 85, 95, 41, -1},
		{115, 85, 95, 41, -1},
		{220, 85, 95, 41, -1},
	};

	std::vector<Structs::ButtonPos> barPos = {
		{0, 215, 320, 25, -1},
	};
};

#endif