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

#include "Common.hpp"
#include "TopList.hpp"
#include "Utils.hpp"


void TopList::Draw() {
	if (!UU::App->Store->Indexes.empty()) {
		for (size_t Idx = 0; Idx < 3 && Idx < UU::App->Store->Indexes.size(); Idx++) {

			if (Idx + UU::App->Store->ScreenIndex == UU::App->Store->SelectedIndex) {
				UU::App->GData->DrawBox(this->List[Idx].x, this->List[Idx].y, this->List[Idx].w, this->List[Idx].h);
			}

			if (UU::App->Store->Indexes.size() > Idx + UU::App->Store->ScreenIndex) {
				UU::App->GData->DrawUniStoreIcon(
					UU::App->Store->GetEntryIcon(UU::App->Store->Indexes[Idx + UU::App->Store->ScreenIndex]),
					UU::App->Store->GetEntrySheet(UU::App->Store->Indexes[Idx + UU::App->Store->ScreenIndex]),
					this->List[Idx].x + 1, this->List[Idx].y + 1
				);

				/* TODO: Handle Update display. */

				Gui::DrawStringCentered(29, this->List[Idx].y + 5, TEXT_LARGE, TEXT_WHITE, UU::App->Store->GetEntryTitle(UU::App->Store->Indexes[Idx + UU::App->Store->ScreenIndex]), 300, 0);
				Gui::DrawStringCentered(29, this->List[Idx].y + 24, TEXT_LARGE, TEXT_WHITE, UU::App->Store->GetEntryAuthor(UU::App->Store->Indexes[Idx + UU::App->Store->ScreenIndex]), 300, 0);
			}
		}
	}
};


void TopList::Handler() {
	if (UU::App->Store) { // Ensure, Store is not a nullptr.
		if (UU::App->Repeat & KEY_DOWN) {
			if (UU::App->Store->SelectedIndex < UU::App->Store->Indexes.size() - 1) UU::App->Store->SelectedIndex = UU::App->Store->SelectedIndex + 1;
			else UU::App->Store->SelectedIndex = 0;
		}

		if (UU::App->Repeat & KEY_RIGHT) {
			if (UU::App->Store->SelectedIndex < UU::App->Store->Indexes.size() - 3) UU::App->Store->SelectedIndex = UU::App->Store->SelectedIndex + 3;
			else UU::App->Store->SelectedIndex = UU::App->Store->Indexes.size() - 1;
		}

		if (UU::App->Repeat & KEY_LEFT) {
			if (UU::App->Store->SelectedIndex - 2 > 0) UU::App->Store->SelectedIndex = UU::App->Store->SelectedIndex - 3;
			else UU::App->Store->SelectedIndex = 0;
		}

		if (UU::App->Repeat & KEY_UP) {
			if (UU::App->Store->SelectedIndex > 0) UU::App->Store->SelectedIndex = UU::App->Store->SelectedIndex - 1;
			else UU::App->Store->SelectedIndex = UU::App->Store->Indexes.size() - 1;
		}

		/* TODO: Handle A Press for going to the Download List. */
		
		/* Scroll Logic. */
		if (UU::App->Store->SelectedIndex < UU::App->Store->ScreenIndex) UU::App->Store->ScreenIndex = UU::App->Store->SelectedIndex;
		else if (UU::App->Store->SelectedIndex > UU::App->Store->ScreenIndex + 3 - 1) UU::App->Store->ScreenIndex = UU::App->Store->SelectedIndex - 3 + 1;
	}
};