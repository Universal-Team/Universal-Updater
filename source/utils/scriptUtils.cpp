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

#include "animation.hpp"
#include "cia.hpp"
#include "download.hpp"
#include "extract.hpp"
#include "fileBrowse.hpp"
#include "files.hpp"
#include "scriptUtils.hpp"
#include <regex>
#include <unistd.h>

extern bool showProgressBar;
extern ProgressBar progressbarType;
extern char progressBarMsg[128];
extern int filesExtracted, extractFilesCount;

extern void downloadFailed();
static Thread thread;

bool ScriptUtils::matchPattern(const std::string &pattern, const std::string &tested) {
	std::regex patternRegex(pattern);
	return regex_match(tested, patternRegex);
}

/* Remove a File. */
Result ScriptUtils::removeFile(const std::string &file, bool isARG) {
	std::string out;
	out = std::regex_replace(file, std::regex("%ARCHIVE_DEFAULT%"), config->archPath());
	out = std::regex_replace(out, std::regex("%3DSX%/(.*)\\.(.*)"), config->_3dsxPath() + (config->_3dsxInFolder() ? "/$1/$1.$2" : "/$1.$2"));
	out = std::regex_replace(out, std::regex("%3DSX%"), config->_3dsxPath());
	out = std::regex_replace(out, std::regex("%NDS%"), config->ndsPath());
	out = std::regex_replace(out, std::regex("%FIRM%"), config->firmPath());

	Result ret = NONE;
	if (access(out.c_str(), F_OK) != 0) return DELETE_ERROR;

	deleteFile(out.c_str());
	return ret;
}

/* Prompt message. */
Result ScriptUtils::prompt(const std::string &message, const std::string &name) {
	Result ret = NONE;
	if (!Msg::promptMsg(message, name)) ret = SCRIPT_CANCELED;

	return ret;
}

/* Copy. */
Result ScriptUtils::copyFile(const std::string &source, const std::string &destination, const std::string &message, bool isARG) {
	Result ret = NONE;
	if (access(source.c_str(), F_OK) != 0) return COPY_ERROR;

	std::string _source, _dest;
	_source = std::regex_replace(source, std::regex("%ARCHIVE_DEFAULT%"), config->archPath());
	_source = std::regex_replace(_source, std::regex("%3DSX%/(.*)\\.(.*)"), config->_3dsxPath() + (config->_3dsxInFolder() ? "/$1/$1.$2" : "/$1.$2"));
	_source = std::regex_replace(_source, std::regex("%3DSX%"), config->_3dsxPath());
	_source = std::regex_replace(_source, std::regex("%NDS%"), config->ndsPath());
	_source = std::regex_replace(_source, std::regex("%FIRM%"), config->firmPath());

	_dest = std::regex_replace(destination, std::regex("%ARCHIVE_DEFAULT%"), config->archPath());
	_dest = std::regex_replace(_dest, std::regex("%3DSX%/(.*)\\.(.*)"), config->_3dsxPath() + (config->_3dsxInFolder() ? "/$1/$1.$2" : "/$1.$2"));
	_dest = std::regex_replace(_dest, std::regex("%3DSX%"), config->_3dsxPath());
	_dest = std::regex_replace(_dest, std::regex("%NDS%"), config->ndsPath());
	_dest = std::regex_replace(_dest, std::regex("%FIRM%"), config->firmPath());

	if (isARG) {
		snprintf(progressBarMsg, sizeof(progressBarMsg), message.c_str());
		showProgressBar = true;
		progressbarType = ProgressBar::Copying;

		s32 prio = 0;
		svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
		thread = threadCreate((ThreadFunc)Animation::displayProgressBar, NULL, 64 * 1024, prio - 1, -2, false);
	}

	/* If destination does not exist, create dirs. */
	if (access(_dest.c_str(), F_OK) != 0) makeDirs(_dest.c_str());
	ret = fcopy(_source.c_str(), _dest.c_str());

	if (ret == -1) ret = COPY_ERROR;
	else if (ret == 1) ret = NONE;

	if (isARG) {
		showProgressBar = false;
		threadJoin(thread, U64_MAX);
		threadFree(thread);
	}

	return ret;
}

