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

#include "common.hpp"
#include "gfx.hpp"

void GFX::DrawTop(void) {
	Gui::ScreenDraw(Top);
	Gui::Draw_Rect(0, 0, 400, 25, C2D_Color32(50, 73, 98, 255));
	Gui::Draw_Rect(0, 25, 400, 215, C2D_Color32(38, 44, 77, 255));
	Gui::Draw_Rect(0, 25, 400, 1, C2D_Color32(25, 30, 53, 255));
}

void GFX::DrawBottom(bool useBar) {
	Gui::ScreenDraw(Bottom);

	Gui::Draw_Rect(0, useBar ? 25 : 0, 320, useBar ? 190 : 240, C2D_Color32(38, 44, 77, 255));

	if (useBar) {
		Gui::Draw_Rect(0, 0, 320, 25, C2D_Color32(57, 84, 114, 255));
		Gui::Draw_Rect(0, 215, 320, 25, C2D_Color32(57, 84, 114, 255));
		DrawSprite(sprites_bottom_screen_top_idx, 0, 0);
		DrawSprite(sprites_bottom_screen_bot_idx, 0, 215);
	}
}

void GFX::drawBox(float xPos, float yPos, float width, float height, bool selected, uint32_t clr) {
	static constexpr int w	= 1;
	const u32 outlineColor = selected ? C2D_Color32(240, 0, 0, 255) : C2D_Color32(0, 0, 0, 255);
	C2D_DrawRectSolid(xPos, yPos, 0.5, width, height, clr);

	C2D_DrawRectSolid(xPos, yPos, 0.5, width, w, outlineColor); // top
	C2D_DrawRectSolid(xPos, yPos + w, 0.5, w, height - 2 * w, outlineColor); // left
	C2D_DrawRectSolid(xPos + width - w, yPos + w, 0.5, w, height - 2 * w, outlineColor); // right
	C2D_DrawRectSolid(xPos, yPos + height - w, 0.5, width, w, outlineColor); // bottom
}

extern C2D_SpriteSheet sprites;

void GFX::DrawSprite(int img, int x, int y, float ScaleX, float ScaleY) {
	Gui::DrawSprite(sprites, img, x, y, ScaleX, ScaleY);
}