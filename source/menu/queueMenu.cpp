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

#include "common.hpp"
#include "queueSystem.hpp"
#include "scriptUtils.hpp"
#include "storeUtils.hpp"
#include "structs.hpp"
#include <curl/curl.h>
#include <math.h> // for std::min.

extern u32 extractSize, writeOffset;
extern u32 installSize, installOffset;
extern u32 copyOffset, copySize;
extern int filesExtracted, extractFilesCount;

extern curl_off_t downloadTotal;
extern curl_off_t downloadNow;
bool ShowQueueProgress = true; // Queue Mode View.
int queueMenuIdx = 0; // Queue Menu Index.

#define QUEUE_ENTRIES 2 // 2 entries per screen or so.
extern bool touching(touchPosition touch, Structs::ButtonPos button);

static const std::vector<Structs::ButtonPos> QueueBoxes = {
	{ 46, 32, 266, 80 },
	{ 46, 132, 266, 80 },
	{ 288, 62, 24, 24 }, // Cancel current Queue.
	{ 288, 162, 24, 24 } // Remove next Queue.
};

extern std::deque<std::unique_ptr<Queue>> queueEntries;

void DrawStatus(QueueStatus s) {
	if (!ShowQueueProgress) {
		if (!queueEntries.empty()) {
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 5, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, queueEntries[0]->name, 250, 0, font);

			char prog[256];
			snprintf(prog, sizeof(prog), Lang::get("QUEUE_PROGRESS").c_str(), queueEntries[0]->current, queueEntries[0]->total);
			Gui::DrawString(QueueBoxes[0].x + 150, QueueBoxes[0].y + 58, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, prog, 250, 0, font);

			Gui::Draw_Rect(QueueBoxes[0].x + 60, QueueBoxes[0].y + 25, 182, 30, GFX::Themes[GFX::SelectedTheme].ProgressbarOut);
			Gui::Draw_Rect(QueueBoxes[0].x + 60 + 1, QueueBoxes[0].y + 25 + 1, (int)(((float)queueEntries[0]->current / (float)queueEntries[0]->total) * 180.0f), 28, GFX::Themes[GFX::SelectedTheme].ProgressbarIn);

			switch(s) {
				case QueueStatus::Done:
				case QueueStatus::Failed:
				case QueueStatus::None:
					break;

				case QueueStatus::Copying:
					Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 68, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("OP_CURRENT") + Lang::get("OP_COPYING"), 250, 0, font);
					break;

				case QueueStatus::Deleting:
					Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 68, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("OP_CURRENT") + Lang::get("OP_DELETING"), 250, 0, font);
					break;

				case QueueStatus::Downloading:
					Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 68, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("OP_CURRENT") + Lang::get("OP_DOWNLOADING"), 250, 0, font);
					break;

				case QueueStatus::Extracting:
					Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 68, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("OP_CURRENT") + Lang::get("OP_EXTRACTING"), 250, 0, font);
					break;

				case QueueStatus::Installing:
					Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 68, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("OP_CURRENT") + Lang::get("OP_INSTALLING"), 250, 0, font);
					break;

				case QueueStatus::Request:
					Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 68, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("OP_CURRENT") + Lang::get("OP_WAITING"), 250, 0, font);
					break;
			}
		}

		return;
	}

	char str[256], str2[256];

	/* Progress. */
	if (!queueEntries.empty()) {
		char prog[256];
		snprintf(prog, sizeof(prog), Lang::get("QUEUE_PROGRESS").c_str(), queueEntries[0]->current, queueEntries[0]->total);
		Gui::DrawString(QueueBoxes[0].x + 150, QueueBoxes[0].y + 62, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, prog, 250, 0, font);
	}

	/* String Handle. */
	switch(s) {
		case QueueStatus::Done:
		case QueueStatus::Failed:
		case QueueStatus::None:
			break;

		case QueueStatus::Copying:
			snprintf(str, sizeof(str), Lang::get("COPYING").c_str(),
					StringUtils::formatBytes(copyOffset).c_str(),
					StringUtils::formatBytes(copySize).c_str(),
					((float)copyOffset/(float)copySize) * 100.0f);
			break;

		case QueueStatus::Deleting:
			snprintf(str, sizeof(str), Lang::get("DELETING").c_str());
			break;

		case QueueStatus::Downloading:
			if (downloadTotal < 1.0f) downloadTotal = 1.0f;
			if (downloadTotal < downloadNow) downloadTotal = downloadNow;

			snprintf(str, sizeof(str), Lang::get("DOWNLOADING").c_str(),
					StringUtils::formatBytes(downloadNow).c_str(),
					StringUtils::formatBytes(downloadTotal).c_str(),
					((float)downloadNow/(float)downloadTotal) * 100.0f);
			break;

		case QueueStatus::Extracting:
			snprintf(str, sizeof(str), Lang::get("EXTRACTING").c_str(),
					StringUtils::formatBytes(writeOffset).c_str(),
					StringUtils::formatBytes(extractSize).c_str(),
					((float)writeOffset/(float)extractSize) * 100.0f);

			snprintf(str2, sizeof(str2), Lang::get("FILES").c_str(),
					filesExtracted, extractFilesCount);

			break;

		case QueueStatus::Installing:
			snprintf(str, sizeof(str), Lang::get("INSTALLING").c_str(),
					StringUtils::formatBytes(installOffset).c_str(),
					StringUtils::formatBytes(installSize).c_str(),
					((float)installOffset/(float)installSize) * 100.0f);
			break;

		case QueueStatus::Request:
			snprintf(str, sizeof(str), Lang::get("ACTION_REQUIRED").c_str());
			break;
	}

	/* Draw Handle. */
	switch(s) {
		case QueueStatus::Done:
		case QueueStatus::Failed:
		case QueueStatus::None:
			break;

		case QueueStatus::Copying:
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 5, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, str, 250, 0, font);
			Gui::Draw_Rect(QueueBoxes[0].x + 60, QueueBoxes[0].y + 25, 182, 30, GFX::Themes[GFX::SelectedTheme].ProgressbarOut);
			Gui::Draw_Rect(QueueBoxes[0].x + 60 + 1, QueueBoxes[0].y + 25 + 1, (int)(((float)copyOffset / (float)copySize) * 180.0f), 28, GFX::Themes[GFX::SelectedTheme].ProgressbarIn);
			break;

		case QueueStatus::Deleting:
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 5, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, str, 250, 0, font);
			break;

		case QueueStatus::Downloading:
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 5, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, str, 250, 0, font);
			Gui::Draw_Rect(QueueBoxes[0].x + 60, QueueBoxes[0].y + 25, 182, 30, GFX::Themes[GFX::SelectedTheme].ProgressbarOut);
			Gui::Draw_Rect(QueueBoxes[0].x + 60 + 1, QueueBoxes[0].y + 25 + 1, (int)(((float)downloadNow / (float)downloadTotal) * 180.0f), 28, GFX::Themes[GFX::SelectedTheme].ProgressbarIn);
			break;

		case QueueStatus::Extracting:
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 5, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, str, 250, 0, font);
			Gui::Draw_Rect(QueueBoxes[0].x + 60, QueueBoxes[0].y + 25, 182, 30, GFX::Themes[GFX::SelectedTheme].ProgressbarOut);
			Gui::Draw_Rect(QueueBoxes[0].x + 60 + 1, QueueBoxes[0].y + 25 + 1, (int)(((float)writeOffset / (float)extractSize) * 180.0f), 28, GFX::Themes[GFX::SelectedTheme].ProgressbarIn);
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 62, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, str2, 250, 0, font);
			break;

		case QueueStatus::Installing:
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 5, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, str, 250, 0, font);
			Gui::Draw_Rect(QueueBoxes[0].x + 60, QueueBoxes[0].y + 25, 182, 30, GFX::Themes[GFX::SelectedTheme].ProgressbarOut);
			Gui::Draw_Rect(QueueBoxes[0].x + 60 + 1, QueueBoxes[0].y + 25 + 1, (int)(((float)installOffset / (float)installSize) * 180.0f), 28, GFX::Themes[GFX::SelectedTheme].ProgressbarIn);
			break;

		case QueueStatus::Request:
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 5, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, str, 250, 0, font);
			break;
	}
}

