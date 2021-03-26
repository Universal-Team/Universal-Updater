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

#ifndef _UNIVERSAL_UPDATER_QUEUE_SYSTEM_HPP
#define _UNIVERSAL_UPDATER_QUEUE_SYSTEM_HPP

#include "json.hpp"
#include <citro2d.h>
#include <deque>
#include <memory>

/* Extend this, if more statuses are neccessary. */
enum class QueueStatus {
	None,
	Copying,
	Deleting,
	Downloading,
	Extracting,
	Installing,
	Moving,
	Request, // For User needed Requests.
	Failed,
	Done
};

enum RequestType {
	PROMPT_RET = -3,
	NO_REQUEST = -1,
	RMDIR_REQUEST = 1, // remove dir prompt request.
	PROMPT_REQUEST = 2, // skip prompt request.
	PROMPT_ERROR = 3 // Error message prompt. Unused right now.
};

/* Of course also a namespace to that part, so we can do that in a Thread. */
namespace QueueSystem {
	extern int RequestNeeded, RequestAnswer;
	extern std::string RequestMsg, EndMsg;
	extern int LastElement;
	extern bool Wait, Popup, CancelCallback;

	void QueueHandle(); // Handles the Queue.
	void AddToQueue(nlohmann::json obj, const C2D_Image &icn, const std::string &name, const std::string &uName, const std::string &eName, const std::string &lUpdated); // Adds to Queue.
	void ClearQueue(); // Clears the Queue.
	void Resume();
};

class Queue {
public:
	Queue(nlohmann::json object, const C2D_Image &img, const std::string &name, const std::string &uName, const std::string &eName, const std::string &lUpdated) :
		obj(object), icn(img), total(object.size()), current(QueueSystem::LastElement), name(name), unistoreName(uName), entryName(eName), lastUpdated(lUpdated) { };

	QueueStatus status = QueueStatus::None;
	nlohmann::json obj;
	C2D_Image icn;
	int total, current;
	std::string name = "", unistoreName = "", entryName = "", lastUpdated = "";
};

#endif