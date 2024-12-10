// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_DOWN_LIST_HPP
#define _UNIVERSAL_UPDATER_DOWN_LIST_HPP

#include "structs.hpp"
#include <vector>


class DownList {
public:
	void DrawTopOvl();
	void Draw();
	void Handler();
	void Reset();
private:
	size_t SelectedIndex = 0, ScreenIndex = 0;

	static constexpr Structs::ButtonPos DownPos[7] = {
		{ 46, 32, 241, 22 },
		{ 46, 62, 241, 22 },
		{ 46, 92, 241, 22 },
		{ 46, 122, 241, 22 },
		{ 46, 152, 241, 22 },
		{ 46, 182, 241, 22 },
		{ 46, 212, 241, 22 }
	};

	static constexpr Structs::ButtonPos InstallPos[7] = {
		{ 288, 32, 24, 24 },
		{ 288, 62, 24, 24 },
		{ 288, 92, 24, 24 },
		{ 288, 122, 24, 24 },
		{ 288, 152, 24, 24 },
		{ 288, 182, 24, 24 },
		{ 288, 212, 24, 24 }
	};
};

#endif