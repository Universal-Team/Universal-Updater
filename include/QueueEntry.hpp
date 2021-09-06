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

#ifndef _UNIVERSAL_UPDATER_QUEUE_ENTRY_HPP
#define _UNIVERSAL_UPDATER_QUEUE_ENTRY_HPP

#include "Action.hpp"
#include "JSON.hpp"

class QueueEntry {
public:
	QueueEntry(size_t Idx, const nlohmann::json *Script) : Idx(Idx), Script(Script) { };

	void Handler();
	void Draw() const;

	void Cancel() { this->Cancelling = true; if (this->CurrentAction) this->CurrentAction->Cancel(); };

	size_t UniStoreIndex() const { return Idx; };
	size_t CurrentStep() const { return Step; };
	size_t TotalSteps() const { if (this->Script && this->Script->is_array()) return this->Script->size(); return 0; };

private:
	static bool ObjectContains(const nlohmann::json &Item, const std::vector<std::string> Keys);

	std::unique_ptr<Action> CurrentAction = nullptr;
	size_t Step = 0;
	bool Cancelling = false;
	size_t Idx;
	const nlohmann::json *Script;
};

#endif
