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

#include "DownloadFile.hpp"
#include "DownloadUtils.hpp"
#include "GFXData.hpp"
#include "gui.hpp"
#include "QueueMenu.hpp"
#include "QueueSystem.hpp"
#include "UniversalUpdater.hpp"
#include "Utils.hpp"
#include <string.h>


/*
	TODO: Proper handling or so.
*/
void DownloadFile::Handler() {
	Utils::MakeDirs(this->Path);

	const int Res = DownloadUtils::DownloadToFile(this->URL.c_str(), this->Path.c_str());
	this->Done = true;
};


void DownloadFile::Draw() const {
	char Str[256];
	const QueueEntry *Entry = QueueSystem::Current();
	if (!Entry) return;

	Gui::Draw_Rect(QueueMenu::Boxes[0].x, QueueMenu::Boxes[0].y, QueueMenu::Boxes[0].w, QueueMenu::Boxes[0].h, TABS_UNSELECTED); // TODO: Color

	/* Progress. */
	sniprintf(Str, sizeof(Str), "Step: %d / %d", Entry->CurrentStep(), Entry->TotalSteps());
	Gui::DrawString((QueueMenu::Boxes[0].x + 241), QueueMenu::Boxes[0].y + 68, 0.4f, TEXT_COLOR, Str, 80, 0, nullptr, C2D_AlignRight);

	sniprintf(Str, sizeof(Str), "Downloading... %s / %s (%lld%%)",
			Utils::FormatBytes(DownloadUtils::CurrentProgress()).c_str(),
			Utils::FormatBytes(DownloadUtils::TotalSize()).c_str(),
			DownloadUtils::CurrentProgress() * 100 / DownloadUtils::TotalSize());
	Gui::DrawString(QueueMenu::Boxes[0].x + 10, QueueMenu::Boxes[0].y + 5, 0.4f, TEXT_COLOR, Str, 230);

	sniprintf(Str, sizeof(Str), "Speed: %s/s", Utils::FormatBytes(DownloadUtils::Speed()).c_str());
	Gui::DrawString(QueueMenu::Boxes[0].x + 10, QueueMenu::Boxes[0].y + 68, 0.4f, TEXT_COLOR, Str, 230);


	Gui::Draw_Rect(QueueMenu::Boxes[0].x + 60, QueueMenu::Boxes[0].y + 30, 182, 30, BAR_OUTLINE); // TODO: Color
	Gui::Draw_Rect(QueueMenu::Boxes[0].x + 60 + 1, QueueMenu::Boxes[0].y + 30 + 1, DownloadUtils::CurrentProgress() * 180 / DownloadUtils::TotalSize(), 28, BAR_COLOR); // TODO: Color
};
