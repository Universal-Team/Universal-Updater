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

#include "config.hpp"
#include "init.hpp"
#include "gfx.hpp"
#include "gui.hpp"
#include "lang.hpp"
#include "logging.hpp"
#include "mainMenu.hpp"
#include "screenCommon.hpp"
#include "scriptlist.hpp"
#include "startup.hpp"
#include "sound.h"
#include "unistore.hpp"

#include <3ds.h>
#include <dirent.h>
#include <unistd.h>

bool exiting = false;
bool dspFound = false;
touchPosition touch;
sound *bgm = NULL;
bool songIsFound = false;
bool UniStoreAutoboot = false;
int AutobootWhat = 0; // 0 -> MainMenu ; 1 -> Store; 2 -> Script.
bool changesMade = false;

// Include all spritesheet's.
C2D_SpriteSheet sprites;
std::unique_ptr<Config> config;

// If button Position pressed -> Do something.
bool touching(touchPosition touch, Structs::ButtonPos button) {
	if (touch.px >= button.x && touch.px <= (button.x + button.w) && touch.py >= button.y && touch.py <= (button.y + button.h))
		return true;
	else
		return false;
}

void Init::loadSoundEffects(void) {
	if (dspFound) {
		if (access(config->musicPath().c_str(), F_OK ) != -1) {
			bgm = new sound(config->musicPath(), 1, true);
			songIsFound = true;
		}
	}
}

void Init::playMusic(void) {
	if (songIsFound) {
		bgm->play();
	}
}

void Init::stopMusic(void) {
	if (songIsFound) {
		bgm->stop();
	}
}


Result Init::Initialize() {
	gfxInitDefault();
	romfsInit();
	amInit();
	Gui::init();
	cfguInit();
	acInit();
	// Create Folder if missing.
	mkdir("sdmc:/3ds", 0777);
	mkdir("sdmc:/3ds/Universal-Updater", 0777);
	mkdir("sdmc:/3ds/Universal-Updater/scripts", 0777);
	mkdir("sdmc:/3ds/Universal-Updater/stores", 0777);
	
	// We need to make sure, the file exist.
	config = std::make_unique<Config>();

	Lang::load(config->language());

	if (config->screenFade()) {
		fadein = true;
		fadealpha = 255;
	}

	// In case it takes a bit longer to autoboot a script or so.
	Msg::DisplayStartMSG();
	if (config->logging()) {
		Logging::createLogFile();
	}

	Gui::loadSheet("romfs:/gfx/sprites.t3x", sprites);

	AutobootWhat = config->autoboot();

	if (!config->firstStartup()) {
		if (AutobootWhat == 1) {
			if (access(config->autobootFile().c_str(), F_OK) == 0) {
				Gui::setScreen(std::make_unique<UniStore>(true, config->autobootFile()), false, true);
			} else {
				AutobootWhat = 0;
				config->autoboot(0);
				Gui::setScreen(std::make_unique<MainMenu>(), false, true);
			}
		} else if (AutobootWhat == 2) {
			if (access(config->autobootFile().c_str(), F_OK) == 0) {
				Gui::setScreen(std::make_unique<ScriptList>(), false, true);
			} else {
				AutobootWhat = 0;
				config->autoboot(0);
				Gui::setScreen(std::make_unique<MainMenu>(), false, true);
			}
		} else {
			AutobootWhat = 0;
			config->autoboot(0);
			Gui::setScreen(std::make_unique<MainMenu>(), false, true);
		}
	}

	if (config->firstStartup()) {
		Gui::setScreen(std::make_unique<Startup>(AutobootWhat, config->autobootFile()), false, true);
	}

	osSetSpeedupEnable(true);	// Enable speed-up for New 3DS users

 	if ( access( "sdmc:/3ds/dspfirm.cdc", F_OK ) != -1 ) {
		ndspInit();
		dspFound = true;
		loadSoundEffects();
		playMusic();
	}

	return 0;
}

Result Init::MainLoop() {
	// Initialize everything.
	Initialize();
	// Loop as long as the status is not exiting.
	while (aptMainLoop()) {
		hidScanInput();
		u32 hHeld = hidKeysHeld();
		u32 hDown = hidKeysDown();
		hidTouchRead(&touch);
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(Top, BLACK);
		C2D_TargetClear(Bottom, BLACK);
		Gui::clearTextBufs();
		Gui::DrawScreen(true);
		Gui::ScreenLogic(hDown, hHeld, touch, true, true);
		C3D_FrameEnd(0);
		gspWaitForVBlank();
		if (exiting) {
			if (!fadeout)	break;
		}

		Gui::fadeEffects(16, 16, true);
	}
	// Exit all services and exit the app.
	Exit();
	return 0;
}

Result Init::Exit() {
	if (songIsFound) {
		stopMusic();
	}
	delete bgm;
	if (dspFound) {
		ndspExit();
	}

	config->save();

	Gui::exit();
	Gui::unloadSheet(sprites);
	gfxExit();
	cfguExit();
	acExit();
	amExit();
	romfsExit();
	return 0;
}