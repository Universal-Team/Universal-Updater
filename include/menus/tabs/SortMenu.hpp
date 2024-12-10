// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_SORT_MENU_HPP
#define _UNIVERSAL_UPDATER_SORT_MENU_HPP

#include "structs.hpp"


class SortMenu {
public:
	enum class SortType : uint8_t { Title = 0, Author, LastUpdated };
	SortMenu() { this->SortCurrent(); };
	void Draw();
	void Handler();

	void SortCurrent() { this->DoSort(this->SType); }; // Needed for Search.
private:
	bool Ascending = false;
	SortType SType = SortType::LastUpdated;

	uint8_t GetType(const SortMenu::SortType SType) const;
	void DoSort(const SortMenu::SortType SType);

	static constexpr Structs::ButtonPos SortPos[7] = {
		{ 71, 60, 104, 16 },
		{ 71, 80, 104, 16 },
		{ 71, 100, 104, 16 },

		{ 201, 60, 104, 16 },
		{ 201, 80, 104, 16 },

		{ 71, 170, 104, 16 },
		{ 71, 190, 104, 16 }
	};
};

#endif