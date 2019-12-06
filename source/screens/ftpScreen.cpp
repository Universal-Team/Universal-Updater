/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

#include "lang/lang.hpp"

#include "screens/ftpScreen.hpp"
#include "screens/screenCommon.hpp"

#include "utils/config.hpp"

#include <algorithm>
#include <fstream>
#include <unistd.h>

extern "C" {
	#include "ftp.h"
}

void FTPScreen::Draw(void) const
{
	ftp_init();
	Result ret = 0;
	char buf[137], hostname[128];
	u32 wifiStatus = 0;
	ret = gethostname(hostname, sizeof(hostname));

	while(ftpEnabled == 1) {
		ftp_loop();
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		Gui::DrawTop();
		Gui::DrawString((400-Gui::GetStringWidth(0.8f, Lang::get("FTP_MODE")))/2, 0, 0.8f, Config::TxtColor, Lang::get("FTP_MODE"), 400);
		Gui::DrawBottom();
		ret = ACU_GetWifiStatus(&wifiStatus);

		if ((wifiStatus != 0) && R_SUCCEEDED(ret)) {
			Gui::DrawStringCentered(0, 40, 0.48f, Config::TxtColor, Lang::get("FTP_INITIALIZED"), 320);
			snprintf(buf, 137, "IP: %s:5000", R_FAILED(ret)? Lang::get("FAILED_GET_IP").c_str() : hostname);

			if (strlen(ftp_accepted_connection) != 0)
				Gui::DrawStringCentered(0, 80, 0.45f, Config::TxtColor, ftp_accepted_connection, 320);

			if (strlen(ftp_file_transfer) != 0)
				Gui::DrawStringCentered(0, 150, 0.45f, Config::TxtColor, ftp_file_transfer, 320);
		}
		else {
			Gui::DrawStringCentered(0, 40, 0.48f, Config::TxtColor, Lang::get("FAILED_INITIALIZE_FTP"), 320);
			snprintf(buf, 18, Lang::get("WIFI_NOT_ENABLED").c_str());
		}

		Gui::DrawStringCentered(0, 60, 0.48, Config::TxtColor, buf, 320);
		Gui::DrawStringCentered(0, 220, 0.48f, Config::TxtColor, Lang::get("B_FTP_EXIT"), 320);

		Gui::clearTextBufs();
		C3D_FrameEnd(0);
		hidScanInput();
		u32 hDown = hidKeysDown();

		if (hDown & KEY_B)
			break;
	}
	memset(ftp_accepted_connection, 0, 20); // Empty accepted connection address.
	memset(ftp_file_transfer, 0, 50); // Empty transfer status.
	ftp_exit();

	Screen::back();
	return;
}

// Needed here, otherwise it won't compile.
void FTPScreen::Logic(u32 hDown, u32 hHeld, touchPosition touch)
{
}