void StoreUtils::DrawQueueMenu(const int queueIndex) {
	Gui::Draw_Rect(40, 0, 280, 25, GFX::Themes[GFX::SelectedTheme].EntryBar);
	Gui::Draw_Rect(40, 25, 280, 1, GFX::Themes[GFX::SelectedTheme].EntryOutline);
	Gui::DrawStringCentered(17, 2, 0.6, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("QUEUE"), 273, 0, font);

	LightLock_Lock(&QueueSystem::lock);
	if (!queueEntries.empty() && !QueueSystem::DoNothing) {
		Gui::Draw_Rect(QueueBoxes[0].x, QueueBoxes[0].y, QueueBoxes[0].w, QueueBoxes[0].h, GFX::Themes[GFX::SelectedTheme].MarkSelected);
		C2D_DrawImageAt(queueEntries[0]->icn, QueueBoxes[0].x + 5, QueueBoxes[0].y + 21, 0.5f);
		DrawStatus(queueEntries[0]->status);
		//GFX::DrawSprite(sprites_cancel_idx, QueueBoxes[2].x, QueueBoxes[2].y); // Don't show until properly implemented.

		/* The next Queue Entries being displayed below. */
		if ((1 + queueMenuIdx) < (int)queueEntries.size()) {
			Gui::Draw_Rect(QueueBoxes[1].x, QueueBoxes[1].y, QueueBoxes[1].w, QueueBoxes[1].h, GFX::Themes[GFX::SelectedTheme].MarkUnselected);
			C2D_DrawImageAt(queueEntries[1 + queueMenuIdx]->icn, QueueBoxes[1].x + 5, QueueBoxes[1].y + 21, 0.5f);
			Gui::DrawString(QueueBoxes[1].x + 10, QueueBoxes[1].y + 5, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, queueEntries[1 + queueMenuIdx]->name, 250, 0, font);

			Gui::DrawString(QueueBoxes[1].x + 60, QueueBoxes[1].y + 30, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("QUEUE_POSITION") + ": " + std::to_string(queueMenuIdx + 2), 0, 0, font);

			/* Cancel. */
			GFX::DrawSprite(sprites_cancel_idx, QueueBoxes[3].x, QueueBoxes[3].y);
		}
	}

	LightLock_Unlock(&QueueSystem::lock);
}

