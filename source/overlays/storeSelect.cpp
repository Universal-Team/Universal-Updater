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
#include "download.hpp"
#include "fileBrowse.hpp"
#include "files.hpp"
#include "keyboard.hpp"
#include "overlay.hpp"
#include "qrcode.hpp"
#include "scriptUtils.hpp"
#include "storeUtils.hpp"
#include <unistd.h>

extern bool checkWifiStatus();
extern void notConnectedMsg();
extern bool touching(touchPosition touch, Structs::ButtonPos button);
static const std::vector<Structs::ButtonPos> mainButtons = {
	{ 10, 34, 300, 22 },
	{ 10, 64, 300, 22 },
	{ 10, 94, 300, 22 },
	{ 10, 124, 300, 22 },
	{ 10, 154, 300, 22 },
	{ 10, 184, 300, 22 },

	{ 112, 215, 16, 16 }, // Delete.
	{ 154, 215, 16, 16 }, // Update.
	{ 200, 215, 16, 16 }, // Add.
	{ 4, 0, 24, 24 } // Back.
};

/*
	Delete a store.. including the Spritesheets, if found.

	const std::string &file: The file of the UniStore.
*/
static void DeleteStore(const std::string &file) {
	nlohmann::json storeJson;
	FILE *temp = fopen((std::string(_STORE_PATH) + file).c_str(), "rt");
	if (temp) {
		storeJson = nlohmann::json::parse(temp, nullptr, false);
		fclose(temp);
	}
	if (storeJson.is_discarded())
		storeJson = {};

	/* Check, if Spritesheet exist on UniStore. */
	if (storeJson["storeInfo"].contains("sheet") && storeJson["storeInfo"]["sheet"].is_array()) {
		const std::vector<std::string> sht = storeJson["storeInfo"]["sheet"].get<std::vector<std::string>>();

		/* Cause it's an array, delete all Spritesheets which exist. */
		for (int i = 0; i < (int)sht.size(); i++) {
			if (sht[i] != "") {
				if (!(StringUtils::lower_case(sht[i]).find(StringUtils::lower_case("/")) != std::string::npos)) {
					if (access((std::string(_STORE_PATH) + sht[i]).c_str(), F_OK) == 0) {
						deleteFile((std::string(_STORE_PATH) + sht[i]).c_str());
					}
				}
			}
		}

	/* Else, if it's just a string.. check and delete single Spritesheet. */
	} else if (storeJson["storeInfo"].contains("sheet") && storeJson["storeInfo"]["sheet"].is_string()) {
		const std::string fl = storeJson["storeInfo"]["sheet"];

		if (fl != "") {
			if (!(StringUtils::lower_case(fl).find(StringUtils::lower_case("/")) != std::string::npos)) {
				if (access((std::string(_STORE_PATH) + fl).c_str(), F_OK) == 0) {
					deleteFile((std::string(_STORE_PATH) + fl).c_str());
				}
			}
		}
	}

	deleteFile((std::string(_STORE_PATH) + file).c_str()); // Now delete UniStore.
}

/*
	Download a store.. including the SpriteSheets, if found.
*/
static bool DownloadStore() {
	bool doSheet = false;
	std::string file = "";

	const std::string URL = QR_Scanner::StoreHandle();
	if (URL != "") doSheet = DownloadUniStore(URL, -1, file, true);

	if (doSheet) {
		nlohmann::json storeJson;
		FILE *temp = fopen(file.c_str(), "rt");
		if (temp) {
			storeJson = nlohmann::json::parse(temp, nullptr, false);
			fclose(temp);
		}
		if (storeJson.is_discarded())
			storeJson = { };

		if (storeJson["storeInfo"].contains("sheetURL") && storeJson["storeInfo"]["sheetURL"].is_array()) {
			if (storeJson["storeInfo"].contains("sheet") && storeJson["storeInfo"]["sheet"].is_array()) {
				const std::vector<std::string> locs = storeJson["storeInfo"]["sheetURL"].get<std::vector<std::string>>();
				const std::vector<std::string> sht = storeJson["storeInfo"]["sheet"].get<std::vector<std::string>>();

				if (locs.size() == sht.size()) {
					for (int i = 0; i < (int)sht.size(); i++) {
						if (!(sht[i].find("/") != std::string::npos)) {
							char msg[150];
							snprintf(msg, sizeof(msg), Lang::get("DOWNLOADING_SPRITE_SHEET2").c_str(), i + 1, sht.size());
							Msg::DisplayMsg(msg);
							DownloadSpriteSheet(locs[i], sht[i]);

						} else {
							Msg::waitMsg(Lang::get("SHEET_SLASH"));
						}
					}
				}
			}

		} else if (storeJson["storeInfo"].contains("sheetURL") && storeJson["storeInfo"]["sheetURL"].is_string()) {
			if (storeJson["storeInfo"].contains("sheet") && storeJson["storeInfo"]["sheet"].is_string()) {
				const std::string fl = storeJson["storeInfo"]["sheetURL"];
				const std::string fl2 = storeJson["storeInfo"]["sheet"];

				if (!(fl2.find("/") != std::string::npos)) {
					Msg::DisplayMsg(Lang::get("DOWNLOADING_SPRITE_SHEET"));
					DownloadSpriteSheet(fl, fl2);

				} else {
					Msg::waitMsg(Lang::get("SHEET_SLASH"));
				}
			}
		}
	}

	hidScanInput(); // Re-Scan.
	return doSheet;
}

