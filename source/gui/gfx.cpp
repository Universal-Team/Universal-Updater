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

/*
	Draw the base top screen.
*/
void GFX::DrawTop(void) {
	Gui::ScreenDraw(Top);
	Gui::Draw_Rect(0, 0, 400, 25, BAR_COLOR);
	Gui::Draw_Rect(0, 25, 400, 215, BG_COLOR);
	Gui::Draw_Rect(0, 25, 400, 1, BAR_OUTL_COLOR);
}

/*
	Draw the base bottom screen.
*/
void GFX::DrawBottom() {
	Gui::ScreenDraw(Bottom);
	Gui::Draw_Rect(0, 0, 320, 240, BG_COLOR);
}

/*
	Draw the box.

	const float &xPos: Const Reference to the X-Position where to draw the box.
	const float &yPos: Const Reference to the Y-Position where to draw the box.
	const float &width: Const Reference to the Width of the button.
	const float &height: Const Reference to the Height of the button.
	const bool &selected: Const Reference, if outline is selected (Red) or not (Black).
	const uint32_t &clr: (Optional) The color of the inside of the box.
*/
void GFX::drawBox(const float &xPos, const float &yPos, const float &width, const float &height, const bool &selected, const uint32_t &clr) {
	static constexpr int w	= 1;
	const uint32_t outlineColor = selected ? BOX_SELECTED_COLOR : BOX_UNSELECTED_COLOR; // Get Selected | Unselected color.

	Gui::Draw_Rect(xPos, yPos, width, height, clr); // Draw middle BG.

	Gui::Draw_Rect(xPos, yPos, width, w, outlineColor); // Top.
	Gui::Draw_Rect(xPos, yPos + w, w, height - 2 * w, outlineColor); // Left.
	Gui::Draw_Rect(xPos + width - w, yPos + w, w, height - 2 * w, outlineColor); // Right.
	Gui::Draw_Rect(xPos, yPos + height - w, width, w, outlineColor); // Bottom.
}

extern C2D_SpriteSheet sprites;

/*
	Draw a Sprite of the sprites SpriteSheet.

	const int &img: Const Reference to the Image index.
	const int &x: Const Reference to the X-Position where to draw.
	const int &y: Const Reference to the Y-Position where to draw.
	const float &ScaleX: (Optional) Const Reference to the X-Scale of the Sprite. (1 by default)
	const float &ScaleY: (Optional) Const Reference to the Y-Scale of the Sprite. (1 by default)
*/
void GFX::DrawSprite(const int &img, const int &x, const int &y, const float &ScaleX, const float &ScaleY) {
	Gui::DrawSprite(sprites, img, x, y, ScaleX, ScaleY);
}

/*
	Draw a button (actually the box) with a centered string in it.

	const float &xPos: Const Reference to the X-Position where to draw the box.
	const float &yPos: Const Reference to the Y-Position where to draw the box.
	const float &width: Const Reference to the Width of the button.
	const float &height: Const Reference to the Height of the button.
	const bool &selected: Const Reference, if outline is selected (Red) or not (Black).
	const std::string &Text: Const Reference of the Text which should be drawn.
*/
void GFX::DrawButton(const float &xPos, const float &yPos, const float &width, const float &height, const bool &selected, const std::string &Text) {
	drawBox(xPos, yPos, width, height, selected);

	Gui::DrawStringCentered(xPos - 160 + (width / 2), yPos + (height / 2) - (Gui::GetStringHeight(0.4f, Text) / 2), 0.4f, TEXT_COLOR, Text, width - 4, height - 4);
}