void StoreUtils::QueueMenuHandle(int &queueIndex, int &storeMode) {
	if (!queueEntries.empty()) {
		if ((1 + queueMenuIdx) > (int)queueEntries.size() - 1) queueMenuIdx = std::max<int>((int)(queueEntries.size() - 1) - 1, 0); // Ensure this really doesn't go below 0.
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, QueueBoxes[0])) {
			if (QueueSystem::RequestNeeded != -1) { // -1 means no request.
				switch(QueueSystem::RequestNeeded) {
					case 1: // Remove Directory message.
						QueueSystem::RequestAnswer = Msg::promptMsg(QueueSystem::RequestMsg);
						QueueSystem::Wait = false;
						QueueSystem::Resume();
						break;

					case 2: // Skip prompt message.
						QueueSystem::RequestAnswer = ScriptUtils::prompt(QueueSystem::RequestMsg);
						QueueSystem::Wait = false;
						QueueSystem::Resume();
						break;
				}

			} else {
				ShowQueueProgress = !ShowQueueProgress; // In case no request expected, switch from progress to total progress mode etc.
			}

			/* Current Queue Cancel. */
		} else if (touching(touch, QueueBoxes[2])) { // TODO: Cancel current Queue.

			/* Remove from Queue. */
		} else if (touching(touch, QueueBoxes[3])) { // Remove Queue entries.
			if (!QueueSystem::DoNothing && queueEntries.size() > 1) queueEntries.erase(queueEntries.begin() + 1 + queueMenuIdx);
		}
	}

	if (hDown & KEY_DOWN) {
		if (!queueEntries.empty()) {
			if ((1 + queueMenuIdx) < (int)queueEntries.size() - 1) queueMenuIdx++;
		}
	}

	if (hDown & KEY_UP) {
		if (queueMenuIdx > 0) queueMenuIdx--;
	}

	if (hDown & KEY_B) storeMode = 0; // Go to EntryInfo.
}