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

#include "lodepng.h"
#include "msg.hpp"
#include "screenshot.hpp"

static constexpr Tex3DS_SubTexture subtex = { 512, 256, 0.0f, 1.0f, 1.0f, 0.0f };

C2D_Image Screenshot::Convert(const std::string &filename) {
	std::vector<u8> ImageBuffer;
	unsigned width, height;
	C2D_Image img;
	lodepng::decode(ImageBuffer, width, height, filename.c_str());

	img.tex = new C3D_Tex;
	img.subtex = &subtex;

	C3D_TexInit(img.tex, 512, 256, GPU_RGBA8);
	C3D_TexSetFilter(img.tex, GPU_LINEAR, GPU_LINEAR);
	img.tex->border = 0xFFFFFFFF;
	C3D_TexSetWrap(img.tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);



	Msg::waitMsg("Width: " + std::to_string(width) + "\nheight: " + std::to_string(height));
	for (u32 x = 0; x < width; x++) {
		for (u32 y = 0; y < height; y++) {
			const u32 dstPos = ((((y >> 3) * (512 >> 3) + (x >> 3)) << 6) +
								((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) |
								((x & 4) << 2) | ((y & 4) << 3)));

			const u32 srcPos = (y * width + x);
			*((volatile uint8_t *) (((uint8_t*)img.tex->data) + dstPos)) = ImageBuffer.data()[srcPos];
		}
	}

	Msg::waitMsg("Success!");
	return img;
}