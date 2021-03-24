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

#include "animation.hpp"
#include "common.hpp"
#include "overlay.hpp"
#include "storeUtils.hpp"

extern bool touching(touchPosition touch, Structs::ButtonPos button);
static const std::vector<Structs::ButtonPos> mainButtons = {
	{ 10, 6, 300, 22 },
	{ 10, 36, 300, 22 },
	{ 10, 66, 300, 22 },
	{ 10, 96, 300, 22 },
	{ 10, 126, 300, 22 },
	{ 10, 156, 300, 22 },
	{ 10, 186, 300, 22 }
};

extern std::vector<std::pair<std::string, std::string>> Themes;

/* Select a Theme. */
void Overlays::SelectTheme() {
	bool Finish = false;
	int selection = 0, sPos = 0;

	while(!Finish) {
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(Top, TRANSPARENT);
		C2D_TargetClear(Bottom, TRANSPARENT);

		if (StoreUtils::store && config->usebg() && StoreUtils::store->customBG()) {
			Gui::ScreenDraw(Top);
			Gui::Draw_Rect(0, 0, 400, 25, UIThemes->BarColor());
			Gui::Draw_Rect(0, 25, 400, 1, UIThemes->BarOutline());
			C2D_DrawImageAt(StoreUtils::store->GetStoreImg(), 0, 26, 0.5f, nullptr);

		} else {
			GFX::DrawTop();
		}

		Gui::DrawStringCentered(0, 1, 0.7f, UIThemes->TextColor(), Lang::get("SELECT_A_THEME"), 380, 0, font);

		Gui::Draw_Rect(0, 215, 400, 25, UIThemes->BarColor());
		Gui::Draw_Rect(0, 214, 400, 1, UIThemes->BarOutline());
		Gui::DrawStringCentered(0, 40, 0.5f, UIThemes->TextColor(), Themes[selection].second, 380, 140, font);

		Animation::QueueEntryDone();
		GFX::DrawBottom();

		Gui::Draw_Rect(0, 215, 320, 25, UIThemes->BarColor());
		Gui::Draw_Rect(0, 214, 320, 1, UIThemes->BarOutline());

		if (Themes.size() > 0) {
			for(int i = 0; i < 7 && i < (int)Themes.size(); i++) {
				if (sPos + i == selection) Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, UIThemes->MarkSelected());
				Gui::DrawStringCentered(10 - 160 + (300 / 2), mainButtons[i].y + 4, 0.45f, UIThemes->TextColor(), Themes[sPos + i].first, 295, 0, font);
			}
		}

		C3D_FrameEnd(0);
		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);
		u32 hRepeat = hidKeysDownRepeat();
		Animation::HandleQueueEntryDone();

		if (Themes.size() > 0) {
			if (hRepeat & KEY_DOWN) {
				if (selection < (int)Themes.size() - 1) selection++;
				else selection = 0;
			}

			if (hRepeat & KEY_UP) {
				if (selection > 0) selection--;
				else selection = Themes.size() - 1;
			}

			if (hRepeat & KEY_RIGHT) {
				if (selection + 7 < (int)Themes.size()-1) selection += 7;
				else selection = Themes.size()-1;
			}

			if (hRepeat & KEY_LEFT) {
				if (selection - 7 > 0) selection -= 7;
				else selection = 0;
			}

			if (hidKeysDown() & KEY_A) {
				UIThemes->LoadTheme(Themes[selection].first);
				config->theme(Themes[selection].first);
				Finish = true;
			}

			if (hidKeysDown() & KEY_TOUCH) {
				for (int i = 0; i < 7; i++) {
					if (touching(touch, mainButtons[i])) {
						if (i + sPos < (int)Themes.size()) {
							UIThemes->LoadTheme(Themes[i + sPos].first);
							config->theme(Themes[i + sPos].first);
							Finish = true;
						}
					}
				}
			}

			if (selection < sPos) sPos = selection;
			else if (selection > sPos + 7 - 1) sPos = selection - 7 + 1;
		}

		if (hidKeysDown() & KEY_B) Finish = true;
	}
}