/* Rename / Move a file. */
Result ScriptUtils::renameFile(const std::string &oldName, const std::string &newName, bool isARG) {
	Result ret = NONE;
	if (access(oldName.c_str(), F_OK) != 0) return MOVE_ERROR;

	std::string old, _new;
	old = std::regex_replace(oldName, std::regex("%ARCHIVE_DEFAULT%"), config->archPath());
	old = std::regex_replace(old, std::regex("%3DSX%/(.*)\\.(.*)"), config->_3dsxPath() + (config->_3dsxInFolder() ? "/$1/$1.$2" : "/$1.$2"));
	old = std::regex_replace(old, std::regex("%3DSX%"), config->_3dsxPath());
	old = std::regex_replace(old, std::regex("%NDS%"), config->ndsPath());
	old = std::regex_replace(old, std::regex("%FIRM%"), config->firmPath());

	_new = std::regex_replace(newName, std::regex("%ARCHIVE_DEFAULT%"), config->archPath());
	_new = std::regex_replace(_new, std::regex("%3DSX%/(.*)\\.(.*)"), config->_3dsxPath() + (config->_3dsxInFolder() ? "/$1/$1.$2" : "/$1.$2"));
	_new = std::regex_replace(_new, std::regex("%3DSX%"), config->_3dsxPath());
	_new = std::regex_replace(_new, std::regex("%NDS%"), config->ndsPath());
	_new = std::regex_replace(_new, std::regex("%FIRM%"), config->firmPath());

	/* TODO: Kinda avoid that? */
	makeDirs(_new.c_str());
	rename(old.c_str(), _new.c_str());
	return ret;
}

/* Download from GitHub Release. */
Result ScriptUtils::downloadRelease(const std::string &repo, const std::string &file, const std::string &output, bool includePrereleases, const std::string &message, bool isARG) {
	std::string out;
	out = std::regex_replace(output, std::regex("%3DSX%/(.*)\\.(.*)"), config->_3dsxPath() + (config->_3dsxInFolder() ? "/$1/$1.$2" : "/$1.$2"));
	out = std::regex_replace(out, std::regex("%3DSX%"), config->_3dsxPath());
	out = std::regex_replace(out, std::regex("%NDS%"), config->ndsPath());
	out = std::regex_replace(out, std::regex("%ARCHIVE_DEFAULT%"), config->archPath());
	out = std::regex_replace(out, std::regex("%FIRM%"), config->firmPath());

	Result ret = NONE;

	if (isARG) {
		snprintf(progressBarMsg, sizeof(progressBarMsg), message.c_str());
		showProgressBar = true;
		progressbarType = ProgressBar::Downloading;

		s32 prio = 0;
		svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
		thread = threadCreate((ThreadFunc)Animation::displayProgressBar, NULL, 64 * 1024, prio - 1, -2, false);
	}

	if (downloadFromRelease("https://github.com/" + repo, file, out, includePrereleases) != 0) {
		ret = FAILED_DOWNLOAD;

		if (isARG) {
			showProgressBar = false;

			downloadFailed();

			threadJoin(thread, U64_MAX);
			threadFree(thread);
		}
		return ret;
	}

	if (isARG) {
		showProgressBar = false;
		threadJoin(thread, U64_MAX);
		threadFree(thread);
	}

	return ret;
}

/* Download a file. */
Result ScriptUtils::downloadFile(const std::string &file, const std::string &output, const std::string &message, bool isARG) {
	std::string out;
	out = std::regex_replace(output, std::regex("%3DSX%/(.*)\\.(.*)"), config->_3dsxPath() + (config->_3dsxInFolder() ? "/$1/$1.$2" : "/$1.$2"));
	out = std::regex_replace(out, std::regex("%3DSX%"), config->_3dsxPath());
	out = std::regex_replace(out, std::regex("%NDS%"), config->ndsPath());
	out = std::regex_replace(out, std::regex("%ARCHIVE_DEFAULT%"), config->archPath());
	out = std::regex_replace(out, std::regex("%FIRM%"), config->firmPath());

	Result ret = NONE;

	if (isARG) {
		snprintf(progressBarMsg, sizeof(progressBarMsg), message.c_str());
		showProgressBar = true;
		progressbarType = ProgressBar::Downloading;

		s32 prio = 0;
		svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
		thread = threadCreate((ThreadFunc)Animation::displayProgressBar, NULL, 64 * 1024, prio - 1, -2, false);
	}

	if (downloadToFile(file, out) != 0) {
		ret = FAILED_DOWNLOAD;

		if (isARG) {
			showProgressBar = false;

			downloadFailed();

			threadJoin(thread, U64_MAX);
			threadFree(thread);
		}

		return ret;
	}

	if (isARG) {
		showProgressBar = false;
		threadJoin(thread, U64_MAX);
		threadFree(thread);
	}

	return ret;
}

