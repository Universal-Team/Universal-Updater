// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_ACTION_HPP
#define _UNIVERSAL_UPDATER_ACTION_HPP

#include <string>


class Action {
public:
	enum class ActionType : uint8_t { None = 0, Extracting, Moving, Copying, Deleting, DownloadFile, DownloadRelease };

	virtual void Handler() = 0; // The main function that handles things.
	virtual void Draw() const = 0; // The function that draws the progress in the queue menu.

	virtual uint8_t State() const = 0; // The current state.
	virtual ActionType Type() const = 0; // The Action Type.

	bool IsDone() const { return Done; }; // If the action is done or nah.
	void Cancel() { Cancelling = true; }; // Call this to cancel the action.

protected:
	bool Done = false;
	bool Cancelling = false;
};

#endif