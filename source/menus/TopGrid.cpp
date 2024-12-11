// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#include "TopGrid.hpp"

#include "UniversalUpdater.hpp"

void TopGrid::Draw() {
	std::vector<GFXData::UniStoreIcon> Icons;

	for (size_t Idx = 0, Idx2 = 0 + (UU::App->Store->ScreenIndex * 5); Idx2 < 15 + (UU::App->Store->ScreenIndex * 5) && Idx2 < UU::App->Store->Indexes.size(); Idx2++, Idx++) {
		/* Box. */
		if (Idx == this->Box) UU::App->GData->DrawBox(TOP_GRID_X(this->Box), TOP_GRID_Y(this->Box), TOP_GRID_W, TOP_GRID_H, true);


		/* Ensure, Indexes is larger than the index. */
		if (UU::App->Store->Indexes.size() > Idx2) {
			/* Get icon index. */
			Icons.emplace_back(
				UU::App->Store->GetEntryIcon(UU::App->Store->Indexes[Idx2]),
				UU::App->Store->GetEntrySheet(UU::App->Store->Indexes[Idx2]),
				UU::App->MData->UpdateAvailable(UU::App->Store->GetUniStoreTitle(),
					UU::App->Store->GetEntryTitle(UU::App->Store->Indexes[Idx2]),
					UU::App->Store->GetEntryLastUpdated(UU::App->Store->Indexes[Idx2])
				)
			);
		}
	}

	UU::App->GData->DrawUniStoreIcons(Icons);
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

		if (UU::App->Down & KEY_A) UU::App->_Tabs->SwitchTab(Tabs::Tab::DownloadList);
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