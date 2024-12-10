// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#include "QueueMenu.hpp"

#include "GFXData.hpp"
#include "gui.hpp"
#include "QueueSystem.hpp"
#include "Utils.hpp"


void QueueMenu::Draw() {
	/* Draw Top bar. */
	Gui::Draw_Rect(40, 0, 280, 25, BAR_COLOR);
	Gui::Draw_Rect(40, 25, 280, 1, BAR_OUTLINE);
	Gui::DrawStringCentered(17, 2, TEXT_MEDIUM, TEXT_COLOR, "Queue System", 273, 0);

	/* Draw active queue item. */
	QueueSystem::Draw();
};


void QueueMenu::Handler() {

};