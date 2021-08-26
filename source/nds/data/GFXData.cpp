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

#include "GFXData.hpp"

#include "Common.hpp"
#include "gui.hpp"

#include <nds.h>


GFXData::GFXData() : Sprites("nitro:/graphics/sprites.tdx") {
	for(size_t i = 0; i < UniStoreSprites.size(); i++) {
		UniStoreSprites[i] = new Sprite(true, SpriteSize_32x32, SpriteColorFormat_Bmp);
	}

	UpdateUniStoreSprites();
};


GFXData::~GFXData() {
	for(Sprite *UniStoreSprite : UniStoreSprites) {
		if (UniStoreSprite) delete UniStoreSprite;
	}
};


void GFXData::DrawTop() {
	Gui::ScreenDraw(true);
	Gui::Draw_Rect(0, 0, 320, 25, BAR_COLOR);
	Gui::Draw_Rect(0, 26, 320, 215, BG_COLOR);
	Gui::Draw_Rect(0, 25, 320, 1, BAR_OUTLINE);
};


void GFXData::DrawBottom() {
	Gui::ScreenDraw(false);
	Gui::Draw_Rect(0, 0, 320, 240, BG_COLOR);
};


void GFXData::LoadUniStoreSheet(const std::string &SheetFile) {
	/* Ensure it exist. */
	if (access(SheetFile.c_str(), F_OK) == 0) {
		this->UniStoreSheetPaths.push_back(SheetFile);
	}
};


void GFXData::UnloadUniStoreSheets() {
	UniStoreSheetPaths.clear();
};


void GFXData::UpdateUniStoreSprites() {
	for(size_t i = 0; i < this->UniStoreSprites.size(); i++) {
		switch(UU::App->TMode) {
			case UU::TopMode::Grid:
				this->UniStoreSprites[i]->visibility(true);
				this->UniStoreSprites[i]->position(TOP_GRID_X(i) + 5, TOP_GRID_Y(i) + 5);
				break;
			case UU::TopMode::List:
				if (i < 3) {
					this->UniStoreSprites[i]->visibility(true);
					this->UniStoreSprites[i]->position(TOP_LIST_X + 5, TOP_LIST_Y(i) + 5);
				} else {
					this->UniStoreSprites[i]->visibility(false);
				}
				break;
		}
	}

	Sprite::update(true);
}


void GFXData::DrawUniStoreIcons(const std::vector<std::pair<int, int>> Indexes) {
	std::vector<u32> SheetIndexes[this->UniStoreSheetPaths.size()];
	for (const std::pair<int, int> &Index : Indexes) {
		if (Index.first > 0) SheetIndexes[Index.second].push_back(Index.first);
	}

	std::vector<Spritesheet> Sheets;
	for (size_t i = 0; i < this->UniStoreSheetPaths.size(); i++) {
		Sheets.emplace_back(this->UniStoreSheetPaths[i], SheetIndexes[i]);
	}

	for (size_t i = 0; i < 15; i++) {
		Sprite &Spr = *this->UniStoreSprites[i];
		Spr.clear();
		if ((UU::App->TMode == UU::TopMode::Grid || i < 3) && i < Indexes.size() && Indexes[i].second < (int)this->UniStoreSheetPaths.size()) {
			Image &Img = Sheets[Indexes[i].second][Indexes[i].first];
			Spr.drawImage((Spr.width() - Img.width()) / 2, (Spr.height() - Img.height()) / 2, Img);
		}
	}
};


/*
	Draw the box.

	const int XPos: The X-Position where to draw the box.
	const int YPos: The Y-Position where to draw the box.
	const int Width: The Width of the button.
	const int Height: The Height of the button.
	const bool Selected: If selected, or not.
*/
void GFXData::DrawBox(const int XPos, const int YPos, const int Width, const int Height, const bool Selected) {
	Gui::Draw_Rect(XPos, YPos, Width, Height, BAR_COLOR); // Draw middle BG.

	if (Selected) {
		static constexpr int Depth = 3;

		Gui::Draw_Rect(XPos - Depth, YPos - Depth, Width + Depth * 2, Depth, BAR_OUTLINE); // Top.
		Gui::Draw_Rect(XPos - Depth, YPos - Depth, Depth, Height + Depth * 2, BAR_OUTLINE); // Left.
		Gui::Draw_Rect(XPos + Width, YPos - Depth, Depth, Height + Depth * 2, BAR_OUTLINE); // Right.
		Gui::Draw_Rect(XPos - Depth, YPos + Height, Width + Depth * 2, Depth, BAR_OUTLINE); // Bottom.
	}
};



void GFXData::DrawSprite(const int Idx, const int X, const int Y) {
	if (Idx < (int)this->Sprites.size()) this->Sprites[Idx].draw(X, Y, false);
};


void GFXData::DrawSpriteBlend(const int Idx, const int X, const int Y, int color) {
	if (Idx < (int)this->Sprites.size()) {
		this->Sprites[Idx].changePaletteStart(color - 1);
		this->Sprites[Idx].draw(X, Y);
	}
};

/*
	Draw the checkbox.

	const int XPos: The X-Position where to draw the box.
	const int YPos: The Y-Position where to draw the box.
	const bool Selected: if checked, or not.
*/
void GFXData::DrawCheckbox(const int XPos, const int YPos, const bool Selected) {
	this->DrawSpriteBlend((Selected ? sprites_checked_idx : sprites_unchecked_idx), XPos, YPos, TEXT_COLOR);
};


/*
	Draw the toggle.
	
	const int XPos: The X-Position where to draw the toggle.
	const int YPos: The Y-Position where to draw the toggle.
	const bool Toggled: if toggled, or not.
*/
void GFXData::DrawToggle(const int XPos, const int YPos, const bool Toggled) {
	this->DrawSpriteBlend((Toggled ? sprites_toggle_on_idx : sprites_toggle_off_idx), XPos, YPos, TEXT_COLOR);
};
