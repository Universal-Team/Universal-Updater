// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_TABS_HPP
#define _UNIVERSAL_UPDATER_TABS_HPP

#include "DownList.hpp"
#include "EntryInfo.hpp"
#include "SearchMenu.hpp"
#include "SortMenu.hpp"
#include "SettingsMenu.hpp"
#include "QueueMenu.hpp"
#include "structs.hpp"

#include <memory>

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