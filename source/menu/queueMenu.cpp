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
extern bool exiting, QueueRuns;

extern curl_off_t downloadTotal;
extern curl_off_t downloadNow;
extern curl_off_t downloadSpeed;
extern CURL *CurlHandle;
bool ShowQueueProgress = true; // Queue Mode View.
int queueMenuIdx = 0; // Queue Menu Index.

#define QUEUE_ENTRIES 2 // 2 entries per screen or so.
extern bool touching(touchPosition touch, Structs::ButtonPos button);

static const std::vector<Structs::ButtonPos> QueueBoxes = {
	{ 47, 36, 266, 90 },
	{ 47, 139, 266, 90 },
	{ 292, 37, 20, 20 }, // Cancel current Queue.
	{ 292, 140, 20, 20 } // Remove next Queue.
};

extern std::deque<std::unique_ptr<Queue>> queueEntries;

void DrawStatus(QueueStatus s) {
	if (!ShowQueueProgress) {
		if (!queueEntries.empty()) {
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 5, 0.4f, UIThemes->TextColor(), queueEntries[0]->name, 230, 0, font);

			char prog[256];
			snprintf(prog, sizeof(prog), Lang::get("QUEUE_PROGRESS").c_str(), queueEntries[0]->current, queueEntries[0]->total);
			Gui::DrawString(QueueBoxes[0].x + 241, QueueBoxes[0].y + 68, 0.4f, UIThemes->TextColor(), prog, 80, 0, font, C2D_AlignRight);

			Gui::Draw_Rect(QueueBoxes[0].x + 60, QueueBoxes[0].y + 30, 182, 30, UIThemes->ProgressbarOut());
			Gui::Draw_Rect(QueueBoxes[0].x + 60 + 1, QueueBoxes[0].y + 30 + 1, (int)(((float)queueEntries[0]->current / (float)queueEntries[0]->total) * 180.0f), 28, UIThemes->ProgressbarIn());

			switch(s) {
				case QueueStatus::Done:
				case QueueStatus::Failed:
				case QueueStatus::None:
					break;

				case QueueStatus::Copying:
					Gui::DrawString(QueueBoxes[0].x + 60, QueueBoxes[0].y + 68, 0.4f, UIThemes->TextColor(), Lang::get("OP_COPYING"), 120, 0, font);
					break;

				case QueueStatus::Deleting:
					Gui::DrawString(QueueBoxes[0].x + 60, QueueBoxes[0].y + 68, 0.4f, UIThemes->TextColor(), Lang::get("OP_DELETING"), 120, 0, font);
					break;

				case QueueStatus::Downloading:
					Gui::DrawString(QueueBoxes[0].x + 60, QueueBoxes[0].y + 68, 0.4f, UIThemes->TextColor(), Lang::get("OP_DOWNLOADING"), 120, 0, font);
					break;

				case QueueStatus::Extracting:
					Gui::DrawString(QueueBoxes[0].x + 60, QueueBoxes[0].y + 68, 0.4f, UIThemes->TextColor(), Lang::get("OP_EXTRACTING"), 120, 0, font);
					break;

				case QueueStatus::Installing:
					Gui::DrawString(QueueBoxes[0].x + 60, QueueBoxes[0].y + 68, 0.4f, UIThemes->TextColor(), Lang::get("OP_INSTALLING"), 120, 0, font);
					break;

				case QueueStatus::Moving:
					Gui::DrawString(QueueBoxes[0].x + 60, QueueBoxes[0].y + 68, 0.4f, UIThemes->TextColor(), Lang::get("OP_MOVING"), 120, 0, font);
					break;

				case QueueStatus::Request:
					Gui::DrawString(QueueBoxes[0].x + 60, QueueBoxes[0].y + 68, 0.4f, UIThemes->TextColor(), Lang::get("OP_WAITING"), 120, 0, font);
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
		Gui::DrawString((QueueBoxes[0].x + 241), QueueBoxes[0].y + 68, 0.4f, UIThemes->TextColor(), prog, 80, 0, font, C2D_AlignRight);
	}

	/* String Handle. */
	switch(s) {
		case QueueStatus::Done:
		case QueueStatus::Failed:
		case QueueStatus::None:
		case QueueStatus::Moving:
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
			if (CurlHandle) curl_easy_getinfo(CurlHandle, CURLINFO_SPEED_DOWNLOAD_T, &downloadSpeed);
			else downloadSpeed = 0;

			if (downloadTotal < 1.0f) downloadTotal = 1.0f;
			if (downloadTotal < downloadNow) downloadTotal = downloadNow;

			snprintf(str, sizeof(str), Lang::get("DOWNLOADING").c_str(),
					StringUtils::formatBytes(downloadNow).c_str(),
					StringUtils::formatBytes(downloadTotal).c_str(),
					((float)downloadNow/(float)downloadTotal) * 100.0f);

			snprintf(str2, sizeof(str2), Lang::get("DOWNLOAD_SPEED").c_str(),
					((downloadSpeed / 1024)));
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
			snprintf(str, sizeof(str), Lang::get("OP_WAITING").c_str());
			snprintf(str2, sizeof(str2), Lang::get("ACTION_REQUIRED").c_str());
			break;
	}

	/* Draw Handle. */
	switch(s) {
		case QueueStatus::Done:
		case QueueStatus::Failed:
		case QueueStatus::None:
			break;

		case QueueStatus::Copying:
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 5, 0.4f, UIThemes->TextColor(), str, 230, 0, font);
			Gui::Draw_Rect(QueueBoxes[0].x + 60, QueueBoxes[0].y + 30, 182, 30, UIThemes->ProgressbarOut());
			Gui::Draw_Rect(QueueBoxes[0].x + 60 + 1, QueueBoxes[0].y + 30 + 1, (int)(((float)copyOffset / (float)copySize) * 180.0f), 28, UIThemes->ProgressbarIn());
			break;

		case QueueStatus::Deleting:
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 5, 0.4f, UIThemes->TextColor(), str, 230, 0, font);
			break;

		case QueueStatus::Downloading:
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 5, 0.4f, UIThemes->TextColor(), str, 230, 0, font);
			Gui::Draw_Rect(QueueBoxes[0].x + 60, QueueBoxes[0].y + 30, 182, 30, UIThemes->ProgressbarOut());
			Gui::Draw_Rect(QueueBoxes[0].x + 60 + 1, QueueBoxes[0].y + 30 + 1, (int)(((float)downloadNow / (float)downloadTotal) * 180.0f), 28, UIThemes->ProgressbarIn());
			Gui::DrawString(QueueBoxes[0].x + 60, QueueBoxes[0].y + 68, 0.4f, UIThemes->TextColor(), str2, 120, 0, font);
			break;

		case QueueStatus::Extracting:
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 5, 0.4f, UIThemes->TextColor(), str, 230, 0, font);
			Gui::Draw_Rect(QueueBoxes[0].x + 60, QueueBoxes[0].y + 30, 182, 30, UIThemes->ProgressbarOut());
			Gui::Draw_Rect(QueueBoxes[0].x + 60 + 1, QueueBoxes[0].y + 30 + 1, (int)(((float)writeOffset / (float)extractSize) * 180.0f), 28, UIThemes->ProgressbarIn());
			Gui::DrawString(QueueBoxes[0].x + 60, QueueBoxes[0].y + 68, 0.4f, UIThemes->TextColor(), str2, 120, 0, font);
			break;

		case QueueStatus::Installing:
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 5, 0.4f, UIThemes->TextColor(), str, 230, 0, font);
			Gui::Draw_Rect(QueueBoxes[0].x + 60, QueueBoxes[0].y + 30, 182, 30, UIThemes->ProgressbarOut());
			Gui::Draw_Rect(QueueBoxes[0].x + 60 + 1, QueueBoxes[0].y + 30 + 1, (int)(((float)installOffset / (float)installSize) * 180.0f), 28, UIThemes->ProgressbarIn());
			break;

		case QueueStatus::Moving:
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 5, 0.4f, UIThemes->TextColor(), Lang::get("OP_MOVING"), 230, 0, font);
			break;

		case QueueStatus::Request:
			Gui::DrawString(QueueBoxes[0].x + 10, QueueBoxes[0].y + 5, 0.4f, UIThemes->TextColor(), str, 230, 0, font);
			Gui::Draw_Rect(QueueBoxes[0].x + 60, QueueBoxes[0].y + 30, 182, 30, UIThemes->ProgressbarOut());
			Gui::DrawStringCentered(QueueBoxes[0].x + 151 - 160, QueueBoxes[0].y + 32, 0.8f, UIThemes->TextColor(), str2, 180, 0, font);
			break;
	}
}

