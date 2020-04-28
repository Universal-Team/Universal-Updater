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

extern bool isScriptSelected;
extern u32 progressBar;
extern u32 selected;

extern C2D_SpriteSheet sprites;

// Draws a Rectangle as the progressbar.
void Animation::DrawProgressBar(float currentProgress, float totalProgress, int mode) {
	if (mode == 1) {
		Gui::Draw_Rect(31, 121, (int)(((float)currentProgress / (float)totalProgress) * 338.0f), 28, progressBar);
	} else {
		Gui::Draw_Rect(31, 121, (int)(((float)currentProgress / (float)totalProgress) * 338.0f), 28, Config::progressbarColor);
	}
}

void Animation::DrawProgressBarInstall(u64 currentProgress, u64 totalProgress, int mode) {
	if (mode == 1) {
		Gui::Draw_Rect(31, 121, (int)(((float)currentProgress / (float)totalProgress) * 338.0f), 28, progressBar);
	} else {
		Gui::Draw_Rect(31, 121, (int)(((float)currentProgress / (float)totalProgress) * 338.0f), 28, Config::progressbarColor);
	}
}

void Animation::DrawProgressBarExtract(u64 currentProgress, u64 totalProgress, int mode) {
	if (mode == 1) {
		Gui::Draw_Rect(31, 141, (int)(((float)currentProgress / (float)totalProgress) * 338.0f), 28, progressBar);
	} else {
		Gui::Draw_Rect(31, 141, (int)(((float)currentProgress / (float)totalProgress) * 338.0f), 28, Config::progressbarColor);
	}
}

void Animation::Button(int x, int y, float speed)
{
	static float timer	= 0.0f;
	float highlight_multiplier = fmax(0.0, fabs(fmod(timer, 1.0) - 0.5) / 0.5);
	u8 r, g, b;
	if (isScriptSelected) {
		r	= selected & 0xFF;
		g	= (selected >> 8) & 0xFF;
		b	= (selected >> 16) & 0xFF;
	} else {
		r	= Config::SelectedColor & 0xFF;
		g	= (Config::SelectedColor >> 8) & 0xFF;
		b	= (Config::SelectedColor >> 16) & 0xFF;
	}

	u32 color = C2D_Color32(r + (255 - r) * highlight_multiplier, g + (255 - g) * highlight_multiplier, b + (255 - b) * highlight_multiplier, 255);

	// The actual draw part.
	C2D_ImageTint tint;
	C2D_SetImageTint(&tint, C2D_TopLeft, color, 1);
	C2D_SetImageTint(&tint, C2D_TopRight, color, 1);
	C2D_SetImageTint(&tint, C2D_BotLeft, color, 1);
	C2D_SetImageTint(&tint, C2D_BotRight, color, 1);
	C2D_DrawImageAt(C2D_SpriteSheetGetImage(sprites, sprites_selector_idx), x+4, y+4, 0.5f, &tint); // +4 because to fit the button.

	timer += speed; // Speed of the animation. Example : .030f / .030
}