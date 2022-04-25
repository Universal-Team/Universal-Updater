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

/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2020 Bernardo Giordano, Admiral Fish, piepie62
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

#include "common.hpp"
#include "download.hpp"
#include "keyboard.hpp"
#include "qrcode.hpp"
#include <cstring>

static const std::vector<Structs::ButtonPos> mainButtons = {
	{ 10, 34, 300, 22 },
	{ 10, 64, 300, 22 },
	{ 10, 94, 300, 22 },
	{ 10, 124, 300, 22 },
	{ 10, 154, 300, 22 },
	{ 10, 184, 300, 22 },

	{ 5, 215, 24, 24 }, // QR Code / List.
	{ 35, 215, 24, 24 }, // Keyboard.
	{ 4, 0, 24, 24 } // Back.
};
extern bool touching(touchPosition touch, Structs::ButtonPos button);

/*
	Initialize everything needed for the camera.
*/
QRCode::QRCode() {
	this->image.tex = new C3D_Tex;
	this->image.subtex = &this->subtex;
	this->qrData = quirc_new();
	std::fill(this->cameraBuffer.begin(), this->cameraBuffer.end(), 0);
	C3D_TexInit(this->image.tex, 512, 256, GPU_RGB565);
	C3D_TexSetFilter(this->image.tex, GPU_LINEAR, GPU_LINEAR);
	this->image.tex->border = 0xFFFFFFFF;
	C3D_TexSetWrap(this->image.tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
	LightLock_Init(&this->bufferLock);
	LightLock_Init(&this->imageLock);
	svcCreateEvent(&this->exitEvent, RESET_STICKY);
	quirc_resize(this->qrData, 400, 240);

	if (checkWifiStatus()) this->stores = FetchStores(); // Fetching Stores here.

	if (this->stores.size() > 0) this->displayList = true;
}

/*
	Destroy everything.
*/
QRCode::~QRCode() {
	C3D_TexDelete(this->image.tex);
	delete this->image.tex;
	quirc_destroy(this->qrData);
	svcCloseHandle(this->exitEvent);
}

/*
	mem copy the captured Image to the buffer, used for drawing.
*/
void QRCode::buffToImage() {
	LightLock_Lock(&this->bufferLock);

	for (u32 x = 0; x < 400; x++) {
		for (u32 y = 0; y < 240; y++) {
			const u32 dstPos = ((((y >> 3) * (512 >> 3) + (x >> 3)) << 6) +
								 ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) * 2;

			const u32 srcPos = (y * 400 + x) * 2;
			memcpy(((u8 *)this->image.tex->data) + dstPos, ((u8 *)this->cameraBuffer.data()) + srcPos, 2);
		}
	}

	LightLock_Unlock(&this->bufferLock);
}

/*
	Finish and unlock everything.
*/
void QRCode::finish() {
	svcSignalEvent(this->exitEvent);
	while (!this->done()) svcSleepThread(1000000);
	LightLock_Lock(&this->bufferLock);
	LightLock_Unlock(&this->bufferLock);
	LightLock_Lock(&this->imageLock);
	LightLock_Unlock(&this->imageLock);
}

/*
	The Draw Thread of the Camera.
*/
void QRCode::drawThread() {
	LightLock_Lock(&this->imageLock);

	/* If we aren't done.. do the draw to scan. */
	while (!this->done()) {
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(Top, TRANSPARENT);
		C2D_TargetClear(Bottom, TRANSPARENT);

		if (!this->displayList) {
			this->buffToImage(); // Fetch image.
			Gui::ScreenDraw(Top);
			C2D_DrawImageAt(this->image, 0, 0, 0.5, nullptr, 1.0f, 1.0f);

			GFX::DrawBottom();
			Gui::Draw_Rect(0, 0, 320, 25, UIThemes->EntryBar());
			Gui::Draw_Rect(0, 25, 320, 1, UIThemes->EntryOutline());

		} else {
			GFX::DrawTop();
			Gui::DrawStringCentered(0, 1, 0.7, UIThemes->TextColor(), Lang::get("STORE_INFO"), 390, 0, font);

			if (this->stores.size() > 0) {
				Gui::DrawStringCentered(0, 30, 0.7f, UIThemes->TextColor(), this->stores[this->selectedStore].Title, 390, 0, font);
				Gui::DrawStringCentered(0, 50, 0.6f, UIThemes->TextColor(), this->stores[this->selectedStore].Author, 380, 0, font);
				Gui::DrawStringCentered(0, 90, 0.5f, UIThemes->TextColor(), this->stores[this->selectedStore].Description, 380, 130, font, C2D_WordWrap);
			}

			GFX::DrawBottom();
			Gui::Draw_Rect(0, 0, 320, 25, UIThemes->EntryBar());
			Gui::Draw_Rect(0, 25, 320, 1, UIThemes->EntryOutline());
			Gui::DrawStringCentered(0, 2, 0.6, UIThemes->TextColor(), Lang::get("RECOMMENDED_UNISTORES"), 310, 0, font);

			for(int i = 0; i < 6 && i < (int)this->stores.size(); i++) {
				if (this->sPos + i == this->selectedStore) Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, UIThemes->MarkSelected());
				Gui::DrawStringCentered(10 - 160 + (300 / 2), mainButtons[i].y + 4, 0.45f, UIThemes->TextColor(), this->stores[this->sPos + i].Title, 295, 0, font);
			}
		}

		GFX::DrawIcon((this->displayList ? sprites_qr_code_idx : sprites_list_idx), mainButtons[6].x, mainButtons[6].y, UIThemes->TextColor());
		if (this->displayList) GFX::DrawIcon(sprites_keyboard_idx, mainButtons[7].x, mainButtons[7].y, UIThemes->TextColor());
		GFX::DrawIcon(sprites_arrow_idx, mainButtons[8].x, mainButtons[8].y, UIThemes->TextColor());
		C3D_FrameEnd(0);
	}

	LightLock_Unlock(&this->imageLock);
}

