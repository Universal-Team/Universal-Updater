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

#include "download.hpp"
#include "fileBrowse.hpp"
#include "keyboard.hpp"
#include "overlay.hpp"
#include "scriptUtils.hpp"
#include "storeUtils.hpp"

extern bool checkWifiStatus();
extern bool touching(touchPosition touch, Structs::ButtonPos button);
static const std::vector<Structs::ButtonPos> mainButtons = {
	{10, 6, 300, 22},
	{10, 36, 300, 22},
	{10, 66, 300, 22},
	{10, 96, 300, 22},
	{10, 126, 300, 22},
	{10, 156, 300, 22},
	{10, 186, 300, 22},

	/* Add.. */
	{10, 215, 20, 20}
};

void Overlays::SelectStore(std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, std::unique_ptr<Meta> &meta) {
	bool doOut = false;
	int selection = 0, sPos = 0;

	std::vector<UniStoreInfo> info = GetUniStoreInfo("sdmc:/3ds/Universal-Updater/stores/");

	while(!doOut) {
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
		C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));

		GFX::DrawTop();
		Gui::DrawStringCentered(0, 1, 0.7f, C2D_Color32(255, 255, 255, 255), Lang::get("SELECT_UNISTORE_2"));

		if (info.size() > 0) {
			if (info[selection].StoreSize != -1) {
				Gui::DrawStringCentered(0, 30, 0.6f, C2D_Color32(255, 255, 255, 255), info[selection].Title, 370);
				Gui::DrawStringCentered(0, 70, 0.5f, C2D_Color32(255, 255, 255, 255), info[selection].Author, 370);
				if (info[selection].Description != "") Gui::DrawStringCentered(0, 110, 0.5f, C2D_Color32(255, 255, 255, 255), info[selection].Description, 370, 90, nullptr, C2D_WordWrap);

			} else {
				Gui::DrawStringCentered(0, 30, 0.6f, C2D_Color32(255, 255, 255, 255), Lang::get("INVALID_UNISTORE"), 370);
			}

			GFX::DrawBottom();

			for(int i = 0; i < 7 && i < (int)info.size(); i++) {
				GFX::drawBox(10, 6 + (i * 30), 300, 22, sPos + i == selection);
				Gui::DrawStringCentered(0, mainButtons[i].y + 4, 0.45f, C2D_Color32(255, 255, 255, 255), info[sPos + i].FileName, 280);
			}
		}

		if (info.size() <= 0) GFX::DrawBottom(); // Otherwise we'd draw on top.
		GFX::drawBox(10, 215, 20, 20, false);
		C3D_FrameEnd(0);

		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);
		u32 hRepeat = hidKeysDownRepeat();

		if (info.size() > 0) {
			if (hRepeat & KEY_DOWN) {
				if (selection < (int)info.size() - 1) selection++;
			}

			if (hRepeat & KEY_UP) {
				if (selection > 0) selection--;
			}

			if (hidKeysDown() & KEY_A) {
				/* Load selected one. */
				if (info[selection].StoreSize > 0) {
					store = std::make_unique<Store>("sdmc:/3ds/Universal-Updater/stores/" + info[selection].FileName);
					StoreUtils::ResetAll(store, meta, entries);
					config->lastStore(info[selection].FileName);
					doOut = true;
				}
			}

			if (selection < sPos) sPos = selection;
			else if (selection > sPos + 7 - 1) sPos = selection - 7 + 1;
		}

		if ((hidKeysDown() & KEY_Y) || (hidKeysDown() & KEY_TOUCH && touching(touch, mainButtons[7]))) {
			if (checkWifiStatus()) {
				const std::string URL = Input::setkbdString(50, Lang::get("ENTER_URL"));

				if (URL != "") {
					Msg::DisplayMsg(Lang::get("DOWNLOAD_UNISTORE"));

					if (DownloadUniStore(URL)) {
						info = GetUniStoreInfo("sdmc:/3ds/Universal-Updater/stores/");

					} else {
						Msg::waitMsg(Lang::get("INVALID_UNISTORE"));
					}
				}
			}
		}

		if (hidKeysDown() & KEY_B) doOut = true;
	}
}