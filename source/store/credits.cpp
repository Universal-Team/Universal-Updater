/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 Universal-Team
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

#include "storeUtils.hpp"

/*
	Draw the Credits.
*/
void StoreUtils::DrawCredits() {
	GFX::DrawTop();
	Gui::DrawStringCentered(0, 1, 0.7f, C2D_Color32(255, 255, 255, 255), "Universal-Updater - " + Lang::get("CREDITS"));

	Gui::DrawString(10, 30, 0.5f, C2D_Color32(255, 255, 255, 255), "Universal-Team");
	Gui::DrawString(10, 50, 0.4f, C2D_Color32(255, 255, 255, 255), Lang::get("APP_DEVELOPEMENT"));


	Gui::DrawString(10, 77, 0.5f, C2D_Color32(255, 255, 255, 255), Lang::get("CONTRIBUTORS"));
	Gui::DrawString(10, 97, 0.4f, C2D_Color32(255, 255, 255, 255), Lang::get("CONTRIBUTORS_2"), 390);

	Gui::DrawString(10, 124, 0.5f, C2D_Color32(255, 255, 255, 255), "devkitPro");
	Gui::DrawString(10, 144, 0.4f, C2D_Color32(255, 255, 255, 255), Lang::get("LIBRARIES"), 390);

	Gui::DrawString(10, 171, 0.5f, C2D_Color32(255, 255, 255, 255), Lang::get("TRANSLATORS"));
	Gui::DrawString(10, 191, 0.4f, C2D_Color32(255, 255, 255, 255), Lang::get("TRANSLATORS_2"), 390);

	Gui::Draw_Rect(0, 215, 400, 25, C2D_Color32(50, 73, 98, 255));
	Gui::Draw_Rect(0, 214, 400, 1, C2D_Color32(25, 30, 53, 255));
	Gui::DrawStringCentered(0, 217, 0.6f, C2D_Color32(255, 255, 255, 255), Lang::get("CURRENT_VERSION") + std::string(V_STRING), 390);
}