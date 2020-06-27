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

#ifndef _UNIVERSAL_UPDATER_COLOR_HELPER_HPP
#define _UNIVERSAL_UPDATER_COLOR_HELPER_HPP

#include <citro2d.h>
#include <citro3d.h>
#include <sstream>
#include <string>

/**
 * @brief Creates a 8 byte RGBA color
 * @param r red component of the color
 * @param g green component of the color
 * @param b blue component of the color
 * @param a alpha component of the color
 */
#define RGBA8(r, g, b, a) ((((r)&0xFF)<<0) | (((g)&0xFF)<<8) | (((b)&0xFF)<<16) | (((a)&0xFF)<<24))

#define BarColor 				C2D_Color32(57, 84, 114, 255)
#define TopBGColor 				C2D_Color32(96, 168, 192, 255)
#define BottomBGColor 			C2D_Color32(38, 44, 77, 255)
#define SelectedColordefault	C2D_Color32(120, 192, 216, 255)
#define UnselectedColordefault	C2D_Color32(77, 118, 132, 255)
#define BLACK 					C2D_Color32(0, 0, 0, 255)
#define WHITE 					C2D_Color32(255, 255, 255, 255)
#define TRANSPARENT				C2D_Color32(0, 0, 0, 0)

typedef u32 Color;

namespace ColorHelper {
	int getColorValue(int color, int bgr);
	std::string getColorName(int color, int bgr);
}

#endif