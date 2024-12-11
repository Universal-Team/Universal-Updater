// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

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
#define TEXT_LARGE	0
#define TEXT_MEDIUM	1
#define TEXT_SMALL	2

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
	struct UniStoreIcon {
		int index;
		int sheet;
		bool updated;
	};

	GFXData();
	~GFXData();

	void StartFrame();
	void EndFrame();

	void DrawTop();
	void DrawBottom();

	/* UniStore Sprite Handler. */
	void LoadUniStoreSheet(const std::string &SheetFile);
	void UnloadUniStoreSheets(void) { /* NOP */ };
	void UpdateUniStoreSprites();
	void HideUniStoreSprites();
	void DrawUniStoreIcons(const std::vector<UniStoreIcon> &Icons);
	
	void DrawBox(const int XPos, const int YPos, const int Width = 50, const int Height = 50, const bool Selected = false);
	void DrawSprite(const int Idx, const int X, const int Y);
	void DrawSpriteBlend(const int Idx, const int X, const int Y, int Color = TAB_ICON_COLOR);
	void DrawCheckbox(const int XPos, const int YPos, const bool Selected);
	void DrawToggle(const int XPos, const int YPos, const bool Toggled);
	void DrawSort(const int XPos, const int YPos, const bool Checked);

	bool SpritesDrawn = false;
private:
	Spritesheet Sprites;

	std::array<Sprite *, 15> UniStoreSprites;

	std::vector<std::string> UniStoreSheetPaths; // Paths to UniStore Icon SpriteSheets.
};

#endif