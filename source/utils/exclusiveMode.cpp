/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2026 Universal-Team
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
#include <3ds.h>

static bool Active = false;

void ExclusiveMode::Enter() {
	if (Active) return;

	aptSetHomeAllowed(false);
	aptSetSleepAllowed(false);

	// Prevent StreetPass on lid close
	ndmuInit();
	Result res = NDMU_EnterExclusiveState(NDM_EXCLUSIVE_STATE_INFRASTRUCTURE);
	if (R_SUCCEEDED(res)) res = NDMU_LockState();
	Active = R_SUCCEEDED(res);
}

void ExclusiveMode::Exit() {
	if (!Active) return;

	aptSetHomeAllowed(true);
	aptSetSleepAllowed(true);

	Result res = NDMU_LeaveExclusiveState();
	if (R_SUCCEEDED(res)) res = NDMU_LockState();
	Active = R_FAILED(res);
	ndmuExit();
}