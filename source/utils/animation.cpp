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

#include "animation.hpp"
#include "common.hpp"
#include "stringutils.hpp"
#include <curl/curl.h>

extern int filesExtracted, extractFilesCount;
extern std::string extractingFile;
char progressBarMsg[128] = "";
bool showProgressBar = false;
ProgressBar progressbarType = ProgressBar::Downloading;

extern u32 extractSize, writeOffset;
extern u32 installSize, installOffset;
extern u32 copyOffset, copySize;

extern curl_off_t downloadTotal;
extern curl_off_t downloadNow;

/*
	Draw the progressbar.

	u64 currentProgress: The current progress.
	u64 totalProgress: The total progress.
*/
void Animation::DrawProgressBar(u64 currentProgress, u64 totalProgress) {
	Gui::Draw_Rect(30, 120, 340, 30, PROGRESSBAR_OUT_COLOR);
	Gui::Draw_Rect(31, 121, (int)(((float)currentProgress / (float)totalProgress) * 338.0f), 28, PROGRESSBAR_IN_COLOR);
}

/*
	Display the progressbar.
*/
void Animation::displayProgressBar() {
	char str[256];

	while(showProgressBar) {
		switch(progressbarType) {
			case ProgressBar::Downloading:
				if (downloadTotal < 1.0f) downloadTotal = 1.0f;
				if (downloadTotal < downloadNow) downloadTotal = downloadNow;

				snprintf(str, sizeof(str), "%s / %s (%.2f%%)",
						StringUtils::formatBytes(downloadNow).c_str(),
						StringUtils::formatBytes(downloadTotal).c_str(),
						((float)downloadNow/(float)downloadTotal) * 100.0f);
				break;

			case ProgressBar::Extracting:
				snprintf(str, sizeof(str), "%s / %s (%.2f%%)",
						StringUtils::formatBytes(writeOffset).c_str(),
						StringUtils::formatBytes(extractSize).c_str(),
						((float)writeOffset/(float)extractSize) * 100.0f);
				break;

			case ProgressBar::Installing:
				snprintf(str, sizeof(str), "%s / %s (%.2f%%)",
						StringUtils::formatBytes(installOffset).c_str(),
						StringUtils::formatBytes(installSize).c_str(),
						((float)installOffset/(float)installSize) * 100.0f);
				break;

			case ProgressBar::Copying:
				snprintf(str, sizeof(str), "%s / %s (%.2f%%)",
						StringUtils::formatBytes(copyOffset).c_str(),
						StringUtils::formatBytes(copySize).c_str(),
						((float)copyOffset/(float)copySize) * 100.0f);
				break;
		}

		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(Top, TRANSPARENT);
		C2D_TargetClear(Bottom, TRANSPARENT);
		GFX::DrawTop();
		Gui::DrawStringCentered(0, 1, 0.7f, TEXT_COLOR, progressBarMsg, 390, 0, font);

		switch(progressbarType) {
			case ProgressBar::Downloading:
				Gui::DrawStringCentered(0, 80, 0.6f, TEXT_COLOR, str, 390, 0, font);
				Animation::DrawProgressBar(downloadNow, downloadTotal);
				break;

			case ProgressBar::Extracting:
				Gui::DrawStringCentered(0, 180, 0.6f, TEXT_COLOR, str, 390, 0, font);
				Gui::DrawStringCentered(0, 100, 0.6f, TEXT_COLOR, std::to_string(filesExtracted) + " / " + std::to_string(extractFilesCount) + " " + (filesExtracted == 1 ? (Lang::get("FILE_EXTRACTED")).c_str() :(Lang::get("FILES_EXTRACTED"))), 390, 0, font);
				Gui::DrawStringCentered(0, 40, 0.6f, TEXT_COLOR, Lang::get("CURRENTLY_EXTRACTING") + "\n" + extractingFile, 390, 0, font);
				Animation::DrawProgressBar(writeOffset, extractSize);
				break;

			case ProgressBar::Installing:
				Gui::DrawStringCentered(0, 80, 0.6f, TEXT_COLOR, str, 390, 0, font);
				Animation::DrawProgressBar(installOffset, installSize);
				break;

			case ProgressBar::Copying:
				Gui::DrawStringCentered(0, 80, 0.6f, TEXT_COLOR, str, 390, 0, font);
				Animation::DrawProgressBar(copyOffset, copySize);
				break;
		}

		GFX::DrawBottom();
		C3D_FrameEnd(0);
	}
}