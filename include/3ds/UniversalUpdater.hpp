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
#include "screenCommon.hpp"
#include "structs.hpp"
#include "ThemeData.hpp"
#include "UniStore.hpp"

/* Menus. */
#include "Tabs.hpp"
#include "TopGrid.hpp"
#include "TopList.hpp"


#include <3ds.h>
#include <memory>
#include <string>


/* Include Definitions for Universal-Updater here. */
#define CONFIG_PATH "sdmc:/3ds/Universal-Updater/Config.json"
#define _STORE_PATH "sdmc:/3ds/Universal-Updater/stores/"
#define THEME_JSON "sdmc:/3ds/Universal-Updater/Themes.json"

#define SHEET_PATH_KEY "sheet"
#define SHEET_URL_KEY "sheetURL"

/* Meta data related things. */
#define _META_PATH "sdmc:/3ds/Universal-Updater/MetaData.json"
#define _OLD_UPDATE_PATH "sdmc:/3ds/Universal-Updater/updates.json"


class UU {
public:
	enum class TopMode : uint8_t { Grid = 0, List };
	void Initialize();
	void ScanInput();

	void Draw();
	int Handler();

	bool Touched(const Structs::ButtonPos Pos) const;
	void SwitchTopMode();

	static std::unique_ptr<UU> App;
	std::unique_ptr<ConfigData> CData = nullptr;
	std::unique_ptr<GFXData> GData = nullptr;
	std::unique_ptr<Meta> MData = nullptr;
	std::unique_ptr<UniStore> Store = nullptr;
	std::unique_ptr<ThemeData> TData = nullptr; // TODO: Find a good way to handle the active theme through defines.
	
	uint32_t Down = 0, Repeat = 0; // Key Down and Key Repeat.
	touchPosition T = { 0, 0 };
	bool Exiting = false;
	TopMode TMode = TopMode::Grid;
private:
	std::unique_ptr<Tabs> _Tabs = nullptr;
	std::unique_ptr<TopGrid> TGrid = nullptr;
	std::unique_ptr<TopList> TList = nullptr;
};

#endif