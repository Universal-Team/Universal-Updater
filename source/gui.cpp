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

#include "gui.hpp"

#include "screens/screenCommon.hpp"

#include "utils/config.hpp"

#include <assert.h>

C3D_RenderTarget* top;
C3D_RenderTarget* bottom;

C2D_TextBuf sizeBuf;
std::stack<std::unique_ptr<Screen>> screens;
C2D_SpriteSheet sprites;
bool currentScreen = false;
extern bool isScriptSelected;

extern u32 barColor;
extern u32 bgTopColor;
extern u32 bgBottomColor;
extern u32 TextColor;

// Clear Text.
void Gui::clearTextBufs(void)
{
	C2D_TextBufClear(sizeBuf);
}

// Initialize GUI.
Result Gui::init(void)
{
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	sprites = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	sizeBuf = C2D_TextBufNew(4096);
	return 0;
}

// Exit the whole GUI.
void Gui::exit(void)
{
	C2D_TextBufDelete(sizeBuf);
	C2D_SpriteSheetFree(sprites);
	C2D_Fini();
	C3D_Fini();
}

void DisplayMsg(std::string text) {
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(top, BLACK);
	C2D_TargetClear(bottom, BLACK);
	Gui::DrawTop();
	if (isScriptSelected == false) {
		Gui::DrawString(10, 40, 0.45f, Config::TxtColor, text, 380);
	} else if (isScriptSelected == true) {
		Gui::DrawString(10, 40, 0.45f, TextColor, text, 380);
	}
	Gui::DrawBottom();
	C3D_FrameEnd(0);
}

void Gui::sprite(int key, int x, int y, float ScaleX, float ScaleY)
{
	C2D_DrawImageAt(C2D_SpriteSheetGetImage(sprites, key), x, y, 0.5f, NULL, ScaleX, ScaleY);
}

void Gui::DisplayWarnMsg(std::string Text)
{
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(top, BLACK);
	C2D_TargetClear(bottom, BLACK);
	Gui::DrawTop();
	if (isScriptSelected == false) {
		Gui::DrawStringCentered(0, 1, 0.6f, Config::TxtColor, Text, 400);
	} else if (isScriptSelected == true) {
		Gui::DrawStringCentered(0, 1, 0.6f, TextColor, Text, 400);
	}
	Gui::DrawBottom();
	C3D_FrameEnd(0);
	for (int i = 0; i < 60*3; i++) {
		gspWaitForVBlank();
	}
}

void Gui::DrawStringCentered(float x, float y, float size, u32 color, std::string Text, int maxWidth) {
	Gui::DrawString((currentScreen ? 200 : 160)+x-(std::min(maxWidth, (int)Gui::GetStringWidth(size, Text))/2), y, size, color, Text, maxWidth);
}

// Draw String or Text.
void Gui::DrawString(float x, float y, float size, u32 color, std::string Text, int maxWidth) {
	C2D_Text c2d_text;
	C2D_TextParse(&c2d_text, sizeBuf, Text.c_str());
	C2D_TextOptimize(&c2d_text);
	if(maxWidth == 0) {
		C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, size, size, color);
	} else {
		C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, std::min(size, size*(maxWidth/Gui::GetStringWidth(size, Text))), size, color);
	}
}


// Get String or Text Width.
float Gui::GetStringWidth(float size, std::string Text) {
	float width = 0;
	GetStringSize(size, &width, NULL, Text);
	return width;
}

// Get String or Text Size.
void Gui::GetStringSize(float size, float *width, float *height, std::string Text) {
	C2D_Text c2d_text;
	C2D_TextParse(&c2d_text, sizeBuf, Text.c_str());
	C2D_TextGetDimensions(&c2d_text, size, size, width, height);
}


// Get String or Text Height.
float Gui::GetStringHeight(float size, std::string Text) {
	float height = 0;
	GetStringSize(size, NULL, &height, Text.c_str());
	return height;
}

// Draw a Rectangle.
bool Gui::Draw_Rect(float x, float y, float w, float h, u32 color) {
	return C2D_DrawRectSolid(x, y, 0.5f, w, h, color);
}

