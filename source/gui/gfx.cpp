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

#include "common.hpp"
#include "gfx.hpp"
#include "stringutils.hpp"
#include <ctime>

int GFX::SelectedTheme = 0;

/* All available Themes here inside that vector. */
std::vector<UITheme> GFX::Themes = {
	/* Default Theme. */
	{
		C2D_Color32(50, 73, 98, 255), // Bar.
		C2D_Color32(38, 44, 77, 255), // BG.
		C2D_Color32(25, 30, 53, 255), // Bar Outline.
		WHITE, // Text.
		C2D_Color32(50, 73, 98, 255), // Entry bar.
		C2D_Color32(25, 30, 53, 255), // Entry Outline.
		C2D_Color32(28, 33, 58, 255), // Box Inside.
		C2D_Color32(108, 130, 155, 255), // Box Outside.
		BLACK, // Box Selected.
		C2D_Color32(28, 33, 58, 255), // Progressbar Out.
		C2D_Color32(77, 101, 128, 255), // Progressbar In.
		C2D_Color32(51, 75, 102, 255), // Searchbar.
		C2D_Color32(25, 30, 53, 255), // Searchbar Outline.
		C2D_Color32(108, 130, 155, 255), // Sidebar Selected.
		C2D_Color32(77, 101, 128, 255), // Sidebar Unselected.
		C2D_Color32(77, 101, 128, 255), // Mark Selected.
		C2D_Color32(28, 33, 58, 255), // Mark Unselected.
		C2D_Color32(28, 33, 58, 255), // Downlist Preview (Top).
		C2D_Color32(173, 204, 239, 255) // SideBar Icon Color.
	},
	/* Stack Theme. */
	{
		C2D_Color32(44, 48, 64, 255), // Bar.
		C2D_Color32(52, 56, 64, 255), // BG.
		C2D_Color32(22, 24, 32, 255), // Bar Outline.
		C2D_Color32(216, 228, 228, 255), // Text.
		C2D_Color32(60, 63, 113, 255), // Entry bar.
		C2D_Color32(42, 46, 54, 255), // Entry Outline.
		C2D_Color32(60, 63, 113, 255), // Box Inside.
		C2D_Color32(42, 46, 54, 255), // Box Outside.
		C2D_Color32(102, 105, 170, 255), // Box Selected.
		C2D_Color32(42, 46, 54, 255), // Progressbar Out.
		C2D_Color32(60, 63, 113, 255), // Progressbar In.
		C2D_Color32(60, 63, 113, 255), // Searchbar.
		C2D_Color32(42, 46, 54, 255), // Searchbar Outline.
		C2D_Color32(60, 63, 113, 255), // Sidebar Selected.
		C2D_Color32(42, 46, 54, 255), // Sidebar Unselected.
		C2D_Color32(60, 63, 113, 255), // Mark Selected.
		C2D_Color32(42, 46, 54, 255), // Mark Unselected.
		C2D_Color32(52, 60, 76, 255), // Downlist Preview (Top).
		C2D_Color32(102, 105, 170, 255) // SideBar Icon Color.
	}
};

/* Draw the base top screen. */
void GFX::DrawTop(void) {
	Gui::ScreenDraw(Top);
	Gui::Draw_Rect(0, 0, 400, 25, GFX::Themes[GFX::SelectedTheme].BarColor);
	Gui::Draw_Rect(0, 26, 400, 214, GFX::Themes[GFX::SelectedTheme].BGColor);
	Gui::Draw_Rect(0, 25, 400, 1, GFX::Themes[GFX::SelectedTheme].BarOutline);
}

/* Draw the base bottom screen. */
void GFX::DrawBottom() {
	Gui::ScreenDraw(Bottom);
	Gui::Draw_Rect(0, 0, 320, 240, GFX::Themes[GFX::SelectedTheme].BGColor);
}

