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

#include "DownList.hpp"

#include "gui.hpp"
#include "QueueSystem.hpp"
#include "UniversalUpdater.hpp"
#include "Utils.hpp"

#define DOWNLOAD_ENTRIES 7


void DownList::DrawTopOvl() {
	/* For the Top Screen. */
	if (!UU::App->Store->Indexes.empty() && UU::App->Store->SelectedIndex <= UU::App->Store->Indexes.size() - 1) {
		const std::vector<std::string> Entries = UU::App->Store->GetDownloadList(UU::App->Store->Indexes[UU::App->Store->SelectedIndex]);
		
		if (!Entries.empty()) {
			Gui::Draw_Rect(0, 174, 400, 66, BAR_OUTLINE);
			Gui::DrawString(70, 174 + 15, TEXT_MEDIUM, TEXT_COLOR, Entries[this->SelectedIndex], 310, 0);

			const std::string Size = UU::App->Store->GetFileSizes(UU::App->Store->Indexes[UU::App->Store->SelectedIndex], Entries[this->SelectedIndex]);
			if (Size != "") Gui::DrawString(70, 174 + 30, TEXT_MEDIUM, TEXT_COLOR, "Size: " +  Size, 310, 0);
		}
	}
};



void DownList::Draw() {
	/* Draw Top bar. */
	Gui::Draw_Rect(40, 0, 280, 25, BAR_COLOR);
	Gui::Draw_Rect(40, 25, 280, 1, BAR_OUTLINE);
	Gui::DrawStringCentered(17, 2, TEXT_MEDIUM, TEXT_COLOR, "Available Downloads", 273, 0);

	/* Ensure the Selected Index is in scope of the indexes size. */
	if (!UU::App->Store->Indexes.empty() && UU::App->Store->SelectedIndex <= UU::App->Store->Indexes.size() - 1) {
		const std::vector<std::string> Entries = UU::App->Store->GetDownloadList(UU::App->Store->Indexes[UU::App->Store->SelectedIndex]);
		const std::vector<std::string> Installed = UU::App->MData->GetInstalled(UU::App->Store->GetUniStoreTitle(), UU::App->Store->GetEntryTitle(UU::App->Store->Indexes[UU::App->Store->SelectedIndex]));

		if (!Entries.empty()) {
			for (size_t Idx = 0; Idx < DOWNLOAD_ENTRIES && Idx < Entries.size(); Idx++) {
				if (this->SelectedIndex == Idx + this->ScreenIndex) Gui::Draw_Rect(this->DownPos[Idx].x, this->DownPos[Idx].y, this->DownPos[Idx].w, this->DownPos[Idx].h, BAR_OUTLINE);
			
				Gui::DrawStringCentered(46 - 160 + (241 / 2), this->DownPos[Idx].y + 4, TEXT_SMALL, TEXT_COLOR, Entries[Idx + this->ScreenIndex], 235, 0);

				auto Result = std::find(Installed.begin(), Installed.end(), Entries[Idx + this->ScreenIndex]);
				if (Result != Installed.end()) {
					/* Uncomment it, when the installed sprite has been added to the NDS version. */
					UU::App->GData->DrawSpriteBlend(sprites_installed_idx, this->InstallPos[Idx].x, this->InstallPos[Idx].y);
				}
			}

		} else { // If no downloads available..
			Gui::DrawStringCentered(46 - 160 + (241 / 2), this->DownPos[0].y + 4, TEXT_MEDIUM, TEXT_COLOR, "No Downloads available.", 235, 0);
		}
	}
};


void DownList::Handler() {
	/* Only do things if selected index is in scope. */
	if (!UU::App->Store->Indexes.empty() && UU::App->Store->SelectedIndex <= UU::App->Store->Indexes.size() - 1) {
		/* Create the variable to save vector fetching logic. */
		const std::vector<std::string> Entries = UU::App->Store->GetDownloadList(UU::App->Store->Indexes[UU::App->Store->SelectedIndex]);

		/* Handle Up / Down stuff. */
		if (UU::App->Repeat & KEY_DOWN || UU::App->Repeat & KEY_UP) {
			if (!Entries.empty()) {
				if (UU::App->Repeat & KEY_DOWN) {
					if (this->SelectedIndex < Entries.size() - 1) this->SelectedIndex++;
					else this->SelectedIndex = 0;
				}

				if (UU::App->Repeat & KEY_UP) {
					if (this->SelectedIndex > 0) this->SelectedIndex--;
					else this->SelectedIndex = Entries.size() - 1;
				}
			}
		}

		if (UU::App->Down & KEY_A) {
			QueueSystem::Add(UU::App->Store->Indexes[UU::App->Store->SelectedIndex], this->SelectedIndex, UU::App->Store->GetScript(UU::App->Store->Indexes[UU::App->Store->SelectedIndex], Entries[this->SelectedIndex]));
		}

		if (UU::App->Down & KEY_B) UU::App->_Tabs->PrevTab(); // Go back to previous tab.
	}

	/* Scroll Handle. */
	if (this->SelectedIndex < this->ScreenIndex) this->ScreenIndex = this->SelectedIndex;
	else if (this->SelectedIndex > this->ScreenIndex + DOWNLOAD_ENTRIES - 1) this->ScreenIndex = this->SelectedIndex - DOWNLOAD_ENTRIES + 1;
};


void DownList::Reset() { this->SelectedIndex = 0, this->ScreenIndex = 0; };