void StoreUtils::DrawQueueMenu(const int queueIndex) {
	Gui::Draw_Rect(40, 0, 280, 25, UIThemes->EntryBar());
	Gui::Draw_Rect(40, 25, 280, 1, UIThemes->EntryOutline());
	Gui::DrawStringCentered(17, 2, 0.6, UIThemes->TextColor(), Lang::get("QUEUE"), 273, 0, font);

	if (!queueEntries.empty()) {
		Gui::Draw_Rect(QueueBoxes[0].x, QueueBoxes[0].y, QueueBoxes[0].w, QueueBoxes[0].h, UIThemes->MarkSelected());

		const C2D_Image tempImg = queueEntries[0]->icn;
		const uint8_t offsetW = (48 - tempImg.subtex->width) / 2; // Center W.
		const uint8_t offsetH = (48 - tempImg.subtex->height) / 2; // Center H.
		C2D_DrawImageAt(tempImg, QueueBoxes[0].x + 5 + offsetW, QueueBoxes[0].y + 21 + offsetH, 0.5f);

		DrawStatus(queueEntries[0]->status);
		GFX::DrawIcon(sprites_cancel_idx, QueueBoxes[2].x, QueueBoxes[2].y, UIThemes->TextColor());

		/* The next Queue Entries being displayed below. */
		if ((1 + queueMenuIdx) < (int)queueEntries.size()) {
			Gui::Draw_Rect(QueueBoxes[1].x, QueueBoxes[1].y, QueueBoxes[1].w, QueueBoxes[1].h, UIThemes->MarkUnselected());

			const C2D_Image tempImg2 = queueEntries[1 + queueMenuIdx]->icn;
			const uint8_t offsetW2 = (48 - tempImg2.subtex->width) / 2; // Center W.
			const uint8_t offsetH2 = (48 - tempImg2.subtex->height) / 2; // Center H.
			C2D_DrawImageAt(tempImg2, QueueBoxes[1].x + 5 + offsetW2, QueueBoxes[1].y + 21 + offsetH2, 0.5f);

			Gui::DrawString(QueueBoxes[1].x + 10, QueueBoxes[1].y + 5, 0.4f, UIThemes->TextColor(), queueEntries[1 + queueMenuIdx]->name, 230, 0, font);

			Gui::DrawString(QueueBoxes[1].x + 60, QueueBoxes[1].y + 30, 0.4f, UIThemes->TextColor(), Lang::get("QUEUE_POSITION") + ": " + std::to_string(queueMenuIdx + 1), 0, 0, font);

			/* Cancel. */
			GFX::DrawIcon(sprites_cancel_idx, QueueBoxes[3].x, QueueBoxes[3].y, UIThemes->TextColor());
		}
	}
}

