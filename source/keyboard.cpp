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

Structs::Key keysQWERTY[] = {
	{"1", 0, 0}, {"2", 25, 0}, {"3", 50, 0}, {"4", 75, 0}, {"5", 100, 0}, {"6", 125, 0}, {"7", 150, 0}, {"8", 175, 0}, {"9", 200, 0}, {"0", 225, 0}, {"-", 250, 0}, {"=", 275, 0},
	{"q", 12, 22}, {"w", 37, 22}, {"e", 62, 22}, {"r", 87, 22}, {"t", 112, 22}, {"y", 137, 22}, {"u", 162, 22}, {"i", 187, 22}, {"o", 212, 22}, {"p", 237, 22}, {"[", 262, 22}, {"]", 287, 22},
	{"a", 25, 45}, {"s", 50, 45}, {"d", 75, 45}, {"f", 100, 45}, {"g", 125, 45}, {"h", 150, 45}, {"j", 175, 45}, {"k", 200, 45}, {"l", 225, 45}, {";", 250, 45}, {"'", 275, 45},
	{"z", 35, 67}, {"x", 60, 67}, {"c", 85, 67}, {"v", 110, 67}, {"b", 135, 67}, {"n", 160, 67}, {"m", 185, 67}, {",", 210, 67}, {".", 235, 67}, {"/", 260, 67}, {"\\", 210, 90},
};
Structs::Key keysQWERTYShift[] = {
	{"!", 0, 0}, {"@", 25, 0}, {"#", 50, 0}, {"$", 75, 0}, {"%", 100, 0}, {"^", 125, 0}, {"&", 150, 0}, {"*", 175, 0}, {"(", 200, 0}, {")", 225, 0}, {"_", 250, 0}, {"+", 275, 0},
	{"Q", 12, 22}, {"W", 37, 22}, {"E", 62, 22}, {"R", 87, 22}, {"T", 112, 22}, {"Y", 137, 22}, {"U", 162, 22}, {"I", 187, 22}, {"O", 212, 22}, {"P", 237, 22}, {"{", 262, 22}, {"}", 287, 22},
	{"A", 25, 45}, {"S", 50, 45}, {"D", 75, 45}, {"F", 100, 45}, {"G", 125, 45}, {"H", 150, 45}, {"J", 175, 45}, {"K", 200, 45}, {"L", 225, 45}, {":", 250, 45}, {"\"", 275, 45},
	{"Z", 35, 67}, {"X", 60, 67}, {"C", 85, 67}, {"V", 110, 67}, {"B", 135, 67}, {"N", 160, 67}, {"M", 185, 67}, {"<,", 210, 67}, {">", 235, 67}, {"?", 260, 67}, {"\\", 210, 90},
};
Structs::Key modifierKeys[] = {
	{"bksp", 300, 0, 20},	// Backspace
	{"caps",   0, 45, 20},	// Caps Lock
	{"entr", 300, 45, 20},	// Enter
	{"lsft",   0, 67, 30},	// Left Shift
	{"rsft", 285, 67, 35},	// Right Shift
	{"	",    60, 90, 20},	// Tab
	{" ",     85, 90, 120},	// Space
};

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

