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

#include "common.hpp"
#include "msg.hpp"

extern bool touching(touchPosition touch, Structs::ButtonPos button);

const std::vector<Structs::ButtonPos> promptButtons = {
	{24, 94, 124, 48},
	{172, 94, 124, 48}
};
const std::vector<std::string> promptLabels = {
	"CANCEL", "CONFIRM"
};

/*
	Displays just a message until the next draw frame.

	const std::string &Text: The Message, which should be displayed.
*/
void Msg::DisplayMsg(const std::string &Text) {
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, TRANSPARENT);
	C2D_TargetClear(Bottom, TRANSPARENT);

	GFX::DrawTop();
	Gui::DrawStringCentered(0, (240 - Gui::GetStringHeight(0.6f, Text)) / 2, 0.6f, UIThemes->TextColor(), Text, 395, 0, font);
	GFX::DrawBottom();
	C3D_FrameEnd(0);
}

/*
	Displays a warn message for 3 seconds.

	const std::string &Text: The Message, which should be displayed.
*/
void Msg::DisplayWarnMsg(const std::string &Text) {
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, TRANSPARENT);
	C2D_TargetClear(Bottom, TRANSPARENT);

	GFX::DrawTop();
	Gui::DrawStringCentered(0, 1, 0.6f, UIThemes->TextColor(), Text, 390, 0, font);

	GFX::DrawBottom();
	C3D_FrameEnd(0);

	for (int i = 0; i < 60 * 3; i++) {
		gspWaitForVBlank();
	}
}

/*
	Display a Message, which needs to be confirmed with A/B.

	const std::string &promptMsg: The Message, which should be displayed.
*/
bool Msg::promptMsg(const std::string &promptMsg) {
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, TRANSPARENT);
	C2D_TargetClear(Bottom, TRANSPARENT);

	GFX::DrawTop();
	Gui::Draw_Rect(0, 215, 400, 25, UIThemes->BarColor());
	Gui::Draw_Rect(0, 214, 400, 1, UIThemes->BarOutline());
	Gui::DrawStringCentered(0, (240 - Gui::GetStringHeight(0.6f, promptMsg)) / 2, 0.6f, UIThemes->TextColor(), promptMsg, 395, 0, font);

	Gui::DrawStringCentered(0, 218, 0.6f, UIThemes->TextColor(), Lang::get("CONFIRM_OR_CANCEL"), 390, 0, font);
	GFX::DrawBottom();
	for(uint i = 0; i < promptButtons.size(); i++) {
		Gui::Draw_Rect(promptButtons[i].x, promptButtons[i].y, promptButtons[i].w, promptButtons[i].h, UIThemes->BarColor());
		Gui::DrawStringCentered(promptButtons[i].x - 160 + promptButtons[i].w / 2, promptButtons[i].y + 15, 0.6f, UIThemes->TextColor(), Lang::get(promptLabels[i]), promptButtons[i].w - 10, 0, font);
	}
	C3D_FrameEnd(0);

	for (int i = 0; i < 3; i++) gspWaitForVBlank();
	hidScanInput();

	uint32_t Down = 0;
	touchPosition Touch;
	while(1) {
		do {
			gspWaitForVBlank();
			hidScanInput();
			Down = hidKeysDown();
			hidTouchRead(&Touch);
		} while (!Down);

		if ((Down & KEY_A) || (Down & KEY_TOUCH && touching(Touch, promptButtons[1]))) return true;
		else if ((Down & KEY_B) || (Down & KEY_TOUCH && touching(Touch, promptButtons[0]))) return false;
	};
}

/*
	Display a message, which can be "confirmed" with any key.

	const std::string &msg: The message which should be displayed.
*/
void Msg::waitMsg(const std::string &msg) {
	bool doOut = false;

	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, TRANSPARENT);
	C2D_TargetClear(Bottom, TRANSPARENT);

	GFX::DrawTop();
	Gui::DrawStringCentered(0, (240 - Gui::GetStringHeight(0.6f, msg)) / 2, 0.6f, UIThemes->TextColor(), msg, 395, 0, font);
	Gui::Draw_Rect(0, 215, 400, 25, UIThemes->BarColor());
	Gui::Draw_Rect(0, 214, 400, 1, UIThemes->BarOutline());
	Gui::DrawStringCentered(0, 218, 0.6f, UIThemes->TextColor(), Lang::get("KEY_CONTINUE"), 390, 0, font);
	GFX::DrawBottom();
	C3D_FrameEnd(0);

	for (int i = 0; i < 3; i++) gspWaitForVBlank();
	hidScanInput();

	uint32_t Down = 0;
	while(!doOut) {
		do {
			gspWaitForVBlank();
			hidScanInput();
			Down = hidKeysDown();
		} while (!Down);

		doOut = true;
	}
}