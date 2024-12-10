// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#include "EntryInfo.hpp"

#include "gui.hpp"
#include "UniversalUpdater.hpp"
#include "Utils.hpp"


void EntryInfo::Draw() {
	/* Ensure the Selected Index is in scope of the indexes size. */
	if (!UU::App->Store->Indexes.empty() && UU::App->Store->SelectedIndex <= UU::App->Store->Indexes.size() - 1) {
		/* Draw Top bar. */
		Gui::Draw_Rect(40, 0, 280, 36, BAR_COLOR);
		Gui::Draw_Rect(40, 36, 280, 1, BAR_OUTLINE);

		/* Basic Info. */
		Gui::DrawStringCentered(17, 0, TEXT_LARGE, TEXT_COLOR, UU::App->Store->GetEntryTitle(UU::App->Store->Indexes[UU::App->Store->SelectedIndex]), 273, 0);
		Gui::DrawStringCentered(17, 20, TEXT_SMALL, TEXT_COLOR, UU::App->Store->GetEntryAuthor(UU::App->Store->Indexes[UU::App->Store->SelectedIndex]), 273, 0);
		Gui::DrawStringCentered(17, 50, TEXT_SMALL, TEXT_COLOR, UU::App->Store->GetEntryDescription(UU::App->Store->Indexes[UU::App->Store->SelectedIndex]), 248, 0, nullptr, C2D_WordWrap);

		/* Other Info. */
		Gui::DrawString(53, 130, TEXT_MEDIUM, TEXT_COLOR, "Version: " + UU::App->Store->GetEntryVersion(UU::App->Store->Indexes[UU::App->Store->SelectedIndex]), 248, 0);
		Gui::DrawString(53, 145, TEXT_MEDIUM, TEXT_COLOR, "Category: " + Utils::VectorToString(UU::App->Store->GetEntryCategories(UU::App->Store->Indexes[UU::App->Store->SelectedIndex])), 248, 0);
		Gui::DrawString(53, 160, TEXT_MEDIUM, TEXT_COLOR, "Console: " + Utils::VectorToString(UU::App->Store->GetEntryConsoles(UU::App->Store->Indexes[UU::App->Store->SelectedIndex])), 248, 0);
		Gui::DrawString(53, 175, TEXT_MEDIUM, TEXT_COLOR, "Last Updated: " + UU::App->Store->GetEntryLastUpdated(UU::App->Store->Indexes[UU::App->Store->SelectedIndex]), 248, 0);
		Gui::DrawString(53, 190, TEXT_MEDIUM, TEXT_COLOR, "License: " + UU::App->Store->GetEntryLicense(UU::App->Store->Indexes[UU::App->Store->SelectedIndex]), 248, 0);
	}
};


void EntryInfo::Handler() {

};