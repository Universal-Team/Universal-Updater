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

#include "Platform.hpp"

#include "gui.hpp"
#include "nitrofs.h"
#include "tonccpy.h"

#include <dswifi9.h>
#include <fat.h>

// TODO: This is kinda terrible! Only one non-main thread at a time!
Thread thread;
#define THREAD_STACK_SIZE 64 * 1024
alignas(8) u8 threadStack[THREAD_STACK_SIZE];
u8 *stackEnd = &threadStack[THREAD_STACK_SIZE];

void Platform::Initialize(char *ARGV[]) {
	keysSetRepeat(20, 8);

	if (!fatInitDefault()) {
		consoleDemoInit();
		iprintf("FAT init failed!\n");
		while (pmMainLoop()) swiWaitForVBlank();
	}

	/* Try init NitroFS at a few likely locations. */
	if (!nitroFSInit(ARGV[0])) {
		if (!nitroFSInit("Universal-Updater.nds")) {
			if (!nitroFSInit("/_nds/Universal-Updater/Universal-Updater.nds")) {
				consoleDemoInit();
				iprintf("NitroFS init failed!\n\n");
				iprintf("Copy Universal-Updater.nds to\n\n");
				iprintf("/_nds/Universal-Updater/\n");
				iprintf("           Universal-Updater.nds\n");
				iprintf("or launch using TWiLight Menu++\nor nds-hb-menu.");
				while (pmMainLoop()) swiWaitForVBlank();
			}
		}
	}

	/* Initialize graphics. */
	Gui::init(std::array<std::vector<std::string>, UNIVCORE_FONT_COUNT>({{
		{"/_nds/Universal-Updater/large.nftr", "nitro:/graphics/font/large.nftr"},
		{"/_nds/Universal-Updater/medium.nftr", "nitro:/graphics/font/medium.nftr"},
		{"/_nds/Universal-Updater/small.nftr", "nitro:/graphics/font/small.nftr"}
	}}));

	constexpr uint16_t Palette[] = {
		0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xB126, 0xB126, 0xB126, 0xB126, 0x9883, 0x9883, 0x9883, 0x9883, 0xA4A5, 0xA4A5, 0xA4A5, 0xA4A5,
		0xFFFF, 0xB9CE, 0xD6B5, 0xFFFF, 0xCE0D, 0xCE0D, 0xCE0D, 0xCE0D, 0xC189, 0xC189, 0xC189, 0xC189, 0xF735, 0xC1CC, 0xD22E, 0xF735
	};
	tonccpy(BG_PALETTE, Palette, sizeof(Palette));
	tonccpy(BG_PALETTE_SUB, Palette, sizeof(Palette));
}

ThreadPtr Platform::CreateThread(ThreadFunc entrypoint) {
	threadPrepare(&thread, entrypoint, NULL, stackEnd, MAIN_THREAD_PRIO + 1);
	threadStart(&thread);

	return &thread;
}

void Platform::WiFi::Init() {
	// Don't initialize Wi-Fi if using no$gba
	if(strncmp((const char *)0x4FFFA00, "no$gba", 6) != 0)
		Wifi_InitDefault(true);
}

bool Platform::WiFi::Connected() {
	return Wifi_AssocStatus() == ASSOCSTATUS_ASSOCIATED;
}
