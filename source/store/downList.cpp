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

#include "keyboard.hpp"
#include "scriptUtils.hpp"
#include "storeUtils.hpp"
#include "structs.hpp"
#include <fstream>

#define DOWNLOAD_ENTRIES 7
extern std::string _3dsxPath;
extern bool is3DSX;
extern bool touching(touchPosition touch, Structs::ButtonPos button);
static const std::vector<Structs::ButtonPos> downloadBoxes = {
	{ 54, 32, 262, 22 },
	{ 54, 62, 262, 22 },
	{ 54, 92, 262, 22 },
	{ 54, 122, 262, 22 },
	{ 54, 152, 262, 22 },
	{ 54, 182, 262, 22 },
	{ 54, 212, 262, 22 },

	{ 50, 216, 24, 24 }
};

/*
	With this, we can create a shortcut. ;P

	const std::string &entryName: The name of the Entry. AKA: The Title Name.
	int index: The Download index.
	const std::string &unistoreName: The name of the UniStore filename.
	const std::string &author: The author of the app.
*/
static void CreateShortcut(const std::string &entryName, int index, const std::string &unistoreName, const std::string &author) {
	std::string sName = Input::setkbdString(30, Lang::get("ENTER_SHORTCUT_FILENAME"), {});
	if (sName == "") sName = "tmp";
	std::ofstream out(config->shortcut() + "/" + sName + ".xml", std::ios::binary);

	out << "<shortcut>" << std::endl;

	/* Executable. */
	const std::string executable = _3dsxPath.substr(5, _3dsxPath.size()); // It must be '/3ds/...'.
	out << "	<executable>" << executable << "</executable>" << std::endl;

	/* Arguments. */
	out << "	<arg>\"" << unistoreName << "\" \"" << entryName << "\" \"" << std::to_string(index) << "\"" << "</arg>" << std::endl;

	/* Title. */
	const std::string title = Input::setkbdString(30, Lang::get("ENTER_TITLE_SHORTCUT"), {});
	if (title != "") out << "	<name>" << title << "</name>" << std::endl;
	else out << "	<name>" << entryName << "</name>" << std::endl;

	/* Description. */
	const std::string desc = Input::setkbdString(50, Lang::get("ENTER_DESC_SHORTCUT"), {});
	if (desc != "") out << "	<description>" << desc << "</description>" << std::endl;
	else out << "	<description>" << entryName << "</description>" << std::endl;

	/* Author and end. */
	out << "	<author>" << author << "</author>" << std::endl;
	out << "</shortcut>" << std::endl;
	out.close();
}


