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
extern int filesExtracted;

extern void downloadFailed();

static Thread thread;

bool ScriptUtils::matchPattern(std::string pattern, std::string tested) {
	std::regex patternRegex(pattern);
	return regex_match(tested, patternRegex);
}

/*
	Remove a File.
*/
Result ScriptUtils::removeFile(std::string file, std::string message) {
	std::string out;
	out = std::regex_replace(file, std::regex("%ARCHIVE_DEFAULT%"), "sdmc:");

	Result ret = NONE;
	if (access(out.c_str(), F_OK) != 0) return DELETE_ERROR;

	Msg::DisplayMsg(message);
	deleteFile(out.c_str());
	return ret;
}

/*
	Boot a title.
*/
void ScriptUtils::bootTitle(const std::string TitleID, bool isNAND, std::string message) {
	std::string MSG = Lang::get("BOOT_TITLE") + "\n\n";
	if (isNAND)	MSG += Lang::get("MEDIATYPE_NAND") + "\n" + TitleID;
	else MSG += Lang::get("MEDIATYPE_SD") + "\n" + TitleID;

	const u64 ID = std::stoull(TitleID, 0, 16);
	if (Msg::promptMsg(MSG)) {
		Msg::DisplayMsg(message);
		CIA_LaunchTitle(ID, isNAND ? MEDIATYPE_NAND : MEDIATYPE_SD);
	}
}

/*
	Prompt message.
*/
Result ScriptUtils::prompt(std::string message) {
	Result ret = NONE;
	if (!Msg::promptMsg(message)) ret = SCRIPT_CANCELED;

	return ret;
}

/*
	Copy.
*/
Result ScriptUtils::copyFile(std::string source, std::string destination, std::string message) {
	Result ret = NONE;
	if (access(source.c_str(), F_OK) != 0) return COPY_ERROR;

	Msg::DisplayMsg(message);
	/* If destination does not exist, create dirs. */
	if (access(destination.c_str(), F_OK) != 0) makeDirs(destination.c_str());

	fcopy(source.c_str(), destination.c_str());
	return ret;
}

/*
	Rename / Move a file.
*/
Result ScriptUtils::renameFile(std::string oldName, std::string newName, std::string message) {
	Result ret = NONE;
	if (access(oldName.c_str(), F_OK) != 0) return MOVE_ERROR;

	Msg::DisplayMsg(message);

	/* TODO: Kinda avoid that? */
	makeDirs(newName.c_str());
	rename(oldName.c_str(), newName.c_str());
	return ret;
}

/*
	Download from GitHub Release.
*/
Result ScriptUtils::downloadRelease(std::string repo, std::string file, std::string output, bool includePrereleases, std::string message) {
	std::string out;
	out = std::regex_replace(output, std::regex("%3DSX%"), "sdmc:/3ds");
	out = std::regex_replace(out, std::regex("%NDS%"), "sdmc:");
	out = std::regex_replace(out, std::regex("%ARCHIVE_DEFAULT%"), "sdmc:");

	Result ret = NONE;

	snprintf(progressBarMsg, sizeof(progressBarMsg), message.c_str());
	showProgressBar = true;
	progressbarType = ProgressBar::Downloading;

	s32 prio = 0;
	svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
	thread = threadCreate((ThreadFunc)displayProgressBar, NULL, 64 * 1024, prio - 1, -2, false);

	if (downloadFromRelease("https://github.com/" + repo, file, out, message, includePrereleases) != 0) {
		showProgressBar = false;
		downloadFailed();
		ret = FAILED_DOWNLOAD;
		threadJoin(thread, U64_MAX);
		threadFree(thread);
		return ret;
	}

	showProgressBar = false;
	threadJoin(thread, U64_MAX);
	threadFree(thread);
	return ret;
}

