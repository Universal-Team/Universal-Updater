// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#include "SortMenu.hpp"

#include "gui.hpp"
#include "UniversalUpdater.hpp"
#include "Utils.hpp"


static bool CompareTitleAscending(const size_t A, const size_t B) {
	if (UU::App->Store && UU::App->Store->UniStoreValid() && A < UU::App->Store->Indexes.size() && B < UU::App->Store->Indexes.size()) {
		return strcasecmp(Utils::LowerCase(UU::App->Store->GetEntryTitle(B)).c_str(), Utils::LowerCase(UU::App->Store->GetEntryTitle(A)).c_str()) > 0;
	}

	return true;
};
static bool CompareTitleDescending(const size_t A, const size_t B) {
	if (UU::App->Store && UU::App->Store->UniStoreValid() && A < UU::App->Store->Indexes.size() && B < UU::App->Store->Indexes.size()) {
		return strcasecmp(Utils::LowerCase(UU::App->Store->GetEntryTitle(A)).c_str(), Utils::LowerCase(UU::App->Store->GetEntryTitle(B)).c_str()) > 0;
	}
	
	return true;
};


static bool CompareAuthorAscending(const size_t A, const size_t B) {
	if (UU::App->Store && UU::App->Store->UniStoreValid() && A < UU::App->Store->Indexes.size() && B < UU::App->Store->Indexes.size()) {
		return strcasecmp(Utils::LowerCase(UU::App->Store->GetEntryAuthor(B)).c_str(), Utils::LowerCase(UU::App->Store->GetEntryAuthor(A)).c_str()) > 0;
	}

	return true;
};
static bool CompareAuthorDescending(const size_t A, const size_t B) {
	if (UU::App->Store && UU::App->Store->UniStoreValid() && A < UU::App->Store->Indexes.size() && B < UU::App->Store->Indexes.size()) {
		return strcasecmp(Utils::LowerCase(UU::App->Store->GetEntryAuthor(A)).c_str(), Utils::LowerCase(UU::App->Store->GetEntryAuthor(B)).c_str()) > 0;
	}
	
	return true;
};


static bool CompareLastUpdatedAscending(const size_t A, const size_t B) {
	if (UU::App->Store && UU::App->Store->UniStoreValid() && A < UU::App->Store->Indexes.size() && B < UU::App->Store->Indexes.size()) {
		return strcasecmp(Utils::LowerCase(UU::App->Store->GetEntryLastUpdated(B)).c_str(), Utils::LowerCase(UU::App->Store->GetEntryLastUpdated(A)).c_str()) > 0;
	}

	return true;
};
static bool CompareLastUpdatedDescending(const size_t A, const size_t B) {
	if (UU::App->Store && UU::App->Store->UniStoreValid() && A < UU::App->Store->Indexes.size() && B < UU::App->Store->Indexes.size()) {
		return strcasecmp(Utils::LowerCase(UU::App->Store->GetEntryLastUpdated(A)).c_str(), Utils::LowerCase(UU::App->Store->GetEntryLastUpdated(B)).c_str()) > 0;
	}
	
	return true;
};


/*
	Return SortType as an uint8_t.

	const SortMenu::SortType SType: The SortType variable.
*/
uint8_t SortMenu::GetType(const SortMenu::SortType SType) const {
	switch(SType) {
		case SortMenu::SortType::Title:
			return 0;

		case SortMenu::SortType::Author:
			return 1;

		case SortMenu::SortType::LastUpdated:
			return 2;
	}

	return 1;
};


void SortMenu::DoSort(const SortMenu::SortType SType) {
	this->SType = SType;
	UU::App->Store->SelectedIndex = 0, UU::App->Store->ScreenIndex = 0; // Reset after that.

	switch(this->SType) {
		case SortMenu::SortType::Title:
			this->Ascending ?
				std::sort(UU::App->Store->Indexes.begin(), UU::App->Store->Indexes.end(), CompareTitleAscending) :
				std::sort(UU::App->Store->Indexes.begin(), UU::App->Store->Indexes.end(), CompareTitleDescending);
			break;

		case SortMenu::SortType::Author:
			this->Ascending ?
				std::sort(UU::App->Store->Indexes.begin(), UU::App->Store->Indexes.end(), CompareAuthorAscending) :
				std::sort(UU::App->Store->Indexes.begin(), UU::App->Store->Indexes.end(), CompareAuthorDescending);
			break;

		case SortMenu::SortType::LastUpdated:
			this->Ascending ?
				std::sort(UU::App->Store->Indexes.begin(), UU::App->Store->Indexes.end(), CompareLastUpdatedAscending) :
				std::sort(UU::App->Store->Indexes.begin(), UU::App->Store->Indexes.end(), CompareLastUpdatedDescending);
			break;
	}
};


