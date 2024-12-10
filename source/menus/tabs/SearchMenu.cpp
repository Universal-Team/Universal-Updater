// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#include "SearchMenu.hpp"

#include "gui.hpp"
#include "keyboard.hpp"
#include "UniversalUpdater.hpp"
#include "Utils.hpp"


/*
	Find a query from a vector.

	const std::vector<std::string> &Items: Const Reference to the vector strings / items.
	const std::string &Query: Const Reference to the query.
*/
bool SearchMenu::FindInVector(const std::vector<std::string> &Items, const std::string &Query) {
	for(const std::string &Item : Items) {
		if (Utils::LowerCase(Item).find(Query) != std::string::npos) return true;
	}

	return false;
};


/*
	NOTE: This code is a pain, lol.
*/
void SearchMenu::DoSearch() {
	if (this->IsAnd) {
		for (auto It = UU::App->Store->Indexes.begin(); It != UU::App->Store->Indexes.end(); ++It) {
			if (!(((this->Includes[0] && Utils::LowerCase(UU::App->Store->GetEntryTitle((*It))).find(Utils::LowerCase(this->SearchText)) != std::string::npos)
			|| (this->Includes[1] && Utils::LowerCase(UU::App->Store->GetEntryAuthor((*It))).find(Utils::LowerCase(this->SearchText)) != std::string::npos)
			|| (this->Includes[2] && this->FindInVector(UU::App->Store->GetEntryCategories((*It)), Utils::LowerCase(this->SearchText)))
			|| (this->Includes[3] && this->FindInVector(UU::App->Store->GetEntryConsoles((*It)), Utils::LowerCase(this->SearchText)))
			|| (!this->Includes[0] && !this->Includes[1] && !this->Includes[2] && !this->Includes[3]))
			&& ((this->Marks == 0 && !this->UpdateFilter) || (((UU::App->MData->GetMarks(UU::App->Store->GetUniStoreTitle(), UU::App->Store->GetEntryTitle((*It))) & this->Marks) == this->Marks) && (!this->UpdateFilter || UU::App->MData->UpdateAvailable(UU::App->Store->GetUniStoreTitle(), UU::App->Store->GetEntryTitle((*It)), UU::App->Store->GetEntryLastUpdated((*It)))))))) {
				It = UU::App->Store->Indexes.erase(It);
				--It;
			}
		}

	} else {
		for (auto It = UU::App->Store->Indexes.begin(); It != UU::App->Store->Indexes.end(); ++It) {
			if (!(((this->Includes[0] && Utils::LowerCase(UU::App->Store->GetEntryTitle((*It))).find(Utils::LowerCase(this->SearchText)) != std::string::npos)
			|| (this->Includes[1] && Utils::LowerCase(UU::App->Store->GetEntryAuthor((*It))).find(Utils::LowerCase(this->SearchText)) != std::string::npos)
			|| (this->Includes[2] && this->FindInVector(UU::App->Store->GetEntryCategories((*It)), Utils::LowerCase(this->SearchText)))
			|| (this->Includes[3] && this->FindInVector(UU::App->Store->GetEntryConsoles((*It)), Utils::LowerCase(this->SearchText)))
			|| (!this->Includes[0] && !this->Includes[1] && !this->Includes[2] && !this->Includes[3]))
			&& ((this->Marks == 0 && !this->UpdateFilter) || UU::App->MData->GetMarks(UU::App->Store->GetUniStoreTitle(), UU::App->Store->GetEntryTitle((*It))) & this->Marks || (this->UpdateFilter && UU::App->MData->UpdateAvailable(UU::App->Store->GetUniStoreTitle(), UU::App->Store->GetEntryTitle((*It)), UU::App->Store->GetEntryLastUpdated((*It))))))) {
				It = UU::App->Store->Indexes.erase(It);
				--It;
			}
		}
	}
};


