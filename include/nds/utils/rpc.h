/*
 * Copyright (c) 2021 Max Thomas
 * This file is part of DSiWifi and is distributed under the MIT license.
 * See dsiwifi_license.txt for terms of use.
 */

#ifndef _RPC_H
#define _RPC_H

#ifdef __cplusplus
extern "C" {
#endif

void rpc_tick(void);
void rpc_init(void);
void rpc_deinit(void);

#ifdef __cplusplus
}
#endif

#endif // _RPC_H

