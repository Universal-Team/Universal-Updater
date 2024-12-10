// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#include "GFXData.hpp"

#include "font.hpp"
#include "gui.hpp"
#include "UniversalUpdater.hpp"

#include <nds.h>


GFXData::GFXData() : Sprites("nitro:/graphics/sprites.tdx") {
	/* Configure palettes. */
	this->Sprites[sprites_toggle_off_idx].paletteStart(0x30);
	this->Sprites[sprites_toggle_off_idx].copyPalette();
	this->Sprites[sprites_toggle_on_idx].paletteStart(0x38);
	this->Sprites[sprites_toggle_on_idx].copyPalette();
	this->Sprites[sprites_update_app_idx].paletteStart(0x40);
	this->Sprites[sprites_update_app_idx].copyPalette();

	for(size_t Idx = 0; Idx < this->UniStoreSprites.size(); Idx++) {
		this->UniStoreSprites[Idx] = new Sprite(true, SpriteSize_32x32, SpriteColorFormat_Bmp);
	}

	this->UpdateUniStoreSprites();
};


GFXData::~GFXData() {
	for(Sprite *UniStoreSprite : this->UniStoreSprites) {
		if (UniStoreSprite) delete UniStoreSprite;
	}
};


void GFXData::StartFrame() {
	Gui::clearTextBufs();
};


void GFXData::EndFrame() {
	Gui::updateTextBufs(true);
	Gui::updateTextBufs(false);
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
	this->UniStoreSheetPaths.clear();
};


void GFXData::UpdateUniStoreSprites() {
	for(size_t Idx = 0; Idx < this->UniStoreSprites.size(); Idx++) {
		switch(UU::App->TMode) {
			case UU::TopMode::Grid:
				this->UniStoreSprites[Idx]->visibility(true);
				this->UniStoreSprites[Idx]->position(TOP_GRID_X(Idx) + 5, TOP_GRID_Y(Idx) + 5);
				break;
			case UU::TopMode::List:
				if (Idx < 3) {
					this->UniStoreSprites[Idx]->visibility(true);
					this->UniStoreSprites[Idx]->position(TOP_LIST_X + 5, TOP_LIST_Y(Idx) + 5);
				} else {
					this->UniStoreSprites[Idx]->visibility(false);
				}
				break;
		}
	}

	Sprite::update(true);
	if (!this->SpritesDrawn) this->SpritesDrawn = true; // Not hidden.
};


void GFXData::HideUniStoreSprites() {
	for(size_t Idx = 0; Idx < this->UniStoreSprites.size(); Idx++) {
		this->UniStoreSprites[Idx]->visibility(false);
	}

	Sprite::update(true);
	if (this->SpritesDrawn) this->SpritesDrawn = false; // Hidden.
};


void GFXData::DrawUniStoreIcons(const std::vector<std::tuple<int, int, bool>> &Indexes) {
	std::vector<uint32_t> SheetIndexes[this->UniStoreSheetPaths.size()];
	for (const std::tuple<int, int, bool> &Index : Indexes) {
		if (std::get<0>(Index) > 0) SheetIndexes[std::get<1>(Index)].push_back(std::get<0>(Index));
	}

	std::vector<Spritesheet> Sheets;
	for (size_t Idx = 0; Idx < this->UniStoreSheetPaths.size(); Idx++) {
		Sheets.emplace_back(this->UniStoreSheetPaths[Idx], SheetIndexes[Idx]);
	}

	for (size_t Idx = 0; Idx < 15; Idx++) {
		Sprite &Spr = *this->UniStoreSprites[Idx];
		Spr.clear();

		if ((UU::App->TMode == UU::TopMode::Grid || Idx < 3) && Idx < Indexes.size() && std::get<1>(Indexes[Idx]) < (int)this->UniStoreSheetPaths.size()) {
			Image &Img = Sheets[std::get<1>(Indexes[Idx])][std::get<0>(Indexes[Idx])];
			Spr.drawImage((Spr.width() - Img.width()) / 2, (Spr.height() - Img.height()) / 2, Img);

			/* If update available... TODO: Handle this properly. */
			if (std::get<2>(Indexes[Idx])) {
				switch(UU::App->TMode) {
					case UU::TopMode::Grid:
						this->DrawSprite(sprites_update_app_idx, TOP_GRID_X(Idx) + 32, TOP_GRID_Y(Idx) + 32);
						break;

					case UU::TopMode::List:
						this->DrawSprite(sprites_update_app_idx, TOP_LIST_X + 32, TOP_LIST_Y(Idx) + 32);
						break;
				}
			}
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
		this->Sprites[Idx].paletteStart(color);
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


/*
	Draw the sort.
	
	const int XPos: The X-Position where to draw the sort.
	const int YPos: The Y-Position where to draw the sort.
	const bool Checked: if the sort is checked, or not.
*/
void GFXData::DrawSort(const int XPos, const int YPos, const bool Checked) {
	this->DrawSpriteBlend((Checked ? sprites_sort_checked_idx : sprites_sort_unchecked_idx), XPos, YPos, TEXT_COLOR);
};