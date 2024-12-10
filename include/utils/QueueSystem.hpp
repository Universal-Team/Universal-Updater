// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

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
