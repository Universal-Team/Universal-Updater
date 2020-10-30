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

	const std::unique_ptr<Store> &store: Const Reference to the Store class.
	const std::vector<std::string> &entries: Const Reference to the download list as a vector of strings.
	const bool &fetch: Const Reference to Fetch.
*/
void StoreUtils::DrawDownList(const std::unique_ptr<Store> &store, const std::vector<std::string> &entries, const bool &fetch) {
	if (store && !fetch) {
		if (entries.size() > 0) {
			for (int i = 0; i < DOWNLOAD_ENTRIES && i < (int)entries.size(); i++) {
				GFX::drawBox(downloadBoxes[i].x, downloadBoxes[i].y, downloadBoxes[i].w, downloadBoxes[i].h, store->GetDownloadIndex() == i + store->GetDownloadSIndex());
				Gui::DrawStringCentered(54 - 160 + (262 / 2), downloadBoxes[i].y + 4, 0.45f, TEXT_COLOR, entries[(i + store->GetDownloadSIndex())], 260);
			}

		} else { // If no downloads available..
			Gui::DrawStringCentered(25, downloadBoxes[0].y + 4, 0.5f, TEXT_COLOR, Lang::get("NO_DOWNLOADS_AVAILABLE"), 260);
		}
	}
}

/*
	This is the Download List handle.
	Here you can..

	- Scroll through the download list, if any available.
	- Execute an Entry of the download list.
	- Return back to EntryInfo through `B`.

	u32 hDown: The hidKeysDown() variable.
	u32 hHeld: The hidKeysHeld() variable.
	touchPosition touch: The TouchPosition variable.
	const std::unique_ptr<Store> &store: Const Reference to the Store class, since we do not modify anything in it.
	const std::unique_ptr<StoreEntry> &entry: Const Reference to the current StoreEntry, since we do not modify anything in it.
	const std::vector<std::string> &entries: Const Reference to the download list, since we do not modify anything in it.
	int &currentMenu: Reference to the StoreMode / Menu, so we can switch back to EntryInfo with `B`.
	std::unique_ptr<Meta> &meta: Reference to the Meta, to apply the updates stuff.
	const int &lastMode: Const Reference to the last mode.
	int &smallDelay: Reference to the small delay. This helps to not directly press A.
*/
void StoreUtils::DownloadHandle(u32 hDown, u32 hHeld, touchPosition touch, const std::unique_ptr<Store> &store, const std::unique_ptr<StoreEntry> &entry, const std::vector<std::string> &entries, int &currentMenu, std::unique_ptr<Meta> &meta, const int &lastMode, int &smallDelay) {
	if (store && entry) { // Ensure, store & entry is not a nullptr.
		if (smallDelay > 0) {
			smallDelay--;
		}

		if (hRepeat & KEY_DOWN) {
			if (entries.size() <= 0) return; // Smaller *than* 0 -> Invalid.

			if (store->GetDownloadIndex() < (int)entries.size() - 1) store->SetDownloadIndex(store->GetDownloadIndex() + 1);
			else store->SetDownloadIndex(0);
		}

		if (hRepeat & KEY_UP) {
			if (entries.size() <= 0) return; // Smaller *than* 0 -> Invalid.

			if (store->GetDownloadIndex() > 0) store->SetDownloadIndex(store->GetDownloadIndex() - 1);
			else store->SetDownloadIndex(entries.size() - 1);
		}


		if (hRepeat & KEY_RIGHT) {
			if (entries.size() <= 0) return; // Smaller *than* 0 -> Invalid.

			if (store->GetDownloadIndex() + DOWNLOAD_ENTRIES < (int)entries.size()-1) store->SetDownloadIndex(store->GetDownloadIndex() + DOWNLOAD_ENTRIES);
			else store->SetDownloadIndex(entries.size()-1);
		}

		if (hRepeat & KEY_LEFT) {
			if (entries.size() <= 0) return; // Smaller *than* 0 -> Invalid.

			if (store->GetDownloadIndex() - DOWNLOAD_ENTRIES > 0) store->SetDownloadIndex(store->GetDownloadIndex() - DOWNLOAD_ENTRIES);
			else store->SetDownloadIndex(0);
		}

		if (smallDelay == 0 && hDown & KEY_TOUCH) {
			if (entries.size() <= 0) return; // Smaller *than* 0 -> Invalid.

			for (int i = 0; i < DOWNLOAD_ENTRIES; i++) {
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

		if (smallDelay == 0 && hDown & KEY_A) {
			if (entries.size() <= 0) return; // Smaller *than* 0 -> Invalid.

			const std::string tmp = Lang::get("EXECUTE_ENTRY") + "\n\n" + entries[store->GetDownloadIndex()];
			if (Msg::promptMsg(tmp)) {
				ScriptUtils::runFunctions(store->GetJson(), entry->GetEntryIndex(), entries[store->GetDownloadIndex()]);
				if (meta) meta->SetUpdated(store->GetUniStoreTitle(), entry->GetTitle(), entry->GetLastUpdated());
				entry->SetUpdateAvl(false);
			}
		}

		if (hDown & KEY_B) currentMenu = lastMode; // Go back to EntryInfo.

		/* Scroll Handle. */
		if (store->GetDownloadIndex() < store->GetDownloadSIndex()) store->SetDownloadSIndex(store->GetDownloadIndex());
		else if (store->GetDownloadIndex() > store->GetDownloadSIndex() + DOWNLOAD_ENTRIES - 1) store->SetDownloadSIndex(store->GetDownloadIndex() - DOWNLOAD_ENTRIES + 1);
	}
}