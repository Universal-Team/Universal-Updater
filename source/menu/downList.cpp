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
extern int fadeAlpha;
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
	{ 292, 32, 24, 24 },
	{ 292, 62, 24, 24 },
	{ 292, 92, 24, 24 },
	{ 292, 122, 24, 24 },
	{ 292, 152, 24, 24 },
	{ 292, 182, 24, 24 },
	{ 292, 212, 24, 24 },
};

static const Structs::ButtonPos clearUpdatePos = { 296, 4, 20, 20 };

/*
	With this, we can create a shortcut. ;P

	const std::string &entryName: The name of the Entry. AKA: The Title Name.
	int index: The Download index.
	const std::string &unistoreName: The name of the UniStore filename.
	const std::string &author: The author of the app.
*/
static bool CreateShortcut(const std::string &entryName, int index, const std::string &unistoreName, const std::string &author) {
	std::string sName;
	if (!Input::getTextKeyboard(sName, 30, Lang::get("ENTER_SHORTCUT_FILENAME"))) return false; // Just cancel.
	std::ofstream out(config->shortcut() + "/" + sName + ".xml", std::ios::binary);

	out << "<shortcut>" << std::endl;

	/* Executable. */
	const std::string executable = _3dsxPath.substr(5, _3dsxPath.size()); // It must be '/3ds/...'.
	out << "	<executable>" << executable << "</executable>" << std::endl;

	/* Arguments. */
	out << "	<arg>\"" << unistoreName << "\" \"" << entryName << "\" \"" << std::to_string(index) << "\"" << "</arg>" << std::endl;

	/* Title. */
	std::string title;
	if (Input::getTextKeyboard(title, 30, Lang::get("ENTER_TITLE_SHORTCUT"))) out << "	<name>" << title << "</name>" << std::endl;
	else out << "	<name>" << entryName << "</name>" << std::endl;

	/* Description. */
	std::string desc;
	if (Input::getTextKeyboard(desc, 50, Lang::get("ENTER_DESC_SHORTCUT"))) out << "	<description>" << desc << "</description>" << std::endl;
	else out << "	<description>" << entryName << "</description>" << std::endl;

	/* Author and end. */
	out << "	<author>" << author << "</author>" << std::endl;
	out << "</shortcut>" << std::endl;
	out.close();
	return true;
}


