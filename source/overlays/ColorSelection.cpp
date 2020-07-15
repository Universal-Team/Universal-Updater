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

#include "colorHelper.hpp"
#include "config.hpp"
#include "gui.hpp"
#include "overlay.hpp"
#include "structs.hpp"

extern std::unique_ptr<Config> config;
extern touchPosition touch;

extern bool touching(Structs::ButtonPos button);

// Draw RGB Colors.
static void DrawRGBColor(u8 r, u8 g, u8 b) {
	// Display RGB line.
	for (int i = 0; i < 256; i++) {
		Gui::Draw_Rect((32.5 + i), 30, 1, 20, C2D_Color32(i, 0, 0, 255));
		Gui::Draw_Rect((32.5 + i), 80, 1, 20, C2D_Color32(0, i, 0, 255));
		Gui::Draw_Rect((32.5 + i), 130, 1, 20, C2D_Color32(0, 0, i, 255));
	}

	Gui::Draw_Rect((32.5 + r), 30, 1, 20, C2D_Color32(255, 255, 255, 255));
	Gui::Draw_Rect((32.5 + g), 80, 1, 20, C2D_Color32(255, 255, 255, 255));
	Gui::Draw_Rect((32.5 + b), 130, 1, 20, C2D_Color32(255, 255, 255, 255));

	// Display Values.
	Gui::DrawStringCentered(0, 10, 0.7f, config->textColor(), std::to_string(r));
	Gui::DrawStringCentered(0, 60, 0.7f, config->textColor(), std::to_string(g));
	Gui::DrawStringCentered(0, 110, 0.7f, config->textColor(), std::to_string(b));

	// Display as formated string.
	char hexValues[16];

	snprintf(hexValues, sizeof hexValues, "#%02x%02x%02x", r, g, b);
	Gui::DrawStringCentered(0, 158, 0.7f, config->textColor(), "RGB: " + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + " - Hex: " + hexValues, 310);
	Gui::Draw_Rect(110, 180, 100, 30, C2D_Color32(r, g, b, 255));
}

// Draw.
static void Draw(u8 r, u8 g, u8 b) {
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
	C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
	GFX::DrawTop();
	Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(0, 0, 0, 190));
	Gui::DrawStringCentered(0, config->useBars() ? 0 : 2, 0.7f, config->textColor(), Lang::get("SELECT_RGB_COLOR"), 400);
	Gui::DrawStringCentered(0, (240-Gui::GetStringHeight(0.7f, Lang::get("UI_COLOR_BEHAVIOUR")))/2, 0.7f, config->textColor(), Lang::get("UI_COLOR_BEHAVIOUR"), 390, 70);
	Gui::ScreenDraw(Bottom);
	Gui::Draw_Rect(0, 0, 320, 240, config->bottomBG() + C2D_Color32(0, 0, 0, 190));
	DrawRGBColor(r, g, b);
	C3D_FrameEnd(0);
}


u32 Overlays::SelectColor(u32 oldColor) {
	u8 r = ColorHelper::getColorValue(oldColor, 2);
	u8 g = ColorHelper::getColorValue(oldColor, 1);
	u8 b = ColorHelper::getColorValue(oldColor, 0);

	int selection = 0;
	while(1) {
		Draw(r, g, b);

		hidScanInput();
		hidTouchRead(&touch);

		if (hidKeysDown() & KEY_UP) {
			if (selection > 0) selection--;
		}

		if (hidKeysDown() & KEY_DOWN) {
			if (selection < 2) selection++;
		}

		if (hidKeysHeld() & KEY_RIGHT) {
			switch(selection) {
				case 0: // Red.
					if (r < 255) r++;
					break;
				case 1: // Green.
					if (g < 255) g++;
					break;
				case 2:
					if (b < 255) b++;
					break;
			}
		}

		if (hidKeysHeld() & KEY_LEFT) {
			switch(selection) {
				case 0: // Red.
					if (r > 0) r--;
					break;
				case 1: // Green.
					if (g > 0) g--;
					break;
				case 2:
					if (b > 0) b--;
					break;
			}
		}

		if (hidKeysDown() & KEY_START) {
			return RGBA8(r, g, b, 255);
		}

		if (hidKeysDown() & KEY_B) {
			return oldColor;
		}

		if (hidKeysHeld() & KEY_TOUCH) {
			for (int i = 0; i < 256; i++) {
				if (touch.px >= (32.5 + i) && touch.px <= (32.5 + i) + 1 && touch.py >= 30 && touch.py <= 30 + 20) {
					r = i;
				}

				if (touch.px >= (32.5 + i) && touch.px <= (32.5 + i) + 1 && touch.py >= 80 && touch.py <= 80 + 20) {
					g = i;
				}

				if (touch.px >= (32.5 + i) && touch.px <= (32.5 + i) + 1 && touch.py >= 130 && touch.py <= 130 + 20) {
					b = i;
				}
			}
		}
	}
}