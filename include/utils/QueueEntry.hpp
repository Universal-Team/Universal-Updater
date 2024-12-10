// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_QUEUE_ENTRY_HPP
#define _UNIVERSAL_UPDATER_QUEUE_ENTRY_HPP

#include "Action.hpp"
#include "JSON.hpp"

class QueueEntry {
public:
	QueueEntry(const size_t Idx, const size_t DLIdx, const nlohmann::json *Script) : Idx(Idx), DLIdx(DLIdx), Script(Script) { };

	void Handler();
	void Draw() const;

	void Cancel() { this->Cancelling = true; if (this->CurrentAction) this->CurrentAction->Cancel(); };

	size_t UniStoreIndex() const { return this->Idx; };
	size_t DownloadIndex() const { return this->DLIdx; };
	size_t CurrentStep() const { return this->Step; };
	size_t TotalSteps() const { if (this->Script && this->Script->is_array()) return this->Script->size(); return 0; };

private:
	static bool ObjectContains(const nlohmann::json &Item, const std::vector<std::string> Keys);

	std::unique_ptr<Action> CurrentAction = nullptr;
	size_t Step = 0;
	bool Cancelling = false;
	size_t Idx, DLIdx;
	const nlohmann::json *Script;
};

#endif
