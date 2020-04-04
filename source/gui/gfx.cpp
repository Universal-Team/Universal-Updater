/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

extern bool isScriptSelected;
extern u32 barColor;
extern u32 bgTopColor;
extern u32 bgBottomColor;
extern u32 TextColor;

void GFX::DrawTop(void) {
	Gui::ScreenDraw(Top);
	if (isScriptSelected == false) {
		Gui::Draw_Rect(0, 0, 400, 25, Config::Color1);
		Gui::Draw_Rect(0, 25, 400, 190, Config::Color2);
		Gui::Draw_Rect(0, 215, 400, 25, Config::Color1);
		if (Config::UseBars == true) {
			DrawSprite(sprites_top_screen_top_idx, 0, 0);
			DrawSprite(sprites_top_screen_bot_idx, 0, 215);
		}
	} else if (isScriptSelected == true) {
		Gui::Draw_Rect(0, 0, 400, 30, barColor);
		Gui::Draw_Rect(0, 25, 400, 190, bgBottomColor);
		Gui::Draw_Rect(0, 215, 400, 25, barColor);
		if (Config::UseBars == true) {
			DrawSprite(sprites_top_screen_top_idx, 0, 0);
			DrawSprite(sprites_top_screen_bot_idx, 0, 215);
		}
	}
}

void GFX::DrawBottom(void) {
	Gui::ScreenDraw(Bottom);
	if (isScriptSelected == false) {
		Gui::Draw_Rect(0, 0, 320, 25, Config::Color1);
		Gui::Draw_Rect(0, 25, 320, 190, Config::Color3);
		Gui::Draw_Rect(0, 215, 320, 25, Config::Color1);
		if (Config::UseBars == true) {
			DrawSprite(sprites_bottom_screen_top_idx, 0, 0);
			DrawSprite(sprites_bottom_screen_bot_idx, 0, 215);
		}
	} else if (isScriptSelected == true) {
		Gui::Draw_Rect(0, 0, 320, 30, barColor);
		Gui::Draw_Rect(0, 25, 320, 190, bgBottomColor);
		Gui::Draw_Rect(0, 215, 320, 25, barColor);
		if (Config::UseBars == true) {
			DrawSprite(sprites_bottom_screen_top_idx, 0, 0);
			DrawSprite(sprites_bottom_screen_bot_idx, 0, 215);
		}
	}
}

extern C2D_SpriteSheet sprites;

void GFX::DrawSprite(int img, int x, int y, float ScaleX, float ScaleY)
{
	Gui::DrawSprite(sprites, img, x, y, ScaleX, ScaleY);
}

void GFX::DrawSpriteBlend(int img, int x, int y, float ScaleX, float ScaleY)
{
	C2D_ImageTint tint;
	if (isScriptSelected) {
		C2D_SetImageTint(&tint, C2D_TopLeft, TextColor, 0.5);
		C2D_SetImageTint(&tint, C2D_TopRight, TextColor, 0.5);
		C2D_SetImageTint(&tint, C2D_BotLeft, TextColor, 0.5);
		C2D_SetImageTint(&tint, C2D_BotRight, TextColor, 0.5);
	} else {
		C2D_SetImageTint(&tint, C2D_TopLeft, Config::TxtColor, 0.5);
		C2D_SetImageTint(&tint, C2D_TopRight, Config::TxtColor, 0.5);
		C2D_SetImageTint(&tint, C2D_BotLeft, Config::TxtColor, 0.5);
		C2D_SetImageTint(&tint, C2D_BotRight, Config::TxtColor, 0.5);	
	}
	C2D_DrawImageAt(C2D_SpriteSheetGetImage(sprites, img), x, y, 0.5f, &tint, ScaleX, ScaleY);
}

void GFX::DrawArrow(int x, int y, float rotation, int arrowSprite) {
	C2D_Sprite sprite;
	C2D_ImageTint tint;
	if (isScriptSelected) {
		C2D_SetImageTint(&tint, C2D_TopLeft, TextColor, 0.5);
		C2D_SetImageTint(&tint, C2D_TopRight, TextColor, 0.5);
		C2D_SetImageTint(&tint, C2D_BotLeft, TextColor, 0.5);
		C2D_SetImageTint(&tint, C2D_BotRight, TextColor, 0.5);
	} else {
		C2D_SetImageTint(&tint, C2D_TopLeft, Config::TxtColor, 0.5);
		C2D_SetImageTint(&tint, C2D_TopRight, Config::TxtColor, 0.5);
		C2D_SetImageTint(&tint, C2D_BotLeft, Config::TxtColor, 0.5);
		C2D_SetImageTint(&tint, C2D_BotRight, Config::TxtColor, 0.5);
	}

	if (arrowSprite == 0) {
		C2D_SpriteFromSheet(&sprite, sprites, sprites_arrow_idx);
	} else {
		C2D_SpriteFromSheet(&sprite, sprites, sprites_side_arrow_idx);
	}
	C2D_SpriteRotateDegrees(&sprite, rotation);
	C2D_SpriteSetPos(&sprite, x, y);
	C2D_SpriteSetDepth(&sprite, 0.5);
	C2D_DrawSpriteTinted(&sprite, &tint);
}

// Draw a Button and draw Text on it.
void GFX::DrawButton(int x, int y, std::string ButtonText, u32 color) {
	C2D_ImageTint tint;
	C2D_SetImageTint(&tint, C2D_TopLeft, color, 0.5);
	C2D_SetImageTint(&tint, C2D_TopRight, color, 0.5);
	C2D_SetImageTint(&tint, C2D_BotLeft, color, 0.5);
	C2D_SetImageTint(&tint, C2D_BotRight, color, 0.5);
	C2D_DrawImageAt(C2D_SpriteSheetGetImage(sprites, sprites_button_idx), x, y, 0.5f, &tint);

	// Draw String. TODO: Center.
	if (isScriptSelected) {
		Gui::DrawStringCentered(- (158/2) + x, y + (61/2) - (Gui::GetStringHeight(0.6f, ButtonText) / 2), 0.6f, TextColor, ButtonText, 145, 30);
	} else {
		Gui::DrawStringCentered(- (158/2) + x, y + (61/2) - (Gui::GetStringHeight(0.6f, ButtonText) / 2), 0.6f, Config::TxtColor, ButtonText, 145, 30);
	}
}