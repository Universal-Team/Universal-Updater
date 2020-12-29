/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 Universal-Team
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
#include "queueSystem.hpp"
#include "scriptUtils.hpp"
#include <unistd.h>

std::deque<std::unique_ptr<Queue>> queueEntries;
bool QueueRuns = false;
static Thread queueThread = nullptr;

/*
	Adds an entry to the queue.

	nlohmann::json obj: The object.
	C2D_Image icn: The icon.
*/
void QueueSystem::AddToQueue(nlohmann::json obj, C2D_Image icn, std::string name) {
	queueEntries.push_back( std::make_unique<Queue>(obj, icn, name) );

	/* If not already running, let it run!! */
	if (!QueueRuns) {
		QueueRuns = true; // We enable the queue run state here.
		s32 prio = 0;
		svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
		queueThread = threadCreate((ThreadFunc)QueueSystem::QueueHandle, NULL, 64 * 1024, prio - 1, -2, false);
	}
}

/*
	Clears the queue.
*/
void QueueSystem::ClearQueue() {
	QueueRuns = false;
	queueEntries.clear();
}

/*
	The whole handle.
*/
void QueueSystem::QueueHandle() {
	while(QueueRuns) {
		Result ret = NONE; // No Error as of yet.

		queueEntries[0]->total = queueEntries[0]->obj.size();
		queueEntries[0]->current = 0;

		for(int i = 0; i < (int)queueEntries[0]->obj.size(); i++) {
			queueEntries[0]->current++;

			if (ret == NONE) {
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


					if (queueEntries[0]->obj[i].contains("file") && queueEntries[0]->obj[i]["file"].is_string()) {
						file = queueEntries[0]->obj[i]["file"];
					} else missing = true;

					if (!missing) ret = ScriptUtils::removeFile(file, "");
					else ret = SYNTAX_ERROR;

					/* Downloading from a URL. */
				} else if (type == "downloadFile") {
					queueEntries[0]->status = QueueStatus::Downloading;

					bool missing = false;
					std::string file = "", output = "";

					if (queueEntries[0]->obj[i].contains("file") && queueEntries[0]->obj[i]["file"].is_string()) {
						file = queueEntries[0]->obj[i]["file"];
					} else missing = true;

					if (queueEntries[0]->obj[i].contains("output") && queueEntries[0]->obj[i]["output"].is_string()) {
						output = queueEntries[0]->obj[i]["output"];
					} else missing = true;

					if (!missing) ret = ScriptUtils::downloadFile(file, output, "");
					else ret = SYNTAX_ERROR;

					/* Download from a GitHub Release. */
				} else if (type == "downloadRelease") {
					queueEntries[0]->status = QueueStatus::Downloading;
					bool missing = false, includePrereleases = false;
					std::string repo = "", file = "", output = "";

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

					if (!missing) ret = ScriptUtils::downloadRelease(repo, file, output, includePrereleases, "");
					else ret = SYNTAX_ERROR;

					/* Extracting files. */
				} else if (type == "extractFile") {
					queueEntries[0]->status = QueueStatus::Extracting;
					bool missing = false;
					std::string file = "", input = "", output = "";

					if (queueEntries[0]->obj[i].contains("file") && queueEntries[0]->obj[i]["file"].is_string()) {
						file = queueEntries[0]->obj[i]["file"];
					} else missing = true;

					if (queueEntries[0]->obj[i].contains("input") && queueEntries[0]->obj[i]["input"].is_string()) {
						input = queueEntries[0]->obj[i]["input"];
					} else missing = true;

					if (queueEntries[0]->obj[i].contains("output") && queueEntries[0]->obj[i]["output"].is_string()) {
						output = queueEntries[0]->obj[i]["output"];
					} else missing = true;

					if (!missing) ScriptUtils::extractFile(file, input, output, "");
					else ret = SYNTAX_ERROR;

				/* Installing CIAs. */
				} else if (type == "installCia") {
					queueEntries[0]->status = QueueStatus::Installing;
					bool missing = false, updateSelf = false;
					std::string file = "";

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

				} else if (type == "rmdir") {
					bool missing = false;
					std::string directory = "", message = "", promptmsg = "";

					if (queueEntries[0]->obj[i].contains("directory") && queueEntries[0]->obj[i]["directory"].is_string()) {
						directory = queueEntries[0]->obj[i]["directory"];
					} else missing = true;

					promptmsg = Lang::get("DELETE_PROMPT") + "\n" + directory;
					if (!missing && directory != "") {
						if (access(directory.c_str(), F_OK) != 0) ret = DELETE_ERROR;
						else {
							if (Msg::promptMsg(promptmsg)) removeDirRecursive(directory.c_str());
						}
					}

					else ret = SYNTAX_ERROR;

				} else if (type == "promptMessage" || type == "promptMsg") {
					std::string Message = "";
					int skipCount = -1;

					if (queueEntries[0]->obj[i].contains("message") && queueEntries[0]->obj[i]["message"].is_string()) {
						Message = queueEntries[0]->obj[i]["message"];
					}

					if (queueEntries[0]->obj[i].contains("count") && queueEntries[0]->obj[i]["count"].is_number()) {
						skipCount = queueEntries[0]->obj[i]["count"];
					}

					Result res = ScriptUtils::prompt(Message);

					if (skipCount > -1 && res == SCRIPT_CANCELED) i += skipCount; // Skip.

				} else if (type == "exit") {
					ret = SCRIPT_CANCELED;

				} else if (type == "copy") {
					std::string source = "", destination = "";
					bool missing = false;

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
		}

		/* Canceled or None is for me -> Done. */
		if (ret == NONE || ret == SCRIPT_CANCELED) {
			queueEntries[0]->status = QueueStatus::Done;

		} else { // Else it failed..
			queueEntries[0]->status = QueueStatus::Failed;
		}

		queueEntries.pop_front();
		if (queueEntries.empty()) QueueRuns = false; // The queue ended.
		ret = NONE; // Reset.

		/* TODO: Display a message if something ends? */
	}
}