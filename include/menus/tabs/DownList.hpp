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

#ifndef _UNIVERSAL_UPDATER_DOWN_LIST_HPP
#define _UNIVERSAL_UPDATER_DOWN_LIST_HPP

#include "structs.hpp"
#include <vector>


class DownList {
public:
	void DrawTopOvl();
	void Draw();
	void Handler();
	void Reset();
private:
	size_t SelectedIndex = 0, ScreenIndex = 0;

	static constexpr Structs::ButtonPos DownPos[7] = {
		{ 46, 32, 241, 22 },
		{ 46, 62, 241, 22 },
		{ 46, 92, 241, 22 },
		{ 46, 122, 241, 22 },
		{ 46, 152, 241, 22 },
		{ 46, 182, 241, 22 },
		{ 46, 212, 241, 22 }
	};

	static constexpr Structs::ButtonPos InstallPos[7] = {
		{ 288, 32, 24, 24 },
		{ 288, 62, 24, 24 },
		{ 288, 92, 24, 24 },
		{ 288, 122, 24, 24 },
		{ 288, 152, 24, 24 },
		{ 288, 182, 24, 24 },
		{ 288, 212, 24, 24 }
	};
};

#endif