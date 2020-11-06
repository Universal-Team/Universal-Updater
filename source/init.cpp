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
#include "download.hpp"
#include "init.hpp"
#include "mainScreen.hpp"

#include <dirent.h>
#include <unistd.h>

bool exiting = false, is3DSX = false;
C2D_SpriteSheet sprites;
int fadeAlpha = 0;
u32 old_time_limit;

/*
	Set, if 3DSX or CIA.
*/
static void getCurrentUsage(){
	u64 id;
	APT_GetProgramID(&id);
	is3DSX = (id != 0x0004000004391700);
}

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

	APT_GetAppCpuTimeLimit(&old_time_limit);
	APT_SetAppCpuTimeLimit(30); // Needed for QR Scanner to work.
	getCurrentUsage();
	aptSetSleepAllowed(false);

	/* Create Directories, if missing. */
	mkdir("sdmc:/3ds", 0777);
	mkdir("sdmc:/3ds/Universal-Updater", 0777);
	mkdir("sdmc:/3ds/Universal-Updater/stores", 0777);

	config = std::make_unique<Config>();
	Lang::load(config->language());

	Gui::loadSheet("romfs:/gfx/sprites.t3x", sprites);


	osSetSpeedupEnable(true); // Enable speed-up for New 3DS users.

	/* Check here for updates. */
	if (config->updatecheck()) {
		if (IsUUUpdateAvailable()) UpdateAction();
	}

	if (exiting) return -1; // In case the update was successful.

	Gui::setScreen(std::make_unique<MainScreen>(), false, false);
	return 0;
}

/*
	MainLoop of Universal-Updater.
*/
Result Init::MainLoop() {
	bool fullExit = false;

	if (Initialize() == -1) fullExit = true;
	hidSetRepeatParameters(20, 8);

	/* Loop as long as the status is not fullExit. */
	while (aptMainLoop() && !fullExit) {
		hidScanInput();
		hHeld = hidKeysHeld();
		hDown = hidKeysDown();
		hRepeat = hidKeysDownRepeat();
		hidTouchRead(&touch);

		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
		C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));

		Gui::DrawScreen(false);
		if (!exiting) Gui::ScreenLogic(hDown, hHeld, touch, true, false);
		C3D_FrameEnd(0);

		if (exiting) {
			if (hDown & KEY_START) fullExit = true; // Make it optionally faster.

			if (fadeAlpha < 255) {
				fadeAlpha += 4;
				if (fadeAlpha >= 255) {
					fullExit = true;
				}
			}
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
	config->save();
	acExit();
	amExit();

    if (old_time_limit != UINT32_MAX) APT_SetAppCpuTimeLimit(old_time_limit); // Restore old limit.
	aptSetSleepAllowed(true);

	romfsExit();
	return 0;
}