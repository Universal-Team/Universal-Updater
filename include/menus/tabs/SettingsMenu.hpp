// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_SETTINGS_MENU_HPP
#define _UNIVERSAL_UPDATER_SETTINGS_MENU_HPP

#include "structs.hpp"


class SettingsMenu {
public:
	void Draw();
	void Handler();
private:
	enum class Menu : uint8_t { Main = 0 };

	size_t Selection = 0, ScreenPos = 0;
	Menu _Menu = Menu::Main;

	/* Main Settings. */
	static constexpr Structs::ButtonPos MainPos[7] = {
		{ 45, 32, 271, 22 },
		{ 45, 62, 271, 22 },
		{ 45, 92, 271, 22 },
		{ 45, 122, 271, 22 },
		{ 45, 152, 271, 22 },
		{ 45, 182, 271, 22 },
		{ 45, 212, 271, 22 }
	};

	void DrawMain();
	void HandleMain();
	const std::string MainStrings[2] = { "Select UniStore", "Exit Universal-Updater" };
};

#endif