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
static const std::vector<std::string> languages = { "Bruh", "Dansk", "Deutsch", "English", "Español", "Français", "Italiano", "Lietuvių", "Polski", "Português", "Русский", "日本語" };
static const std::string langsTemp[] = { "br", "da", "de", "en", "es", "fr", "it", "lt", "pl", "pt", "ru", "jp "};

static const std::vector<Structs::ButtonPos> mainButtons = {
	{85, 6, 150, 22},
	{85, 36, 150, 22},
	{85, 66, 150, 22},
	{85, 96, 150, 22},
	{85, 126, 150, 22},
	{85, 156, 150, 22},
	{85, 186, 150, 22},
	{85, 216, 150, 22}
};

void Overlays::SelectLanguage() {
	bool doOut = false;
	int selection = 0, sPos = 0;

	while(!doOut) {
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
		C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));

		GFX::DrawTop();
		Gui::DrawStringCentered(0, 1, 0.7f, C2D_Color32(255, 255, 255, 255), Lang::get("SELECT_LANG"));
		GFX::DrawBottom();

		for(int i = 0; i < 8 && i < (int)languages.size(); i++) {
			GFX::drawBox(85, 6 + (i * 30), 150, 22, sPos + i == selection);
			Gui::DrawStringCentered(0, mainButtons[i].y + 4, 0.45f, C2D_Color32(255, 255, 255, 255), languages[sPos + i], 280);
		}
		C3D_FrameEnd(0);

		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);
		u32 hRepeat = hidKeysDownRepeat();

		if (hRepeat & KEY_DOWN) {
			if (selection < (int)languages.size() - 1) selection++;
		}

		if (hRepeat & KEY_UP) {
			if (selection > 0) selection--;
		}

		if (hidKeysDown() & KEY_A) {
			const std::string l = langsTemp[selection];
			config->language(l);
			Lang::load(config->language());
			doOut = true;
		}

		if (selection < sPos) sPos = selection;
		else if (selection > sPos + 8 - 1) sPos = selection - 8 + 1;

		if (hidKeysDown() & KEY_B) doOut = true;
	}
}