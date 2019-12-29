/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

#pragma once

#include <3ds.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "utils/files.h"

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef __cplusplus
}

#include "utils/json.hpp"
#include "utils/stringutils.hpp"

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <curl/curl.h>

using json = nlohmann::json;

#endif

extern char * arg0;

#define WORKING_DIR	"/"
#define SCRIPTS_PATH	"sdmc:/3ds/Universal-Updater/scripts/" // The Scripts will be here.
#define MUSIC_PATH	"sdmc:/3ds/Universal-Updater/Music.wav" // Default Music File / Path.
#define SCRIPT_VERSION	3
#define STORE_PATH "sdmc:/3ds/Universal-Updater/stores/" // Default Store path.
#define ENTRIES_PER_SCREEN 3
#define ENTRIES_PER_LIST 7