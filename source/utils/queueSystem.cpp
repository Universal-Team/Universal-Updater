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

#include "files.hpp"
#include "gui.hpp"
#include "queueSystem.hpp"
#include "scriptUtils.hpp"
#include "storeUtils.hpp"
#include <unistd.h>

std::deque<std::unique_ptr<Queue>> queueEntries;
int QueueSystem::RequestNeeded = -1, QueueSystem::RequestAnswer = -1;
bool QueueSystem::Wait = false, QueueSystem::Popup = false, QueueSystem::CancelCallback = false;
std::string QueueSystem::RequestMsg = "", QueueSystem::EndMsg = "";
int QueueSystem::LastElement = 0;

bool QueueRuns = false;
static Thread queueThread = nullptr;

/*
	Adds an entry to the queue.

	nlohmann::json obj: The object.
	C2D_Image icn: The icon.
*/
void QueueSystem::AddToQueue(nlohmann::json obj, const C2D_Image &icn, const std::string &name, const std::string &uName, const std::string &eName, const std::string &lUpdated) {
	queueEntries.push_back( std::make_unique<Queue>(obj, icn, name, uName, eName, lUpdated) );

	/* If not already running, let it run!! */
	if (!QueueRuns && !QueueSystem::Wait) {
		QueueRuns = true; // We enable the queue run state here.

		if (queueThread) {
			threadJoin(queueThread, U64_MAX);
			threadFree(queueThread);
			queueThread = nullptr;
		}

		s32 prio = 0;

		svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
		queueThread = threadCreate((ThreadFunc)QueueSystem::QueueHandle, NULL, 64 * 1024, prio - 1, -2, false);
		aptSetHomeAllowed(false);
	}
}

/*
	Clears the queue.
*/
void QueueSystem::ClearQueue() {
	QueueRuns = false;
	queueEntries.clear();

	if (queueThread) {
		threadJoin(queueThread, U64_MAX);
		threadFree(queueThread);
		queueThread = nullptr;
	}
}

/*
	Use this, to go back to the queue after the Request.
*/
void QueueSystem::Resume() {
	aptSetHomeAllowed(false);
	QueueSystem::Wait = false;
	QueueRuns = true;

	if (queueThread) {
		threadJoin(queueThread, U64_MAX);
		threadFree(queueThread);
		queueThread = nullptr;
	}

	s32 prio = 0;
	svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
	queueThread = threadCreate((ThreadFunc)QueueSystem::QueueHandle, NULL, 64 * 1024, prio - 1, -2, false);
}

