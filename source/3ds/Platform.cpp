// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team


#include "Platform.hpp"

#include "gui.hpp"
#include "Utils.hpp"

#include <dirent.h>


/*
	Initialize everything as needed.
*/
void Platform::Initialize(char *ARGV[]) {
	(void)ARGV;

	romfsInit();
	gfxInitDefault();
	Gui::init();
	acInit();
	hidSetRepeatParameters(20, 8);
	osSetSpeedupEnable(true); // Enable speed-up for New 3DS users.
};

void Platform::Exit() {
	acExit();
	Gui::exit();
	gfxExit();
	romfsExit();
}

bool Platform::WiFi::Connected() {
	// return true; // For Citra

	uint32_t WifiStatus;
	return R_SUCCEEDED(ACU_GetWifiStatus(&WifiStatus)) && WifiStatus;
}
