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