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
