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

/* Boot a title. */
void ScriptUtils::bootTitle(const std::string &TitleID, bool isNAND, bool isARG) {
	std::string MSG = Lang::get("BOOT_TITLE") + "\n\n";
	if (isNAND)	MSG += Lang::get("MEDIATYPE_NAND") + "\n" + TitleID;
	else MSG += Lang::get("MEDIATYPE_SD") + "\n" + TitleID;

	const u64 ID = std::stoull(TitleID, 0, 16);
	if (isARG) {
		if (Msg::promptMsg(MSG)) {
			Title::Launch(ID, isNAND ? MEDIATYPE_NAND : MEDIATYPE_SD);
		}

	} else {
		Title::Launch(ID, isNAND ? MEDIATYPE_NAND : MEDIATYPE_SD);
	}
}

/* Prompt message. */
Result ScriptUtils::prompt(const std::string &message) {
	Result ret = NONE;
	if (!Msg::promptMsg(message)) ret = SCRIPT_CANCELED;

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
Result ScriptUtils::runFunctions(nlohmann::json storeJson, int selection, const std::string &entry) {
	Result ret = NONE; // No Error as of yet.

	if (!storeJson.contains("storeContent")) { Msg::waitMsg(Lang::get("SYNTAX_ERROR")); return SYNTAX_ERROR; };
	if ((int)storeJson["storeContent"].size() < selection) { Msg::waitMsg(Lang::get("SYNTAX_ERROR")); return SYNTAX_ERROR; };
	if (!storeJson["storeContent"][selection].contains(entry)) { Msg::waitMsg(Lang::get("SYNTAX_ERROR")); return SYNTAX_ERROR; };

	nlohmann::json Script = nullptr;

	/* Detect if array or new object thing. Else return Syntax error. :P */
	if (storeJson["storeContent"][selection][entry].type() == nlohmann::json::value_t::array) {
		Script = storeJson["storeContent"][selection][entry];

	} else if (storeJson["storeContent"][selection][entry].type() == nlohmann::json::value_t::object) {
		if (storeJson["storeContent"][selection][entry].contains("script") && storeJson["storeContent"][selection][entry]["script"].is_array()) {
			Script = storeJson["storeContent"][selection][entry]["script"];

		} else {
			Msg::waitMsg(Lang::get("SYNTAX_ERROR"));
			return SYNTAX_ERROR;
		}
	}


	for(int i = 0; i < (int)Script.size(); i++) {
		if (ret == NONE) {
			std::string type = "";

			if (Script[i].contains("type") && Script[i]["type"].is_string()) {
				type = Script[i]["type"];

			} else {
				ret = SYNTAX_ERROR;
			}

			if (type == "deleteFile") {
				bool missing = false;
				std::string file = "";


				if (Script[i].contains("file") && Script[i]["file"].is_string()) {
					file = Script[i]["file"];
				}
				else missing = true;

				if (!missing) ret = ScriptUtils::removeFile(file, true);
				else ret = SYNTAX_ERROR;

			} else if (type == "downloadFile") {
				bool missing = false;
				std::string file = "", output = "";

				if (Script[i].contains("file") && Script[i]["file"].is_string()) {
					file = Script[i]["file"];
				}
				else missing = true;

				if (Script[i].contains("output") && Script[i]["output"].is_string()) {
					output = Script[i]["output"];
				}
				else missing = true;

				char message[256];
				snprintf(message, sizeof(message), Lang::get("SHORTCUT_DOWNLOADING").c_str(), output.substr(output.find_first_of("/") + 1).c_str());

				if (!missing) ret = ScriptUtils::downloadFile(file, output, message, true);
				else ret = SYNTAX_ERROR;

			} else if (type == "downloadRelease") {
				bool missing = false, includePrereleases = false;
				std::string repo = "", file = "", output = "";

				if (Script[i].contains("repo") && Script[i]["repo"].is_string()) {
					repo = Script[i]["repo"];
				}
				else missing = true;

				if (Script[i].contains("file") && Script[i]["file"].is_string()) {
					file = Script[i]["file"];
				}
				else  missing = true;

				if (Script[i].contains("output") && Script[i]["output"].is_string()) {
					output = Script[i]["output"];
				}
				else missing = true;

				if (Script[i].contains("includePrereleases") && Script[i]["includePrereleases"].is_boolean())
					includePrereleases = Script[i]["includePrereleases"];

				char message[256];
				snprintf(message, sizeof(message), Lang::get("SHORTCUT_DOWNLOADING").c_str(), output.substr(output.find_first_of("/") + 1).c_str());

				if (!missing) ret = ScriptUtils::downloadRelease(repo, file, output, includePrereleases, message, true);
				else ret = SYNTAX_ERROR;

			} else if (type == "extractFile") {
				bool missing = false;
				std::string file = "", input = "", output = "";

				if (Script[i].contains("file") && Script[i]["file"].is_string()) {
					file = Script[i]["file"];
				}
				else missing = true;

				if (Script[i].contains("input") && Script[i]["input"].is_string()) {
					input = Script[i]["input"];
				}
				else missing = true;

				if (Script[i].contains("output") && Script[i]["output"].is_string()) {
					output = Script[i]["output"];
				}
				else missing = true;

				char message[256];
				snprintf(message, sizeof(message), Lang::get("SHORTCUT_EXTRACTING").c_str(), file.substr(file.find_first_of("/") + 1).c_str());

				if (!missing) ret = ScriptUtils::extractFile(file, input, output, message, true);
				else ret = SYNTAX_ERROR;

			} else if (type == "installCia") {
				bool missing = false, updateSelf = false;
				std::string file = "";

				if (Script[i].contains("file") && Script[i]["file"].is_string()) {
					file = Script[i]["file"];
				}
				else missing = true;

				if (Script[i].contains("updateSelf") && Script[i]["updateSelf"].is_boolean()) {
					updateSelf = Script[i]["updateSelf"];
				}

				char message[256];
				snprintf(message, sizeof(message), Lang::get("SHORTCUT_INSTALLING").c_str(), file.substr(file.find_first_of("/") + 1).c_str());

				if (!missing) ScriptUtils::installFile(file, updateSelf, message, true);
				else ret = SYNTAX_ERROR;

			} else if (type == "mkdir") {
				bool missing = false;
				std::string directory = "";

				if (Script[i].contains("directory") && Script[i]["directory"].is_string()) {
					directory = Script[i]["directory"];
				}
				else missing = true;

				if (!missing) makeDirs(directory.c_str());
				else ret = SYNTAX_ERROR;

			} else if (type == "rmdir") {
				bool missing = false;
				std::string directory = "", promptmsg = "";
				bool required = false;

				if (Script[i].contains("directory") && Script[i]["directory"].is_string()) {
					directory = Script[i]["directory"];
				}
				else missing = true;

				if (Script[i].contains("required") && Script[i]["required"].is_boolean()) {
					required = Script[i]["required"];
				}

				promptmsg = Lang::get("DELETE_PROMPT") + "\n" + directory;
				if (!missing && directory != "") {
					if (access(directory.c_str(), F_OK) != 0 && required) ret = DELETE_ERROR;
					else {
						if (Msg::promptMsg(promptmsg)) removeDirRecursive(directory.c_str());
					}
				}

				else ret = SYNTAX_ERROR;

			} else if (type == "promptMessage" || type == "promptMsg") {
				std::string Message = "";
				int skipCount = -1;

				if (Script[i].contains("message") && Script[i]["message"].is_string()) {
					Message = Script[i]["message"];
				}

				if (Script[i].contains("count") && Script[i]["count"].is_number()) {
					skipCount = Script[i]["count"];
				}

				ret = ScriptUtils::prompt(Message);

				if (skipCount > -1 && ret == SCRIPT_CANCELED) {
					ret = NONE;
					i += skipCount; // Skip.
				}

			} else if (type == "exit") {
				break;

			} else if (type == "copy") {
				std::string source = "", destination = "";
				bool missing = false;

				if (Script[i].contains("source") && Script[i]["source"].is_string()) {
					source = Script[i]["source"];
				}
				else missing = true;

				if (Script[i].contains("destination") && Script[i]["destination"].is_string()) {
					destination = Script[i]["destination"];
				}
				else missing = true;

				char message[256];
				snprintf(message, sizeof(message), Lang::get("SHORTCUT_COPYING").c_str(), source.substr(source.find_first_of("/") + 1).c_str());

				if (!missing) ret = ScriptUtils::copyFile(source, destination, message, true);
				else ret = SYNTAX_ERROR;

			} else if (type == "move") {
				std::string oldFile = "", newFile = "";
				bool missing = false;

				if (Script[i].contains("old") && Script[i]["old"].is_string()) {
					oldFile = Script[i]["old"];
				}
				else missing = true;

				if (Script[i].contains("new") && Script[i]["new"].is_string()) {
					newFile = Script[i]["new"];
				}
				else missing = true;

				if (!missing) ret = ScriptUtils::renameFile(oldFile, newFile, true);
				else ret = SYNTAX_ERROR;

			} else if (type == "skip") {
				int skipCount = -1;

				if (Script[i].contains("count") && Script[i]["count"].is_number()) {
					skipCount = Script[i]["count"];
				}

				if (skipCount > 0) {
					i += skipCount; // Skip.
				}
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