/*
	Draw the Download entries part.
*/
void StoreUtils::DrawDownList(const std::shared_ptr<StoreEntry> &entry) {
	uint32_t accentColor = (config->useAccentColor() && entry) ? entry->GetAccentColor() : 0;

	/* For the Top Screen. */
	if (StoreUtils::store && StoreUtils::store->GetValid() && entry) {
		if (accentColor) Gui::Draw_Rect(0, 173, 400, 1, UIThemes->EntryOutline());
		Gui::Draw_Rect(0, 174, 400, 66, accentColor ? accentColor : UIThemes->DownListPrev());
		const C2D_Image tempImg = entry->GetIcon();
		const uint8_t offsetW = (48 - tempImg.subtex->width) / 2; // Center W.
		const uint8_t offsetH = (48 - tempImg.subtex->height) / 2; // Center H.
		C2D_DrawImageAt(tempImg, 9 + offsetW, 174 + 9 + offsetH, 0.5);

		if ((int)entry->GetScripts().size() > StoreUtils::store->GetDownloadIndex()) {
			const Script &script = entry->GetScript(StoreUtils::store->GetDownloadIndex());
			Gui::DrawString(70, 174 + 15, 0.45f, accentColor ? WHITE : UIThemes->TextColor(), script.GetName(), 310, 0, font);

			if (script.GetSize() != "") {
				Gui::DrawString(70, 174 + 30, 0.45f, accentColor ? WHITE : UIThemes->TextColor(), Lang::get("SIZE") + ": " + script.GetSize(), 310, 0, font);
			}
		}
	}

	if (fadeAlpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(0, 0, 0, fadeAlpha));
	GFX::DrawBottom();
	Gui::Draw_Rect(40, 0, 280, 25, accentColor ? accentColor : UIThemes->EntryBar());
	Gui::Draw_Rect(40, 25, 280, 1, UIThemes->EntryOutline());
	Gui::DrawStringCentered(17, 2, 0.6, accentColor ? WHITE : UIThemes->TextColor(), Lang::get("AVAILABLE_DOWNLOADS"), 223, 0, font);


	if (StoreUtils::store && StoreUtils::store->GetValid() && entry) {
		if(entry->GetUpdateAvl()) {
			GFX::DrawSprite(sprites_update_app_idx, clearUpdatePos.x, clearUpdatePos.y);
		}

		if (entry->GetScripts().size() > 0) {
			for (int i = 0; i < DOWNLOAD_ENTRIES && i < (int)entry->GetScripts().size(); i++) {
				const Script &script = entry->GetScript(i);

				if (StoreUtils::store->GetDownloadIndex() == i + StoreUtils::store->GetDownloadScrollOffset())
					Gui::Draw_Rect(downloadBoxes[i].x, downloadBoxes[i].y, downloadBoxes[i].w, downloadBoxes[i].h, UIThemes->MarkSelected());
				Gui::DrawStringCentered(46 - 160 + (241 / 2), downloadBoxes[i].y + 4, 0.45f, UIThemes->TextColor(), script.GetName(), 235, 0, font);

				GFX::DrawIcon(sprites_installed_idx, installedPos[i].x, installedPos[i].y, script.IsInstalled() ? UIThemes->TextColor() : UIThemes->MarkSelected());
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

	const std::shared_ptr<StoreEntry> &entry: Const Reference to the current StoreEntry, since we do not modify anything in it.
	int &currentMenu: Reference to the StoreMode / Menu, so we can switch back to EntryInfo with `B`.
	const int &lastMode: Const Reference to the last mode.
	int &smallDelay: Reference to the small delay. This helps to not directly press A.
	std::vector<bool> &installs: Reference to the installed states.
*/
void StoreUtils::DownloadHandle(const std::shared_ptr<StoreEntry> &entry, int &currentMenu, const int &lastMode, int &smallDelay) {
	if (!StoreUtils::store || !entry) return; // Ensure, store & entry is not a nullptr.
	if (smallDelay > 0) {
		smallDelay--;
	}

	if ((hDown & (KEY_Y | KEY_SELECT) || (hDown & KEY_TOUCH && touching(touch, downloadBoxes[6]))) && !entry->GetScripts().empty()) {
		if (is3DSX) { // Only allow if 3DSX.
			if (Msg::promptMsg(Lang::get("CREATE_SHORTCUT"), "Universal-Updater/shortcut", true)) {
				if (CreateShortcut(entry->GetTitle(), StoreUtils::store->GetDownloadIndex(), StoreUtils::store->GetInfo().file, entry->GetAuthor())) {
					Msg::waitMsg(Lang::get("SHORTCUT_CREATED"));
				}
			}
		}
	}

	if (hRepeat & KEY_DOWN) {
		if (StoreUtils::store->GetDownloadIndex() < (int)entry->GetScripts().size() - 1) StoreUtils::store->SetDownloadIndex(StoreUtils::store->GetDownloadIndex() + 1);
		else StoreUtils::store->SetDownloadIndex(0);
	}

	if (hRepeat & KEY_UP) {
		if (StoreUtils::store->GetDownloadIndex() > 0) StoreUtils::store->SetDownloadIndex(StoreUtils::store->GetDownloadIndex() - 1);
		else StoreUtils::store->SetDownloadIndex(entry->GetScripts().size() - 1);
	}


	if (hRepeat & KEY_RIGHT) {
		if (StoreUtils::store->GetDownloadIndex() + DOWNLOAD_ENTRIES < (int)entry->GetScripts().size() - 1) StoreUtils::store->SetDownloadIndex(StoreUtils::store->GetDownloadIndex() + DOWNLOAD_ENTRIES);
		else StoreUtils::store->SetDownloadIndex(entry->GetScripts().size() - 1);
	}

	if (hRepeat & KEY_LEFT) {
		if (StoreUtils::store->GetDownloadIndex() - DOWNLOAD_ENTRIES > 0) StoreUtils::store->SetDownloadIndex(StoreUtils::store->GetDownloadIndex() - DOWNLOAD_ENTRIES);
		else StoreUtils::store->SetDownloadIndex(0);
	}

	bool selected = false;
	if (smallDelay == 0 && hDown & KEY_TOUCH) {
		if (touching(touch, clearUpdatePos)) {
			StoreUtils::meta->SetUpdated(entry->GetUniStore(), entry->GetTitle(), entry->GetLastUpdated());
			StoreUtils::RefreshInstalledApps(entry->GetTitle());
		}

		for (int i = 0; i < DOWNLOAD_ENTRIES; i++) {
			if (touching(touch, downloadBoxes[i])) {
				if (i + StoreUtils::store->GetDownloadScrollOffset() < (int)entry->GetScripts().size()) {
					if(StoreUtils::store->GetDownloadIndex() == i + StoreUtils::store->GetDownloadScrollOffset()) {
						selected = true;
					} else {
						int scrollIndex = StoreUtils::store->GetDownloadScrollOffset();
						StoreUtils::store->SetDownloadIndex(scrollIndex + i);

						if (i == 0 && scrollIndex > 0)
							StoreUtils::store->SetDownloadScrollOffset(scrollIndex - 1);
						else if (i == (DOWNLOAD_ENTRIES - 1) && scrollIndex + DOWNLOAD_ENTRIES < (int)entry->GetScripts().size())
							StoreUtils::store->SetDownloadScrollOffset(scrollIndex + 1);
					}
				}

				break;
			}

			if (touching(touch, installedPos[i])) {
				if (i + StoreUtils::store->GetDownloadScrollOffset() < (int)entry->GetScripts().size()) {
					Script &script = entry->GetScript(i + StoreUtils::store->GetDownloadScrollOffset());
					if (script.IsInstalled()) {
						StoreUtils::meta->RemoveInstalled(entry->GetUniStore(), entry->GetTitle(), script.GetName());
					} else {
						if (StoreUtils::meta->GetUpdated(entry->GetUniStore(), entry->GetTitle()) == "")
							StoreUtils::meta->SetUpdated(entry->GetUniStore(), entry->GetTitle(), "---");

						StoreUtils::meta->SetInstalled(entry->GetUniStore(), entry->GetTitle(), script.GetName());
					}
					StoreUtils::RefreshInstalledApps(entry->GetTitle());
				}

				break;
			}
		}
	}

	if (smallDelay == 0 && (hDown & KEY_A || selected) && !entry->GetScripts().empty()) {
		const Script &script = entry->GetScript(StoreUtils::store->GetDownloadIndex());
		std::string Msg = Lang::get("EXECUTE_ENTRY") + "\n\n" + script.GetName();
		std::string PromptSaveKey = "";
		if (script.IsGit()) Msg += "\n\n" + Lang::get("NOTE_GIT");
		else if (script.IsPrerelease()) Msg += "\n\n" + Lang::get("NOTE_PRERELEASE");
		else PromptSaveKey = "Universal-Updater/confirm-install";

		const std::string &preinstallMessage = entry->GetPreinstallMessage();
		std::string SecondMsg = preinstallMessage + "\n\n" + Lang::get("EXECUTE_ENTRY_WITH_MESSAGE");

		if (Msg::promptMsg(Msg, PromptSaveKey, true) && (preinstallMessage.empty() || Msg::promptMsg(SecondMsg))) {
			QueueSystem::AddToQueue(entry, StoreUtils::store->GetDownloadIndex());
		}
	}

	if (hDown & KEY_X && !entry->GetScripts().empty()) {
		Script &script = entry->GetScript(StoreUtils::store->GetDownloadIndex());
		if (script.IsInstalled()) {
			StoreUtils::meta->RemoveInstalled(entry->GetUniStore(), entry->GetTitle(), script.GetName());
		} else {
			if (StoreUtils::meta->GetUpdated(entry->GetUniStore(), entry->GetTitle()) == "")
				StoreUtils::meta->SetUpdated(entry->GetUniStore(), entry->GetTitle(), "---");

			StoreUtils::meta->SetInstalled(entry->GetUniStore(), entry->GetTitle(), script.GetName());
		}
		StoreUtils::RefreshInstalledApps(entry->GetTitle());
	}

	if (hDown & KEY_B) currentMenu = lastMode; // Go back to EntryInfo.

	/* Quit UU. */
	if (hDown & KEY_START && !QueueRuns)
		exiting = true;

	/* Scroll Handle. */
	if (StoreUtils::store->GetDownloadIndex() < StoreUtils::store->GetDownloadScrollOffset()) StoreUtils::store->SetDownloadScrollOffset(StoreUtils::store->GetDownloadIndex());
	else if (StoreUtils::store->GetDownloadIndex() > StoreUtils::store->GetDownloadScrollOffset() + DOWNLOAD_ENTRIES - 1) StoreUtils::store->SetDownloadScrollOffset(StoreUtils::store->GetDownloadIndex() - DOWNLOAD_ENTRIES + 1);
}