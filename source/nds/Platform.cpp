// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

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
	threadPrepare(&thread, entrypoint, NULL, stackEnd, MAIN_THREAD_PRIO);
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