/*
	Draw the box.

	float xPos: The X-Position where to draw the box.
	float yPos: The Y-Position where to draw the box.
	float width: The Width of the button.
	float height: The Height of the button.
	bool selected: If selected, or not.
	uint32_t clr: (Optional) The color of the inside of the box.
*/
void GFX::DrawBox(float xPos, float yPos, float width, float height, bool selected, uint32_t clr) {
	Gui::Draw_Rect(xPos, yPos, width, height, GFX::Themes[GFX::SelectedTheme].BoxInside); // Draw middle BG.

	if (selected) {
		static constexpr int depth = 3;

		Gui::Draw_Rect(xPos - depth, yPos - depth, width + depth * 2, depth, GFX::Themes[GFX::SelectedTheme].BoxSelected); // Top.
		Gui::Draw_Rect(xPos - depth, yPos - depth, depth, height + depth * 2, GFX::Themes[GFX::SelectedTheme].BoxSelected); // Left.
		Gui::Draw_Rect(xPos + width, yPos - depth, depth, height + depth * 2, GFX::Themes[GFX::SelectedTheme].BoxSelected); // Right.
		Gui::Draw_Rect(xPos - depth, yPos + height, width + depth * 2, depth, GFX::Themes[GFX::SelectedTheme].BoxSelected); // Bottom.
	}
}

extern C2D_SpriteSheet sprites;

/*
	Draw a Sprite of the sprites SpriteSheet.

	int img: The Image index.
	int x: The X-Position where to draw.
	int y: The Y-Position where to draw.
	float ScaleX: (Optional) The X-Scale of the Sprite. (1 by default)
	float ScaleY: (Optional) The Y-Scale of the Sprite. (1 by default)
*/
void GFX::DrawSprite(int img, int x, int y, float ScaleX, float ScaleY) {
	Gui::DrawSprite(sprites, img, x, y, ScaleX, ScaleY);
}

/*
	Draw the checkbox.

	float xPos: The X-Position where to draw the box.
	float yPos: The Y-Position where to draw the box.
	bool selected: if checked, or not.
*/
void GFX::DrawCheckbox(float xPos, float yPos, bool selected) {
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

void GFX::DrawTime() {
	time_t unixTime			= time(nullptr);
	struct tm *timeStruct	= gmtime((const time_t *)&unixTime);
	const std::string str	= StringUtils::format("%02i:%02i", timeStruct->tm_hour, timeStruct->tm_min); // <Hour>:<Minute>.

	Gui::DrawString(11, 5, 0.5f, GFX::Themes[GFX::SelectedTheme].TextColor, str, 0, 0, font);
}

static int blinkDelay = 40;
static bool blinkState = true, batteryLow = false;
void GFX::DrawBattery() {
	u8 chargeState = false, level = 0;
	PTMU_GetBatteryChargeState(&chargeState); // Get Charge state.
	PTMU_GetBatteryLevel(&level); // Get Battery Level.

	if (chargeState) {
		GFX::DrawSprite((level < 5 ? sprites_battery_charge_idx : sprites_battery_charge_full_idx), 366, 1);
		if (batteryLow) batteryLow = false; // Cause we're charging.

	} else {
		switch(level) {
			case 0: // Blinky.
				GFX::DrawSprite((blinkState ? sprites_battery_blink_idx : sprites_battery_0_idx), 366, 1);
				if (!batteryLow) batteryLow = true;
				break;

			case 1: // Red.
				GFX::DrawSprite(sprites_battery_0_idx, 366, 1);
				if (batteryLow) batteryLow = false; // Cause we're not low.
				break;

			case 2: // One.
				GFX::DrawSprite(sprites_battery_1_idx, 366, 1);
				if (batteryLow) batteryLow = false; // Cause we're not low.
				break;

			case 3: // Two.
				GFX::DrawSprite(sprites_battery_2_idx, 366, 1);
				if (batteryLow) batteryLow = false; // Cause we're not low.
				break;

			case 4: // Three.
				GFX::DrawSprite(sprites_battery_3_idx, 366, 1);
				if (batteryLow) batteryLow = false; // Cause we're not low.
				break;

			case 5: // Full.
				GFX::DrawSprite(sprites_battery_4_idx, 366, 1);
				if (batteryLow) batteryLow = false; // Cause we're not low.
				break;
		}
	}
}

void GFX::HandleBattery() {
	if (batteryLow) {
		if (blinkDelay > 0) {
			blinkDelay--;

			if (blinkDelay == 0) {
				blinkState = !blinkState;
				blinkDelay = 40;
			}
		}
	}
}

/*
	Draws the sidebar icons blended with the SideBarIconColor.
*/
void GFX::DrawIcon(const int Idx, int X, int Y, float ScaleX, float ScaleY) {
	C2D_ImageTint tint;
	C2D_PlainImageTint(&tint, GFX::Themes[GFX::SelectedTheme].SideBarIconColor, 1.0f);

	C2D_DrawImageAt(C2D_SpriteSheetGetImage(sprites, Idx), X, Y, 0.5f, &tint, ScaleX, ScaleY);
}