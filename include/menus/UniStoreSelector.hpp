/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2021 Universal-Team
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#ifndef _UNIVERSAL_UPDATER_UNISTORE_SELECTOR_HPP
#define _UNIVERSAL_UPDATER_UNISTORE_SELECTOR_HPP

#include "structs.hpp"
#include "UniStore.hpp"
#include <string>


class UniStoreSelector {
public:
	void Handler();
private:
	size_t ScreenIndex = 0, SelectedIndex = 0;
	std::vector<UniStore::Info> Infos;
	bool Done = false;

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