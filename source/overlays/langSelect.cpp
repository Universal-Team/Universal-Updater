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

#include "fileBrowse.hpp"
#include "keyboard.hpp"
#include "overlay.hpp"
#include "scriptUtils.hpp"
#include "storeUtils.hpp"

extern bool checkWifiStatus();
extern bool touching(touchPosition touch, Structs::ButtonPos button);
static const std::vector<std::string> languages = { "Bruh", "Dansk", "Deutsch", "English", "Español", "Français", "Italiano", "Lietuvių", "Magyar", "Polski", "Português", "Русский", "日本語" };
static const std::string langsTemp[] = { "br", "da", "de", "en", "es", "fr", "it", "lt", "hu", "pl", "pt", "ru", "jp"};

static const std::vector<Structs::ButtonPos> mainButtons = {
	{ 10, 4, 300, 22 },
	{ 10, 34, 300, 22 },
	{ 10, 64, 300, 22 },
	{ 10, 94, 300, 22 },
	{ 10, 124, 300, 22 },
	{ 10, 154, 300, 22 },
	{ 10, 184, 300, 22 },
	{ 10, 214, 300, 22 }
};

/*
	Select a Language.

	Can be skipped with `B`.
*/
void Overlays::SelectLanguage() {
	bool doOut = false;
	int selection = 0, sPos = 0;

	while(!doOut) {
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(Top, TRANSPARENT);
		C2D_TargetClear(Bottom, TRANSPARENT);

		GFX::DrawTop();
		Gui::DrawStringCentered(0, 1, 0.7f, TEXT_COLOR, Lang::get("SELECT_LANG"));
		GFX::DrawBottom();

		for(int i = 0; i < 8 && i < (int)languages.size(); i++) {
			if (sPos + i == selection) GFX::DrawBox(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, false);
			Gui::DrawStringCentered(0, mainButtons[i].y + 4, 0.45f, TEXT_COLOR, languages[sPos + i], 280);
		}
		C3D_FrameEnd(0);

		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);
		u32 hRepeat = hidKeysDownRepeat();

		if (hRepeat & KEY_DOWN) {
			if (selection < (int)languages.size() - 1) selection++;
			else selection = 0;
		}

		if (hRepeat & KEY_UP) {
			if (selection > 0) selection--;
			else selection = languages.size() - 1;
		}

		if (hRepeat & KEY_RIGHT) {
			if (selection + 8 < (int)languages.size()-1) selection += 8;
			else selection = languages.size()-1;
		}

		if (hRepeat & KEY_LEFT) {
			if (selection - 8 > 0) selection -= 8;
			else selection = 0;
		}

		if (hidKeysDown() & KEY_A) {
			const std::string l = langsTemp[selection];
			config->language(l);
			Lang::load(config->language());
			doOut = true;
		}

		if (hidKeysDown() & KEY_TOUCH) {
			for (int i = 0; i < 8; i++) {
				if (touching(touch, mainButtons[i])) {
					if (i + sPos < (int)languages.size()) {
						const std::string l = langsTemp[i + sPos];
						config->language(l);
						Lang::load(config->language());
						doOut = true;
					}
				}
			}
		}

		if (selection < sPos) sPos = selection;
		else if (selection > sPos + 8 - 1) sPos = selection - 8 + 1;

		if (hidKeysDown() & KEY_B) doOut = true;
	}
}