/*
	Download a file.
*/
Result ScriptUtils::downloadFile(std::string file, std::string output, std::string message) {
	std::string out;
	out = std::regex_replace(output, std::regex("%3DSX%"), "sdmc:/3ds");
	out = std::regex_replace(out, std::regex("%NDS%"), "sdmc:");
	out = std::regex_replace(out, std::regex("%ARCHIVE_DEFAULT%"), "sdmc:");

	Result ret = NONE;
	snprintf(progressBarMsg, sizeof(progressBarMsg), message.c_str());
	showProgressBar = true;
	progressbarType = ProgressBar::Downloading;

	s32 prio = 0;
	svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
	thread = threadCreate((ThreadFunc)displayProgressBar, NULL, 64 * 1024, prio - 1, -2, false);

	if (downloadToFile(file, out) != 0) {
		showProgressBar = false;
		downloadFailed();
		ret = FAILED_DOWNLOAD;
		threadJoin(thread, U64_MAX);
		threadFree(thread);
		return ret;
	}

	showProgressBar = false;
	threadJoin(thread, U64_MAX);
	threadFree(thread);
	return ret;
}

/*
	Install CIA files.
*/
void ScriptUtils::installFile(std::string file, bool updatingSelf, std::string message) {
	snprintf(progressBarMsg, sizeof(progressBarMsg), message.c_str());
	showProgressBar = true;
	progressbarType = ProgressBar::Installing;

	s32 prio = 0;
	svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
	thread = threadCreate((ThreadFunc)displayProgressBar, NULL, 64 * 1024, prio - 1, -2, false);

	installCia(file.c_str(), updatingSelf);
	showProgressBar = false;
	threadJoin(thread, U64_MAX);
	threadFree(thread);
}

/*
	Extract files.
*/
void ScriptUtils::extractFile(std::string file, std::string input, std::string output, std::string message) {
	std::string out, in;
	in = std::regex_replace(file, std::regex("%ARCHIVE_DEFAULT%"), "sdmc:");
	out = std::regex_replace(output, std::regex("%ARCHIVE_DEFAULT%"), "sdmc:");

	snprintf(progressBarMsg, sizeof(progressBarMsg), message.c_str());
	showProgressBar = true;
	filesExtracted = 0;
	progressbarType = ProgressBar::Extracting;

	s32 prio = 0;
	svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
	thread = threadCreate((ThreadFunc)displayProgressBar, NULL, 64 * 1024, prio - 1, -2, false);

	extractArchive(in, input, out);
	showProgressBar = false;
	threadJoin(thread, U64_MAX);
	threadFree(thread);
}

