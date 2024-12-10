// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _PLATFORM_NDS_HPP
#define _PLATFORM_NDS_HPP

#include <nds.h>

/* Include Definitions for Universal-Updater here. */
#define CONFIG_PATH "/_nds/Universal-Updater/Config.json"
#define _STORE_PATH "/_nds/Universal-Updater/stores/"
#define _SHORTCUT_PATH "/_nds/Universal-Updater/shortcuts/"
#define THEME_JSON "/_nds/Universal-Updater/Themes.json"

#define SHEET_PATH_KEY "dsSheet"
#define SHEET_URL_KEY "dsSheetURL"

/* Meta data related things. */
#define _META_PATH "/_nds/Universal-Updater/MetaData.json"
#define _OLD_UPDATE_PATH "/_nds/Universal-Updater/updates.json" // Technically not needed.

#define DEFAULT_BASE_URL "https://github.com/Universal-Team/db/raw/master/docs/unistore/"
#define DEFAULT_UNISTORE_NAME "universal-db.unistore"
#define DEFAULT_SPRITESHEET_NAME "universal-db.tdx"
#define DEFAULT_UNISTORE (DEFAULT_BASE_URL DEFAULT_UNISTORE_NAME)
#define DEFAULT_SPRITESHEET (DEFAULT_BASE_URL DEFAULT_SPRITESHEET_NAME)

typedef Thread *ThreadPtr;

namespace Platform {
	inline void ScanKeys(void) { return scanKeys(); }
	inline u32 KeysDown(void) { return keysDown(); }
	inline u32 KeysDownRepeat(void) { return keysDownRepeat(); }
	inline void TouchRead(touchPosition *data) {
		touchRead(data);
		data->px = data->px * 5 / 4;
		data->py = data->py * 5 / 4;
	}

	inline bool MainLoop() {return pmMainLoop(); }
	inline void waitForVBlank(void) { return threadWaitForVBlank(); }
	inline void AllowExit(bool allow) { (void)allow; /* NOP */ }

	inline int ThreadJoin(ThreadPtr t, u64 timeout_ns) { (void)timeout_ns; return threadJoin(t); }
	ThreadPtr CreateThread(ThreadFunc entrypoint);

	void Initialize(char *ARGV[]);
	inline void Exit() { /* NOP */ }

	namespace WiFi {
		void Init(void);
		bool Connected(void);
	}
}

#endif