// Mainloop the GUI.
void Gui::mainLoop(u32 hDown, u32 hHeld, touchPosition touch) {
	screens.top()->Draw();
	screens.top()->Logic(hDown, hHeld, touch);
}

// Set the current Screen.
void Gui::setScreen(std::unique_ptr<Screen> screen)
{
	screens.push(std::move(screen));
}

// Go a Screen back.
void Gui::screenBack()
{
	screens.pop();
}

// Select, on which Screen should be drawn.
void Gui::ScreenDraw(C3D_RenderTarget * screen)
{
	C2D_SceneBegin(screen);
	currentScreen = screen == top ? 1 : 0;
}

void Gui::DrawTop(void) {
	Gui::ScreenDraw(top);
	if (isScriptSelected == false) {
		Gui::Draw_Rect(0, 0, 400, 25, Config::Color1);
		Gui::Draw_Rect(0, 25, 400, 190, Config::Color2);
		Gui::Draw_Rect(0, 215, 400, 25, Config::Color1);
		Gui::sprite(sprites_top_screen_top_idx, 0, 0);
		Gui::sprite(sprites_top_screen_bot_idx, 0, 215);
	} else if (isScriptSelected == true) {
		Gui::Draw_Rect(0, 0, 400, 30, barColor);
		Gui::Draw_Rect(0, 25, 400, 190, bgBottomColor);
		Gui::Draw_Rect(0, 215, 400, 25, barColor);
		Gui::sprite(sprites_top_screen_top_idx, 0, 0);
		Gui::sprite(sprites_top_screen_bot_idx, 0, 215);
	}
}

void Gui::DrawBottom(void) {
	Gui::ScreenDraw(bottom);
	if (isScriptSelected == false) {
		Gui::Draw_Rect(0, 0, 320, 25, Config::Color1);
		Gui::Draw_Rect(0, 25, 320, 190, Config::Color3);
		Gui::Draw_Rect(0, 215, 320, 25, Config::Color1);
		Gui::sprite(sprites_bottom_screen_top_idx, 0, 0);
		Gui::sprite(sprites_bottom_screen_bot_idx, 0, 215);
	} else if (isScriptSelected == true) {
		Gui::Draw_Rect(0, 0, 320, 30, barColor);
		Gui::Draw_Rect(0, 25, 320, 190, bgBottomColor);
		Gui::Draw_Rect(0, 215, 320, 25, barColor);
		Gui::sprite(sprites_bottom_screen_top_idx, 0, 0);
		Gui::sprite(sprites_bottom_screen_bot_idx, 0, 215);
	}
}

// Display a Message, which needs to be confirmed with A/B.
bool Gui::promptMsg(std::string promptMsg)
{
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(top, BLACK);
	C2D_TargetClear(bottom, BLACK);
	Gui::DrawTop();
	if (isScriptSelected == false) {
		Gui::DrawString((400-Gui::GetStringWidth(0.6f, promptMsg.c_str()))/2, 100, 0.6f, Config::TxtColor, promptMsg.c_str(), 400);
		Gui::DrawString((400-Gui::GetStringWidth(0.72f, Lang::get("CONFIRM_OR_CANCEL")))/2, 217, 0.72f, Config::TxtColor, Lang::get("CONFIRM_OR_CANCEL"), 400);
	} else if (isScriptSelected == true) {
		Gui::DrawString((400-Gui::GetStringWidth(0.6f, promptMsg.c_str()))/2, 100, 0.6f, TextColor, promptMsg.c_str(), 400);
		Gui::DrawString((400-Gui::GetStringWidth(0.72f, Lang::get("CONFIRM_OR_CANCEL")))/2, 217, 0.72f, TextColor, Lang::get("CONFIRM_OR_CANCEL"), 400);
	}
	Gui::DrawBottom();
	C3D_FrameEnd(0);
	while(1)
	{
		gspWaitForVBlank();
		hidScanInput();
		if(hidKeysDown() & KEY_A) {
			return true;
		} else if(hidKeysDown() & KEY_B) {
			return false;
		}
	}
}