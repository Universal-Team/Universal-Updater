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

#include "Common.hpp"
#include "GFXData.hpp"
#include <unistd.h>


GFXData::GFXData() {
	Gui::loadSheet("romfs:/gfx/sprites.t3x", this->Sprites);
};


GFXData::~GFXData() {
	Gui::unloadSheet(this->Sprites);
};


void GFXData::DrawTop() {
	Gui::ScreenDraw(Top);
	Gui::Draw_Rect(0, 0, 400, 25, BAR_COLOR);
	Gui::Draw_Rect(0, 26, 400, 214, BG_COLOR);
	Gui::Draw_Rect(0, 25, 400, 1, BAR_OUTLINE);
};


void GFXData::DrawBottom() {
	Gui::ScreenDraw(Bottom);
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
	
	for (size_t Idx = this->UniStoreSheets.size() - 1; Idx > 0; Idx--) {
		Gui::unloadSheet(this->UniStoreSheets[Idx]);
		this->UniStoreSheets.pop_back(); // Remove.
	}
};


void GFXData::DrawUniStoreIcon(const int Idx, const int Sheet, const int XPos, const int YPos) {
	/* Idx -1 / Sheet -1 or no sheet loaded --> Draw NoIcon. */
	if (Idx < 0 || Sheet < 0 || this->UniStoreSheets.empty()) {
		this->DrawSprite(sprites_noIcon_idx, XPos, YPos);
		return;
	}

	bool DrawNoIcon = true;

	/* Check, if we can draw that one icon. */
	if (Sheet < (int)this->UniStoreSheets.size()) {
		if (Idx < (int)C2D_SpriteSheetCount(this->UniStoreSheets[Sheet])) {
			DrawNoIcon = false;
		}
	}

	if (!DrawNoIcon) {
		/* Center. */
		const C2D_Image TempImg = C2D_SpriteSheetGetImage(this->UniStoreSheets[Sheet], Idx);

		/* Only max 48x48 allowed. */
		if (TempImg.subtex->width > 48 || TempImg.subtex->height > 48) this->DrawSprite(sprites_noIcon_idx, XPos, YPos);
		else {
			const uint8_t OffsW = (48 - TempImg.subtex->width) / 2; // Center W.
			const uint8_t OffsH = (48 - TempImg.subtex->height) / 2; // Center H.

			C2D_DrawImageAt(TempImg, XPos + OffsW, YPos + OffsH, 0.5);
		}
	} else this->DrawSprite(sprites_noIcon_idx, XPos, YPos);
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