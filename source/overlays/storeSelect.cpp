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
#include "files.hpp"
#include "keyboard.hpp"
#include "overlay.hpp"
#include "scriptUtils.hpp"
#include "storeUtils.hpp"
#include <unistd.h>

extern bool checkWifiStatus();
extern bool touching(touchPosition touch, Structs::ButtonPos button);
static const std::vector<Structs::ButtonPos> mainButtons = {
	{ 10, 4, 300, 22 },
	{ 10, 34, 300, 22 },
	{ 10, 64, 300, 22 },
	{ 10, 94, 300, 22 },
	{ 10, 124, 300, 22 },
	{ 10, 154, 300, 22 },
	{ 10, 184, 300, 22 },

	/* Add, Delete, Info.. */
	{ 110, 215, 20, 20 },
	{ 150, 215, 20, 20 },
	{ 190, 215, 20, 20 }
};

/*
	Delete a Store.. including the Spritesheets, if found.

	const std::string &file: The file of the UniStore.
*/
static void DeleteStore(const std::string &file) {
	FILE *temp = fopen((std::string(_STORE_PATH) + file).c_str(), "rt");
	nlohmann::json storeJson = nlohmann::json::parse(temp, nullptr, false);
	fclose(temp);

	/* Check, if Spritesheet exist on UniStore. */
	if (storeJson["storeInfo"].contains("sheet") && storeJson["storeInfo"]["sheet"].is_array()) {
		const std::vector<std::string> sht = storeJson["storeInfo"]["sheet"].get<std::vector<std::string>>();

		/* Cause it's an array, delete all Spritesheets which exist. */
		for (int i = 0; i < (int)sht.size(); i++) {
			if ((std::string(_STORE_PATH) + sht[i]) != "") {
				if (access((std::string(_STORE_PATH) + sht[i]).c_str(), F_OK) == 0) {
					deleteFile((std::string(_STORE_PATH) + sht[i]).c_str());
				}
			}
		}

	/* Else, if it's just a string.. check and delete single Spritesheet. */
	} else if (storeJson["storeInfo"].contains("sheetURL") && storeJson["storeInfo"]["sheetURL"].is_string()) {
		const std::string fl = storeJson["storeInfo"]["sheet"];

		if ((std::string(_STORE_PATH) + fl) != "") {
			if (access((std::string(_STORE_PATH) + fl).c_str(), F_OK) == 0) {
				deleteFile((std::string(_STORE_PATH) + fl).c_str());
			}
		}
	}

	deleteFile((std::string(_STORE_PATH) + file).c_str()); // Now delete UniStore.
}

/*
	Download a Store.. including the SpriteSheets, if found.
*/
static bool DownloadStore() {
	bool doSheet = false;
	std::string file = "";

	if (checkWifiStatus()) {
		const std::string URL = Input::setkbdString(50, Lang::get("ENTER_URL"));

		if (URL != "") doSheet = DownloadUniStore(URL, -1, file, true);

		if (doSheet) {
			FILE *temp = fopen(file.c_str(), "rt");
			nlohmann::json storeJson = nlohmann::json::parse(temp, nullptr, false);
			fclose(temp);

			if (doSheet) {
				if (storeJson["storeInfo"].contains("sheetURL") && storeJson["storeInfo"]["sheetURL"].is_array()) {
					if (storeJson["storeInfo"].contains("sheet") && storeJson["storeInfo"]["sheet"].is_array()) {
						const std::vector<std::string> locs = storeJson["storeInfo"]["sheetURL"].get<std::vector<std::string>>();
						const std::vector<std::string> sht = storeJson["storeInfo"]["sheet"].get<std::vector<std::string>>();

						if (locs.size() == sht.size()) {
							for (int i = 0; i < (int)sht.size(); i++) {
								char msg[150];
								snprintf(msg, sizeof(msg), Lang::get("DOWNLOADING_SPRITE_SHEET2").c_str(), i + 1, sht.size());
								Msg::DisplayMsg(msg);
								DownloadSpriteSheet(locs[i], sht[i]);
							}
						}
					}

				} else if (storeJson["storeInfo"].contains("sheetURL") && storeJson["storeInfo"]["sheetURL"].is_string()) {
					if (storeJson["storeInfo"].contains("sheet") && storeJson["storeInfo"]["sheet"].is_string()) {
						const std::string fl = storeJson["storeInfo"]["sheetURL"];
						const std::string fl2 = storeJson["storeInfo"]["sheet"];
						Msg::DisplayMsg(Lang::get("DOWNLOADING_SPRITE_SHEET"));
						DownloadSpriteSheet(fl, fl2);
					}
				}
			}
		}
	}

	return doSheet;
}

