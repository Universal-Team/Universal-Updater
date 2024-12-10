// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

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