/*
	The whole handle.
*/
void QueueSystem::QueueHandle() {
	while(QueueRuns) {
		Result ret = NONE; // No Error as of yet.

		for(int i = QueueSystem::LastElement; ret == NONE && i < queueEntries[0]->total && !QueueSystem::CancelCallback; i++) {
			queueEntries[0]->current++;

			std::string type = "";

			if (queueEntries[0]->obj[i].contains("type") && queueEntries[0]->obj[i]["type"].is_string()) {
				type = queueEntries[0]->obj[i]["type"];

			} else {
				ret = SYNTAX_ERROR;
			}

			/* Deleting a file. */
			if (type == "deleteFile") {
				bool missing = false;
				std::string file = "";
				queueEntries[0]->status = QueueStatus::Deleting;

				if (queueEntries[0]->obj[i].contains("file") && queueEntries[0]->obj[i]["file"].is_string()) {
					file = queueEntries[0]->obj[i]["file"];
				} else missing = true;

				if (!missing) ret = ScriptUtils::removeFile(file, "");
				else ret = SYNTAX_ERROR;

				/* Downloading from a URL. */
			} else if (type == "downloadFile") {
				bool missing = false;
				std::string file = "", output = "";

				queueEntries[0]->status = QueueStatus::Downloading;

				if (queueEntries[0]->obj[i].contains("file") && queueEntries[0]->obj[i]["file"].is_string()) {
					file = queueEntries[0]->obj[i]["file"];
				} else missing = true;

				if (queueEntries[0]->obj[i].contains("output") && queueEntries[0]->obj[i]["output"].is_string()) {
					output = queueEntries[0]->obj[i]["output"];
				} else missing = true;

				if (!missing) ret = ScriptUtils::downloadFile(file, output, "", false);
				else ret = SYNTAX_ERROR;

				/* Download from a GitHub Release. */
			} else if (type == "downloadRelease") {
				bool missing = false, includePrereleases = false;
				std::string repo = "", file = "", output = "";

				queueEntries[0]->status = QueueStatus::Downloading;

				if (queueEntries[0]->obj[i].contains("repo") && queueEntries[0]->obj[i]["repo"].is_string()) {
					repo = queueEntries[0]->obj[i]["repo"];
				} else missing = true;

				if (queueEntries[0]->obj[i].contains("file") && queueEntries[0]->obj[i]["file"].is_string()) {
					file = queueEntries[0]->obj[i]["file"];
				} else missing = true;

				if (queueEntries[0]->obj[i].contains("output") && queueEntries[0]->obj[i]["output"].is_string()) {
					output = queueEntries[0]->obj[i]["output"];
				} else missing = true;

				if (queueEntries[0]->obj[i].contains("includePrereleases") && queueEntries[0]->obj[i]["includePrereleases"].is_boolean())
					includePrereleases = queueEntries[0]->obj[i]["includePrereleases"];

				if (!missing) ret = ScriptUtils::downloadRelease(repo, file, output, includePrereleases, "", false);
				else ret = SYNTAX_ERROR;

				/* Extracting files. */
			} else if (type == "extractFile") {
				bool missing = false;
				std::string file = "", input = "", output = "";
				queueEntries[0]->status = QueueStatus::Extracting;

				if (queueEntries[0]->obj[i].contains("file") && queueEntries[0]->obj[i]["file"].is_string()) {
					file = queueEntries[0]->obj[i]["file"];
				} else missing = true;

				if (queueEntries[0]->obj[i].contains("input") && queueEntries[0]->obj[i]["input"].is_string()) {
					input = queueEntries[0]->obj[i]["input"];
				} else missing = true;

				if (queueEntries[0]->obj[i].contains("output") && queueEntries[0]->obj[i]["output"].is_string()) {
					output = queueEntries[0]->obj[i]["output"];
				} else missing = true;

				if (!missing) ret = ScriptUtils::extractFile(file, input, output, "", false);
				else ret = SYNTAX_ERROR;

			/* Installing CIAs. */
			} else if (type == "installCia") {
				bool missing = false, updateSelf = false;
				std::string file = "";
				queueEntries[0]->status = QueueStatus::Installing;

				if (queueEntries[0]->obj[i].contains("file") && queueEntries[0]->obj[i]["file"].is_string()) {
					file = queueEntries[0]->obj[i]["file"];
				} else missing = true;

				if (queueEntries[0]->obj[i].contains("updateSelf") && queueEntries[0]->obj[i]["updateSelf"].is_boolean()) {
					updateSelf = queueEntries[0]->obj[i]["updateSelf"];
				}

				if (!missing) ScriptUtils::installFile(file, updateSelf, "");
				else ret = SYNTAX_ERROR;

			} else if (type == "mkdir") {
				bool missing = false;
				std::string directory = "";

				if (queueEntries[0]->obj[i].contains("directory") && queueEntries[0]->obj[i]["directory"].is_string()) {
					directory = queueEntries[0]->obj[i]["directory"];
				} else missing = true;

				if (!missing) makeDirs(directory.c_str());
				else ret = SYNTAX_ERROR;

			/* Request Type 1. */
			} else if (type == "rmdir") {
				bool missing = false;
				std::string directory = "", message = "", promptmsg = "";
				bool required = false;
				queueEntries[0]->status = QueueStatus::Request;

				if (queueEntries[0]->obj[i].contains("directory") && queueEntries[0]->obj[i]["directory"].is_string()) {
					directory = queueEntries[0]->obj[i]["directory"];
				} else missing = true;

				if (queueEntries[0]->obj[i].contains("required") && queueEntries[0]->obj[i]["required"].is_boolean()) {
					required = queueEntries[0]->obj[i]["required"];
				}

				promptmsg = Lang::get("DELETE_PROMPT") + "\n" + directory;

				if (!missing && directory != "") {
					if (access(directory.c_str(), F_OK) != 0 && required) ret = DELETE_ERROR;
					else {
						if (QueueSystem::RequestNeeded == RMDIR_REQUEST) {
							/* There we already did it. :) */
							queueEntries[0]->status = QueueStatus::Deleting;
							if (QueueSystem::RequestAnswer == 1) removeDirRecursive(directory.c_str());
							/* Reset. */
							QueueSystem::RequestNeeded = NO_REQUEST;
							QueueSystem::RequestAnswer = NO_REQUEST;
							QueueSystem::RequestMsg = "";

						} else {
							/* We are in the process of the need of an answer. */
							QueueSystem::RequestNeeded = RMDIR_REQUEST; // Type 1.
							QueueSystem::RequestMsg = promptmsg;
							QueueSystem::LastElement = i; // So we know, where we go again after the Request.
							ret = PROMPT_RET;
						}
					}
				}

				else ret = SYNTAX_ERROR;

			/* Request Type 2. */
			} else if (type == "promptMessage" || type == "promptMsg") {
				std::string Message = "";
				int skipCount = -1;
				queueEntries[0]->status = QueueStatus::Request;

				if (queueEntries[0]->obj[i].contains("message") && queueEntries[0]->obj[i]["message"].is_string()) {
					Message = queueEntries[0]->obj[i]["message"];
				}

				if (queueEntries[0]->obj[i].contains("count") && queueEntries[0]->obj[i]["count"].is_number()) {
					skipCount = queueEntries[0]->obj[i]["count"];
				}

				if (QueueSystem::RequestNeeded == PROMPT_REQUEST) {
					if ((skipCount > -1) && (QueueSystem::RequestAnswer == SCRIPT_CANCELED)) {
						i += skipCount; // Skip.
						queueEntries[0]->current += skipCount;
					}

					/* Reset. */
					QueueSystem::RequestAnswer = NO_REQUEST;
					QueueSystem::RequestNeeded = NO_REQUEST;
					QueueSystem::RequestMsg = "";

				} else {
					QueueSystem::RequestNeeded = PROMPT_REQUEST; // Type 2.
					QueueSystem::RequestMsg = Message;
					QueueSystem::LastElement = i; // So we know, where we go again after the Request.
					ret = PROMPT_RET;
				}

			} else if (type == "exit") {
				ret = SCRIPT_CANCELED;

			} else if (type == "copy") {
				std::string source = "", destination = "";
				bool missing = false;
				queueEntries[0]->status = QueueStatus::Copying;

				if (queueEntries[0]->obj[i].contains("source") && queueEntries[0]->obj[i]["source"].is_string()) {
					source = queueEntries[0]->obj[i]["source"];
				} else missing = true;

				if (queueEntries[0]->obj[i].contains("destination") && queueEntries[0]->obj[i]["destination"].is_string()) {
					destination = queueEntries[0]->obj[i]["destination"];
				} else missing = true;

				if (!missing) ret = ScriptUtils::copyFile(source, destination, "");
				else ret = SYNTAX_ERROR;

			} else if (type == "move") {
				std::string oldFile = "", newFile = "";
				bool missing = false;
				queueEntries[0]->status = QueueStatus::Moving;

				if (queueEntries[0]->obj[i].contains("old") && queueEntries[0]->obj[i]["old"].is_string()) {
					oldFile = queueEntries[0]->obj[i]["old"];
				} else missing = true;

				if (queueEntries[0]->obj[i].contains("new") && queueEntries[0]->obj[i]["new"].is_string()) {
					newFile = queueEntries[0]->obj[i]["new"];
				} else missing = true;

				if (!missing) ret = ScriptUtils::renameFile(oldFile, newFile, "");
				else ret = SYNTAX_ERROR;

			} else if (type == "skip") {
				int skipCount = -1;

				if (queueEntries[0]->obj[i].contains("count") && queueEntries[0]->obj[i]["count"].is_number()) {
					skipCount = queueEntries[0]->obj[i]["count"];
				}

				if (skipCount > 0) i += skipCount; // Skip.
			}
		}

		/* If we expect a prompt, we go to this. */
		if (ret == PROMPT_RET) {
			queueEntries[0]->current = QueueSystem::LastElement + 1; // Cause no Zero.
			QueueSystem::Wait = true;
			QueueRuns = false;
		}

		if (!QueueSystem::Wait) {
			/* Canceled or None is for me -> Done. */
			if (ret == NONE || ret == SCRIPT_CANCELED) {
				queueEntries[0]->status = QueueStatus::Done;

			} else { // Else it failed..
				queueEntries[0]->status = QueueStatus::Failed;
			}

			/* Display if failed or succeeded. */
			if (config->prompt()) {
				char msg[256];

				if (QueueSystem::CancelCallback) {
					snprintf(msg, sizeof(msg), Lang::get("ACTION_CANCELED").c_str(), queueEntries[0]->name.c_str());

				} else {
					if (queueEntries[0]->status == QueueStatus::Failed) {
						snprintf(msg, sizeof(msg), Lang::get("ACTION_FAILED").c_str(), queueEntries[0]->name.c_str());

					} else {
						snprintf(msg, sizeof(msg), Lang::get("ACTION_SUCCEEDED").c_str(), queueEntries[0]->name.c_str());
					}
				}

				QueueSystem::EndMsg = msg;
				QueueSystem::Popup = true;
			}

			if (queueEntries[0]->status == QueueStatus::Done) { // ONLY update, if successful.
				if (StoreUtils::meta) {
					StoreUtils::meta->SetUpdated(queueEntries[0]->unistoreName, queueEntries[0]->entryName, queueEntries[0]->lastUpdated);
					StoreUtils::meta->SetInstalled(queueEntries[0]->unistoreName, queueEntries[0]->entryName, queueEntries[0]->name);
					StoreUtils::RefreshUpdateAVL();
				}
			}

			if (QueueSystem::CancelCallback) QueueSystem::CancelCallback = false; // Reset.

			queueEntries.pop_front();
			if (QueueSystem::LastElement != 0) QueueSystem::LastElement = 0;
			if (queueEntries.empty()) QueueRuns = false; // The queue ended.
			ret = NONE; // Reset.
		}
	}

	aptSetHomeAllowed(true);
}