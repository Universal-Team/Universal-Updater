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

#include "common.hpp"
#include "storeUtils.hpp"
#include "structs.hpp"

static const std::vector<Structs::ButtonPos> GridBoxes = {
	{ 25, 45, 50, 50 },
	{ 100, 45, 50, 50 },
	{ 175, 45, 50, 50 },
	{ 250, 45, 50, 50 },
	{ 325, 45, 50, 50 },

	{ 25, 105, 50, 50 },
	{ 100, 105, 50, 50 },
	{ 175, 105, 50, 50 },
	{ 250, 105, 50, 50 },
	{ 325, 105, 50, 50 },

	{ 25, 165, 50, 50 },
	{ 100, 165, 50, 50 },
	{ 175, 165, 50, 50 },
	{ 250, 165, 50, 50 },
	{ 325, 165, 50, 50 }
};

/* Draw the Top Grid. */
void StoreUtils::DrawGrid() {
	if (StoreUtils::store) { // Ensure, store is not a nullptr.

		if (config->usebg() && StoreUtils::store->customBG()) {
			C2D_DrawImageAt(StoreUtils::store->GetStoreImg(), 0, 26, 0.5f, nullptr);

		} else {
			Gui::Draw_Rect(0, 26, 400, 214, UIThemes->BGColor());
		}

		for (int i = 0, i2 = 0 + (StoreUtils::store->GetScreenIndx() * 5); i2 < 15 + (StoreUtils::store->GetScreenIndx() * 5) && i2 < (int)StoreUtils::entries.size(); i2++, i++) {
			/* Boxes. */
			if (i == StoreUtils::store->GetBox()) GFX::DrawBox(GridBoxes[i].x, GridBoxes[i].y, 50, 50, true);

			/* Ensure, entries is larger than the index. */
			if ((int)StoreUtils::entries.size() > i2) {
				if (StoreUtils::entries[i2]) { // Ensure, the Entry is not nullptr.
					const C2D_Image tempImg = StoreUtils::entries[i2]->GetIcon();
					const uint8_t offsetW = (48 - tempImg.subtex->width) / 2; // Center W.
					const uint8_t offsetH = (48 - tempImg.subtex->height) / 2; // Center H.

					C2D_DrawImageAt(tempImg, GridBoxes[i].x + 1 + offsetW, GridBoxes[i].y + 1 + offsetH, 0.5);

					/* Update Available mark. */
					if (StoreUtils::entries[i2]->GetUpdateAvl()) GFX::DrawSprite(sprites_update_app_idx, GridBoxes[i].x + 32, GridBoxes[i].y + 32);
				}
			}
		}
	}
}


/*
	Top Grid Logic Handle.
	Here you can..

	- Scroll through the Grid with the D-Pad.

	const int &currentMode: Reference to the current Mode.
	int &lastMode: Reference to the last mode.
	bool &fetch: Reference to fetch.
	int &smallDelay: Reference to the small delay.
*/
void StoreUtils::GridLogic(int &currentMode, int &lastMode, bool &fetch, int &smallDelay) {
	if (StoreUtils::store) { // Ensure, store is not a nullptr.
		if (hRepeat & KEY_DOWN) {
			if (StoreUtils::store->GetBox() > 9) {
				if (StoreUtils::store->GetEntry() + 5 < (int)StoreUtils::entries.size() - 1) {
					StoreUtils::store->SetEntry(StoreUtils::store->GetEntry() + 5);

					if (StoreUtils::entries.size() > 15) StoreUtils::store->SetScreenIndx((StoreUtils::store->GetEntry() / 5) - 2);

				} else {
					if (StoreUtils::store->GetEntry() < (int)StoreUtils::entries.size() - 1) {
						StoreUtils::store->SetEntry(StoreUtils::entries.size() - 1);
						StoreUtils::store->SetBox(10 + (StoreUtils::store->GetEntry() % 5));

						if (StoreUtils::entries.size() > 15) StoreUtils::store->SetScreenIndx((StoreUtils::store->GetEntry() / 5) - 2);
					}
				}

			} else {
				if (StoreUtils::store->GetEntry() + 5 < (int)StoreUtils::entries.size()) {
					StoreUtils::store->SetBox(StoreUtils::store->GetBox() + 5);
					StoreUtils::store->SetEntry(StoreUtils::store->GetEntry() + 5);
				}
			}
		}

		if (hRepeat & KEY_RIGHT) {
			if (StoreUtils::store->GetEntry() < (int)StoreUtils::entries.size() - 1) {
				if (StoreUtils::store->GetBox() < 14) {
					StoreUtils::store->SetBox(StoreUtils::store->GetBox() + 1);
					StoreUtils::store->SetEntry(StoreUtils::store->GetEntry() + 1);

				} else {
					StoreUtils::store->SetBox(10);
					StoreUtils::store->SetEntry(StoreUtils::store->GetEntry() + 1);

					StoreUtils::store->SetScreenIndx((StoreUtils::store->GetEntry() / 5) - 2);
				}
			}
		}

		if (hRepeat & KEY_LEFT) {
			if (StoreUtils::store->GetEntry() > 0) {
				if (StoreUtils::store->GetBox() > 0) {
					StoreUtils::store->SetBox(StoreUtils::store->GetBox() - 1);
					StoreUtils::store->SetEntry(StoreUtils::store->GetEntry() - 1);

				} else {
					StoreUtils::store->SetBox(4);
					StoreUtils::store->SetEntry(StoreUtils::store->GetEntry() - 1);

					StoreUtils::store->SetScreenIndx((StoreUtils::store->GetEntry() / 5));
				}
			}
		}

		if (hRepeat & KEY_UP) {
			if (StoreUtils::store->GetBox() < 5) {
				if (StoreUtils::store->GetEntry() > 4) {
					StoreUtils::store->SetEntry(StoreUtils::store->GetEntry() - 5);

					StoreUtils::store->SetScreenIndx((StoreUtils::store->GetEntry() / 5));
				}

			} else {
				StoreUtils::store->SetBox(StoreUtils::store->GetBox() - 5);
				StoreUtils::store->SetEntry(StoreUtils::store->GetEntry() - 5);
			}
		}

		if (hDown & KEY_A) {
			fetch = true;
			smallDelay = 5;
			lastMode = currentMode;
			currentMode = 1;
		}
	}
}