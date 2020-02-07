/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
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

#include "formatting.hpp"

// adapted from GM9i's byte parsing.
std::string formatBytes(int bytes) {
    char out[32];
    if(bytes == 1)
        snprintf(out, sizeof(out), "%d Byte", bytes);
    else if(bytes < 1024)
        snprintf(out, sizeof(out), "%d Bytes", bytes);
    else if(bytes < 1024 * 1024)
        snprintf(out, sizeof(out), "%.1f KB", (float)bytes / 1024);
    else if (bytes < 1024 * 1024 * 1024)
        snprintf(out, sizeof(out), "%.1f MB", (float)bytes / 1024 / 1024);
    else
        snprintf(out, sizeof(out), "%.1f GB", (float)bytes / 1024 / 1024 / 1024);

    return out;
}