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
#include "init.hpp"
#include "mainScreen.hpp"

#include <dirent.h>
#include <unistd.h>

bool exiting = false;
touchPosition touch;
C2D_SpriteSheet sprites;

/*
	If button Position pressed -> Do something.

	touchPosition touch: The TouchPosition variable.
	Structs::ButtonPos button: The Button Struct.
*/
bool touching(touchPosition touch, Structs::ButtonPos button) {
	if (touch.px >= button.x && touch.px <= (button.x + button.w) && touch.py >= button.y && touch.py <= (button.y + button.h)) return true;
	return false;
}

/*
	Initialize Universal-Updater.
*/
Result Init::Initialize() {
	gfxInitDefault();
	romfsInit();
	Gui::init();

	cfguInit();
	amInit();
	acInit();
	Lang::load("en");

	/* Create Directories, if missing. */
	mkdir("sdmc:/3ds", 0777);
	mkdir("sdmc:/3ds/Universal-Updater", 0777);
	mkdir("sdmc:/3ds/Universal-Updater/scripts", 0777);
	mkdir("sdmc:/3ds/Universal-Updater/stores", 0777);

	Gui::loadSheet("romfs:/gfx/sprites.t3x", sprites);

	osSetSpeedupEnable(true); // Enable speed-up for New 3DS users.
	Gui::setScreen(std::make_unique<MainScreen>(), false, false);
	return 0;
}

/*
	MainLoop of Universal-Updater.
*/
Result Init::MainLoop() {
	bool fullExit = false;

	Initialize();
	hidSetRepeatParameters(10, 10);

	/* Loop as long as the status is not fullExit. */
	while (aptMainLoop() && !fullExit) {
		hidScanInput();
		u32 hHeld = hidKeysHeld();
		u32 hDown = hidKeysDown();
		hidTouchRead(&touch);

		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
		C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));

		Gui::DrawScreen(false);
		Gui::ScreenLogic(hDown, hHeld, touch, true, false);
		C3D_FrameEnd(0);

		if (exiting) {
			fullExit = true;
		}
	}

	/* Exit all services and exit the app. */
	Exit();
	return 0;
}

/*
	Exit Universal-Updater.
*/
Result Init::Exit() {
	Gui::exit();
	Gui::unloadSheet(sprites);

	gfxExit();
	cfguExit();

	acExit();
	amExit();

	romfsExit();
	return 0;
}