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

#include "queueSystem.hpp"
#include "storeUtils.hpp"
#include "structs.hpp"
#include <curl/curl.h>

extern u32 extractSize, writeOffset;
extern u32 installSize, installOffset;
extern u32 copyOffset, copySize;

extern curl_off_t downloadTotal;
extern curl_off_t downloadNow;

#define QUEUE_ENTRIES 2 // 2 Entries per screen or so.
extern bool touching(touchPosition touch, Structs::ButtonPos button);

/* TODO: Rework positions + sizes.. */
static const std::vector<Structs::ButtonPos> QueueBoxes = {
	{ 46, 32, 266, 80 },
	{ 46, 132, 266, 80 }
};

extern std::deque<std::unique_ptr<Queue>> queueEntries;

void DrawStatus(QueueStatus s) {
	char str[256];

	/* String Handle. */
	switch(s) {
		case QueueStatus::None:
		case QueueStatus::Failed:
		case QueueStatus::Done:
			break;

		case QueueStatus::Downloading:
			if (downloadTotal < 1.0f) downloadTotal = 1.0f;
			if (downloadTotal < downloadNow) downloadTotal = downloadNow;

			snprintf(str, sizeof(str), "Downloading... %s / %s (%.2f%%)",
					StringUtils::formatBytes(downloadNow).c_str(),
					StringUtils::formatBytes(downloadTotal).c_str(),
					((float)downloadNow/(float)downloadTotal) * 100.0f);
			break;

		case QueueStatus::Extracting:
			snprintf(str, sizeof(str), "Extracting... %s / %s (%.2f%%)",
					StringUtils::formatBytes(writeOffset).c_str(),
					StringUtils::formatBytes(extractSize).c_str(),
					((float)writeOffset/(float)extractSize) * 100.0f);
			break;

		case QueueStatus::Installing:
			snprintf(str, sizeof(str), "Installing... %s / %s (%.2f%%)",
					StringUtils::formatBytes(installOffset).c_str(),
					StringUtils::formatBytes(installSize).c_str(),
					((float)installOffset/(float)installSize) * 100.0f);
			break;
	}

	/* Draw Handle. */
	switch(s) {
		case QueueStatus::None:
		case QueueStatus::Failed:
		case QueueStatus::Done:
			break;

		case QueueStatus::Downloading:
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 5, 0.4f, TEXT_COLOR, str, 250, 0, font);

			Gui::Draw_Rect(QueueBoxes[0].x + 60, QueueBoxes[0].y + 25, 180, 30, BLACK);
			Gui::Draw_Rect(QueueBoxes[0].x + 60 + 1, QueueBoxes[0].y + 25 + 1, (int)(((float)downloadNow / (float)downloadTotal) * 180.0f), 28, WHITE);
			break;

		case QueueStatus::Extracting:
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 5, 0.4f, TEXT_COLOR, str, 250, 0, font);

			Gui::Draw_Rect(QueueBoxes[0].x + 60, QueueBoxes[0].y + 25, 180, 30, BLACK);
			Gui::Draw_Rect(QueueBoxes[0].x + 60 + 1, QueueBoxes[0].y + 25 + 1, (int)(((float)writeOffset / (float)extractSize) * 180.0f), 28, WHITE);
			break;

		case QueueStatus::Installing:
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 5, 0.4f, TEXT_COLOR, str, 250, 0, font);

			Gui::Draw_Rect(QueueBoxes[0].x + 60, QueueBoxes[0].y + 25, 180, 30, BLACK);
			Gui::Draw_Rect(QueueBoxes[0].x + 60 + 1, QueueBoxes[0].y + 25 + 1, (int)(((float)installOffset / (float)installSize) * 180.0f), 28, WHITE);
			break;
	}
}


void StoreUtils::DrawQueueMenu(const int queueIndex) {
	Gui::Draw_Rect(40, 0, 280, 25, ENTRY_BAR_COLOR);
	Gui::Draw_Rect(40, 25, 280, 1, ENTRY_BAR_OUTL_COLOR);
	Gui::DrawStringCentered(17, 2, 0.6, TEXT_COLOR, "Queue System", 273, 0, font);

	LightLock_Lock(&QueueSystem::lock);

	if (!queueEntries.empty()) {
		GFX::DrawBox(QueueBoxes[0].x, QueueBoxes[0].y, QueueBoxes[0].w, QueueBoxes[0].h, false);

		C2D_DrawImageAt(queueEntries[0]->icn, QueueBoxes[0].x + 5, QueueBoxes[0].y + 21, 0.5f);
		DrawStatus(queueEntries[0]->status);
	}

	LightLock_Unlock(&QueueSystem::lock);
}

void StoreUtils::QueueMenuHandle(int &queueIndex) {

}