/*
	TODO: Better colors for filter boxes and such. I was too lazy to add the current Universal-Updater ones.
*/
void SearchMenu::Draw() {
	/* Draw Top bar. */
	Gui::Draw_Rect(40, 0, 280, 25, BAR_COLOR);
	Gui::Draw_Rect(40, 25, 280, 1, BAR_OUTLINE);
	Gui::DrawStringCentered(17, 2, TEXT_MEDIUM, TEXT_COLOR, "Search and Filters", 273, 0);

	Gui::Draw_Rect(50, 40, 264, this->SearchPos[0].h + 2, BAR_OUTLINE);
	Gui::Draw_Rect(this->SearchPos[0].x, this->SearchPos[0].y, this->SearchPos[0].w, this->SearchPos[0].h, BAR_COLOR);

	Gui::DrawStringCentered(24, 46, TEXT_LARGE, TEXT_COLOR, this->SearchText, 265, 0);

	/* Checkboxes. */
	for (uint8_t Idx = 0; Idx < 4; Idx++) {
		UU::App->GData->DrawCheckbox(this->SearchPos[Idx + 1].x, this->SearchPos[Idx + 1].y, this->Includes[Idx]);
	}

	Gui::DrawString(84, 81, TEXT_MEDIUM, TEXT_COLOR, "Include in results", 265, 0);

	Gui::DrawString(this->SearchPos[1].x + 18, this->SearchPos[1].y + 1, TEXT_SMALL, TEXT_COLOR, "Title", 90, 0);
	Gui::DrawString(this->SearchPos[2].x + 18, this->SearchPos[2].y + 1, TEXT_SMALL, TEXT_COLOR, "Author", 90, 0);

	Gui::DrawString(this->SearchPos[3].x + 18, this->SearchPos[3].y + 1, TEXT_SMALL, TEXT_COLOR, "Categories", 90, 0);
	Gui::DrawString(this->SearchPos[4].x + 18, this->SearchPos[4].y + 1, TEXT_SMALL, TEXT_COLOR, "Consoles", 90, 0);

	/* Filters. */
	Gui::DrawString(84, this->SearchPos[5].y - 20, TEXT_MEDIUM, TEXT_COLOR, "Filter to", 265, 0);

	Gui::Draw_Rect(this->SearchPos[5].x, this->SearchPos[5].y, this->SearchPos[5].w, this->SearchPos[5].h, ((this->Marks & (int)SearchMenu::FavoriteMarks::Star) ?
		TABS_SELECTED : TABS_UNSELECTED));

	Gui::Draw_Rect(this->SearchPos[6].x, this->SearchPos[6].y, this->SearchPos[6].w, this->SearchPos[6].h, ((this->Marks & (int)SearchMenu::FavoriteMarks::Heart) ?
		TABS_SELECTED : TABS_UNSELECTED));

	Gui::Draw_Rect(this->SearchPos[7].x, this->SearchPos[7].y, this->SearchPos[7].w, this->SearchPos[7].h, ((this->Marks & (int)SearchMenu::FavoriteMarks::Diamond) ?
		TABS_SELECTED : TABS_UNSELECTED));

	Gui::Draw_Rect(this->SearchPos[8].x, this->SearchPos[8].y, this->SearchPos[8].w, this->SearchPos[8].h, ((this->Marks & (int)SearchMenu::FavoriteMarks::Clubs) ?
		TABS_SELECTED : TABS_UNSELECTED));

	Gui::Draw_Rect(this->SearchPos[9].x, this->SearchPos[9].y, this->SearchPos[9].w, this->SearchPos[9].h, ((this->Marks & (int)SearchMenu::FavoriteMarks::Spade) ?
		TABS_SELECTED : TABS_UNSELECTED));

	Gui::Draw_Rect(this->SearchPos[10].x, this->SearchPos[10].y, this->SearchPos[10].w, this->SearchPos[10].h, (this->UpdateFilter ?
		TABS_SELECTED : TABS_UNSELECTED));

	/* Draw the Filters. */
	Gui::DrawString(this->SearchPos[5].x + 9, this->SearchPos[5].y + 7, TEXT_MEDIUM, TEXT_COLOR, "★", 0, 0);
	Gui::DrawString(this->SearchPos[6].x + 9, this->SearchPos[6].y + 7, TEXT_MEDIUM, TEXT_COLOR, "♥", 0, 0);
	Gui::DrawString(this->SearchPos[7].x + 9, this->SearchPos[7].y + 7, TEXT_MEDIUM, TEXT_COLOR, "♦", 0, 0);
	Gui::DrawString(this->SearchPos[8].x + 9, this->SearchPos[8].y + 7, TEXT_MEDIUM, TEXT_COLOR, "♣", 0, 0);
	Gui::DrawString(this->SearchPos[9].x + 9, this->SearchPos[9].y + 7, TEXT_MEDIUM, TEXT_COLOR, "♠", 0, 0);
	UU::App->GData->DrawSpriteBlend(sprites_update_filter_idx, this->SearchPos[10].x + 8, this->SearchPos[10].y + 8, TEXT_COLOR);

	Gui::Draw_Rect(this->SearchPos[11].x, this->SearchPos[11].y, this->SearchPos[11].w, this->SearchPos[11].h, TABS_UNSELECTED);
	Gui::DrawStringCentered(23, this->SearchPos[11].y + 6, TEXT_MEDIUM, TEXT_COLOR, "Add Selection to Queue", 200, 0);

	/* AND / OR. */
	Gui::Draw_Rect(this->SearchPos[12].x, this->SearchPos[12].y, this->SearchPos[12].w, this->SearchPos[12].h, (this->IsAnd ? TABS_SELECTED : TABS_UNSELECTED));
	Gui::DrawString(this->SearchPos[12].x + 4, this->SearchPos[12].y, TEXT_SMALL, TEXT_COLOR, "AND", 0, 0);

	Gui::Draw_Rect(this->SearchPos[13].x, this->SearchPos[13].y, this->SearchPos[13].w, this->SearchPos[13].h, (!this->IsAnd ? TABS_SELECTED : TABS_UNSELECTED));
	Gui::DrawString(this->SearchPos[13].x + 8, this->SearchPos[13].y, TEXT_SMALL, TEXT_COLOR, "OR", 0, 0);
};


