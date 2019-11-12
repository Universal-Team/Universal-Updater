/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019 VoltZ, Epicpkmn11, Flame, RocketRobz, TotallyNotGuy
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

#ifndef GUI_HPP
#define GUI_HPP

#include "colors.hpp"
#include "sprites.h"

#include "screens/screen.hpp"

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
#include <random>
#include <stack>
#include <string.h>
#include <unordered_map>
#include <wchar.h>

namespace Gui
{
	// Init and Exit of the GUI.
	Result init(void);
	void exit(void);

	// Screen and MainLoops.
	void mainLoop(u32 hDown, u32 hHeld, touchPosition touch);
	void setScreen(std::unique_ptr<Screen> screen);
	void screenBack(void);
	C3D_RenderTarget* target(gfxScreen_t t);
	void ScreenDraw(C3D_RenderTarget * screen);

	// Clear Text.
	void clearTextBufs(void);

	// Draw a Sprite from the sheet.
	void sprite(int key, int x, int y, float ScaleX = 1, float ScaleY = 1);
	
	// Misc.
	bool Draw_Rect(float x, float y, float w, float h, u32 color);

	// Text / String Functions.
	void DrawStringCentered(float x, float y, float size, u32 color, std::string Text, int maxWidth = 0);
	void DrawString(float x, float y, float size, u32 color, std::string Text, int maxWidth = 0);
	void GetStringSize(float size, float *width, float *height, std::string Text);
	float GetStringWidth(float size, std::string Text);
	float GetStringHeight(float size, std::string Text);

	// Basic GUI.
	void DrawTop(void);
	void DrawBottom(void);

	void DisplayWarnMsg(std::string Text);
	bool promptMsg(std::string promptMsg);
}

void DisplayMsg(std::string text);

#endif