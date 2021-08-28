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

#ifndef _UNIVERSAL_UPDATER_GFX_DATA_HPP
#define _UNIVERSAL_UPDATER_GFX_DATA_HPP

#include "sprites.h"
#include <citro2d.h>
#include <string>
#include <vector>

/* For now hardcoded; TODO: Themes. */
#define BAR_COLOR		C2D_Color32(50, 73, 98, 255)
#define BAR_OUTLINE 	C2D_Color32(25, 30, 53, 255)
#define BG_COLOR		C2D_Color32(38, 44, 77, 255)
#define TEXT_COLOR		C2D_Color32(255, 255, 255, 255)
#define TABS_SELECTED	C2D_Color32(108, 130, 155, 255)
#define TABS_UNSELECTED	C2D_Color32(77, 101, 128, 255)
#define TAB_ICON_COLOR	C2D_Color32(173, 204, 239, 255)

/* Text sizes. */
#define TEXT_LARGE	0.6f
#define TEXT_MEDIUM	0.5f
#define TEXT_SMALL	0.4f

/* Top grid/list positions. */
#define TOP_GRID_X(Pos) (25 + (Pos % 5) * 75)
#define TOP_GRID_Y(Pos) (45 + (Pos / 5) * 60)
#define TOP_GRID_W 50
#define TOP_GRID_H 50

#define TOP_LIST_X 20
#define TOP_LIST_Y(Pos) (45 + Pos * 60)
#define TOP_LIST_W 360
#define TOP_LIST_H 50

class GFXData {
public:
	GFXData();
	~GFXData();

	void StartFrame();
	void EndFrame();
	void UpdateFont(const bool Top) { };

	void DrawTop();
	void DrawBottom();

	/* UniStore Sprite Handler. */
	void LoadUniStoreSheet(const std::string &SheetFile);
	void UnloadUniStoreSheets();
	void DrawUniStoreIcons(const std::vector<std::tuple<int, int, bool>> &Indexes);

	void DrawBox(const int XPos, const int YPos, const int Width = 50, const int Height = 50, const bool Selected = false);
	void DrawSprite(const int Idx, const int X, const int Y);
	void DrawSpriteBlend(const int Idx, const int X, const int Y, const uint32_t Color = TAB_ICON_COLOR);
	void DrawCheckbox(const int XPos, const int YPos, const bool Selected);
	void DrawToggle(const int XPos, const int YPos, const bool Toggled);
	void DrawSort(const int XPos, const int YPos, const bool Checked);
private:
	C2D_SpriteSheet Sprites = nullptr;
	C2D_Font Font = nullptr;

	std::vector<C2D_SpriteSheet> UniStoreSheets; // UniStore Icon SpriteSheets.
};

#endif