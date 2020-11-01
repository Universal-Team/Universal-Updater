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
	Gui::Draw_Rect(0, 26, 400, 214, BG_COLOR);
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
void GFX::DrawBox(const float &xPos, const float &yPos, const float &width, const float &height, const bool &selected, const uint32_t &clr) {
	Gui::Draw_Rect(xPos, yPos, width, height, BOX_INSIDE_COLOR); // Draw middle BG.

	if (selected) {
		static constexpr int depth = 2;

		Gui::Draw_Rect(xPos - depth, yPos - depth, width + depth * 2, depth, BOX_SELECTED_COLOR); // Top.
		Gui::Draw_Rect(xPos - depth, yPos - depth, depth, height + depth * 2, BOX_SELECTED_COLOR); // Left.
		Gui::Draw_Rect(xPos + width, yPos - depth, depth, height + depth * 2, BOX_SELECTED_COLOR); // Right.
		Gui::Draw_Rect(xPos - depth, yPos + height, width + depth * 2, depth, BOX_SELECTED_COLOR); // Bottom.
	}
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
	Draw the checkbox.

	const float &xPos: Const Reference to the X-Position where to draw the box.
	const float &yPos: Const Reference to the Y-Position where to draw the box.
	const bool &selected: Const Reference, checked or not.
*/
void GFX::DrawCheckbox(const float &xPos, const float &yPos, const bool &selected) {
	GFX::DrawSprite((selected ? sprites_checked_idx : sprites_unchecked_idx), xPos, yPos);
}

/*
	Draw the toggle box.

	float xPos: The X-Position where to draw the toggle.
	float yPos: The Y-Position where to draw the toggle.
	bool toggled: If toggled or not.
*/
void GFX::DrawToggle(float xPos, float yPos, bool toggled) {
	GFX::DrawSprite((toggled ? sprites_toggle_on_idx : sprites_toggle_off_idx), xPos, yPos);
}