/* Install CIA files. */
void ScriptUtils::installFile(const std::string &file, bool updatingSelf, const std::string &message, bool isARG) {
	std::string in;
	in = std::regex_replace(file, std::regex("%ARCHIVE_DEFAULT%"), config->archPath());
	in = std::regex_replace(in, std::regex("%3DSX%/(.*)\\.(.*)"), config->_3dsxPath() + (config->_3dsxInFolder() ? "/$1/$1.$2" : "/$1.$2"));
	in = std::regex_replace(in, std::regex("%3DSX%"), config->_3dsxPath());
	in = std::regex_replace(in, std::regex("%NDS%"), config->ndsPath());
	in = std::regex_replace(in, std::regex("%FIRM%"), config->firmPath());

	if (isARG) {
		snprintf(progressBarMsg, sizeof(progressBarMsg), message.c_str());
		showProgressBar = true;
		progressbarType = ProgressBar::Installing;

		s32 prio = 0;
		svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
		thread = threadCreate((ThreadFunc)Animation::displayProgressBar, NULL, 64 * 1024, prio - 1, -2, false);
	}

	Title::Install(in.c_str(), updatingSelf);

	if (isARG) {
		showProgressBar = false;
		threadJoin(thread, U64_MAX);
		threadFree(thread);
	}
}

/* Extract files. */
Result ScriptUtils::extractFile(const std::string &file, const std::string &input, const std::string &output, const std::string &message, bool isARG) {
	extractFilesCount = 0;
	Result ret = NONE;

	std::string out, in;
	in = std::regex_replace(file, std::regex("%ARCHIVE_DEFAULT%"), config->archPath());
	in = std::regex_replace(in, std::regex("%3DSX%/(.*)\\.(.*)"), config->_3dsxPath() + (config->_3dsxInFolder() ? "/$1/$1.$2" : "/$1.$2"));
	in = std::regex_replace(in, std::regex("%3DSX%"), config->_3dsxPath());
	in = std::regex_replace(in, std::regex("%NDS%"), config->ndsPath());
	in = std::regex_replace(in, std::regex("%FIRM%"), config->firmPath());

	out = std::regex_replace(output, std::regex("%ARCHIVE_DEFAULT%"), config->archPath());
	out = std::regex_replace(out, std::regex("%3DSX%/(.*)\\.(.*)"), config->_3dsxPath() + (config->_3dsxInFolder() ? "/$1/$1.$2" : "/$1.$2"));
	out = std::regex_replace(out, std::regex("%3DSX%"), config->_3dsxPath());
	out = std::regex_replace(out, std::regex("%NDS%"), config->ndsPath());
	out = std::regex_replace(out, std::regex("%FIRM%"), config->firmPath());

	if (isARG) {
		snprintf(progressBarMsg, sizeof(progressBarMsg), message.c_str());
		showProgressBar = true;
		progressbarType = ProgressBar::Extracting;

		s32 prio = 0;
		svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
		thread = threadCreate((ThreadFunc)Animation::displayProgressBar, NULL, 64 * 1024, prio - 1, -2, false);
	}

	filesExtracted = 0;

	getExtractedSize(in, input);
	if(extractArchive(in, input, out) != EXTRACT_ERROR_NONE) {
		ret = EXTRACT_ERROR;
	}

	if (isARG) {
		showProgressBar = false;
		threadJoin(thread, U64_MAX);
		threadFree(thread);
	}

	return ret;
}