void SearchMenu::Handler() {
	/* Checkboxes. */
	if (UU::App->Down & KEY_TOUCH) {
		bool DidTouch = false;

		/* Includes. */
		for (uint8_t Idx = 0; Idx < 4; Idx++) {
			if (this->SearchPos[Idx + 1].Touched(UU::App->T)) {
				this->Includes[Idx] = !this->Includes[Idx];
				DidTouch = true;
				break;
			}
		}

		/* Search bar. */
		if (!DidTouch) {
			if (this->SearchPos[0].Touched(UU::App->T)) {
				UCKeyboard Kbd = UCKeyboard("nitro:/keyboard.json", BG_COLOR, BAR_COLOR, BAR_OUTLINE, TABS_UNSELECTED, TABS_SELECTED, TABS_SELECTED, TEXT_COLOR, TAB_ICON_COLOR);
				if (UU::App->Store && UU::App->Store->UniStoreValid()) {
					this->SearchText = Kbd.GetString(0, "Stack");
					DidTouch = true;

				} else {
					this->SearchText = Kbd.GetString(0, "Stack");
					DidTouch = true;
				}
			}
		}

		/* Filters. */
		if (!DidTouch) {
			if (this->SearchPos[5].Touched(UU::App->T)) {
				this->Marks = this->Marks ^ (int)SearchMenu::FavoriteMarks::Star;
				DidTouch = true;

			} else if (this->SearchPos[6].Touched(UU::App->T)) {
				this->Marks = this->Marks ^ (int)SearchMenu::FavoriteMarks::Heart;
				DidTouch = true;

			} else if (this->SearchPos[7].Touched(UU::App->T)) {
				this->Marks = this->Marks ^ (int)SearchMenu::FavoriteMarks::Diamond;
				DidTouch = true;

			} else if (this->SearchPos[8].Touched(UU::App->T)) {
				this->Marks = this->Marks ^ (int)SearchMenu::FavoriteMarks::Clubs;
				DidTouch = true;

			} else if (this->SearchPos[9].Touched(UU::App->T)) {
				this->Marks = this->Marks ^ (int)SearchMenu::FavoriteMarks::Spade;
				DidTouch = true;

			} else if (this->SearchPos[10].Touched(UU::App->T)) {
				this->UpdateFilter = !this->UpdateFilter;
				DidTouch = true;

			} else if (this->SearchPos[11].Touched(UU::App->T)) {
				/* Add them to Queue TODO. */

			} else if (this->SearchPos[12].Touched(UU::App->T)) {
				this->IsAnd = true;
				DidTouch = true;

			} else if (this->SearchPos[13].Touched(UU::App->T)) {
				this->IsAnd = false;
				DidTouch = true;
			}
		}

		if (DidTouch) { // We touched, so search.
			if (UU::App->Store && UU::App->Store->UniStoreValid()) {
				UU::App->Store->ResetIndexes();
				this->DoSearch();
				UU::App->_Tabs->SortEntries(); // Resort.
				UU::App->SwitchTopMode(UU::App->TMode); // We don't switch actually, it's more for a refresh.
			}
		}
	}

	/* Reset or so. */
	if (UU::App->Down & KEY_X) {
		this->SearchText = "";
		for (uint8_t Idx = 0; Idx < 4; Idx++) this->Includes[Idx] = false;
		this->Marks = 0;
		UU::App->Store->ResetIndexes();
		UU::App->_Tabs->SortEntries(); // Resort.
		UU::App->SwitchTopMode(UU::App->TMode); // We don't switch actually, it's more for a refresh.
	}
};