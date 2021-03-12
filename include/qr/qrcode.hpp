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

#ifndef _UNIVERSAL_UPDATER_QR_CODE_HPP
#define _UNIVERSAL_UPDATER_QR_CODE_HPP

#include "common.hpp"
#include "quirc.hpp"
#include <atomic>
#include <3ds.h>
#include <array>
#include <vector>

class QRCode {
public:
	QRCode();
	~QRCode();

	void drawThread();
	void captureThread();
	void handler(std::string &result);
	bool done() const { return this->finished; };
	bool cancelled() const { return this->cancel; };
	void List(bool v) { this->displayList = v; };
private:
	void buffToImage();
	void finish();
	std::array<u16, 400 * 240> cameraBuffer;
	LightLock bufferLock;
	C2D_Image image;
	LightLock imageLock;
	quirc *qrData;
	Handle exitEvent;
	static constexpr Tex3DS_SubTexture subtex = { 512, 256, 0.0f, 1.0f, 1.0f, 0.0f };
	std::atomic<bool> finished                = false;
	bool capturing                            = false;
	bool cancel                               = false;
	bool displayList						  = false;
	int selectedStore = 0, sPos = 0;
	std::vector<StoreList> stores = { };
	std::vector<u8> out;
};

/*
	This is, what should get called.
*/
namespace QR_Scanner {
	std::string StoreHandle();
};

#endif