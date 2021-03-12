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
#include "overlay.hpp"
#include "version.hpp"

/* Show the Credits. */
void Overlays::ShowCredits() {
	bool doOut = false;

	while(!doOut) {
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(Top, TRANSPARENT);
		C2D_TargetClear(Bottom, TRANSPARENT);

		GFX::DrawTop();
		GFX::DrawSprite(sprites_universal_updater_idx, 220, 26);
		Gui::DrawStringCentered(0, 1, 0.7f, GFX::Themes[GFX::SelectedTheme].TextColor, "Universal-Updater - " + Lang::get("CREDITS"), 395, 0, font);

		Gui::DrawString(10, 30, 0.5f, GFX::Themes[GFX::SelectedTheme].TextColor, "- Universal-Team", 0, 0, font);
		Gui::DrawString(10, 50, 0.5f, GFX::Themes[GFX::SelectedTheme].TextColor, "- devkitPro", 0, 0, font);
		Gui::DrawString(10, 70, 0.5f, GFX::Themes[GFX::SelectedTheme].TextColor, "- dlbeer", 0, 0, font);
		Gui::DrawString(10, 90, 0.5f, GFX::Themes[GFX::SelectedTheme].TextColor, "- FlagBrew", 0, 0, font);
		Gui::DrawString(10, 110, 0.5f, GFX::Themes[GFX::SelectedTheme].TextColor, "- https://icons8.com/", 0, 0, font);
		Gui::DrawString(10, 130, 0.5f, GFX::Themes[GFX::SelectedTheme].TextColor, "- Ivandeve", 0, 0, font);
		Gui::DrawString(10, 150, 0.5f, GFX::Themes[GFX::SelectedTheme].TextColor, "- PabloMK7", 0, 0, font);
		Gui::DrawString(10, 170, 0.5f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("CONTRIBUTOR_TRANSLATORS"), 210, 0, font);
		Gui::DrawString(10, 197, 0.5f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("GITHUB"), 390, 0, font);

		Gui::Draw_Rect(0, 215, 400, 25, GFX::Themes[GFX::SelectedTheme].BarColor);
		Gui::Draw_Rect(0, 214, 400, 1, GFX::Themes[GFX::SelectedTheme].BarOutline);
		Gui::DrawStringCentered(0, 218, 0.6f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("CURRENT_VERSION") + std::string(VER_NUMBER), 390, 0, font);

		Animation::QueueEntryDone();
		GFX::DrawBottom();
		GFX::DrawSprite(sprites_universal_core_idx, 0, 26);
		C3D_FrameEnd(0);

		hidScanInput();
		Animation::HandleQueueEntryDone();
		if ((hidKeysDown() & KEY_START) || (hidKeysDown() & KEY_B) || (hidKeysDown() & KEY_A)) doOut = true;
	}
}