/*
	NOTE: This is for the argument system for now. This might get replaced completely with the Queue System in the future.
*/
Result ScriptUtils::runFunctions(const StoreEntry &entry, const Script &script) {
	Result ret = NONE; // No Error as of yet.

	for(int i = 0; i < (int)script.GetActions().size() && ret == NONE; i++) {
		const Action &action = script.GetAction(i);
		switch (action.GetType()) {
			case Action::Type::DeleteFile: {
				ret = ScriptUtils::removeFile(action.GetInput(), true);
				break;
			}

			case Action::Type::DownloadFile: {
				char message[256];
				snprintf(message, sizeof(message), Lang::get("SHORTCUT_DOWNLOADING").c_str(), action.GetOutput().substr(action.GetOutput().find_first_of("/") + 1).c_str());
				ret = ScriptUtils::downloadFile(action.GetInput(), action.GetOutput(), message, true);
				break;
			}

			case Action::Type::DownloadRelease: {
				char message[256];
				snprintf(message, sizeof(message), Lang::get("SHORTCUT_DOWNLOADING").c_str(), action.GetOutput().substr(action.GetOutput().find_first_of("/") + 1).c_str());
				ret = ScriptUtils::downloadRelease(action.GetExtra(), action.GetInput(), action.GetOutput(), action.GetPrereleases(), message, true);
				break;
			}

			case Action::Type::ExtractFile: {
				char message[256];
				snprintf(message, sizeof(message), Lang::get("SHORTCUT_EXTRACTING").c_str(), action.GetExtra().substr(action.GetExtra().find_first_of("/") + 1).c_str());
				ret = ScriptUtils::extractFile(action.GetExtra(), action.GetInput(), action.GetOutput(), message, true);
				break;
			}

			case Action::Type::InstallCia: {
				char message[256];
				snprintf(message, sizeof(message), Lang::get("SHORTCUT_INSTALLING").c_str(), action.GetInput().substr(action.GetInput().find_first_of("/") + 1).c_str());
				ScriptUtils::installFile(action.GetInput(), false, message, true);
				break;
			}

			case Action::Type::Mkdir: {
				makeDirs(action.GetInput().c_str());
				break;
			}

			case Action::Type::Rmdir: {
				if (action.GetInput() == "") {
					ret = SYNTAX_ERROR;
				} else {
					std::string message = Lang::get("DELETE_PROMPT") + "\n" + action.GetInput();
					if (access(action.GetInput().c_str(), F_OK) == 0) {
						if (Msg::promptMsg(message)) removeDirRecursive(action.GetInput().c_str());
					}
				}
				break;
			}

			case Action::Type::PromptMessage: {
				std::string name = entry.GetTitle() + "/" + action.GetExtra();
				ret = ScriptUtils::prompt(action.GetInput(), name);
				if (action.GetCount() > 0 && ret == SCRIPT_CANCELED) {
					ret = NONE;
					i += action.GetCount(); // Skip.
				}
				break;
			}

			case Action::Type::Error:
			case Action::Type::Exit: {
				ret = SCRIPT_CANCELED;
				break;
			}

			case Action::Type::Copy: {
				char message[256];
				snprintf(message, sizeof(message), Lang::get("SHORTCUT_COPYING").c_str(), action.GetInput().substr(action.GetInput().find_first_of("/") + 1).c_str());
				ret = ScriptUtils::copyFile(action.GetInput(), action.GetOutput(), message, true);
				break;
			}

			case Action::Type::Move: {
				ret = ScriptUtils::renameFile(action.GetInput(), action.GetOutput(), true);
				break;
			}

			case Action::Type::Skip: {
				if (action.GetCount() > 0) i += action.GetCount();
				break;
			}
		}
	}

	if (ret == NONE || ret == SCRIPT_CANCELED) doneMsg();
	else if (ret == FAILED_DOWNLOAD) Msg::waitMsg(Lang::get("DOWNLOAD_ERROR"));
	else if (ret == SYNTAX_ERROR) Msg::waitMsg(Lang::get("SYNTAX_ERROR"));
	else if (ret == COPY_ERROR) Msg::waitMsg(Lang::get("COPY_ERROR"));
	else if (ret == MOVE_ERROR) Msg::waitMsg(Lang::get("MOVE_ERROR"));
	else if (ret == DELETE_ERROR) Msg::waitMsg(Lang::get("DELETE_ERROR"));
	else if (ret == EXTRACT_ERROR) Msg::waitMsg(Lang::get("EXTRACT_ERROR"));
	return ret;
}