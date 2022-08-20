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

/* Draw the base top screen. */
void GFX::DrawTop(void) {
	Gui::ScreenDraw(Top);
	Gui::Draw_Rect(0, 0, 400, 25, UIThemes->BarColor());
	Gui::Draw_Rect(0, 26, 400, 214, UIThemes->BGColor());
	Gui::Draw_Rect(0, 25, 400, 1, UIThemes->BarOutline());
}

/* Draw the base bottom screen. */
void GFX::DrawBottom() {
	Gui::ScreenDraw(Bottom);
	Gui::Draw_Rect(0, 0, 320, 240, UIThemes->BGColor());
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
	Gui::Draw_Rect(xPos, yPos, width, height, UIThemes->BoxInside()); // Draw middle BG.

	if (selected) {
		static constexpr int depth = 3;

		Gui::Draw_Rect(xPos - depth, yPos - depth, width + depth * 2, depth, UIThemes->BoxSelected()); // Top.
		Gui::Draw_Rect(xPos - depth, yPos - depth, depth, height + depth * 2, UIThemes->BoxSelected()); // Left.
		Gui::Draw_Rect(xPos + width, yPos - depth, depth, height + depth * 2, UIThemes->BoxSelected()); // Right.
		Gui::Draw_Rect(xPos - depth, yPos + height, width + depth * 2, depth, UIThemes->BoxSelected()); // Bottom.
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
	GFX::DrawIcon((selected ? sprites_checked_idx : sprites_unchecked_idx), xPos, yPos, UIThemes->TextColor(), 1.0f);
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

	Gui::DrawString(11, 5, 0.5f, UIThemes->TextColor(), str, 0, 0, font);
}

static int blinkDelay = 40;
static bool blinkState = true, batteryLow = false;
void GFX::DrawBattery() {
	u8 chargeState = false, level = 0;
	bool chargerPlugged = false;

	PTMU_GetBatteryChargeState(&chargeState); // Get Charge state.
	PTMU_GetBatteryLevel(&level); // Get Battery Level.
	PTMU_GetAdapterState(&chargerPlugged); // Get if charger adapter is plugged.

	if (chargerPlugged) {
		GFX::DrawIcon((chargeState ? sprites_battery_charge_idx : sprites_battery_charge_full_idx), 366, 1, UIThemes->TextColor());
		if (batteryLow) batteryLow = false; // Cause we're charging.

	} else {
		switch(level) {
			case 0: // Blinky.
				GFX::DrawIcon((blinkState ? sprites_battery_blink_idx : sprites_battery_0_idx), 366, 1, UIThemes->TextColor());
				if (!batteryLow) batteryLow = true;
				break;

			case 1: // Red.
				GFX::DrawIcon(sprites_battery_0_idx, 366, 1, UIThemes->TextColor());
				if (batteryLow) batteryLow = false; // Cause we're not low.
				break;

			case 2: // One.
				GFX::DrawIcon(sprites_battery_1_idx, 366, 1, UIThemes->TextColor());
				if (batteryLow) batteryLow = false; // Cause we're not low.
				break;

			case 3: // Two.
				GFX::DrawIcon(sprites_battery_2_idx, 366, 1, UIThemes->TextColor());
				if (batteryLow) batteryLow = false; // Cause we're not low.
				break;

			case 4: // Three.
				GFX::DrawIcon(sprites_battery_3_idx, 366, 1, UIThemes->TextColor());
				if (batteryLow) batteryLow = false; // Cause we're not low.
				break;

			case 5: // Full.
				GFX::DrawIcon(sprites_battery_4_idx, 366, 1, UIThemes->TextColor());
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
	Draws the Wi-Fi signal icon
*/
void GFX::DrawWifi() {
	constexpr int wifiIcons[] = {
		sprites_wifi_0_idx,
		sprites_wifi_1_idx,
		sprites_wifi_2_idx,
		sprites_wifi_3_idx
	};

	u8 level = osGetWifiStrength();
	GFX::DrawIcon(wifiIcons[level], 330, 0, UIThemes->TextColor());
}

/*
	Draws the sidebar icons blended with the SideBarIconColor.
*/
void GFX::DrawIcon(const int Idx, int X, int Y, uint32_t Color, float BlendPower, float ScaleX, float ScaleY) {
	C2D_ImageTint tint;
	C2D_PlainImageTint(&tint, Color, BlendPower);

	C2D_DrawImageAt(C2D_SpriteSheetGetImage(sprites, Idx), X, Y, 0.5f, &tint, ScaleX, ScaleY);
}