/*
	This is the UniStore Manage Handle.
	Here you can..

	- Delete a UniStore.
	- Download / Add a UniStore.
	- Check for Updates for a UniStore.
	- Switch the UniStore.

	std::unique_ptr<Store> &store: Reference to the Store class.
	std::vector<std::unique_ptr<StoreEntry>> &entries: Reference to the Store Entries.
	std::unique_ptr<Meta> &meta: Reference to the Meta class.
*/
void Overlays::SelectStore(std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, std::unique_ptr<Meta> &meta) {
	bool doOut = false;
	int selection = 0, sPos = 0;

	std::vector<UniStoreInfo> info = GetUniStoreInfo(_STORE_PATH);

	while(!doOut) {
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(Top, TRANSPARENT);
		C2D_TargetClear(Bottom, TRANSPARENT);

		GFX::DrawTop();
		Gui::DrawStringCentered(0, 1, 0.7f, TEXT_COLOR, Lang::get("SELECT_UNISTORE_2"));

		if (info.size() > 0) {
			if (info[selection].StoreSize != -1) {
				Gui::DrawStringCentered(0, 30, 0.6f, TEXT_COLOR, info[selection].Title, 370);
				Gui::DrawStringCentered(0, 60, 0.5f, TEXT_COLOR, info[selection].Author, 370);
				if (info[selection].Description != "") Gui::DrawStringCentered(0, 100, 0.5f, TEXT_COLOR, info[selection].Description, 390, 0, nullptr, C2D_WordWrap);

			} else {
				Gui::DrawStringCentered(0, 30, 0.6f, TEXT_COLOR, Lang::get("INVALID_UNISTORE"), 370);
			}

			GFX::DrawBottom();

			for(int i = 0; i < 7 && i < (int)info.size(); i++) {
				GFX::drawBox(10, mainButtons[i].y, 300, 22, sPos + i == selection);
				Gui::DrawStringCentered(10 - 160 + (300 / 2), mainButtons[i].y + 4, 0.45f, TEXT_COLOR, info[sPos + i].FileName, 295);
			}
		}

		if (info.size() <= 0) GFX::DrawBottom(); // Otherwise we'd draw on top.

		GFX::drawBox(mainButtons[7].x, mainButtons[7].y, mainButtons[7].w, mainButtons[7].h, false);
		Gui::DrawString(mainButtons[7].x + 6, mainButtons[7].y, 0.6f, TEXT_COLOR, "-");

		GFX::drawBox(mainButtons[8].x, mainButtons[8].y, mainButtons[8].w, mainButtons[8].h, false);
		Gui::DrawString(mainButtons[8].x + 8, mainButtons[8].y, 0.6f, TEXT_COLOR, "i");

		GFX::drawBox(mainButtons[9].x, mainButtons[9].y, mainButtons[9].w, mainButtons[9].h, false);
		Gui::DrawString(mainButtons[9].x + 5, mainButtons[9].y, 0.6f, TEXT_COLOR, "+");
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
					store = std::make_unique<Store>(_STORE_PATH + info[selection].FileName);
					StoreUtils::ResetAll(store, meta, entries);
					config->lastStore(info[selection].FileName);
					doOut = true;
				}
			}

			if (hidKeysDown() & KEY_TOUCH) {
				for (int i = 0; i < 7; i++) {
					if (touching(touch, mainButtons[i])) {
						if (i + sPos < (int)info.size()) {
							if (info[i + sPos].StoreSize > 0) {
								store = std::make_unique<Store>(_STORE_PATH + info[i + sPos].FileName);
								StoreUtils::ResetAll(store, meta, entries);
								config->lastStore(info[i + sPos].FileName);
								doOut = true;
							}
						}
					}
				}
			}

			if (hidKeysDown() & KEY_TOUCH && touching(touch, mainButtons[8])) {
				if (checkWifiStatus()) {
					if (info[selection].Revision > 0) {
						const bool result = IsUpdateAvailable(info[selection].URL, info[selection].Revision);
						Msg::waitMsg((result ? Lang::get("UPDATE_AVAILABLE") : Lang::get("UPDATE_NOT_AVAILABLE")));
					}
				}
			}

			if (hidKeysDown() & KEY_TOUCH && touching(touch, mainButtons[7])) {
				if (info[selection].FileName != "") {
					DeleteStore(info[selection].FileName);
					selection = 0;
					info = GetUniStoreInfo(_STORE_PATH);
				}
			}

			if (selection < sPos) sPos = selection;
			else if (selection > sPos + 7 - 1) sPos = selection - 7 + 1;
		}

		if ((hidKeysDown() & KEY_Y) || (hidKeysDown() & KEY_TOUCH && touching(touch, mainButtons[9]))) {
			if (DownloadStore()) {
				selection = 0;
				info = GetUniStoreInfo(_STORE_PATH);

			} else {
				Msg::waitMsg(Lang::get("INVALID_UNISTORE"));
			}
		}

		if (hidKeysDown() & KEY_B) doOut = true;
	}
}