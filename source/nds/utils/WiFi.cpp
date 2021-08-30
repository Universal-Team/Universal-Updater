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

#include "WiFi.hpp"

#include "rpc.h"

#include <dsiwifi9.h>
#include <lwip/sockets.h>

void WiFi::Init(void) {
	DSiWifi_SetLogHandler(Log);
	DSiWifi_SetConnectHandler(Connect);
	DSiWifi_SetReconnectHandler(Reconnect);
	DSiWifi_InitDefault(true);
}

void WiFi::Connect(void) {
	rpc_init();
}
void WiFi::Disconnect(void) {
	rpc_deinit();
}

void WiFi::Reconnect(void) {
	Disconnect();
	Connect();
}

void WiFi::Log(const char* s) {
	iprintf("%s", s);
}

uint32_t WiFi::IpAddress(void) {
	return DSiWifi_GetIP();
}

bool WiFi::Connected(void) {
	// TODO: Probably a better check
	uint32_t Ip = IpAddress();
	return (Ip >> 0x18) != 0 && (Ip >> 0x18) != 255;
}
