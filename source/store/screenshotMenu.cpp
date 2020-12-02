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

#include "storeUtils.hpp"
#include "structs.hpp"

extern bool touching(touchPosition touch, Structs::ButtonPos button);

/*
	Draw the Screenshot menu.

	const C2D_Image &img: The C2D_Image of the screenshot.
	const int sIndex: The Screenshot index.
	const bool sFetch: If fetching screenshots or not.
	const int screenshotSize: The screenshot amount.
	const std::string &name: The name of the screenshot.
	const bool zoom: If zoomed in (true) or out (false).
*/
void StoreUtils::DrawScreenshotMenu(const C2D_Image &img, const int sIndex, const bool sFetch, const int screenshotSize, const std::string &name, const bool zoom) {
	Gui::ScreenDraw(Top);
	Gui::Draw_Rect(0, 0, 400, 240, BG_COLOR);

	if (screenshotSize > 0) {
		if (!zoom) {
			if (img.tex) C2D_DrawImageAt(img, 100, 0, 0.5f, nullptr, 0.5f, 0.5f);

		} else {
			if (img.tex) C2D_DrawImageAt(img, 0, 0, 0.5f, nullptr, 1.0f, 1.0f);
		}
	}

	GFX::DrawBottom();

	/* Bottom. */
	if (zoom) {
		if (img.tex) {
			if (img.subtex->height > 240) {
				C2D_DrawImageAt(img, -40, -240, 0.5f, nullptr, 1.0f, 1.0f);
			}

		} else {
			if (screenshotSize > 0) {
				Gui::Draw_Rect(0, 215, 320, 25, BAR_COLOR);
				Gui::Draw_Rect(0, 214, 320, 1, BAR_OUTL_COLOR);
				Gui::DrawStringCentered(0, 220, 0.5f, TEXT_COLOR, Lang::get("SCREENSHOT_INSTRUCTIONS"), 310, 0, font);

				char screenshots[0x100];
				snprintf(screenshots, sizeof(screenshots), Lang::get("SCREENSHOT").c_str(), sIndex + 1, screenshotSize);
				Gui::DrawStringCentered(0, 2, 0.6f, WHITE, screenshots, 310, 0, font);
				Gui::DrawStringCentered(0, 40, 0.6f, WHITE, name, 310, 0, font);

			} else {
				Gui::DrawStringCentered(0, 2, 0.6f, WHITE, Lang::get("NO_SCREENSHOTS_AVAILABLE"), 310);
			}
		}

	} else {

		if (screenshotSize > 0) {
			Gui::Draw_Rect(0, 215, 320, 25, BAR_COLOR);
			Gui::Draw_Rect(0, 214, 320, 1, BAR_OUTL_COLOR);
			Gui::DrawStringCentered(0, 220, 0.5f, TEXT_COLOR, Lang::get("SCREENSHOT_INSTRUCTIONS"), 310, 0, font);

			char screenshots[0x100];
			snprintf(screenshots, sizeof(screenshots), Lang::get("SCREENSHOT").c_str(), sIndex + 1, screenshotSize);
			Gui::DrawStringCentered(0, 2, 0.6f, WHITE, screenshots, 310, 0, font);
			Gui::DrawStringCentered(0, 40, 0.6f, WHITE, name, 310, 0, font);

		} else {
			Gui::DrawStringCentered(0, 2, 0.6f, WHITE, Lang::get("NO_SCREENSHOTS_AVAILABLE"), 310);
		}
	}
}

/*
	Screenshot Menu handle.

	C2D_Image &img: The C2D_Image of the screenshot.
	int &sIndex: The Screenshot index.
	bool &sFetch: If fetching screenshots or not.
	const int screenshotSize: The screenshot amount.
	bool &zoom: If zoomed in (true) or out (false).
*/
void StoreUtils::ScreenshotMenu(C2D_Image &img, int &sIndex, bool &sFetch, int &storeMode, const int screenshotSize, bool &zoom) {
	if (hDown & KEY_B) {
		zoom = false;
		sIndex = 0;
		storeMode = 0; // Go back to EntryInfo.
	}

	if (hDown & KEY_RIGHT) {
		if (sIndex < screenshotSize - 1) {
			sIndex++;
			sFetch = true;
		}
	}

	if (hDown & KEY_DOWN) zoom = false;

	if (hDown & KEY_UP) zoom = true;

	if (hDown & KEY_LEFT) {
		if (sIndex > 0) {
			sIndex--;
			sFetch = true;
		}
	}
}