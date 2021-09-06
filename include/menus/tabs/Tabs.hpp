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

#ifndef _UNIVERSAL_UPDATER_TABS_HPP
#define _UNIVERSAL_UPDATER_TABS_HPP

#include "DownList.hpp"
#include "EntryInfo.hpp"
#include "SearchMenu.hpp"
#include "SortMenu.hpp"
#include "SettingsMenu.hpp"
#include "QueueMenu.hpp"
#include "structs.hpp"


class Tabs {
public:
	enum class Tab : uint8_t { EntryInfo = 0, DownloadList, QueueSystem, Search, Sort, Settings };
	
	Tabs();
	void DrawTop();
	void DrawBottom();
	void Handler();

	/* Only allow top scroll if in EntryInfo, Search or Sorting. */
	bool HandleTopScroll() const {
		return (this->ActiveTab != Tab::DownloadList && this->ActiveTab != Tab::QueueSystem && this->ActiveTab != Tab::Settings);
	};

	void PrevTab();
	void SwitchTab(const Tab T);
	void SortEntries() { this->SoMenu->SortCurrent(); };
	
	Tab CurrentTab() const { return this->ActiveTab; };
private:
	Tab ActiveTab = Tab::EntryInfo, LastTab = Tab::EntryInfo;

	static constexpr Structs::ButtonPos TabPos[6] = {
		{ 0, 0, 40, 40 },
		{ 0, 40, 40, 40 },
		{ 0, 80, 40, 40 },
		{ 0, 120, 40, 40 },
		{ 0, 160, 40, 40 },
		{ 0, 200, 40, 40 }
	};

	std::unique_ptr<EntryInfo> EInfo = nullptr;
	std::unique_ptr<DownList> DList = nullptr;
	std::unique_ptr<QueueMenu> QMenu = nullptr;
	std::unique_ptr<SearchMenu> SeMenu = nullptr;
	std::unique_ptr<SortMenu> SoMenu = nullptr;
	std::unique_ptr<SettingsMenu> SMenu = nullptr;
};

#endif