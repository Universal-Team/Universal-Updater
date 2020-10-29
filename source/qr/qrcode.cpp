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

#include "qrcode.hpp"
#include <cstring>

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

		this->buffToImage(); // Fetch image.
		Gui::ScreenDraw(Top);
		C2D_DrawImageAt(this->image, 0, 0, 0.5, nullptr, 1.0f, 1.0f);
		GFX::DrawBottom();
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

	std::vector<u8> &out: The Reference, where to output the decoded result.
*/
void QRCode::handler(std::vector<u8> &out) {
	hidScanInput();

	if (hidKeysDown() & KEY_B) { // Cancel with B.
		this->cancel = true;
		this->finish();
		return;
	}

	if (!this->capturing) {
		/* create camera draw thread. */
		if (threadCreate((ThreadFunc)&captureHelper, this, 0x10000, 0x1A, 1, true) != NULL) capturing = true;
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
			out.resize(scan_data.payload_len);
			std::copy(scan_data.payload, scan_data.payload + scan_data.payload_len, out.begin());
		}
	}
}

/*
	Return a vector of u8's from the QR Code.
*/
std::vector<u8> QR_Scanner::scan() {
	std::vector<u8> out = { };
	std::unique_ptr<QRCode> qrData = std::make_unique<QRCode>();
	aptSetHomeAllowed(false); // Block the Home key.

	threadCreate((ThreadFunc)&drawHelper, qrData.get(), 0x10000, 0x1A, 1, true);
    while (!qrData->done()) qrData->handler(out); // Handle.

    aptSetHomeAllowed(true); // Re-Allow it.
    return out;
}

/*
	Return the URL from the QR Code.
*/
std::string QR_Scanner::GetQRURL() {
	std::vector<u8> qrData = QR_Scanner::scan();

	if (qrData.empty()) return ""; // Because it is empty, return "".

	if (qrData.back() == '\0') { // If Terminator, do -1.
		return std::string((char *)qrData.data(), qrData.size() - 1);

	} else {
		return std::string((char *)qrData.data(), qrData.size());
	}

	return "";
}