/*
	Draw the Download Entries part.

	const std::unique_ptr<Store> &store: Const Reference to the Store class.
	const std::vector<std::string> &entries: Const Reference to the download list as a vector of strings.
	bool fetch: if fetching or not.
	const std::unique_ptr<StoreEntry> &entry: Const Reference to the StoreEntry.
	const std::vector<std::string> &sizes: Const Reference to the download sizes as a vector of strings.
*/
void StoreUtils::DrawDownList(const std::unique_ptr<Store> &store, const std::vector<std::string> &entries, bool fetch, const std::unique_ptr<StoreEntry> &entry, const std::vector<std::string> &sizes) {
	/* For the Top Screen. */
	if (store && store->GetValid() && !fetch && entry) {
		if (entries.size() > 0) {
			Gui::Draw_Rect(0, 174, 400, 66, BOX_INSIDE_COLOR);
			const C2D_Image tempImg = entry->GetIcon();
			const uint8_t offsetW = (48 - tempImg.subtex->width) / 2; // Center W.
			const uint8_t offsetH = (48 - tempImg.subtex->height) / 2; // Center H.

			C2D_DrawImageAt(tempImg, 9 + offsetW, 174 + 9 + offsetH, 0.5);

			Gui::DrawString(70, 174 + 15, 0.45f, TEXT_COLOR, entries[store->GetDownloadIndex()], 310, 0, font);

			if (!sizes.empty()) {
				if (sizes[store->GetDownloadIndex()] != "") {
					Gui::DrawString(70, 174 + 30, 0.45f, TEXT_COLOR, Lang::get("SIZE") + ": " +  sizes[store->GetDownloadIndex()], 310, 0, font);
				}
			}
		}
	}

	GFX::DrawBottom();
	Gui::Draw_Rect(48, 0, 272, 25, ENTRY_BAR_COLOR);
	Gui::Draw_Rect(48, 25, 272, 1, ENTRY_BAR_OUTL_COLOR);
	Gui::DrawStringCentered(25, 2, 0.6, TEXT_COLOR, Lang::get("AVAILABLE_DOWNLOADS"), 265, 0, font);

	if (store && store->GetValid() && !fetch && entry) {
		if (entries.size() > 0) {
			for (int i = 0; i < DOWNLOAD_ENTRIES && i < (int)entries.size(); i++) {
				if (store->GetDownloadIndex() == i + store->GetDownloadSIndex()) GFX::DrawBox(downloadBoxes[i].x, downloadBoxes[i].y, downloadBoxes[i].w, downloadBoxes[i].h, false);
				Gui::DrawStringCentered(54 - 160 + (262 / 2), downloadBoxes[i].y + 4, 0.45f, TEXT_COLOR, entries[(i + store->GetDownloadSIndex())], 260, 0, font);
			}

			if (is3DSX) GFX::DrawSprite(sprites_shortcut_idx, downloadBoxes[6].x, downloadBoxes[6].y);

		} else { // If no downloads available..
			Gui::DrawStringCentered(54 - 160 + (262 / 2), downloadBoxes[0].y + 4, 0.5f, TEXT_COLOR, Lang::get("NO_DOWNLOADS_AVAILABLE"), 255, 0, font);
		}
	}
}

/*
	This is the Download List handle.
	Here you can..

	- Scroll through the download list, if any available.
	- Execute an Entry of the download list.
	- Return back to EntryInfo through `B`.

	const std::unique_ptr<Store> &store: Const Reference to the Store class, since we do not modify anything in it.
	const std::unique_ptr<StoreEntry> &entry: Const Reference to the current StoreEntry, since we do not modify anything in it.
	const std::vector<std::string> &entries: Const Reference to the download list, since we do not modify anything in it.
	int &currentMenu: Reference to the StoreMode / Menu, so we can switch back to EntryInfo with `B`.
	std::unique_ptr<Meta> &meta: Reference to the Meta, to apply the updates stuff.
	const int &lastMode: Const Reference to the last mode.
	int &smallDelay: Reference to the small delay. This helps to not directly press A.
*/
void StoreUtils::DownloadHandle(const std::unique_ptr<Store> &store, const std::unique_ptr<StoreEntry> &entry, const std::vector<std::string> &entries, int &currentMenu, std::unique_ptr<Meta> &meta, const int &lastMode, int &smallDelay) {
	if (store && entry) { // Ensure, store & entry is not a nullptr.
		if (smallDelay > 0) {
			smallDelay--;
		}

		if ((hDown & KEY_Y) || (hDown & KEY_START) || (hDown & KEY_TOUCH && touching(touch, downloadBoxes[6]))) {
			if (is3DSX) { // Only allow if 3DSX.
				if (entries.size() <= 0) return; // Smaller than 0 -> No No.

				if (Msg::promptMsg(Lang::get("CREATE_SHORTCUT"))) {
					CreateShortcut(entry->GetTitle(), store->GetDownloadIndex(), store->GetFileName(), entry->GetAuthor());
					Msg::waitMsg(Lang::get("SHORTCUT_CREATED"));
				}
			}
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
						if (Msg::promptMsg(Lang::get("EXECUTE_ENTRY") + "\n\n" + entries[i + store->GetDownloadSIndex()])) {
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

			if (Msg::promptMsg(Lang::get("EXECUTE_ENTRY") + "\n\n" + entries[store->GetDownloadIndex()])) {
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