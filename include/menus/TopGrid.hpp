// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_TOP_GRID_HPP
#define _UNIVERSAL_UPDATER_TOP_GRID_HPP

#include "structs.hpp"
#include <vector>


class TopGrid {
public:
	void Draw();
	void Handler();
	void Update();
private:
	uint8_t Box = 0;
};

#endif