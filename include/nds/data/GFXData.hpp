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

#include "gui.hpp"
#include "sprite.hpp"
#include "sprites.h"
#include "spritesheet.hpp"

#include <string>
#include <vector>

/* For now hardcoded; TODO: Themes. */
#define BAR_COLOR		0x04
#define BAR_OUTLINE		0x08
#define BG_COLOR		0x0C
#define TEXT_COLOR		0x10
#define TABS_SELECTED	0x14
#define TABS_UNSELECTED	0x18
#define TAB_ICON_COLOR	0x1C

/* Text sizes. */
#define TEXT_LARGE	1.0f
#define TEXT_MEDIUM	0.8f
#define TEXT_SMALL	0.7f

/* Top grid/list positions. */
#define TOP_GRID_X(Pos) (15 + (Pos % 5) * 60)
#define TOP_GRID_Y(Pos) (45 + (Pos / 5) * 60)
#define TOP_GRID_W 50
#define TOP_GRID_H 50

#define TOP_LIST_X 15
#define TOP_LIST_Y(Pos) (45 + Pos * 60)
#define TOP_LIST_W 288
#define TOP_LIST_H 50

class GFXData {
public:
	GFXData();
	~GFXData();

	void StartFrame();
	void EndFrame();

	void DrawTop();
	void DrawBottom();

	/* UniStore Sprite Handler. */
	void LoadUniStoreSheet(const std::string &SheetFile);
	void UnloadUniStoreSheets();
	void UpdateUniStoreSprites();
	void HideUniStoreSprites();
	void DrawUniStoreIcons(const std::vector<std::tuple<int, int, bool>> &Indexes);
	
	void DrawBox(const int XPos, const int YPos, const int Width = 50, const int Height = 50, const bool Selected = false);
	void DrawSprite(const int Idx, const int X, const int Y);
	void DrawSpriteBlend(const int Idx, const int X, const int Y, int Color = TAB_ICON_COLOR);
	void DrawCheckbox(const int XPos, const int YPos, const bool Selected);
	void DrawToggle(const int XPos, const int YPos, const bool Toggled);
	void DrawSort(const int XPos, const int YPos, const bool Checked);
private:
	Spritesheet Sprites;

	std::array<Sprite *, 15> UniStoreSprites;

	std::vector<std::string> UniStoreSheetPaths; // Paths to UniStore Icon SpriteSheets.
};

#endif