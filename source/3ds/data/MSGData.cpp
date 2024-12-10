// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#include "MSGData.hpp"

#include "gui.hpp"
#include "UniversalUpdater.hpp"


void MSGData::DisplayWaitMsg(const std::string &MSG) {
	UU::App->GData->StartFrame();
	UU::App->GData->DrawTop();
	Gui::DrawStringCentered(0, 80, 0.5f, TEXT_COLOR, MSG, 390, 80);
	UU::App->GData->DrawBottom();
	UU::App->GData->EndFrame();
};


bool MSGData::PromptMsg(const std::string &MSG) {
	bool Result = false;

	while(1) {
		UU::App->GData->StartFrame();
		UU::App->GData->DrawTop();
		Gui::DrawStringCentered(0, 80, 0.5f, TEXT_COLOR, MSG, 390, 80);
		Gui::DrawStringCentered(0, 210, 0.5f, TEXT_COLOR, "Press \uE000 to continue, \uE001 to cancel.", 390, 80);
		UU::App->GData->DrawBottom();
		UU::App->GData->EndFrame();

		UU::App->ScanInput();
		if (UU::App->Down & KEY_A) {
			Result = true;
			break;
		}

		if (UU::App->Down & KEY_B) {
			Result = false;
			break;
		}
	}

	UU::App->ScanInput();
	return Result;
};