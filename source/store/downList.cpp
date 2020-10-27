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

#include "scriptUtils.hpp"
#include "storeUtils.hpp"
#include "structs.hpp"

#define DOWNLOAD_ENTRIES 8
extern bool touching(touchPosition touch, Structs::ButtonPos button);
static const std::vector<Structs::ButtonPos> downloadBoxes = {
	{ 54, 4, 262, 22 },
	{ 54, 34, 262, 22 },
	{ 54, 64, 262, 22 },
	{ 54, 94, 262, 22 },
	{ 54, 124, 262, 22 },
	{ 54, 154, 262, 22 },
	{ 54, 184, 262, 22 },
	{ 54, 214, 262, 22 }
};

/*
	Draw the Download Entries part.
*/
void StoreUtils::DrawDownList(const std::unique_ptr<Store> &store, const std::vector<std::string> &entries) {
	if (store) {
		if (entries.size() > 0) {
			for (int i = 0; i < DOWNLOAD_ENTRIES && i < (int)entries.size(); i++) {

				if (store->GetDownloadBtn() == i) {
					GFX::drawBox(downloadBoxes[i].x, downloadBoxes[i].y, downloadBoxes[i].w, downloadBoxes[i].h, true);

				} else {
					GFX::drawBox(downloadBoxes[i].x, downloadBoxes[i].y, downloadBoxes[i].w, downloadBoxes[i].h, false);
				}

				Gui::DrawStringCentered(54 - 160 + (262 / 2), downloadBoxes[i].y + 4, 0.45f, TEXT_COLOR, entries[(i + store->GetDownloadSIndex())], 260);
			}

		} else {
			Gui::DrawStringCentered(25, downloadBoxes[0].y + 4, 0.5f, TEXT_COLOR, Lang::get("NO_DOWNLOADS_AVAILABLE"), 260);
		}
	}
}

void StoreUtils::DownloadHandle(u32 hDown, u32 hHeld, touchPosition touch, const std::unique_ptr<Store> &store, const std::unique_ptr<StoreEntry> &entry, const std::vector<std::string> &entries, int &currentMenu, std::unique_ptr<Meta> &meta) {
	bool needUpdate = false;

	if (store && entry) { // Ensure, store & entry is not a nullptr.
		u32 hRepeat = hidKeysDownRepeat();

		if (hRepeat & KEY_DOWN) {
			if (entries.size() <= 0) return;

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
			if (entries.size() <= 0) return;

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

		if (hDown & KEY_TOUCH) {
			if (entries.size() <= 0) return;

			for (int i = 0; i < 8; i++) {
				if (touching(touch, downloadBoxes[i])) {
					if (i + store->GetDownloadSIndex() < (int)entries.size()) {
						const std::string tmp = Lang::get("EXECUTE_ENTRY") + "\n\n" + entries[i + store->GetDownloadSIndex()];

						if (Msg::promptMsg(tmp)) {
							ScriptUtils::runFunctions(store->GetJson(), entry->GetEntryIndex(), entries[i + store->GetDownloadSIndex()]);
							if (meta) meta->SetUpdated(store->GetUniStoreTitle(), entry->GetTitle(), entry->GetLastUpdated());
							entry->SetUpdateAvl(false);
						}
					}
				}
			}
		}

		if (hDown & KEY_A) {
			if (entries.size() <= 0) return;

			const std::string tmp = Lang::get("EXECUTE_ENTRY") + "\n\n" + entries[store->GetDownloadIndex()];
			if (Msg::promptMsg(tmp)) {
				ScriptUtils::runFunctions(store->GetJson(), entry->GetEntryIndex(), entries[store->GetDownloadIndex()]);
				if (meta) meta->SetUpdated(store->GetUniStoreTitle(), entry->GetTitle(), entry->GetLastUpdated());
				entry->SetUpdateAvl(false);
			}
		}

		if (hDown & KEY_B) {
			currentMenu = 0;
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