// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_MSG_DATA_HPP
#define _UNIVERSAL_UPDATER_MSG_DATA_HPP

#include <string>

class MSGData {
public:
	void DisplayWaitMsg(const std::string &MSG);
	bool PromptMsg(const std::string &MSG);
};

#endif