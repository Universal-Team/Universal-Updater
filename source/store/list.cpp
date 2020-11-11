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

static const std::vector<Structs::ButtonPos> StoreBoxesList = {
	{ 20, 45, 360, 50 },
	{ 20, 105, 360, 50 },
	{ 20, 165, 360, 50 }
};

/*
	Draw the top List.

	const std::unique_ptr<Store> &store: Const Reference to the Store class.
	const std::vector<std::unique_ptr<StoreEntry>> &entries: Const Reference to the StoreEntries.
*/
void StoreUtils::DrawList(const std::unique_ptr<Store> &store, const std::vector<std::unique_ptr<StoreEntry>> &entries) {
	if (store) { // Ensure, store is not a nullptr.

		if (config->usebg() && store->customBG()) {
			C2D_DrawImageAt(store->GetStoreImg(), 0, 26, 0.5f, nullptr);

		} else {
			Gui::Draw_Rect(0, 26, 400, 214, BG_COLOR);
		}

		if (entries.size() > 0) {
			for (int i = 0; i < 3 && i < (int)entries.size(); i++) {

				if (i + store->GetScreenIndx() == store->GetEntry()) {
					GFX::DrawBox(StoreBoxesList[i].x, StoreBoxesList[i].y, StoreBoxesList[i].w, StoreBoxesList[i].h, false);
				}

				/* Ensure, entries is larger than the index. */
				if ((int)entries.size() > i + store->GetScreenIndx()) {
					if (entries[i + store->GetScreenIndx()]) { // Ensure, the Entry is not nullptr.
						const C2D_Image tempImg = entries[i + store->GetScreenIndx()]->GetIcon();
						const uint8_t offsetW = (48 - tempImg.subtex->width) / 2; // Center W.
						const uint8_t offsetH = (48 - tempImg.subtex->height) / 2; // Center H.

						C2D_DrawImageAt(tempImg, StoreBoxesList[i].x + 1 + offsetW, StoreBoxesList[i].y + 1 + offsetH, 0.5);
					}

					if (entries[i + store->GetScreenIndx()]->GetUpdateAvl()) GFX::DrawSprite(sprites_update_app_idx, StoreBoxesList[i].x + 32, StoreBoxesList[i].y + 32);
					Gui::DrawStringCentered(29, StoreBoxesList[i].y + 5, 0.6f, TEXT_COLOR, entries[i + store->GetScreenIndx()]->GetTitle(), 300, 0, font);
					Gui::DrawStringCentered(29, StoreBoxesList[i].y + 24, 0.6f, TEXT_COLOR, entries[i + store->GetScreenIndx()]->GetAuthor(), 300, 0, font);
				}
			}
		}
	}
}


/*
	Top List Logic Handle.
	Here you can..

	- Scroll through the Grid with the D-Pad Up / Down and skip 3 Entries with Left / Right.

	std::unique_ptr<Store> &store: Reference to the Store class.
	std::vector<std::unique_ptr<StoreEntry>> &entries: Reference to the StoreEntries.
	int &currentMode: Const Reference to the current Mode.
	int &lastMode: Reference to the last mode.
	bool &fetch: Reference to fetch.
	int &smallDelay: Reference to the small delay.
*/
void StoreUtils::ListLogic(std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, int &currentMode, int &lastMode, bool &fetch, int &smallDelay) {
	if (store) { // Ensure, store is not a nullptr.
		if (hRepeat & KEY_DOWN) {
			if (store->GetEntry() < (int)entries.size() - 1) store->SetEntry(store->GetEntry() + 1);
			else store->SetEntry(0);
		}

		if (hRepeat & KEY_RIGHT) {
			if (store->GetEntry() < (int)entries.size() - 3) store->SetEntry(store->GetEntry() + 3);
			else store->SetEntry(entries.size() - 1);
		}

		if (hRepeat & KEY_LEFT) {
			if (store->GetEntry() - 2 > 0) store->SetEntry(store->GetEntry() - 3);
			else store->SetEntry(0);
		}

		if (hRepeat & KEY_UP) {
			if (store->GetEntry() > 0) store->SetEntry(store->GetEntry() - 1);
			else store->SetEntry(entries.size() - 1);
		}

		if (hDown & KEY_A) {
			fetch = true;
			smallDelay = 5;
			lastMode = currentMode;
			currentMode = 1;
		}

		/* Scroll Logic. */
		if (store->GetEntry() < store->GetScreenIndx()) store->SetScreenIndx(store->GetEntry());
		else if (store->GetEntry() > store->GetScreenIndx() + 3 - 1) store->SetScreenIndx(store->GetEntry() - 3 + 1);
	}
}