// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#include "QueueEntry.hpp"

#include "Copying.hpp"
#include "Deleting.hpp"
#include "DownloadFile.hpp"
#include "DownloadRelease.hpp"
#include "Extracting.hpp"
#include "gui.hpp"
#include "Moving.hpp"
#include "UniversalUpdater.hpp"


bool QueueEntry::ObjectContains(const nlohmann::json &Item, const std::vector<std::string> Keys) {
	for (const std::string &Key : Keys) {
		if (!Item.contains(Key) || !Item[Key].is_string()) return false;
	}

	return true;
};


void QueueEntry::Handler() {
	if (!this->Script || !this->Script->is_array()) return;

	for (this->Step = 0; this->Step < this->Script->size(); this->Step++) {
		const nlohmann::json &Item = (*this->Script)[this->Step];

		if (!Item.is_object() || !this->ObjectContains(Item, {"type"})) break;

		if (Item["type"] == "bootTitle") {
			// TODO!

		} else if (Item["type"] == "copy") {
			if (!this->ObjectContains(Item, {"source", "destination"})) break;

			this->CurrentAction = std::make_unique<Copying>(Item["source"], Item["destination"]);

		} else if (Item["type"] == "deleteFile") {
			if (!this->ObjectContains(Item, {"file"})) break;

			// TODO: Uncomment this after fixing extraction
			// this->CurrentAction = std::make_unique<Deleting>(Item["file"]);

		} else if (Item["type"] == "downloadFile") {
			if (!this->ObjectContains(Item, {"file", "output"})) break;

			this->CurrentAction = std::make_unique<DownloadFile>(Item["file"], Item["output"]);

		} else if (Item["type"] == "downloadRelease") {
			if (!this->ObjectContains(Item, {"repo", "file", "output"})) break;

			this->CurrentAction = std::make_unique<DownloadRelease>(Item["repo"], Item["file"], Item["output"], (Item.contains("includePrereleases") && Item["includePrereleases"].is_string()) ? Item["includePrereleases"].get<bool>() : false);

		} else if (Item["type"] == "exit") {
			break;

		} else if (Item["type"] == "extractFile") {
			if (!this->ObjectContains(Item, {"file", "input", "output"})) break;

			this->CurrentAction = std::make_unique<Extracting>(Item["file"], Item["input"], Item["output"]);

		} else if (Item["type"] == "installCia") {
			// TODO!

		} else if (Item["type"] == "mkdir") {
			// TODO!

		} else if (Item["type"] == "move") {
			if (!this->ObjectContains(Item, {"old", "new"})) break;

			this->CurrentAction = std::make_unique<Moving>(Item["old"], Item["new"]);

		} else if (Item["type"] == "promptMessage") {
			// TODO!

		} else if (Item["type"] == "rmdir") {
			// TODO!

		} else if (Item["type"] == "skip") {
			if (!this->ObjectContains(Item, {"count"})) break;
			this->Step += Item["count"].get<int>();
			continue;

		} else {
			CurrentAction = nullptr;
		}

		if (this->CurrentAction) this->CurrentAction->Handler();
	}

	/* Only set to META if not cancelled. */
	if (!this->Cancelling) {
		/* Set updated state to META. */
		UU::App->MData->SetUpdated(
			UU::App->Store->GetUniStoreTitle(), // UniStore Title.
			UU::App->Store->GetEntryTitle(this->UniStoreIndex()), // Entry Index.
			UU::App->Store->GetEntryLastUpdated(this->UniStoreIndex()) // Last Updated.
		);

		/*
			Set installed state to META.

			TODO: Maybe just pass over a string or so instead of going the vector fetch thing way.
		*/
		const std::vector<std::string> DLList = UU::App->Store->GetDownloadList(this->UniStoreIndex());
		if (!DLList.empty() && this->DownloadIndex() < DLList.size()) {
			UU::App->MData->SetInstalled(
				UU::App->Store->GetUniStoreTitle(), // UniStore Title.
				UU::App->Store->GetEntryTitle(this->UniStoreIndex()), // Entry Index.
				DLList[this->DownloadIndex()]
			);
		}
	}
};


void QueueEntry::Draw() const {
	if (this->CurrentAction) this->CurrentAction->Draw();
};
