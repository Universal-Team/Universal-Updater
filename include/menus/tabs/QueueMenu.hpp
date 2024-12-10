// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_QUEUE_MENU_HPP
#define _UNIVERSAL_UPDATER_QUEUE_MENU_HPP

#include "structs.hpp"

#include <array>


class QueueMenu {
public:
	static constexpr std::array<Structs::ButtonPos, 4> Boxes = {{
		{47,   36, 266, 90},
		{47,  139, 266, 90},
		{292,  37,  20, 20}, // Cancel current Queue.
		{292, 140,  20, 20} // Remove next Queue.
	}};

	void Draw();
	void Handler();
private:

};

#endif