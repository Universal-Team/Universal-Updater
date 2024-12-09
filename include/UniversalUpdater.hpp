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

#ifndef _UNIVERSAL_UPDATER_HPP
#define _UNIVERSAL_UPDATER_HPP

#include "ConfigData.hpp"
#include "GFXData.hpp"
#include "Meta.hpp"
#include "MSGData.hpp"
#include "ThemeData.hpp"
#include "UniStore.hpp"

/* Menus. */
#include "Tabs.hpp"
#include "TopGrid.hpp"
#include "TopList.hpp"
#include "UniStoreSelector.hpp"

class UU {
	std::unique_ptr<TopGrid> TGrid = nullptr;
	std::unique_ptr<TopList> TList = nullptr;

public:
	enum class TopMode : uint8_t { Grid = 0, List };
	void Initialize(char *ARGV[]);
	void ScanInput();

	void Draw();
	int Handler(char *ARGV[]);

	void SwitchTopMode(const UU::TopMode TMode);

	static std::unique_ptr<UU> App;
	std::unique_ptr<ConfigData> CData = nullptr;
	std::unique_ptr<GFXData> GData = nullptr;
	std::unique_ptr<Meta> MData = nullptr;
	std::unique_ptr<MSGData> MSData = nullptr;
	std::unique_ptr<UniStore> Store = nullptr;
	std::unique_ptr<Tabs> _Tabs = nullptr;
	std::unique_ptr<ThemeData> TData = nullptr; // TODO: Find a good way to handle the active theme through defines.
	std::unique_ptr<UniStoreSelector> USelector = nullptr;
	
	uint32_t Down = 0, Repeat = 0; // Key Down and Key Repeat.
	touchPosition T = { 0, 0 };
	bool Exiting = false;
	TopMode TMode = TopMode::Grid;
};

#endif