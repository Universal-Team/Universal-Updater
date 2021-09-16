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

#ifndef _UNIVERSAL_UPDATER_QUEUE_SYSTEM_HPP
#define _UNIVERSAL_UPDATER_QUEUE_SYSTEM_HPP

#include "QueueEntry.hpp"

#include <deque>

class QueueSystem {
public:
	static void Handler();
	static void Draw();

	static void Add(const size_t EntryIndex, const size_t DLIdx, const nlohmann::json *Script);
	static void Remove(const size_t Idx);

	static size_t Count() { return Queue.size(); };
	static const QueueEntry *Current() { return Queue.size() > 0 ? &Queue[0] : nullptr; };

private:
	static std::deque<QueueEntry> Queue;
};

#endif