void StoreUtils::QueueMenuHandle(int &queueIndex, int &storeMode) {
	if (!queueEntries.empty()) {
		if ((1 + queueMenuIdx) > (int)queueEntries.size() - 1) queueMenuIdx = std::max<int>((int)(queueEntries.size() - 1) - 1, 0); // Ensure this really doesn't go below 0.
	}

	if (hDown & KEY_TOUCH) {
		/* Current Queue Cancel. */
		if (QueueSystem::RequestNeeded == NO_REQUEST && touching(touch, QueueBoxes[2])) { // Needs to be above the 0 one, otherwise the callback won't be accepted.
			QueueSystem::CancelCallback = true;

		} else if (touching(touch, QueueBoxes[0])) {
			if (QueueSystem::RequestNeeded != NO_REQUEST) { // -1 means no request.
				switch(QueueSystem::RequestNeeded) {
					case RMDIR_REQUEST: // Remove Directory message.
						QueueSystem::RequestAnswer = Msg::promptMsg(QueueSystem::RequestMsg);

						QueueSystem::Wait = false;
						QueueSystem::Resume();
						break;

					case PROMPT_REQUEST: // Skip prompt message.
						QueueSystem::RequestAnswer = ScriptUtils::prompt(QueueSystem::RequestMsg);

						QueueSystem::Wait = false;
						QueueSystem::Resume();
						break;
				}

			} else {
				ShowQueueProgress = !ShowQueueProgress; // In case no request expected, switch from progress to total progress mode etc.
			}

			/* Remove from Queue. */
		} else if (touching(touch, QueueBoxes[3])) { // Remove Queue entries.
			if (queueEntries.size() > 1) queueEntries.erase(queueEntries.begin() + 1 + queueMenuIdx);
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

	if(hDown & KEY_A) {
		if (QueueSystem::RequestNeeded != NO_REQUEST) { // -1 means no request.
			switch(QueueSystem::RequestNeeded) {
				case RMDIR_REQUEST: // Remove Directory message.
					QueueSystem::RequestAnswer = Msg::promptMsg(QueueSystem::RequestMsg);

					QueueSystem::Wait = false;
					QueueSystem::Resume();
					break;

				case PROMPT_REQUEST: // Skip prompt message.
					QueueSystem::RequestAnswer = ScriptUtils::prompt(QueueSystem::RequestMsg);

					QueueSystem::Wait = false;
					QueueSystem::Resume();
					break;
			}

		} else {
			ShowQueueProgress = !ShowQueueProgress; // In case no request expected, switch from progress to total progress mode etc.
		}
	}

	if (hDown & KEY_B) storeMode = 0; // Go to EntryInfo.

	/* Quit UU. */
	if (hDown & KEY_START && !QueueRuns)
		exiting = true;
}