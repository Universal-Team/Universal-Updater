// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _PLATFORM_3DS_HPP
#define _PLATFORM_3DS_HPP

#include <3ds.h>

/* Include Definitions for Universal-Updater here. */
#define CONFIG_PATH "sdmc:/3ds/Universal-Updater/Config.json"
#define _STORE_PATH "sdmc:/3ds/Universal-Updater/stores/"
#define _SHORTCUT_PATH "sdmc:/3ds/Universal-Updater/shortcuts/"
#define THEME_JSON "sdmc:/3ds/Universal-Updater/Themes.json"

#define SHEET_PATH_KEY "sheet"
#define SHEET_URL_KEY "sheetURL"

/* Meta data related things. */
#define _META_PATH "sdmc:/3ds/Universal-Updater/MetaData.json"
#define _OLD_UPDATE_PATH "sdmc:/3ds/Universal-Updater/updates.json"

#define DEFAULT_BASE_URL "https://github.com/Universal-Team/db/raw/master/docs/unistore/"
#define DEFAULT_UNISTORE_NAME "universal-db.unistore"
#define DEFAULT_SPRITESHEET_NAME "universal-db.t3x"
#define DEFAULT_UNISTORE (DEFAULT_BASE_URL DEFAULT_UNISTORE_NAME)
#define DEFAULT_SPRITESHEET (DEFAULT_BASE_URL DEFAULT_SPRITESHEET_NAME)

typedef Thread ThreadPtr;

namespace Platform {
	inline void ScanKeys(void) { return hidScanInput(); }
	inline u32 KeysDown(void) { return hidKeysDown(); }
	inline u32 KeysDownRepeat(void) { return hidKeysDownRepeat(); }
	inline void TouchRead(touchPosition *data) { hidTouchRead(data); }

	inline bool MainLoop() { return aptMainLoop(); }
	inline void waitForVBlank(void) { return gspWaitForVBlank(); }
	inline void AllowExit(bool allow) { return aptSetHomeAllowed(allow); }

	inline int ThreadJoin(ThreadPtr t, u64 timeout_ns) { return threadJoin(t, timeout_ns); }
	inline ThreadPtr CreateThread(ThreadFunc entrypoint) {
		int32_t Prio = 0;
		svcGetThreadPriority(&Prio, CUR_THREAD_HANDLE);
		return threadCreate(entrypoint, NULL, 64 * 1024, Prio - 1, -2, false);
	}

	void Initialize(char *ARGV[]);
	void Exit(void);

	namespace WiFi {
		inline void Init(void) { /* NOP */ };
		bool Connected(void);
	}
};

#endif
