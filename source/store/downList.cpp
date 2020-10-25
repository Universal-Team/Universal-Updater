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

#define DOWNLOAD_ENTRIES  8

static const std::vector<Structs::ButtonPos> downloadBoxes = {
	{54, 6, 262, 22},
	{54, 36, 262, 22},
	{54, 66, 262, 22},
	{54, 96, 262, 22},
	{54, 126, 262, 22},
	{54, 156, 262, 22},
	{54, 186, 262, 22},
	{54, 216, 262, 22}
};

/*
	Draw the Download Entries part.
*/
void StoreUtils::DrawDownList(const std::unique_ptr<Store> &store, const std::vector<std::string> &entries) {
	if (store) {
		if (entries.size() > 0) {
			for (int i = 0; i < 8 && i < (int)entries.size(); i++) {

				if (store->GetDownloadBtn() == i) {
					GFX::drawBox(downloadBoxes[i].x, downloadBoxes[i].y, downloadBoxes[i].w, downloadBoxes[i].h, true);

				} else {
					GFX::drawBox(downloadBoxes[i].x, downloadBoxes[i].y, downloadBoxes[i].w, downloadBoxes[i].h, false);
				}

				Gui::DrawStringCentered(27, downloadBoxes[i].y + 4, 0.45f, C2D_Color32(255, 255, 255, 255), entries[(i + store->GetDownloadSIndex())], 255);
			}

		} else {
			Gui::DrawStringCentered(0, downloadBoxes[0].y + 4, 0.5f, C2D_Color32(255, 255, 255, 255), Lang::get("NO_DOWNLOADS_AVAILABLE"), 280);
		}
	}
}

void StoreUtils::DownloadHandle(u32 hDown, u32 hHeld, touchPosition touch, const std::unique_ptr<Store> &store, const std::vector<std::string> &entries) {
	if (store) { // Ensure, store is not a nullptr.
		u32 hRepeat = hidKeysDownRepeat();
		bool needUpdate = false;

		if (hRepeat & KEY_DOWN) {
			if (store->GetDownloadBtn() > 6) {
				if (store->GetDownloadIndex() < (int)entries.size() - 1) {
					store->SetDownloadIndex(store->GetDownloadIndex() + 1);
					needUpdate = true;
				}

			} else {
				if (store->GetDownloadIndex() < (int)entries.size() - 1) {
					store->SetDownloadBtn(store->GetDownloadBtn() + 1);
					store->SetDownloadIndex(store->GetDownloadIndex() + 1);
				}
			}
		}

		if (hRepeat & KEY_UP) {
			if (store->GetDownloadBtn() < 1) {
				if (store->GetDownloadIndex() > 0) {
					store->SetDownloadIndex(store->GetDownloadIndex() - 1);
					needUpdate = true;
				}

			} else {
				store->SetDownloadBtn(store->GetDownloadBtn() - 1);
				store->SetDownloadIndex(store->GetDownloadIndex() - 1);
			}
		}

		if (hDown & KEY_A) {
			/* TODO: Execution handle. */
		}

		if (needUpdate) {
			needUpdate = false;

			/* Scroll Logic. */
			if (store->GetDownloadBtn() > 6) {
				if (store->GetDownloadIndex() < (int)entries.size()) {
					store->SetDownloadSIndex(store->GetDownloadSIndex() + 1);
				}

			} else if (store->GetDownloadBtn() < 1) {
				if (store->GetDownloadSIndex() > 0) {
					store->SetDownloadSIndex(store->GetDownloadSIndex() - 1);
				}
			}
		}
	}
}