void Input::drawKeyboard() {
	for(uint i=0;i<(sizeof(keysQWERTY)/sizeof(keysQWERTY[0]));i++) {
		C2D_DrawRectSolid(keysQWERTY[i].x, keysQWERTY[i].y+103, 0.5f, 20, 20, Config::ColorKeys & C2D_Color32(255, 255, 255, 200));
		if(shift) {
			char c[2] = {caps ? (char)toupper(keysQWERTYShift[i].character[0]) : keysQWERTYShift[i].character[0]};
			Gui::DrawString(keysQWERTYShift[i].x+(10-(Gui::GetStringWidth(0.50, c)/2)), keysQWERTYShift[i].y+103+(10-(Gui::GetStringHeight(0.50, c)/2)), 0.50, Config::TxtColor, c);
		} else {
			char c[2] = {caps ? (char)toupper(keysQWERTY[i].character[0]) : keysQWERTY[i].character[0]};
			Gui::DrawString(keysQWERTY[i].x+(10-(Gui::GetStringWidth(0.50, c)/2)), keysQWERTY[i].y+103+(10-(Gui::GetStringHeight(0.50, c)/2)), 0.50, Config::TxtColor, c);
		}
	}
	for(uint i=0;i<(sizeof(modifierKeys)/sizeof(modifierKeys[0]));i++) {
		C2D_DrawRectSolid(modifierKeys[i].x, modifierKeys[i].y+103, 0.5f, modifierKeys[i].w, 20, Config::ColorKeys & C2D_Color32(255, 255, 255, 200));
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
			Gui::DrawString((400-Gui::GetStringWidth(0.55f, Text))/2, 2, 0.55f, WHITE, Text, 400);
			Gui::DrawString(180, 212, 0.8, WHITE, (string+(cursorBlink-- > 0 ? "_" : "")).c_str(), 400);
			if(cursorBlink < -20)	cursorBlink = 20;
			Gui::setDraw(bottom);
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

std::string Input::getString(std::string Text) { return Input::getString(-1, Text); }

std::string Input::getString(uint maxLength, std::string Text) {
	int hDown;
	touchPosition touch;
	std::string string;
	int keyDownDelay = 10, cursorBlink = 20;
	caps = false, shift = 0, enter = false;
	while(1) {
		do {
			C3D_FrameEnd(0);
			Gui::clearTextBufs();
			C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			Gui::DrawTop();
			Gui::DrawString((400-Gui::GetStringWidth(0.55f, Text))/2, 2, 0.55f, WHITE, Text, 400);
			Gui::DrawBottom();
			drawKeyboard();
			C2D_DrawRectSolid(0, 81, 0.5f, 320, 20, Config::ColorKeys & C2D_Color32(200, 200, 200, 200));
			Gui::DrawString(5, 82, 0.6, Config::TxtColor, (string+(cursorBlink-- > 0 ? "_" : "")).c_str());
			if(cursorBlink < -20)	cursorBlink = 20;
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
				// Check if a regular key was pressed
				for(uint i=0;i<(sizeof(keysQWERTY)/sizeof(keysQWERTY[0]));i++) {
					if((touch.px > keysQWERTY[i].x-2 && touch.px < keysQWERTY[i].x+18) && (touch.py > keysQWERTY[i].y+(103)-2 && touch.py < keysQWERTY[i].y+18+(103))) {
						char c = (shift ? keysQWERTYShift[i] : keysQWERTY[i]).character[0];
						string += (shift || caps ? toupper(c) : c);
						shift = 0;
						break;
					}
				}
			}
			// Check if a modifier key was pressed
			for(uint i=0;i<(sizeof(modifierKeys)/sizeof(modifierKeys[0]));i++) {
				if((touch.px > modifierKeys[i].x-2 && touch.px < modifierKeys[i].x+modifierKeys[i].w+2) && (touch.py > modifierKeys[i].y+(103)-2 && touch.py < modifierKeys[i].y+18+(103))) {
					if(modifierKeys[i].character == "bksp") {
						string = string.substr(0, string.length()-1);
					} else if(modifierKeys[i].character == "caps") {
						caps = !caps;
					} else if(modifierKeys[i].character == "entr") {
						enter = true;
					} else if(modifierKeys[i].character == "lsft") {
						if(shift)	shift = 0;
						else		shift = 1;
						if(shift) {
							keyDownDelay = -1;
						} else {
							keyDownDelay = 0;
						}
					} else if(modifierKeys[i].character == "rsft") {
						if(shift)	shift = 0;
						else		shift = 2;
						if(shift) {
							keyDownDelay = -1;
						} else {
							keyDownDelay = 0;
						}
					} else if(modifierKeys[i].character == " " || modifierKeys[i].character == "	") {
						if(string.length() < maxLength) {
							shift = 0;
							string += modifierKeys[i].character[0];
						}
					}
					break;
				}
			}
		} else if(hDown & KEY_B) {
			string = string.substr(0, string.length()-1);
			Gui::DrawString(0, 103, 0.5, BLACK, string.c_str(), 320);
		}

		if(hDown & KEY_START || enter) {
			break;
		}
	}
	return string;
}