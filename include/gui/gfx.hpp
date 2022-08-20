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
#include "theme.hpp"
#include <citro2d.h>
#include <string>

extern std::unique_ptr<Theme> UIThemes;

/* Standard Colors. */
#define WHITE C2D_Color32(255, 255, 255, 255)
#define BLACK C2D_Color32(0, 0, 0, 255)
#define TRANSPARENT C2D_Color32(0, 0, 0, 0)
#define DIM_COLOR C2D_Color32(0, 0, 0, 190)

namespace GFX {
	void DrawTop(void);
	void DrawBottom();
	void DrawSprite(int img, int x, int y, float ScaleX = 1, float ScaleY = 1);
	void DrawBox(float xPos, float yPos, float width = 50, float height = 50, bool selected = false, uint32_t clr = UIThemes->BoxInside());
	void DrawCheckbox(float xPos, float yPos, bool selected);
	void DrawToggle(float xPos, float yPos, bool toggled);
	void DrawTime();
	void DrawBattery();
	void HandleBattery();
	void DrawWifi();
	void DrawIcon(const int Idx, int X, int Y, uint32_t Color = UIThemes->SideBarIconColor(), float BlendPower = 1.0f, float ScaleX = 1, float ScaleY = 1);
};

#endif