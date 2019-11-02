#include "gui.hpp"
#include "keyboard.hpp"

#include "utils/config.hpp"
#include "utils/structs.hpp"

#include <string>
#include <stdio.h>
#include <ctype.h>

extern C3D_RenderTarget* top;
extern C3D_RenderTarget* bottom;

bool caps = false, enter = false;
int shift = 0;


Structs::Key NumpadStruct[] = {
	{"1", 10, 30},
	{"2", 90, 30},
	{"3", 170, 30},

	{"4", 10, 100},
	{"5", 90, 100},
	{"6", 170, 100},

	{"7", 10, 170},
	{"8", 90, 170},
	{"9", 170, 170},

	{"0", 250, 100},

	{"Enter", 250, 170},

	{"Backspace", 250, 30},
};


Structs::ButtonPos Numbers [] = {
	{10, 30, 60, 50}, // 1
	{90, 30, 60, 50}, // 2
	{170, 30, 60, 50}, // 3

	{10, 100, 60, 50},
	{90, 100, 60, 50},
	{170, 100, 60, 50},

	{10, 170, 60, 50},
	{90, 170, 60, 50},
	{170, 170, 60, 50},

	{250, 100, 60, 50}, // 0.

	{250, 170, 60, 50}, // Enter.

	{250, 30, 60, 50}, // Backspace.
};

extern bool touching(touchPosition touch, Structs::ButtonPos button);


void Input::DrawNumpad()
{
	for(uint i=0;i<(sizeof(NumpadStruct)/sizeof(NumpadStruct[0]));i++) {
		Gui::Draw_Rect(NumpadStruct[i].x, NumpadStruct[i].y, 60, 50, Config::Color2);
		char c[2] = {NumpadStruct[i].character[0]};
		Gui::DrawString(NumpadStruct[i].x+25, NumpadStruct[i].y+15, 0.72f, BLACK, c, 50);
	}
}

std::string Input::Numpad(std::string Text) { return Input::Numpad(-1, Text); }

std::string Input::Numpad(uint maxLength, std::string Text)
{
	int hDown;
	touchPosition touch;
	std::string string;
	int keyDownDelay = 10, cursorBlink = 20;
	enter = false;
	while(1) {
		do {
			C3D_FrameEnd(0);
			Gui::clearTextBufs();
			C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C2D_TargetClear(top, BLACK);
			C2D_TargetClear(bottom, BLACK);
			Gui::DrawTop();
			Gui::DrawString((400-Gui::GetStringWidth(0.8f, Text))/2, 2, 0.8f, WHITE, Text, 400);
			Gui::DrawString(180, 212, 0.8, WHITE, (string+(cursorBlink-- > 0 ? "_" : "")).c_str(), 400);
			if(cursorBlink < -20)	cursorBlink = 20;
			Gui::ScreenDraw(bottom);
			Gui::Draw_Rect(0, 0, 320, 240, Config::Color3);
			DrawNumpad();
			scanKeys();
			hDown = keysDown();
			if(keyDownDelay > 0) {
				keyDownDelay--;
			} else if(keyDownDelay == 0) {
				keyDownDelay--;
			}
		} while(!hDown);
		if(keyDownDelay > 0) {
		}
		keyDownDelay = 10;

		if(hDown & KEY_TOUCH) {
			touchRead(&touch);
			if(string.length() < maxLength) {
				if (touching(touch, Numbers[0])) {
					string += "1";
				} else if (touching(touch, Numbers[1])) {
					string += "2";
				} else if (touching(touch, Numbers[2])) {
					string += "3";
				} else if (touching(touch, Numbers[3])) {
					string += "4";
				} else if (touching(touch, Numbers[4])) {
					string += "5";
				} else if (touching(touch, Numbers[5])) {
					string += "6";
				} else if (touching(touch, Numbers[6])) {
					string += "7";
				} else if (touching(touch, Numbers[7])) {
					string += "8";
				} else if (touching(touch, Numbers[8])) {
					string += "9";
				} else if (touching(touch, Numbers[9])) {
					string += "0";
				}
			}
		}

		if(hDown & KEY_B || touching(touch, Numbers[11])) {
			string = string.substr(0, string.length()-1);
		}

		if(hDown & KEY_START || touching(touch, Numbers[10]) || enter) {
			break;
		}
	}

	return string;
	enter = false;
}

int Input::getUint(int max, std::string Text) {
	std::string s = Input::Numpad(3, Text);
	if(s == "" || (atoi(s.c_str()) == 0 && s[0] != '0')) return -1;
	int i = atoi(s.c_str());
	if(i>max)	return 255;
	return i;
}