#include "QueueEntry.hpp"

#include "Copying.hpp"
#include "Deleting.hpp"
#include "DownloadFile.hpp"
#include "DownloadRelease.hpp"
#include "Extracting.hpp"
#include "Moving.hpp"
#include "gui.hpp"

bool QueueEntry::ObjectContains(const nlohmann::json &Item, const std::vector<std::string> Keys) {
	for (const std::string &Key : Keys) {
		if (!Item.contains(Key) || !Item[Key].is_string()) return false;
	}

	return true;
}

void QueueEntry::Handler() {
	if (!this->Script || !this->Script->is_array()) return;

	for (this->Step = 0; this->Step < this->Script->size(); this->Step++) {
		const nlohmann::json &Item = (*this->Script)[this->Step];

		if (!Item.is_object() || !this->ObjectContains(Item, {"type"})) break;

		if (Item["type"] == "bootTitle") {
			// TODO!

		} else if (Item["type"] == "copy") {
			if (!this->ObjectContains(Item, {"source", "destination"})) break;

			CurrentAction = std::make_unique<Copying>(Item["source"], Item["destination"]);

		} else if (Item["type"] == "deleteFile") {
			if (!this->ObjectContains(Item, {"file"})) break;

			// TODO: Uncomment this after fixing extraction
			// CurrentAction = std::make_unique<Deleting>(Item["file"]);

		} else if (Item["type"] == "downloadFile") {
			if (!this->ObjectContains(Item, {"file", "output"})) break;

			CurrentAction = std::make_unique<DownloadFile>(Item["file"], Item["output"]);

		} else if (Item["type"] == "downloadRelease") {
			if (!this->ObjectContains(Item, {"repo", "file", "output"})) break;

			CurrentAction = std::make_unique<DownloadRelease>(Item["repo"], Item["file"], Item["output"], (Item.contains("includePrereleases") && Item["includePrereleases"].is_string()) ? Item["includePrereleases"].get<bool>() : false);

		} else if (Item["type"] == "exit") {
			break;

		} else if (Item["type"] == "extractFile") {
			if (!this->ObjectContains(Item, {"file", "input", "output"})) break;

			CurrentAction = std::make_unique<Extracting>(Item["file"], Item["input"], Item["output"]);

		} else if (Item["type"] == "installCia") {
			// TODO!

		} else if (Item["type"] == "mkdir") {
			// TODO!

		} else if (Item["type"] == "move") {
			if (!this->ObjectContains(Item, {"old", "new"})) break;

			CurrentAction = std::make_unique<Moving>(Item["old"], Item["new"]);

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

		if (CurrentAction) CurrentAction->Handler();
	}
}


void QueueEntry::Draw() const {
	if (CurrentAction) this->CurrentAction->Draw();
}
