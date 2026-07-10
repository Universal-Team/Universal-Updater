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

#include "exclusiveMode.hpp"
#include "files.hpp"
#include "gui.hpp"
#include "queueSystem.hpp"
#include "scriptUtils.hpp"
#include "storeUtils.hpp"
#include <unistd.h>

std::deque<QueueEntry> queueEntries;
int QueueSystem::RequestNeeded = -1, QueueSystem::RequestAnswer = -1;
bool QueueSystem::Wait = false, QueueSystem::Popup = false, QueueSystem::CancelCallback = false;
std::string QueueSystem::RequestMsg = "", QueueSystem::RequestMsgName = "", QueueSystem::EndMsg = "";
int QueueSystem::PriorElement = 0;

bool QueueRuns = false;
static Thread queueThread = nullptr;

/*
	Adds an entry to the queue.

	nlohmann::json obj: The object.
	C2D_Image icn: The icon.
*/
void QueueSystem::AddToQueue(const std::shared_ptr<StoreEntry> &entry, int script) {
	queueEntries.emplace_back(entry, script);

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
		ExclusiveMode::Enter();
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
	if (queueEntries.empty()) return;

	ExclusiveMode::Enter();
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

		QueueEntry &entry = queueEntries[0];
		const Script &script = entry.storeEntry->GetScript(entry.scriptIndex);
		for(int i = QueueSystem::PriorElement; ret == NONE && i < (int)script.GetActions().size() && !QueueSystem::CancelCallback; i++) {
			const Action &action = script.GetAction(i);
			entry.currentStep++;

			switch (action.GetType()) {
				case Action::Type::DeleteFile: {
					entry.status = QueueStatus::Deleting;
					ret = ScriptUtils::removeFile(action.GetInput());
					break;
				}

				case Action::Type::DownloadFile: {
					entry.status = QueueStatus::Downloading;
					ret = ScriptUtils::downloadFile(action.GetInput(), action.GetOutput());
					break;
				}

				case Action::Type::DownloadRelease: {
					entry.status = QueueStatus::Downloading;
					ret = ScriptUtils::downloadRelease(action.GetExtra(), action.GetInput(), action.GetOutput(), action.GetPrereleases());
					break;
				}

				case Action::Type::ExtractFile: {
					entry.status = QueueStatus::Extracting;
					ret = ScriptUtils::extractFile(action.GetExtra(), action.GetInput(), action.GetOutput());
					break;
				}

				case Action::Type::InstallCia: {
					entry.status = QueueStatus::Installing;
					ScriptUtils::installFile(action.GetInput());
					break;
				}

				case Action::Type::Mkdir: {
					makeDirs(action.GetInput().c_str());
					break;
				}

				case Action::Type::Rmdir: {
					if (action.GetInput() == "") {
						ret = SYNTAX_ERROR;
					} else if (access(action.GetInput().c_str(), F_OK) == 0) {
						// Only prompt if the directory is real.
						entry.status = QueueStatus::Request;
						if (QueueSystem::RequestNeeded == RMDIR_REQUEST) {
							/* There we already did it. :) */
							entry.status = QueueStatus::Deleting;
							if (QueueSystem::RequestAnswer == 1) removeDirRecursive(action.GetInput().c_str());
							/* Reset. */
							QueueSystem::RequestNeeded = NO_REQUEST;
							QueueSystem::RequestAnswer = NO_REQUEST;
							QueueSystem::RequestMsg = "";

						} else {
							/* We are in the process of the need of an answer. */
							QueueSystem::RequestNeeded = RMDIR_REQUEST;
							QueueSystem::RequestMsg = Lang::get("DELETE_PROMPT") + "\n" + action.GetInput();
							QueueSystem::PriorElement = i; // So we know, where we go again after the Request.
							ret = PROMPT_RET;
						}
					}
					break;
				}

				case Action::Type::PromptMessage: {
					entry.status = QueueStatus::Request;
					std::string name = entry.storeEntry->GetTitle() + "/" + action.GetExtra();
					if (!name.empty() && config->savedPrompt(name) != PromptValue::Unset) {
						if (config->savedPrompt(name) == PromptValue::No) {
							if(action.GetCount() > 0) {
								i += action.GetCount();
								entry.currentStep += action.GetCount();
							} else {
								ret = SCRIPT_CANCELED;
							}
						}
					} else if (QueueSystem::RequestNeeded == PROMPT_REQUEST) {
						/* This happens when we come back after answering. */
						if (QueueSystem::RequestAnswer == SCRIPT_CANCELED) {
							if(action.GetCount() > 0) {
								i += action.GetCount();
								entry.currentStep += action.GetCount();
							} else {
								ret = SCRIPT_CANCELED;
							}
						}

						/* Reset. */
						QueueSystem::RequestAnswer = NO_REQUEST;
						QueueSystem::RequestNeeded = NO_REQUEST;
						QueueSystem::RequestMsg = "";
						QueueSystem::RequestMsgName = "";

					} else {
						QueueSystem::RequestNeeded = PROMPT_REQUEST;
						QueueSystem::RequestMsg = action.GetInput();
						QueueSystem::RequestMsgName = name;
						QueueSystem::PriorElement = i; // So we know, where we go again after the Request.
						ret = PROMPT_RET;
					}
					break;
				}

				case Action::Type::Error:
				case Action::Type::Exit: {
					ret = SCRIPT_CANCELED;
					break;
				}

				case Action::Type::Copy: {
					entry.status = QueueStatus::Copying;
					ret = ScriptUtils::copyFile(action.GetInput(), action.GetOutput());
					break;
				}

				case Action::Type::Move: {
					entry.status = QueueStatus::Moving;
					ret = ScriptUtils::renameFile(action.GetInput(), action.GetOutput());
					break;
				}

				case Action::Type::Skip: {
					if (action.GetCount() > 0) {
						i += action.GetCount();
						entry.currentStep += action.GetCount();
					}
					break;
				}
			}
		}

		/* If we expect a prompt, we go to this. */
		if (ret == PROMPT_RET) {
			queueEntries[0].currentStep = QueueSystem::PriorElement + 1; // Cause no Zero.
			QueueSystem::Wait = true;
			QueueRuns = false;
		}

		if (!QueueSystem::Wait) {
			/* Canceled or None is for me -> Done. */
			if (ret == NONE || ret == SCRIPT_CANCELED) {
				queueEntries[0].status = QueueStatus::Done;

			} else { // Else it failed..
				queueEntries[0].status = QueueStatus::Failed;
			}

			/* Display if failed or succeeded. */
			if (config->prompt()) {
				char msg[256];

				if (QueueSystem::CancelCallback) {
					snprintf(msg, sizeof(msg), Lang::get("ACTION_CANCELED").c_str(), script.GetName().c_str());

				} else {
					if (queueEntries[0].status == QueueStatus::Failed) {
						snprintf(msg, sizeof(msg), Lang::get("ACTION_FAILED").c_str(), script.GetName().c_str());

					} else {
						snprintf(msg, sizeof(msg), Lang::get("ACTION_SUCCEEDED").c_str(), script.GetName().c_str());
					}
				}

				QueueSystem::EndMsg = msg;
				QueueSystem::Popup = true;
			}

			if (queueEntries[0].status == QueueStatus::Done) { // ONLY update, if successful.
				if (StoreUtils::meta) {
					StoreUtils::meta->SetUpdated(entry.storeEntry->GetUniStore(), entry.storeEntry->GetTitle(), entry.storeEntry->GetLastUpdated());
					StoreUtils::meta->SetInstalled(entry.storeEntry->GetUniStore(), entry.storeEntry->GetTitle(), script.GetName());
					StoreUtils::RefreshInstalledApps(entry.storeEntry->GetTitle());
				}
			}

			if (QueueSystem::CancelCallback) QueueSystem::CancelCallback = false; // Reset.

			queueEntries.pop_front();
			if (QueueSystem::PriorElement != 0) QueueSystem::PriorElement = 0;
			if (queueEntries.empty()) QueueRuns = false; // The queue ended.
			ret = NONE; // Reset.
		}
	}

	ExclusiveMode::Exit();
}