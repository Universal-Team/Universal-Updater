/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2021 Universal-Team
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

#ifndef _UNIVERSAL_UPDATER_SETTINGS_MENU_HPP
#define _UNIVERSAL_UPDATER_SETTINGS_MENU_HPP

#include "structs.hpp"


class SettingsMenu {
public:
	void Draw();
	void Handler();
private:
	enum class Menu : uint8_t { Main = 0 };

	size_t Selection = 0, ScreenPos = 0;
	Menu _Menu = Menu::Main;

	/* Main Settings. */
	static constexpr Structs::ButtonPos MainPos[7] = {
		{ 45, 32, 271, 22 },
		{ 45, 62, 271, 22 },
		{ 45, 92, 271, 22 },
		{ 45, 122, 271, 22 },
		{ 45, 152, 271, 22 },
		{ 45, 182, 271, 22 },
		{ 45, 212, 271, 22 }
	};

	void DrawMain();
	void HandleMain();
	const std::string MainStrings[2] = { "Select UniStore", "Exit Universal-Updater" };
};

#endif