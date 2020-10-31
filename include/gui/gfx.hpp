/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 Universal-Team
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

/*
	Define all used Colors, for easier changes.
*/

/* Standard Colors. */
#define WHITE C2D_Color32(255, 255, 255, 255)
#define BLACK C2D_Color32(0, 0, 0, 255)
#define TRANSPARENT C2D_Color32(0, 0, 0, 0)
#define DIM_COLOR C2D_Color32(0, 0, 0, 190)

/* Bar, Text, BG Colors. */
#define TEXT_COLOR WHITE
#define BAR_COLOR C2D_Color32(50, 73, 98, 255)
#define BAR_OUTL_COLOR C2D_Color32(25, 30, 53, 255)
#define BG_COLOR C2D_Color32(38, 44, 77, 255)

/* Entry Colors. */
#define ENTRY_BAR_COLOR BAR_COLOR
#define ENTRY_BAR_OUTL_COLOR BAR_OUTL_COLOR

/* Entry Box Colors. */
#define BOX_INSIDE_COLOR C2D_Color32(28, 33, 58, 255)
#define BOX_SELECTED_COLOR C2D_Color32(108, 130, 155, 255)
#define BOX_UNSELECTED_COLOR BLACK

/* Progressbar Colors. */
#define PROGRESSBAR_OUT_COLOR BOX_INSIDE_COLOR
#define PROGRESSBAR_IN_COLOR SIDEBAR_UNSELECTED_COLOR

/* Search Menu Colors. */
#define SEARCH_BAR_COLOR C2D_Color32(51, 75, 102, 255)
#define SEARCH_BAR_OUTL_COLOR BAR_OUTL_COLOR

/* Sidebar Colors. */
#define SIDEBAR_SELECTED_COLOR C2D_Color32(108, 130, 155, 255)
#define SIDEBAR_UNSELECTED_COLOR C2D_Color32(77, 101, 128, 255)

namespace GFX {
	void DrawTop(void);
	void DrawBottom();
	void DrawSprite(const int &img, const int &x, const int &y, const float &ScaleX = 1, const float &ScaleY = 1);
	void DrawBox(const float &xPos, const float &yPos, const float &width = 50, const float &height = 50, const bool &selected = false, const uint32_t &clr = BOX_INSIDE_COLOR);
	void DrawButton(const float &xPos, const float &yPos, const float &width, const float &height, const bool &selected, const std::string &Text = "");
	void DrawCheckbox(const float &xPos, const float &yPos, const bool &selected);
	void DrawToggle(float xPos, float yPos, bool toggled);
};

#endif