/*
	The Capture Thread of the camera.
*/
void QRCode::captureThread() {
	Handle events[3] = { 0 };
	events[0] = exitEvent;
	u32 transferUnit;

	u16 *buffer = new u16[400 * 240];
	camInit();
	CAMU_SetSize(SELECT_OUT1, SIZE_CTR_TOP_LCD, CONTEXT_A);
	CAMU_SetOutputFormat(SELECT_OUT1, OUTPUT_RGB_565, CONTEXT_A);
	CAMU_SetFrameRate(SELECT_OUT1, FRAME_RATE_30);

	CAMU_SetNoiseFilter(SELECT_OUT1, true);
	CAMU_SetAutoExposure(SELECT_OUT1, true);
	CAMU_SetAutoWhiteBalance(SELECT_OUT1, true);
	CAMU_SetPhotoMode(SELECT_OUT1, PHOTO_MODE_LETTER);

	/* No clue if this is actually effective or if it's just a placebo effect, but it seems to help? */
	CAMU_SetSharpness(SELECT_OUT1, 127);
	CAMU_Activate(SELECT_OUT1);
	CAMU_GetBufferErrorInterruptEvent(&events[2], PORT_CAM1);
	CAMU_SetTrimming(PORT_CAM1, false);
	CAMU_GetMaxBytes(&transferUnit, 400, 240);
	CAMU_SetTransferBytes(PORT_CAM1, transferUnit, 400, 240);
	CAMU_ClearBuffer(PORT_CAM1);
	CAMU_SetReceiving(&events[1], buffer, PORT_CAM1, 400 * 240 * sizeof(u16), (s16)transferUnit);
	CAMU_StartCapture(PORT_CAM1);

	bool cancel = false;
	while (!cancel) {
		s32 index = 0;
		svcWaitSynchronizationN(&index, events, 3, false, U64_MAX);

		switch (index) {
			case 0:
				cancel = true;
				break;

			case 1:
				svcCloseHandle(events[1]);
				events[1] = 0;
				LightLock_Lock(&this->bufferLock);
				memcpy(this->cameraBuffer.data(), buffer, 400 * 240 * sizeof(u16));
				GSPGPU_FlushDataCache(this->cameraBuffer.data(), 400 * 240 * sizeof(u16));
				LightLock_Unlock(&this->bufferLock);
				CAMU_SetReceiving(&events[1], buffer, PORT_CAM1, 400 * 240 * sizeof(u16), transferUnit);
				break;

			case 2:
				svcCloseHandle(events[1]);
				events[1] = 0;
				CAMU_ClearBuffer(PORT_CAM1);
				CAMU_SetReceiving(&events[1], buffer, PORT_CAM1, 400 * 240 * sizeof(u16), transferUnit);
				CAMU_StartCapture(PORT_CAM1);
				break;

			default:
				break;
		}
	}

	CAMU_StopCapture(PORT_CAM1);

	bool busy = false;
	while (R_SUCCEEDED(CAMU_IsBusy(&busy, PORT_CAM1)) && busy) svcSleepThread(1000000);

	CAMU_ClearBuffer(PORT_CAM1);
	CAMU_Activate(SELECT_NONE);
	camExit();
	delete[] buffer;

	for (int i = 1; i < 3; i++) {
		if (events[i] != 0) {
			svcCloseHandle(events[i]);
			events[i] = 0;
		}
	}

	this->finished = true;
}

