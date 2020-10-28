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
#include "common.hpp"

/*
	Draw the progressbar.

	const u64 &currentProgress: Const Reference to the current progress.
	const u64 &totalProgress: Const Reference to the total progress.
*/
void Animation::DrawProgressBar(const u64 &currentProgress, const u64 &totalProgress) {
	Gui::Draw_Rect(30, 120, 340, 30, PROGRESSBAR_OUT_COLOR);
	Gui::Draw_Rect(31, 121, (int)(((float)currentProgress / (float)totalProgress) * 338.0f), 28, PROGRESSBAR_IN_COLOR);
}