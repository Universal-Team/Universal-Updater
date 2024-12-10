// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_UNISTORE_SELECTOR_HPP
#define _UNIVERSAL_UPDATER_UNISTORE_SELECTOR_HPP

#include "structs.hpp"
#include "UniStore.hpp"
#include <string>


class UniStoreSelector {
public:
	void InitSelector();
	void DrawTop();
	void DrawBottom();

	void Handler();
	bool Done = true;
private:
	size_t ScreenIndex = 0, SelectedIndex = 0;
	std::vector<UniStore::Info> Infos;

	void DeleteUniStore(const std::string &File);
	void DownloadUniStore();
	void UpdateUniStore();

	static constexpr Structs::ButtonPos Pos[10] = {
		{ 10, 34, 300, 22 },
		{ 10, 64, 300, 22 },
		{ 10, 94, 300, 22 },
		{ 10, 124, 300, 22 },
		{ 10, 154, 300, 22 },
		{ 10, 184, 300, 22 },

		{ 112, 215, 16, 16 }, // Delete.
		{ 154, 215, 16, 16 }, // Update.
		{ 200, 215, 16, 16 }, // Add.
		{ 4, 0, 24, 24 } // Back.
	};
};

#endif