/*
	Execute | run the script.
*/
Result ScriptUtils::runFunctions(nlohmann::json storeJson, int selection, std::string entry) {
	Result ret = NONE; // No Error as of yet.

	if (!storeJson.contains("storeContent")) { Msg::waitMsg(Lang::get("SYNTAX_ERROR")); return SYNTAX_ERROR; };
	if ((int)storeJson["storeContent"].size() < selection) { Msg::waitMsg(Lang::get("SYNTAX_ERROR")); return SYNTAX_ERROR; };
	if (!storeJson["storeContent"][selection].contains(entry)) { Msg::waitMsg(Lang::get("SYNTAX_ERROR")); return SYNTAX_ERROR; };

	for(int i = 0; i < (int)storeJson["storeContent"][selection][entry].size(); i++) {
		if (ret == NONE) {
			std::string type = "";

			if (storeJson["storeContent"][selection][entry][i].contains("type") && storeJson["storeContent"][selection][entry][i]["type"].is_string()) {
				type = storeJson["storeContent"][selection][entry][i]["type"];

			} else {
				ret = SYNTAX_ERROR;
			}

			if (type == "deleteFile") {
				bool missing = false;
				std::string file = "", message = "";


				if (storeJson["storeContent"][selection][entry][i].contains("file") && storeJson["storeContent"][selection][entry][i]["file"].is_string()) {
					file = storeJson["storeContent"][selection][entry][i]["file"];
				}
				else missing = true;

				if (storeJson["storeContent"][selection][entry][i].contains("message") && storeJson["storeContent"][selection][entry][i]["message"].is_string()) {
					message = storeJson["storeContent"][selection][entry][i]["message"];
				}

				if (!missing) ret = ScriptUtils::removeFile(file, message);
				else ret = SYNTAX_ERROR;

			} else if (type == "downloadFile") {
				bool missing = false;
				std::string file = "", output = "", message = "";

				if (storeJson["storeContent"][selection][entry][i].contains("file") && storeJson["storeContent"][selection][entry][i]["file"].is_string()) {
					file = storeJson["storeContent"][selection][entry][i]["file"];
				}
				else missing = true;

				if (storeJson["storeContent"][selection][entry][i].contains("output") && storeJson["storeContent"][selection][entry][i]["output"].is_string()) {
					output = storeJson["storeContent"][selection][entry][i]["output"];
				}
				else missing = true;

				if (storeJson["storeContent"][selection][entry][i].contains("message") && storeJson["storeContent"][selection][entry][i]["message"].is_string()) {
					message = storeJson["storeContent"][selection][entry][i]["message"];
				}

				if (!missing) ret = ScriptUtils::downloadFile(file, output, message);
				else ret = SYNTAX_ERROR;

			} else if (type == "downloadRelease") {
				bool missing = false, includePrereleases = false;
				std::string repo = "", file = "", output = "", message = "";

				if (storeJson["storeContent"][selection][entry][i].contains("repo") && storeJson["storeContent"][selection][entry][i]["repo"].is_string()) {
					repo = storeJson["storeContent"][selection][entry][i]["repo"];
				}
				else missing = true;

				if (storeJson["storeContent"][selection][entry][i].contains("file") && storeJson["storeContent"][selection][entry][i]["file"].is_string()) {
					file = storeJson["storeContent"][selection][entry][i]["file"];
				}
				else  missing = true;

				if (storeJson["storeContent"][selection][entry][i].contains("output") && storeJson["storeContent"][selection][entry][i]["output"].is_string()) {
					output = storeJson["storeContent"][selection][entry][i]["output"];
				}
				else missing = true;

				if (storeJson["storeContent"][selection][entry][i].contains("includePrereleases") && storeJson["storeContent"][selection][entry][i]["includePrereleases"].is_boolean())
					includePrereleases = storeJson["storeContent"][selection][entry][i]["includePrereleases"];

				if (storeJson["storeContent"][selection][entry][i].contains("message") && storeJson["storeContent"][selection][entry][i]["message"].is_string()) {
					message = storeJson["storeContent"][selection][entry][i]["message"];
				}

				if (!missing) ret = ScriptUtils::downloadRelease(repo, file, output, includePrereleases, message);
				else ret = SYNTAX_ERROR;

			} else if (type == "extractFile") {
				bool missing = false;
				std::string file = "", input = "", output = "", message = "";

				if (storeJson["storeContent"][selection][entry][i].contains("file") && storeJson["storeContent"][selection][entry][i]["file"].is_string()) {
					file = storeJson["storeContent"][selection][entry][i]["file"];
				}
				else missing = true;

				if (storeJson["storeContent"][selection][entry][i].contains("input") && storeJson["storeContent"][selection][entry][i]["input"].is_string()) {
					input = storeJson["storeContent"][selection][entry][i]["input"];
				}
				else missing = true;

				if (storeJson["storeContent"][selection][entry][i].contains("output") && storeJson["storeContent"][selection][entry][i]["output"].is_string()) {
					output = storeJson["storeContent"][selection][entry][i]["output"];
				}
				else missing = true;

				if (storeJson["storeContent"][selection][entry][i].contains("message") && storeJson["storeContent"][selection][entry][i]["message"].is_string()) {
					message = storeJson["storeContent"][selection][entry][i]["message"];
				}

				if (!missing) ScriptUtils::extractFile(file, input, output, message);
				else ret = SYNTAX_ERROR;

			} else if (type == "installCia") {
				bool missing = false, updateSelf = false;
				std::string file = "", message = "";

				if (storeJson["storeContent"][selection][entry][i].contains("file") && storeJson["storeContent"][selection][entry][i]["file"].is_string()) {
					file = storeJson["storeContent"][selection][entry][i]["file"];
				}
				else missing = true;

				if (storeJson["storeContent"][selection][entry][i].contains("updateSelf") && storeJson["storeContent"][selection][entry][i]["updateSelf"].is_boolean()) {
					updateSelf = storeJson["storeContent"][selection][entry][i]["updateSelf"];
				}

				if (storeJson["storeContent"][selection][entry][i].contains("message") && storeJson["storeContent"][selection][entry][i]["message"].is_string()) {
					message = storeJson["storeContent"][selection][entry][i]["message"];
				}

				if (!missing) ScriptUtils::installFile(file, updateSelf, message);
				else ret = SYNTAX_ERROR;

			} else if (type == "mkdir") {
				bool missing = false;
				std::string directory = "", message = "";

				if (storeJson["storeContent"][selection][entry][i].contains("directory") && storeJson["storeContent"][selection][entry][i]["directory"].is_string()) {
					directory = storeJson["storeContent"][selection][entry][i]["directory"];
				}
				else missing = true;

				if (!missing) makeDirs(directory.c_str());
				else ret = SYNTAX_ERROR;

			} else if (type == "rmdir") {
				bool missing = false;
				std::string directory = "", message = "", promptmsg = "";

				if (storeJson["storeContent"][selection][entry][i].contains("directory") && storeJson["storeContent"][selection][entry][i]["directory"].is_string()) {
					directory = storeJson["storeContent"][selection][entry][i]["directory"];
				}
				else missing = true;

				promptmsg = Lang::get("DELETE_PROMPT") + "\n" + directory;
				if (!missing && directory != "") {
					if (access(directory.c_str(), F_OK) != 0) ret = DELETE_ERROR;
					else {
						if (Msg::promptMsg(promptmsg)) removeDirRecursive(directory.c_str());
					}
				}

				else ret = SYNTAX_ERROR;

			} else if (type == "promptMessage") {
				std::string Message = "";
				if (storeJson["storeContent"][selection][entry][i].contains("message") && storeJson["storeContent"][selection][entry][i]["message"].is_string()) {
					Message = storeJson["storeContent"][selection][entry][i]["message"];
				}

				ret = ScriptUtils::prompt(Message);

				if (ret == SCRIPT_CANCELED) {
					ret = NONE;
					i++; // Skip.
				}

			} else if (type == "copy") {
				std::string Message = "", source = "", destination = "";
				bool missing = false;

				if (storeJson["storeContent"][selection][entry][i].contains("source") && storeJson["storeContent"][selection][entry][i]["source"].is_string()) {
					source = storeJson["storeContent"][selection][entry][i]["source"];
				}
				else missing = true;

				if (storeJson["storeContent"][selection][entry][i].contains("destination") && storeJson["storeContent"][selection][entry][i]["destination"].is_string()) {
					destination = storeJson["storeContent"][selection][entry][i]["destination"];
				}
				else missing = true;

				if (storeJson["storeContent"][selection][entry][i].contains("message") && storeJson["storeContent"][selection][entry][i]["message"].is_string()) {
					Message = storeJson["storeContent"][selection][entry][i]["message"];
				}

				if (!missing) ret = ScriptUtils::copyFile(source, destination, Message);
				else ret = SYNTAX_ERROR;

			} else if (type == "move") {
				std::string Message = "", oldFile = "", newFile = "";
				bool missing = false;

				if (storeJson["storeContent"][selection][entry][i].contains("old") && storeJson["storeContent"][selection][entry][i]["old"].is_string()) {
					oldFile = storeJson["storeContent"][selection][entry][i]["old"];
				}
				else missing = true;

				if (storeJson["storeContent"][selection][entry][i].contains("new") && storeJson["storeContent"][selection][entry][i]["new"].is_string()) {
					newFile = storeJson["storeContent"][selection][entry][i]["new"];
				}
				else missing = true;

				if (storeJson["storeContent"][selection][entry][i].contains("message") && storeJson["storeContent"][selection][entry][i]["message"].is_string()) {
					Message = storeJson["storeContent"][selection][entry][i]["message"];
				}

				if (!missing) ret = ScriptUtils::renameFile(oldFile, newFile, Message);
				else ret = SYNTAX_ERROR;
			}
		}
	}

	if (ret == NONE) doneMsg();
	else if (ret == FAILED_DOWNLOAD) Msg::waitMsg(Lang::get("DOWNLOAD_ERROR"));
	else if (ret == SYNTAX_ERROR) Msg::waitMsg(Lang::get("SYNTAX_ERROR"));
	else if (ret == COPY_ERROR) Msg::waitMsg(Lang::get("COPY_ERROR"));
	else if (ret == MOVE_ERROR) Msg::waitMsg(Lang::get("MOVE_ERROR"));
	else if (ret == DELETE_ERROR) Msg::waitMsg(Lang::get("DELETE_ERROR"));
	return ret;
}