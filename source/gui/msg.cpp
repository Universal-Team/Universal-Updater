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

#include "common.hpp"
#include "msg.hpp"

/*
	Displays just a message until the next draw frame.

	std::string Text: The Message.
*/
void Msg::DisplayMsg(std::string text) {
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
	C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));

	GFX::DrawTop();
	Gui::DrawStringCentered(0, (240 - Gui::GetStringHeight(0.6f, text)) / 2, 0.6f, C2D_Color32(255, 255, 255, 255), text, 395, 100);
	GFX::DrawBottom();
	C3D_FrameEnd(0);
}

/*
	Displays a warn message for 3 seconds.

	std::string Text: The Message.
*/
void Msg::DisplayWarnMsg(std::string Text) {
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
	C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));

	GFX::DrawTop();
	Gui::DrawStringCentered(0, 1, 0.6f, C2D_Color32(255, 255, 255, 255), Text, 400);

	GFX::DrawBottom();
	C3D_FrameEnd(0);

	for (int i = 0; i < 60 * 3; i++) {
		gspWaitForVBlank();
	}
}

extern touchPosition touch;
extern bool touching(touchPosition touch, Structs::ButtonPos button);

/*
	Display a Message, which needs to be confirmed with A/B.

	std::string promptMsg: The Message.
*/
bool Msg::promptMsg(std::string promptMsg) {
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));

	GFX::DrawTop();
	Gui::DrawStringCentered(0, (240 - Gui::GetStringHeight(0.6f, promptMsg)) / 2, 0.6f, C2D_Color32(255, 255, 255, 255), promptMsg, 395, 100);

	Gui::Draw_Rect(0, 215, 400, 25, C2D_Color32(50, 73, 98, 255));
	Gui::Draw_Rect(0, 214, 400, 1, C2D_Color32(25, 30, 53, 255));
	Gui::DrawStringCentered(0, 217, 0.6f, C2D_Color32(255, 255, 255, 255), Lang::get("CONFIRM_OR_CANCEL"), 400);
	C3D_FrameEnd(0);

	while(1) {
		gspWaitForVBlank();
		hidScanInput();

		if (hidKeysDown() & KEY_A) return true;
		else if (hidKeysDown() & KEY_B) return false;
	}
}

void Msg::waitMsg(std::string msg) {
	bool doOut = false;

	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));

	GFX::DrawTop();
	Gui::DrawStringCentered(0, (240 - Gui::GetStringHeight(0.6f, msg)) / 2, 0.6f, C2D_Color32(255, 255, 255, 255), msg, 395, 100);

	Gui::Draw_Rect(0, 215, 400, 25, C2D_Color32(50, 73, 98, 255));
	Gui::Draw_Rect(0, 214, 400, 1, C2D_Color32(25, 30, 53, 255));
	Gui::DrawStringCentered(0, 217, 0.6f, C2D_Color32(255, 255, 255, 255), Lang::get("KEY_CONTINUE"), 400);
	C3D_FrameEnd(0);

	while(!doOut) {
		hidScanInput();

		if (hidKeysDown()) doOut = !doOut;
	}
}