void SortMenu::Draw() {
	/* Draw Top bar. */
	Gui::Draw_Rect(40, 0, 280, 25, BAR_COLOR);
	Gui::Draw_Rect(40, 25, 280, 1, BAR_OUTLINE);
	Gui::DrawStringCentered(17, 2, TEXT_MEDIUM, TEXT_COLOR, "Sorting", 273, 0);

	/* Sort By. */
	Gui::DrawString(this->SortPos[0].x + 1, this->SortPos[0].y - 20, TEXT_LARGE, TEXT_COLOR, "Sort By", 90, 0);
	for (uint8_t Idx = 0; Idx < 3; Idx++) {
		UU::App->GData->DrawSort(this->SortPos[Idx].x + 1, this->SortPos[Idx].y, Idx == this->GetType(this->SType));
	}

	Gui::DrawString(this->SortPos[0].x + 21, this->SortPos[0].y + 2, TEXT_SMALL, TEXT_COLOR, "Title", 80, 0);
	Gui::DrawString(this->SortPos[1].x + 21, this->SortPos[1].y + 2, TEXT_SMALL, TEXT_COLOR, "Author", 80, 0);
	Gui::DrawString(this->SortPos[2].x + 21, this->SortPos[2].y + 2, TEXT_SMALL, TEXT_COLOR, "Last Updated", 80, 0);

	/* Direction. */
	Gui::DrawString(this->SortPos[3].x + 1, this->SortPos[3].y - 20, TEXT_LARGE, TEXT_COLOR, "Direction", 80, 0);
	UU::App->GData->DrawSort(this->SortPos[3].x + 1, this->SortPos[3].y, this->Ascending);
	UU::App->GData->DrawSort(this->SortPos[4].x + 1, this->SortPos[4].y, !this->Ascending);
	Gui::DrawString(this->SortPos[3].x + 21, this->SortPos[3].y + 2, TEXT_SMALL, TEXT_COLOR, "Ascending", 80, 0);
	Gui::DrawString(this->SortPos[4].x + 21, this->SortPos[4].y + 2, TEXT_SMALL, TEXT_COLOR, "Descending", 80, 0);

	/* Top Style. */
	Gui::DrawString(this->SortPos[5].x + 1, this->SortPos[5].y - 20, TEXT_LARGE, TEXT_COLOR, "Top Style", 90, 0);
	UU::App->GData->DrawSort(this->SortPos[5].x + 1, this->SortPos[5].y, UU::App->TMode == UU::TopMode::List);
	UU::App->GData->DrawSort(this->SortPos[6].x + 1, this->SortPos[6].y, UU::App->TMode == UU::TopMode::Grid);
	Gui::DrawString(this->SortPos[5].x + 21, this->SortPos[5].y + 2, TEXT_SMALL, TEXT_COLOR, "List", 90, 0);
	Gui::DrawString(this->SortPos[6].x + 21, this->SortPos[6].y + 2, TEXT_SMALL, TEXT_COLOR, "Grid", 90, 0);
};


void SortMenu::Handler() {
	if (UU::App->Store && UU::App->Store->UniStoreValid() && UU::App->Store->Indexes.size() > 0) {
		if (UU::App->Down & KEY_TOUCH) {

			if (this->SortPos[0].Touched(UU::App->T)) {
				this->DoSort(SortMenu::SortType::Title);

			} else if (this->SortPos[1].Touched(UU::App->T)) {
				this->DoSort(SortMenu::SortType::Author);

			} else if (this->SortPos[2].Touched(UU::App->T)) {
				this->DoSort(SortMenu::SortType::LastUpdated);

			} else if (this->SortPos[3].Touched(UU::App->T)) {
				this->Ascending = true;
				this->DoSort(this->SType);

			} else if (this->SortPos[4].Touched(UU::App->T)) {
				this->Ascending = false;
				this->DoSort(this->SType);

			} else if (this->SortPos[5].Touched(UU::App->T)) {
				if (UU::App->TMode == UU::TopMode::List) return;
				UU::App->SwitchTopMode(UU::TopMode::List);
				UU::App->CData->List(true);

			} else if (this->SortPos[6].Touched(UU::App->T)) {
				if (UU::App->TMode == UU::TopMode::Grid) return;
				UU::App->SwitchTopMode(UU::TopMode::Grid);
				UU::App->CData->List(false);
			}
		}
	}
};