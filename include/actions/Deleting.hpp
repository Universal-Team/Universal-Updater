// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_DELETING_HPP
#define _UNIVERSAL_UPDATER_DELETING_HPP

#include "Action.hpp"
#include <string>


/*
	Handles Moving / Renaming of Files.
*/
class Deleting : public Action {
public:
	enum class Error : uint8_t { Good = 0, notExist };

	Deleting(const std::string &Path) : Path(Path) { };

	void Handler() override;
	void Draw() const override;

	/* Some returns. */
	uint8_t State() const override { return (uint8_t)this->CurState; };
	Action::ActionType Type() const override { return Action::ActionType::Deleting; };
private:
	Error CurState = Error::Good; // The current state of the operation.
	std::string Path = "";
};

#endif