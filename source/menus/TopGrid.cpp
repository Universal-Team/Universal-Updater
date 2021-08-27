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
#include "TopGrid.hpp"


void TopGrid::Draw() {
	std::vector<std::tuple<int, int, bool>> Indexes;

	for (size_t Idx = 0, Idx2 = 0 + (UU::App->Store->ScreenIndex * 5); Idx2 < 15 + (UU::App->Store->ScreenIndex * 5) && Idx2 < UU::App->Store->Indexes.size(); Idx2++, Idx++) {
		/* Box. */
		if (Idx == this->Box) UU::App->GData->DrawBox(TOP_GRID_X(this->Box), TOP_GRID_Y(this->Box), TOP_GRID_W, TOP_GRID_H, true);


		/* Ensure, Indexes is larger than the index. */
		if (UU::App->Store->Indexes.size() > Idx2) {
			/* Get icon index. */
			Indexes.push_back({
				UU::App->Store->GetEntryIcon(UU::App->Store->Indexes[Idx2]),
				UU::App->Store->GetEntrySheet(UU::App->Store->Indexes[Idx2]),

				UU::App->MData->UpdateAvailable(UU::App->Store->GetUniStoreTitle(),
					UU::App->Store->GetEntryTitle(UU::App->Store->Indexes[Idx2]),
					UU::App->Store->GetEntryLastUpdated(UU::App->Store->Indexes[Idx2])
				)
			});
		}
	}

	UU::App->GData->DrawUniStoreIcons(Indexes);
};


void TopGrid::Handler() {
	if (UU::App->Store && !UU::App->Store->Indexes.empty()) { // Ensure, store is not a nullptr.
		if (UU::App->Repeat & KEY_DOWN) {
			if (this->Box > 9) { // If last row in the grid, do this.
				if (UU::App->Store->SelectedIndex + 5 < UU::App->Store->Indexes.size() - 1) {
					UU::App->Store->SelectedIndex += 5;

					if (UU::App->Store->Indexes.size() > 15) UU::App->Store->ScreenIndex = (UU::App->Store->SelectedIndex / 5) - 2;

				} else {
					if (UU::App->Store->SelectedIndex < UU::App->Store->Indexes.size() - 1) {
						UU::App->Store->SelectedIndex = UU::App->Store->Indexes.size() - 1;
						this->Box = 10 + (UU::App->Store->SelectedIndex % 5);

						if (UU::App->Store->Indexes.size() > 15) UU::App->Store->ScreenIndex = (UU::App->Store->SelectedIndex / 5) - 2;
					}
				}

			} else { // Not last row.
				if (UU::App->Store->SelectedIndex + 5 < UU::App->Store->Indexes.size()) {
					this->Box += 5;
					UU::App->Store->SelectedIndex += 5;
				}
			}
		}

		if (UU::App->Repeat & KEY_RIGHT) {
			if (UU::App->Store->SelectedIndex < UU::App->Store->Indexes.size() - 1) {
				if (this->Box < 14) {
					this->Box++;
					UU::App->Store->SelectedIndex++;

				} else {
					this->Box = 10;
					UU::App->Store->SelectedIndex++;

					UU::App->Store->ScreenIndex = (UU::App->Store->SelectedIndex / 5) - 2;

				}
			}
		}

		if (UU::App->Repeat & KEY_LEFT) {
			if (UU::App->Store->SelectedIndex > 0) {
				if (this->Box > 0) {
					this->Box--;
					UU::App->Store->SelectedIndex--;

				} else {
					this->Box = 4;
					UU::App->Store->SelectedIndex--;

					UU::App->Store->ScreenIndex = UU::App->Store->SelectedIndex / 5;
				}
			}
		}

		if (UU::App->Repeat & KEY_UP) {
			if (this->Box < 5) {
				if (UU::App->Store->SelectedIndex > 4) {
					UU::App->Store->SelectedIndex -= 5;

					UU::App->Store->ScreenIndex = UU::App->Store->SelectedIndex / 5;
				}

			} else {
				this->Box -= 5;
				UU::App->Store->SelectedIndex -= 5;
			}
		}

		/* TODO: Handle A Press for going to the Download List. */
	}
};


void TopGrid::Update() {
	/* It is at least in the last row. */
	if (UU::App->Store->SelectedIndex >= 15) {
		this->Box = 10 + (UU::App->Store->SelectedIndex % 5);
		UU::App->Store->ScreenIndex = (UU::App->Store->SelectedIndex / 5) - 2;

	} else {
		this->Box = UU::App->Store->SelectedIndex;
	}
};