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

static const std::vector<Structs::ButtonPos> GridBoxes = {
	{25, 45, 50, 50},
	{100, 45, 50, 50},
	{175, 45, 50, 50},
	{250, 45, 50, 50},
	{325, 45, 50, 50},

	{25, 105, 50, 50},
	{100, 105, 50, 50},
	{175, 105, 50, 50},
	{250, 105, 50, 50},
	{325, 105, 50, 50},

	{25, 165, 50, 50},
	{100, 165, 50, 50},
	{175, 165, 50, 50},
	{250, 165, 50, 50},
	{325, 165, 50, 50}
};

/*
	Draw the Top Grid.

	const std::unique_ptr<Store> &store: Const Reference to the Store class.
	const std::vector<std::unique_ptr<StoreEntry>> &entries: Const Reference to the StoreEntries.
*/
void StoreUtils::DrawGrid(const std::unique_ptr<Store> &store, const std::vector<std::unique_ptr<StoreEntry>> &entries) {
	if (store) { // Ensure, store is not a nullptr.
		for (int i = 0, i2 = 0 + (store->GetScreenIndx() * 5); i2 < 15 + (store->GetScreenIndx() * 5) && i2 < (int)entries.size(); i2++, i++) {

			/* Boxes. */
			if (i == store->GetBox()) {
				GFX::drawBox(GridBoxes[i].x, GridBoxes[i].y, 50, 50, true);

			} else {
				GFX::drawBox(GridBoxes[i].x, GridBoxes[i].y, 50, 50, false);
			}

			/* Ensure, entries is larger than the index. */
			if ((int)entries.size() > i2) {
				if (entries[i2]) { // Ensure, the Entry is not nullptr.
					const C2D_Image tempImg = entries[i2]->GetIcon();
					const uint8_t offsetW = (48 - tempImg.subtex->width) / 2; // Center W.
					const uint8_t offsetH = (48 - tempImg.subtex->height) / 2; // Center H.

					C2D_DrawImageAt(tempImg, GridBoxes[i].x + 1 + offsetW, GridBoxes[i].y + 1 + offsetH, 0.5);

					/* Update Available mark. */
					if (entries[i2]->GetUpdateAvl()) GFX::DrawSprite(sprites_updateStore_idx, GridBoxes[i].x + 30, GridBoxes[i].y + 30);
				}
			}
		}
	}
}


/*
	Top Grid Logic Handle.
	Here you can..

	- Scroll through the Grid with the D-Pad.

	u32 hDown: The hidKeysDown() variable.
	u32 hHeld: The hidKeysHeld() variable.
	touchPosition touch: The TouchPosition variable.
	std::unique_ptr<Store> &store: Reference to the Store class.
	std::vector<std::unique_ptr<StoreEntry>> &entries: Reference to the StoreEntries.
*/
void StoreUtils::GridLogic(u32 hDown, u32 hHeld, touchPosition touch, std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries) {
	if (store) { // Ensure, store is not a nullptr.
		u32 hRepeat = hidKeysDownRepeat();
		bool needUpdate = false;

		if (hRepeat & KEY_DOWN) {
			if (store->GetBox() > 9) {
				if (store->GetEntry() + 5 < (int)entries.size()) {
					store->SetEntry(store->GetEntry() + 5);
					needUpdate = true;
				}

			} else {
				if (store->GetEntry() + 5 < (int)entries.size()) {
					store->SetBox(store->GetBox() + 5);
					store->SetEntry(store->GetEntry() + 5);
				}
			}
		}

		if (hRepeat & KEY_RIGHT) {
			if (store->GetEntry() < (int)entries.size() - 1) {
				if (store->GetBox() < 14) {
					store->SetBox(store->GetBox() + 1);
					store->SetEntry(store->GetEntry() + 1);
				}
			}
		}

		if (hRepeat & KEY_LEFT) {
			if (store->GetEntry() > 0) {
				if (store->GetBox() > 0) {
					store->SetBox(store->GetBox() - 1);
					store->SetEntry(store->GetEntry() - 1);
				}
			}
		}

		if (hRepeat & KEY_UP) {
			if (store->GetBox() < 5) {
				if (store->GetEntry() > 4) {
					store->SetEntry(store->GetEntry() - 5);
					needUpdate = true;
				}

			} else {
				store->SetBox(store->GetBox() - 5);
				store->SetEntry(store->GetEntry() - 5);
			}
		}

		if (needUpdate) {
			needUpdate = false;

			/* Scroll Logic. */
			if (store->GetBox() > 9) {
				if (store->GetEntry() < (int)entries.size()) {
					store->SetScreenIndx(store->GetScreenIndx() + 1);
				}

			} else if (store->GetBox() < 5) {
				if (store->GetScreenIndx() > 0) {
					store->SetScreenIndx(store->GetScreenIndx() - 1);
				}
			}
		}
	}
}