/*
	These are just Helpers.. because Threads.
*/
void drawHelper(void *arg) {
	QRCode *qrData = (QRCode *)arg;
	qrData->drawThread();
}
void captureHelper(void *arg) {
	QRCode *qrData = (QRCode *)arg;
	qrData->captureThread();
}

/*
	Handle the capture.
*/
void QRCode::handler(std::string &result) {
	hidScanInput();
	touchPosition t;
	hidTouchRead(&t);
	u32 keyDown = hidKeysDown();
	u32 keyRepeat = hidKeysDownRepeat();

	if ((keyDown & KEY_B) || (keyDown & KEY_TOUCH && touching(t, mainButtons[8]))) {
		result = "";
		this->finished = true;
		this->finish();
		return;
	}

	/* Keyboard. */
	if (keyDown & KEY_TOUCH && touching(t, mainButtons[7])) {
		if (this->displayList) {
			const std::string temp = Input::setkbdString(150, Lang::get("ENTER_URL"), { });

			if (temp != "") {
				result = temp;
				this->finished = true;
				this->finish();
				return;
			}
		}
	}

	if (this->displayList) {
		gspWaitForVBlank();

		if ((keyDown & KEY_SELECT) || (keyDown & KEY_TOUCH && touching(t, mainButtons[6]))) {
			keyDown = 0;
			this->displayList = false;
		}

		if (this->stores.size() > 0) {
			if (keyRepeat & KEY_DOWN) {
				if (this->selectedStore < (int)this->stores.size() - 1) this->selectedStore++;
				else this->selectedStore = 0;
			}

			if (keyRepeat & KEY_UP) {
				if (this->selectedStore > 0) this->selectedStore--;
				else this->selectedStore = (int)this->stores.size() - 1;
			}

			if (keyDown & KEY_A) {
				result = this->stores[this->selectedStore].URL;
				this->finished = true;
				this->finish();
				return;
			}

			if (keyDown & KEY_TOUCH) {
				for (int i = 0; i < 6; i++) {
					if (touching(t, mainButtons[i])) {
						if (i + this->sPos < (int)this->stores.size()) {
							result = this->stores[i + this->sPos].URL;
							this->finished = true;
							this->finish();
							return;
						}
					}
				}
			}
		}

	} else {
		if (this->stores.size() > 0) {
			if ((keyDown & KEY_SELECT) || (keyDown & KEY_TOUCH && touching(t, mainButtons[6]))) {
				keyDown = 0;
				this->displayList = true;
			}
		}

		if (!this->capturing) {
			/* create camera capture thread. */
			if (threadCreate((ThreadFunc)&captureHelper, this, 0x10000, 0x1A, 1, true) != NULL) this->capturing = true;
			else {
				this->finish();
				return;
			}
		}

		if (this->done()) return;

		int w, h;
		u8 *image = (u8 *)quirc_begin(this->qrData, &w, &h);
		LightLock_Lock(&bufferLock);

		for (ssize_t x = 0; x < w; x++) {
			for (ssize_t y = 0; y < h; y++) {
				u16 px = this->cameraBuffer[y * 400 + x];
				image[y * w + x] = (u8)(((((px >> 11) & 0x1F) << 3) + (((px >> 5) & 0x3F) << 2) + ((px & 0x1F) << 3)) / 3);
			}
		}

		LightLock_Unlock(&this->bufferLock);
		quirc_end(this->qrData);

		if (quirc_count(this->qrData) > 0) {
			struct quirc_code code;
			struct quirc_data scan_data;
			quirc_extract(this->qrData, 0, &code);

			if (!quirc_decode(&code, &scan_data)) {
				this->finish();
				this->out.resize(scan_data.payload_len);
				std::copy(scan_data.payload, scan_data.payload + scan_data.payload_len, this->out.begin());

				/* From scanned stuff. */
				if (this->out.empty()) result = "";

				/* If Terminator, do -1. */
				if (this->out.back() == '\0') result = std::string((char *)this->out.data(), this->out.size() - 1);
				else result = std::string((char *)this->out.data(), this->out.size());
			}
		}
	}

	if (this->selectedStore < this->sPos) this->sPos = this->selectedStore;
	else if (this->selectedStore > this->sPos + 6 - 1) this->sPos = this->selectedStore - 6 + 1;
}

/*
	The store Add QR Code handle and such.
*/
std::string QR_Scanner::StoreHandle() {
	std::string result = "";

	std::unique_ptr<QRCode> qrData = std::make_unique<QRCode>();
	aptSetHomeAllowed(false); // Block the Home key.
	threadCreate((ThreadFunc)&drawHelper, qrData.get(), 0x10000, 0x1A, 1, true);
    while (!qrData->done()) qrData->handler(result); // Handle.
    aptSetHomeAllowed(true); // Re-Allow it.

	return result;
}