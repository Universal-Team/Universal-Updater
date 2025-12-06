/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2021 Universal-Team
*   Copyright (C) 2025 Alvin Wong
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
#include "overlay.hpp"
#include "qrcodegen.h"

extern bool touching(touchPosition touch, Structs::ButtonPos button);
extern bool QueueRuns;
extern bool is3DSX;


static const Structs::ButtonPos backButton{ 4, 0, 24, 24 };
static const Structs::ButtonPos browserButton = { 4, 212, 312, 22 };

static std::vector<std::string> wrapUrlLines(std::string_view url, uint32_t maxLines = std::numeric_limits<uint32_t>::max()) {
	// Crudely line wrap the URL:
	std::vector<std::string> wrappedUrlLines;
	std::string line;
	size_t lastWrap = 0;
	for (size_t i = 0; i < url.size(); i++) {
		char c = url[i];
		line.push_back(c);

		if (i - lastWrap < 30) continue;

		bool wrap = false;
		switch (c) {
		case '-':
		case '/':
		case '_':
		case '?':
		case '=':
		case '&':
		case ' ':
			wrap = true;
			break;
		default:
			if (i - lastWrap >= 40) wrap = true;
			break;
		}

		if (wrap) {
			if (wrappedUrlLines.size() + 1 >= maxLines) {
				line.append("...");
				break;
			}
			if (line[line.size() - 1] == ' ') {
				// add keyboard return symbol to visualize whitespace
				line.append("\uE056");
			}
			wrappedUrlLines.emplace_back(std::move(line));
			lastWrap = i;
		}
	}
	if (!line.empty()) wrappedUrlLines.emplace_back(std::move(line));
	return wrappedUrlLines;
}

/* Show a QR Code URL. */
void Overlays::ShowQrCodeUrl(const std::string &title, const std::string &url) {

	std::unique_ptr<uint8_t[]> qrcode = std::make_unique<uint8_t[]>(qrcodegen_BUFFER_LEN_MAX);
	std::unique_ptr<uint8_t[]> tempBuffer = std::make_unique<uint8_t[]>(qrcodegen_BUFFER_LEN_MAX);
	bool ok = qrcodegen_encodeText(url.c_str(), tempBuffer.get(), qrcode.get(), qrcodegen_Ecc_LOW, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);
	if (!ok) {
		Msg::DisplayMsg(Lang::get("QR_CODE_GEN_FAILED"));
		return;
	}

	int size = qrcodegen_getSize(qrcode.get());
	if (size <= 0 || size > 256) {
		Msg::DisplayMsg(Lang::get("QR_CODE_GEN_FAILED"));
		return;
	}

	C3D_Tex tex;

	C3D_TexInit(&tex, 256, 256, GPU_TEXCOLOR::GPU_A8);
	C3D_TexSetFilter(&tex, GPU_NEAREST, GPU_NEAREST);
	C3D_TexSetWrap(&tex, GPU_CLAMP_TO_EDGE, GPU_CLAMP_TO_EDGE);

	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			const u32 dstPos = ((((y >> 3) * (256 >> 3) + (x >> 3)) << 6) +
								((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) |
								((x & 4) << 2) | ((y & 4) << 3)));

			((uint8_t *)tex.data)[dstPos] = qrcodegen_getModule(qrcode.get(), x, y) ? 255 : 0;
		}
	}

	constexpr int border = 4;
	int scale = 240 / (size + border * 2);
	int drawSize = size * scale;
	int drawX = (400 - drawSize) / 2;
	int drawY = (240 - drawSize) / 2;
	int drawBorder = border * scale;

	C2D_Image qrImage;
	Tex3DS_SubTexture subtex{(u16)drawSize, (u16)drawSize, 0.0f, 1.0f, size / 256.f, 1.0f - (size / 256.f)};
	qrImage.tex = &tex;
	qrImage.subtex = &subtex;
	C2D_Tint tint{
		.color = BLACK,
		.blend = 1.f,
	};
	C2D_ImageTint imageTint{
		.corners = {tint, tint, tint, tint},
	};

	std::vector<std::string> wrappedUrlLines = wrapUrlLines(url, 8);

	bool doOut = false;

	while(!doOut && aptMainLoop()) {
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(Top, TRANSPARENT);
		C2D_TargetClear(Bottom, TRANSPARENT);

		// Not using GFX::DrawTop because the QR doesn't fit as
		// nicely with the border.
		Gui::ScreenDraw(Top);
		Gui::Draw_Rect(0, 0, 400, 240, UIThemes->BGColor());

		C2D_DrawRectSolid(drawX - drawBorder, drawY - drawBorder, 0.75f, drawSize + drawBorder * 2, drawSize + drawBorder * 2, WHITE);
		C2D_DrawImageAt(qrImage, drawX, drawY, 1.f, &imageTint);

		Animation::QueueEntryDone();
		GFX::DrawBottom();
		Gui::Draw_Rect(0, 0, 320, 25, UIThemes->BarColor());
		Gui::Draw_Rect(0, 25, 320, 1, UIThemes->BarOutline());
		GFX::DrawIcon(sprites_arrow_idx, backButton.x, backButton.y, UIThemes->TextColor());
		Gui::DrawStringCentered(0, 2, 0.6, UIThemes->TextColor(), title, 310, 0, font);

		int y = 34;
		for (const std::string &urlLine :  wrappedUrlLines) {
			Gui::DrawStringCentered(0, y, 0.6f, UIThemes->TextColor(), urlLine, 312, 0, font);
			y += 20;
			if (y >= browserButton.y) break;
		}

		u8 consoleModel;
		CFGU_GetSystemModel(&consoleModel);
		
		bool blockOld3DS = !is3DSX && (consoleModel == CFG_MODEL_3DS || consoleModel == CFG_MODEL_3DSXL || consoleModel == CFG_MODEL_2DS);

		const bool browserAllowed = !QueueRuns && aptIsHomeAllowed() && !blockOld3DS;
		if(!blockOld3DS) {
			Gui::Draw_Rect(browserButton.x, browserButton.y, browserButton.w, browserButton.h, UIThemes->MarkSelected());
			Gui::DrawStringCentered(0, browserButton.y + 4, 0.45f, UIThemes->TextColor(), Lang::get("OPEN_URL_WEB_BROWSER"), 255, 0, font);
		}

		C3D_FrameEnd(0);

		hidScanInput();
		touchPosition touch;
		hidTouchRead(&touch);
		Animation::HandleQueueEntryDone();

		if ((hidKeysDown() & KEY_START) || (hidKeysDown() & KEY_B) || (hidKeysDown() & KEY_A) || (hidKeysDown() & KEY_TOUCH && touching(touch, backButton))) doOut = true;

		if ((hidKeysDown() & KEY_SELECT) || (hidKeysDown() & KEY_TOUCH && touching(touch, browserButton))) {
			if (browserAllowed) {
				char *buf = new char[0x1000]; // Needs to be this size size it gets memcpy'd to
				int length = url.size();
				memcpy(buf, url.c_str(), length);
				buf[length] = 0;
				aptLaunchSystemApplet(APPID_WEB, buf, length + 1, 0);

				delete[] buf;

				doOut = true;
			} else if(!blockOld3DS) {
				Msg::waitMsg(Lang::get("OPEN_URL_WEB_BROWSER_DISABLED"));
			}
		}
	}

	C3D_TexDelete(&tex);
}