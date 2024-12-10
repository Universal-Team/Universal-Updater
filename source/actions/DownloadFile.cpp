// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

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

	DownloadUtils::DownloadToFile(this->URL.c_str(), this->Path.c_str());
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
