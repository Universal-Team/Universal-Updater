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

#ifndef _UNIVERSAL_UPDATER_GFX_HPP
#define _UNIVERSAL_UPDATER_GFX_HPP

#include "common.hpp"
#include "sprites.h"
#include <citro2d.h>
#include <string>

/* Standard Colors. */
#define WHITE C2D_Color32(255, 255, 255, 255)
#define BLACK C2D_Color32(0, 0, 0, 255)
#define TRANSPARENT C2D_Color32(0, 0, 0, 0)
#define DIM_COLOR C2D_Color32(0, 0, 0, 190)

struct UITheme {
	uint32_t BarColor;
	uint32_t BGColor;
	uint32_t BarOutline;
	uint32_t TextColor;
	uint32_t EntryBar;
	uint32_t EntryOutline;
	uint32_t BoxInside;
	uint32_t BoxSelected;
	uint32_t BoxUnselected;
	uint32_t ProgressbarOut;
	uint32_t ProgressbarIn;
	uint32_t SearchBar;
	uint32_t SearchbarOutline;
	uint32_t SideBarSelected;
	uint32_t SideBarUnselected;
	/* NOTE: Also used for the buttons. */
	uint32_t MarkSelected;
	uint32_t MarkUnselected;
	uint32_t DownListPrev;
	uint32_t SideBarIconColor;
};

namespace GFX {
	extern std::vector<UITheme> Themes;
	extern int SelectedTheme;

	void DrawTop(void);
	void DrawBottom();
	void DrawSprite(int img, int x, int y, float ScaleX = 1, float ScaleY = 1);
	void DrawBox(float xPos, float yPos, float width = 50, float height = 50, bool selected = false, uint32_t clr = GFX::Themes[GFX::SelectedTheme].BoxInside);
	void DrawCheckbox(float xPos, float yPos, bool selected);
	void DrawToggle(float xPos, float yPos, bool toggled);
	void DrawTime();
	void DrawBattery();
	void HandleBattery();
	void DrawIcon(const int Idx, int X, int Y, float ScaleX = 1, float ScaleY = 1);
};

#endif