static bool UpdateStore(const std::string &URL) {
	bool doSheet = false;
	std::string file = "";

	if (URL != "") doSheet = DownloadUniStore(URL, -1, file, false);

	if (doSheet) {
		nlohmann::json storeJson;
		FILE *temp = fopen(file.c_str(), "rt");
		if (temp) {
			storeJson = nlohmann::json::parse(temp, nullptr, false);
			fclose(temp);
		}
		if (storeJson.is_discarded())
			storeJson = { };

		if (storeJson["storeInfo"].contains("sheetURL") && storeJson["storeInfo"]["sheetURL"].is_array()) {
			if (storeJson["storeInfo"].contains("sheet") && storeJson["storeInfo"]["sheet"].is_array()) {
				const std::vector<std::string> locs = storeJson["storeInfo"]["sheetURL"].get<std::vector<std::string>>();
				const std::vector<std::string> sht = storeJson["storeInfo"]["sheet"].get<std::vector<std::string>>();

				if (locs.size() == sht.size()) {
					for (int i = 0; i < (int)sht.size(); i++) {
						if (!(sht[i].find("/") != std::string::npos)) {
							char msg[150];
							snprintf(msg, sizeof(msg), Lang::get("DOWNLOADING_SPRITE_SHEET2").c_str(), i + 1, sht.size());
							Msg::DisplayMsg(msg);
							DownloadSpriteSheet(locs[i], sht[i]);

						} else {
							Msg::waitMsg(Lang::get("SHEET_SLASH"));
						}
					}
				}
			}

		} else if (storeJson["storeInfo"].contains("sheetURL") && storeJson["storeInfo"]["sheetURL"].is_string()) {
			if (storeJson["storeInfo"].contains("sheet") && storeJson["storeInfo"]["sheet"].is_string()) {
				const std::string fl = storeJson["storeInfo"]["sheetURL"];
				const std::string fl2 = storeJson["storeInfo"]["sheet"];

				if (!(fl2.find("/") != std::string::npos)) {
					Msg::DisplayMsg(Lang::get("DOWNLOADING_SPRITE_SHEET"));
					DownloadSpriteSheet(fl, fl2);

				} else {
					Msg::waitMsg(Lang::get("SHEET_SLASH"));
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
*/
void Overlays::SelectStore() {
	bool doOut = false;
	int selection = 0, sPos = 0;

	std::vector<UniStoreInfo> info = GetUniStoreInfo(_STORE_PATH);

	while(!doOut) {
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

		if (info.size() > 0) {
			if (info[selection].StoreSize != -1) {
				Gui::DrawStringCentered(0, 1, 0.7f, UIThemes->TextColor(), info[selection].Title, 390, 0, font);
				Gui::DrawStringCentered(0, 30, 0.6f, UIThemes->TextColor(), info[selection].Author, 380, 0, font);
				Gui::DrawStringCentered(0, 70, 0.5f, UIThemes->TextColor(), info[selection].Description, 380, 130, font, C2D_WordWrap);

			} else {
				Gui::DrawStringCentered(0, 1, 0.7f, UIThemes->TextColor(), Lang::get("INVALID_UNISTORE"), 390, 0, font);
			}

			Gui::DrawString(10, 200, 0.4, UIThemes->TextColor(), "- " + Lang::get("ENTRIES") + ": " + std::to_string(info[selection].StoreSize), 150, 0, font);
			Gui::DrawString(10, 210, 0.4, UIThemes->TextColor(), "- " + Lang::get("VERSION") + ": " + std::to_string(info[selection].Version), 150, 0, font);
			Gui::DrawString(10, 220, 0.4, UIThemes->TextColor(), "- " + Lang::get("REVISION") + ": " + std::to_string(info[selection].Revision), 150, 0, font);

			Animation::QueueEntryDone();
			GFX::DrawBottom();

			Gui::Draw_Rect(0, 0, 320, 25, UIThemes->BarColor());
			Gui::Draw_Rect(0, 25, 320, 1, UIThemes->BarOutline());
			GFX::DrawIcon(sprites_arrow_idx, mainButtons[9].x, mainButtons[9].y, UIThemes->TextColor());
			Gui::DrawStringCentered(0, 2, 0.6, UIThemes->TextColor(), Lang::get("SELECT_UNISTORE_2"), 310, 0, font);

			for(int i = 0; i < 6 && i < (int)info.size(); i++) {
				if (sPos + i == selection) Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, UIThemes->MarkSelected());
				Gui::DrawStringCentered(10 - 160 + (300 / 2), mainButtons[i].y + 4, 0.45f, UIThemes->TextColor(), info[sPos + i].FileName, 295, 0, font);
			}
		} else {
			GFX::DrawBottom(); // Otherwise we'd draw on top.
		}

		GFX::DrawIcon(sprites_delete_idx, mainButtons[6].x, mainButtons[6].y, UIThemes->TextColor());
		GFX::DrawIcon(sprites_update_idx, mainButtons[7].x, mainButtons[7].y, UIThemes->TextColor());
		GFX::DrawIcon(sprites_add_idx, mainButtons[8].x, mainButtons[8].y, UIThemes->TextColor());
		C3D_FrameEnd(0);

		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);
		u32 hRepeat = hidKeysDownRepeat();
		Animation::HandleQueueEntryDone();

		if (info.size() > 0) {
			if (hRepeat & KEY_DOWN) {
				if (selection < (int)info.size() - 1) selection++;
				else selection = 0;
			}

			if (hRepeat & KEY_UP) {
				if (selection > 0) selection--;
				else selection = info.size() - 1;
			}

			if (hRepeat & KEY_RIGHT) {
				if (selection + 6 < (int)info.size() - 1) selection += 6;
				else selection = info.size() - 1;
			}

			if (hRepeat & KEY_LEFT) {
				if (selection - 6 > 0) selection -= 6;
				else selection = 0;
			}

			if (hidKeysDown() & KEY_A) {
				if (info[selection].File != "") { // Ensure to check for this.
					if (!(info[selection].File.find("/") != std::string::npos)) {
						/* Load selected one. */
						if (info[selection].Version == -1) Msg::waitMsg(Lang::get("UNISTORE_INVALID_ERROR"));
						else if (info[selection].Version < 3) Msg::waitMsg(Lang::get("UNISTORE_TOO_OLD"));
						else if (info[selection].Version > _UNISTORE_VERSION) Msg::waitMsg(Lang::get("UNISTORE_TOO_NEW"));
						else {
							config->lastStore(info[selection].FileName);
							StoreUtils::store = std::make_unique<Store>(_STORE_PATH + info[selection].FileName, info[selection].FileName);
							StoreUtils::ResetAll();
							StoreUtils::SortEntries(false, SortType::LAST_UPDATED);
							doOut = true;
						}

					} else {
						Msg::waitMsg(Lang::get("FILE_SLASH"));
					}
				}
			}

			if (hidKeysDown() & KEY_TOUCH) {
				for (int i = 0; i < 6; i++) {
					if (touching(touch, mainButtons[i])) {
						if (i + sPos < (int)info.size() && info[i + sPos].File != "") { // Ensure to check for this.
							if (!(info[i + sPos].File.find("/") != std::string::npos)) {
								if (info[i + sPos].Version == -1) Msg::waitMsg(Lang::get("UNISTORE_INVALID_ERROR"));
								else if (info[i + sPos].Version < 3) Msg::waitMsg(Lang::get("UNISTORE_TOO_OLD"));
								else if (info[i + sPos].Version > _UNISTORE_VERSION) Msg::waitMsg(Lang::get("UNISTORE_TOO_NEW"));
								else {
									config->lastStore(info[i + sPos].FileName);
									StoreUtils::store = std::make_unique<Store>(_STORE_PATH + info[i + sPos].FileName, info[i + sPos].FileName);
									StoreUtils::ResetAll();
									StoreUtils::SortEntries(false, SortType::LAST_UPDATED);
									doOut = true;
								}

							} else {
								Msg::waitMsg(Lang::get("FILE_SLASH"));
							}
						}
					}
				}
			}

			/* Delete UniStore. */
			if ((hidKeysDown() & KEY_X) || (hidKeysDown() & KEY_TOUCH && touching(touch, mainButtons[6]))) {
				if (info[selection].FileName != "") {
					DeleteStore(info[selection].FileName);
					selection = 0;
					info = GetUniStoreInfo(_STORE_PATH);
				}
			}

			/* Download latest UniStore. */
			if ((hidKeysDown() & KEY_START) || (hidKeysDown() & KEY_TOUCH && touching(touch, mainButtons[7]))) {
				if (checkWifiStatus()) {
					if (info[selection].URL != "") {
						if (UpdateStore(info[selection].URL)) {
							selection = 0;
							info = GetUniStoreInfo(_STORE_PATH);
						}
					}

				} else {
					notConnectedMsg();
				}
			}

			if (selection < sPos) sPos = selection;
			else if (selection > sPos + 6 - 1) sPos = selection - 6 + 1;
		}

		/* UniStore QR Code / URL Download. */
		if ((hidKeysDown() & KEY_Y) || (hidKeysDown() & KEY_TOUCH && touching(touch, mainButtons[8]))) {
			if (checkWifiStatus()) {
				if (DownloadStore()) {
					selection = 0;
					info = GetUniStoreInfo(_STORE_PATH);
				}

			} else {
				notConnectedMsg();
			}
		}

		/* Go out of the menu. */
		if ((hidKeysDown() & KEY_B) || (hidKeysDown() & KEY_TOUCH && touching(touch, mainButtons[9]))) doOut = true;
	}
}