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
#include "keyboard.hpp"
#include "queueSystem.hpp"
#include "scriptUtils.hpp"
#include "storeUtils.hpp"
#include "structs.hpp"
#include <fstream>

#define DOWNLOAD_ENTRIES 7
extern std::string _3dsxPath;
extern bool exiting, is3DSX, QueueRuns;
extern bool touching(touchPosition touch, Structs::ButtonPos button);
static const std::vector<Structs::ButtonPos> downloadBoxes = {
	{ 46, 32, 241, 22 },
	{ 46, 62, 241, 22 },
	{ 46, 92, 241, 22 },
	{ 46, 122, 241, 22 },
	{ 46, 152, 241, 22 },
	{ 46, 182, 241, 22 },
	{ 46, 212, 241, 22 },

	{ 42, 216, 24, 24 }
};

static const std::vector<Structs::ButtonPos> installedPos = {
	{ 288, 32, 24, 24 },
	{ 288, 62, 24, 24 },
	{ 288, 92, 24, 24 },
	{ 288, 122, 24, 24 },
	{ 288, 152, 24, 24 },
	{ 288, 182, 24, 24 },
	{ 288, 212, 24, 24 },
};

/*
	With this, we can create a shortcut. ;P

	const std::string &entryName: The name of the Entry. AKA: The Title Name.
	int index: The Download index.
	const std::string &unistoreName: The name of the UniStore filename.
	const std::string &author: The author of the app.
*/
static bool CreateShortcut(const std::string &entryName, int index, const std::string &unistoreName, const std::string &author) {
	std::string sName = Input::setkbdString(30, Lang::get("ENTER_SHORTCUT_FILENAME"), {});
	if (sName == "") return false; // Just cancel.
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
	return true;
}


/*
	Draw the Download entries part.

	const std::vector<std::string> &entries: Const Reference to the download list as a vector of strings.
	bool fetch: if fetching or not.
	const std::unique_ptr<StoreEntry> &entry: Const Reference to the StoreEntry.
	const std::vector<std::string> &sizes: Const Reference to the download sizes as a vector of strings.
*/
void StoreUtils::DrawDownList(const std::vector<std::string> &entries, bool fetch, const std::unique_ptr<StoreEntry> &entry, const std::vector<std::string> &sizes, const std::vector<bool> &installs) {
	/* For the Top Screen. */
	if (StoreUtils::store && StoreUtils::store->GetValid() && !fetch && entry) {
		if (entries.size() > 0) {
			Gui::Draw_Rect(0, 174, 400, 66, UIThemes->DownListPrev());
			const C2D_Image tempImg = entry->GetIcon();
			const uint8_t offsetW = (48 - tempImg.subtex->width) / 2; // Center W.
			const uint8_t offsetH = (48 - tempImg.subtex->height) / 2; // Center H.
			C2D_DrawImageAt(tempImg, 9 + offsetW, 174 + 9 + offsetH, 0.5);

			Gui::DrawString(70, 174 + 15, 0.45f, UIThemes->TextColor(), entries[StoreUtils::store->GetDownloadIndex()], 310, 0, font);

			if (!sizes.empty()) {
				if (sizes[StoreUtils::store->GetDownloadIndex()] != "") {
					Gui::DrawString(70, 174 + 30, 0.45f, UIThemes->TextColor(), Lang::get("SIZE") + ": " +  sizes[StoreUtils::store->GetDownloadIndex()], 310, 0, font);
				}
			}
		}
	}

	GFX::DrawTime();
	GFX::DrawBattery();
	GFX::DrawWifi();
	Animation::QueueEntryDone();

	GFX::DrawBottom();
	Gui::Draw_Rect(40, 0, 280, 25, UIThemes->EntryBar());
	Gui::Draw_Rect(40, 25, 280, 1, UIThemes->EntryOutline());
	Gui::DrawStringCentered(17, 2, 0.6, UIThemes->TextColor(), Lang::get("AVAILABLE_DOWNLOADS"), 273, 0, font);

	if (StoreUtils::store && StoreUtils::store->GetValid() && !fetch && entry) {
		if (entries.size() > 0) {
			for (int i = 0; i < DOWNLOAD_ENTRIES && i < (int)entries.size(); i++) {
				if (StoreUtils::store->GetDownloadIndex() == i + StoreUtils::store->GetDownloadSIndex()) Gui::Draw_Rect(downloadBoxes[i].x, downloadBoxes[i].y, downloadBoxes[i].w, downloadBoxes[i].h, UIThemes->MarkSelected());
				Gui::DrawStringCentered(46 - 160 + (241 / 2), downloadBoxes[i].y + 4, 0.45f, UIThemes->TextColor(), entries[(i + StoreUtils::store->GetDownloadSIndex())], 235, 0, font);

				if (installs[(i + StoreUtils::store->GetDownloadSIndex())]) GFX::DrawIcon(sprites_installed_idx, installedPos[i].x, installedPos[i].y, UIThemes->TextColor());
			}

			if (is3DSX) GFX::DrawIcon(sprites_shortcut_idx, downloadBoxes[6].x, downloadBoxes[6].y, UIThemes->TextColor());


		} else { // If no downloads available..
			Gui::DrawStringCentered(46 - 160 + (241 / 2), downloadBoxes[0].y + 4, 0.5f, UIThemes->TextColor(), Lang::get("NO_DOWNLOADS_AVAILABLE"), 235, 0, font);
		}
	}
}

