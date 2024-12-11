// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

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
	struct UniStoreIcon {
		int index;
		int sheet;
		bool updated;

		UniStoreIcon(int index, int sheet, bool updated)
			: index(index), sheet(sheet), updated(updated) {}
	};

	GFXData();
	~GFXData();

	void StartFrame();
	void EndFrame();

	void DrawTop();
	void DrawBottom();

	/* UniStore Sprite Handler. */
	void LoadUniStoreSheet(const std::string &SheetFile);
	void UnloadUniStoreSheets();
	inline void UpdateUniStoreSprites() { /* NOP */ }
	inline void HideUniStoreSprites() { /* NOP */ }
	void DrawUniStoreIcons(const std::vector<UniStoreIcon> &Icons);

	void DrawBox(const int XPos, const int YPos, const int Width = 50, const int Height = 50, const bool Selected = false);
	void DrawSprite(const int Idx, const int X, const int Y);
	void DrawSpriteBlend(const int Idx, const int X, const int Y, const uint32_t Color = TAB_ICON_COLOR);
	void DrawCheckbox(const int XPos, const int YPos, const bool Selected);
	void DrawToggle(const int XPos, const int YPos, const bool Toggled);
	void DrawSort(const int XPos, const int YPos, const bool Checked);

	bool SpritesDrawn = false;
private:
	C2D_SpriteSheet Sprites = nullptr;
	C2D_Font Font = nullptr;

	std::vector<C2D_SpriteSheet> UniStoreSheets; // UniStore Icon SpriteSheets.
};

#endif