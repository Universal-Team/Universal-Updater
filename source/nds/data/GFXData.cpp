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


GFXData::GFXData() : Sprites("nitro:/graphics/sprites.tdx") { };


GFXData::~GFXData() { };


void GFXData::DrawTop() {
	Gui::ScreenDraw(true);
	Gui::Draw_Rect(0, 0, 320, 25, BAR_COLOR);
	Gui::Draw_Rect(0, 26, 320, 214, BG_COLOR);
	Gui::Draw_Rect(0, 25, 320, 1, BAR_OUTLINE);
};


void GFXData::DrawBottom() {
	Gui::ScreenDraw(false);
	Gui::Draw_Rect(0, 0, 320, 240, BG_COLOR);
};


void GFXData::LoadUniStoreSheet(const std::string &SheetFile) {

};


void GFXData::UnloadUniStoreSheets() {

};


void GFXData::DrawUniStoreIcon(const int Idx, const int Sheet, const int XPos, const int YPos) {

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