/*
	This is the Download List handle.
	Here you can..

	- Scroll through the download list, if any available.
	- Execute an Entry of the download list.
	- Return back to EntryInfo through `B`.

	const std::unique_ptr<StoreEntry> &entry: Const Reference to the current StoreEntry, since we do not modify anything in it.
	const std::vector<std::string> &entries: Const Reference to the download list, since we do not modify anything in it.
	int &currentMenu: Reference to the StoreMode / Menu, so we can switch back to EntryInfo with `B`.
	const int &lastMode: Const Reference to the last mode.
	int &smallDelay: Reference to the small delay. This helps to not directly press A.
	std::vector<bool> &installs: Reference to the installed states.
*/
void StoreUtils::DownloadHandle(const std::unique_ptr<StoreEntry> &entry, const std::vector<std::string> &entries, int &currentMenu, const int &lastMode, int &smallDelay, std::vector<bool> &installs, const std::vector<std::string> &types) {
	if (StoreUtils::store && entry) { // Ensure, store & entry is not a nullptr.
		if (smallDelay > 0) {
			smallDelay--;
		}

		if ((hDown & (KEY_Y | KEY_SELECT) || (hDown & KEY_TOUCH && touching(touch, downloadBoxes[6]))) && !entries.empty()) {
			if (is3DSX) { // Only allow if 3DSX.
				if (StoreUtils::entries.size() <= 0) return; // Smaller than 0 -> No No.

				if (Msg::promptMsg(Lang::get("CREATE_SHORTCUT"))) {
					if (CreateShortcut(entry->GetTitle(), StoreUtils::store->GetDownloadIndex(), StoreUtils::store->GetFileName(), entry->GetAuthor())) {
						Msg::waitMsg(Lang::get("SHORTCUT_CREATED"));
					}
				}
			}
		}

		if (hRepeat & KEY_DOWN) {
			if (StoreUtils::store->GetDownloadIndex() < (int)entries.size() - 1) StoreUtils::store->SetDownloadIndex(StoreUtils::store->GetDownloadIndex() + 1);
			else StoreUtils::store->SetDownloadIndex(0);
		}

		if (hRepeat & KEY_UP) {
			if (StoreUtils::store->GetDownloadIndex() > 0) StoreUtils::store->SetDownloadIndex(StoreUtils::store->GetDownloadIndex() - 1);
			else StoreUtils::store->SetDownloadIndex(entries.size() - 1);
		}


		if (hRepeat & KEY_RIGHT) {
			if (StoreUtils::store->GetDownloadIndex() + DOWNLOAD_ENTRIES < (int)entries.size()-1) StoreUtils::store->SetDownloadIndex(StoreUtils::store->GetDownloadIndex() + DOWNLOAD_ENTRIES);
			else StoreUtils::store->SetDownloadIndex(entries.size()-1);
		}

		if (hRepeat & KEY_LEFT) {
			if (StoreUtils::store->GetDownloadIndex() - DOWNLOAD_ENTRIES > 0) StoreUtils::store->SetDownloadIndex(StoreUtils::store->GetDownloadIndex() - DOWNLOAD_ENTRIES);
			else StoreUtils::store->SetDownloadIndex(0);
		}

		if (smallDelay == 0 && hDown & KEY_TOUCH) {
			for (int i = 0; i < DOWNLOAD_ENTRIES; i++) {
				if (touching(touch, downloadBoxes[i])) {
					if (i + StoreUtils::store->GetDownloadSIndex() < (int)entries.size()) {
						std::string Msg = Lang::get("EXECUTE_ENTRY") + "\n\n" + entries[i + StoreUtils::store->GetDownloadSIndex()];
						if (types[i + StoreUtils::store->GetDownloadSIndex()] == "nightly") Msg += "\n\n" + Lang::get("NOTE_NIGHTLY");
						else if (types[i + StoreUtils::store->GetDownloadSIndex()] == "prerelease") Msg += "\n\n" + Lang::get("NOTE_PRERELEASE");
						if (Msg::promptMsg(Msg)) {
							StoreUtils::AddToQueue(entry->GetEntryIndex(), entries[i + StoreUtils::store->GetDownloadSIndex()], entry->GetTitle(), entry->GetLastUpdated());
						}
					}

					break;
				}

				if (touching(touch, installedPos[i])) {
					if (i + StoreUtils::store->GetDownloadSIndex() < (int)entries.size()) {
						if (installs[i + StoreUtils::store->GetDownloadSIndex()]) {
							StoreUtils::meta->RemoveInstalled(StoreUtils::store->GetUniStoreTitle(), entry->GetTitle(), entries[i + StoreUtils::store->GetDownloadSIndex()]);
							installs[i + StoreUtils::store->GetDownloadSIndex()] = false;
						}
					}

					break;
				}
			}
		}

		if (smallDelay == 0 && hDown & KEY_A && !entries.empty()) {
			std::string Msg = Lang::get("EXECUTE_ENTRY") + "\n\n" + entries[StoreUtils::store->GetDownloadIndex()];
			if (types[StoreUtils::store->GetDownloadIndex()] == "nightly") Msg += "\n\n" + Lang::get("NOTE_NIGHTLY");
			else if (types[StoreUtils::store->GetDownloadIndex()] == "prerelease") Msg += "\n\n" + Lang::get("NOTE_PRERELEASE");
			if (Msg::promptMsg(Msg)) {
				StoreUtils::AddToQueue(entry->GetEntryIndex(), entries[StoreUtils::store->GetDownloadIndex()], entry->GetTitle(), entry->GetLastUpdated());
			}
		}

		if (hDown & KEY_X && !entries.empty()) {
			if (installs[StoreUtils::store->GetDownloadIndex()]) {
				StoreUtils::meta->RemoveInstalled(StoreUtils::store->GetUniStoreTitle(), entry->GetTitle(), entries[StoreUtils::store->GetDownloadIndex()]);
				installs[StoreUtils::store->GetDownloadIndex()] = false;
			}
		}

		if (hDown & KEY_B) currentMenu = lastMode; // Go back to EntryInfo.

		/* Quit UU. */
		if (hDown & KEY_START && !QueueRuns)
			exiting = true;

		/* Scroll Handle. */
		if (StoreUtils::store->GetDownloadIndex() < StoreUtils::store->GetDownloadSIndex()) StoreUtils::store->SetDownloadSIndex(StoreUtils::store->GetDownloadIndex());
		else if (StoreUtils::store->GetDownloadIndex() > StoreUtils::store->GetDownloadSIndex() + DOWNLOAD_ENTRIES - 1) StoreUtils::store->SetDownloadSIndex(StoreUtils::store->GetDownloadIndex() - DOWNLOAD_ENTRIES + 1);
	}
}