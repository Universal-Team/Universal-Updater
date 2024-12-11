// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#include "GFXData.hpp"

#include "gui.hpp"
#include "UniversalUpdater.hpp"
#include <unistd.h>


GFXData::GFXData() { Gui::loadSheet("romfs:/gfx/sprites.t3x", this->Sprites); };


GFXData::~GFXData() { Gui::unloadSheet(this->Sprites); };


void GFXData::StartFrame() {
	Gui::clearTextBufs();
	C2D_TargetClear(ScreenTop, C2D_Color32(0, 0, 0, 0));
	C2D_TargetClear(ScreenBottom, C2D_Color32(0, 0, 0, 0));
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
};


void GFXData::EndFrame() { C3D_FrameEnd(0); };


void GFXData::DrawTop() {
	Gui::ScreenDraw(ScreenTop);
	Gui::Draw_Rect(0, 0, 400, 25, BAR_COLOR);
	Gui::Draw_Rect(0, 26, 400, 214, BG_COLOR);
	Gui::Draw_Rect(0, 25, 400, 1, BAR_OUTLINE);
};


void GFXData::DrawBottom() {
	Gui::ScreenDraw(ScreenBottom);
	Gui::Draw_Rect(0, 0, 320, 240, BG_COLOR);
};


void GFXData::LoadUniStoreSheet(const std::string &SheetFile) {
	/* Ensure it exist. */
	if (access(SheetFile.c_str(), F_OK) == 0) {
		this->UniStoreSheets.push_back(C2D_SpriteSheet(nullptr));
		Gui::loadSheet(SheetFile.c_str(), this->UniStoreSheets.back());
	}
};


void GFXData::UnloadUniStoreSheets() {
	if (this->UniStoreSheets.empty()) return;
	
	for (ssize_t Idx = (ssize_t)this->UniStoreSheets.size() - 1; Idx >= 0; Idx--) {
		Gui::unloadSheet(this->UniStoreSheets[Idx]);
	}

	this->UniStoreSheets.clear(); // Clear.
};


void GFXData::DrawUniStoreIcons(const std::vector<UniStoreIcon> &Icons) {
	for(size_t Pos = 0; Pos < Icons.size(); Pos++) {
		int XPos = 0, YPos = 0;

		switch(UU::App->TMode) {
			case UU::TopMode::Grid:
				XPos = TOP_GRID_X(Pos) + 1;
				YPos = TOP_GRID_Y(Pos) + 1;
				break;

			case UU::TopMode::List:
				if (Pos >= 3) return;
				XPos = TOP_LIST_X + 1;
				YPos = TOP_LIST_Y(Pos) + 1;
				break;
		}

		const UniStoreIcon &Icon = Icons[Pos];

		/* Index -1 / Sheet -1 or no sheet loaded --> Draw NoIcon. */
		if (Icon.index < 0 || Icon.sheet < 0 || this->UniStoreSheets.empty()) {
			this->DrawSprite(sprites_noIcon_idx, XPos + 5, YPos + 5);
			if (Icon.updated) this->DrawSprite(sprites_update_app_idx, XPos + 31, YPos + 31);
			continue;
		}

		bool DrawNoIcon = true;

		/* Check, if we can draw that one icon. */
		if (Icon.sheet < (int)this->UniStoreSheets.size()) {
			if (Icon.index < (int)C2D_SpriteSheetCount(this->UniStoreSheets[Icon.sheet])) {
				DrawNoIcon = false;
			}
		}

		if (!DrawNoIcon) {
			/* Center. */
			const C2D_Image TempImg = C2D_SpriteSheetGetImage(this->UniStoreSheets[Icon.sheet], Icon.index);

			/* Only max 48x48 allowed. */
			if (TempImg.subtex->width > 48 || TempImg.subtex->height > 48) this->DrawSprite(sprites_noIcon_idx, XPos + 5, YPos + 5);
			else {
				const uint8_t OffsW = (48 - TempImg.subtex->width) / 2; // Center W.
				const uint8_t OffsH = (48 - TempImg.subtex->height) / 2; // Center H.

				C2D_DrawImageAt(TempImg, XPos + OffsW, YPos + OffsH, 0.5);
			}
		} else this->DrawSprite(sprites_noIcon_idx, XPos + 5, YPos + 5);

		if (Icon.updated) this->DrawSprite(sprites_update_app_idx, XPos + 31, YPos + 31);
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
	Gui::DrawSprite(this->Sprites, Idx, X, Y);
};


void GFXData::DrawSpriteBlend(const int Idx, const int X, const int Y, const uint32_t Color) {
	C2D_ImageTint Tint;
	C2D_PlainImageTint(&Tint, Color, 1.0f);
	C2D_DrawImageAt(C2D_SpriteSheetGetImage(this->Sprites, Idx), X, Y, 0.5f, &Tint);
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