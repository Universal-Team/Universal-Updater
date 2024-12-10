// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_MOVING_HPP
#define _UNIVERSAL_UPDATER_MOVING_HPP

#include "Action.hpp"
#include <string>


/*
	Handles Moving / Renaming of Files.
*/
class Moving : public Action {
public:
	enum class Error : uint8_t { Good = 0, SourceNotExist, OutOfSpace };

	Moving(const std::string &OldName, const std::string &NewName)
		: OldName(OldName), NewName(NewName) { };

	void Handler() override;
	void Draw() const override;

	/* Some returns. */
	uint8_t State() const override { return (uint8_t)this->CurState; };
	Action::ActionType Type() const override { return Action::ActionType::Moving; };
private:
	Error CurState = Error::Good; // The current state of the operation.
	std::string OldName = "", NewName = "";
};

#endif