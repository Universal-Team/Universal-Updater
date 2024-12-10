// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_SEARCH_MENU_HPP
#define _UNIVERSAL_UPDATER_SEARCH_MENU_HPP

#include "structs.hpp"
#include <vector>


class SearchMenu {
public:
	enum class FavoriteMarks : int {
		Star = 1 << 0,
		Heart = 1 << 1,
		Diamond = 1 << 2,
		Clubs = 1 << 3,
		Spade = 1 << 4
	};

	void Draw();
	void Handler();
private:
	int Marks = 0x0;
	bool UpdateFilter = false, IsAnd = true, Includes[4] = { false };
	std::string SearchText = "";

	bool FindInVector(const std::vector<std::string> &Items, const std::string &Query);
	void DoSearch();

	static constexpr Structs::ButtonPos SearchPos[14] = {
		{ 51, 41, 262, 30 }, // Search bar.

		/* Includes. */
		{ 85, 101, 50, 10 },
		{ 85, 117, 50, 10 },
		{ 167, 101, 50, 10 },
		{ 167, 117, 50, 10 },

		/* Filters. */
		{ 82, 159, 30, 30 },
		{ 117, 159, 30, 30 },
		{ 152, 159, 30, 30 },
		{ 187, 159, 30, 30 },
		{ 222, 159, 30, 30 },
		{ 257, 159, 30, 30 },

		/* Send to Queue. */
		{ 82, 200, 205, 25 },

		/* AND / OR. */
		{ 222, 139, 30, 13 },
		{ 257, 139, 30, 13 }
	};
};

#endif