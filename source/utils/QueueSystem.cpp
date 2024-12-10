// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#include "QueueSystem.hpp"

#include "Platform.hpp"

static ThreadPtr QueueThread = nullptr;

std::deque<QueueEntry> QueueSystem::Queue;


void QueueSystem::Add(const size_t EntryIndex, const size_t DLIdx, const nlohmann::json *Script) {
	Queue.emplace_back(EntryIndex, DLIdx, Script);

	if (Queue.size() == 1) {
		if (QueueThread) {
			Platform::ThreadJoin(QueueThread, UINT64_MAX);
			threadFree(QueueThread);
			QueueThread = nullptr;
		}

		QueueThread = Platform::CreateThread((ThreadFunc)QueueSystem::Handler);
		Platform::AllowExit(false);
	}
};


void QueueSystem::Draw() {
	if (Queue.size() > 0) Queue[0].Draw();
};


void QueueSystem::Handler() {
	while(Queue.size()) {
		Queue[0].Handler();
		Queue.pop_front();
	}

	Platform::AllowExit(true);
};


void QueueSystem::Remove(const size_t Idx) {
	if (Idx < Queue.size()) {
		if (Idx == 0) Queue[0].Cancel();
		else Queue.erase(Queue.begin() + Idx);
	}
};
