/*
 * Copyright (c) 2021 Max Thomas
 * This file is part of DSiWifi and is distributed under the MIT license.
 * See dsiwifi_license.txt for terms of use.
 */

#include "rpc.h"

#include <nds.h>
#include <nds/ndstypes.h>
#include <fat.h>

#include "lwip/tcp.h"
#include "wifiutils.h"

#define RPC_PORT (8336)

#define RPC_CMD_NOP              (0)
#define RPC_CMD_CREATEFILE       (1)
#define RPC_CMD_WRITEFILE        (2)
#define RPC_CMD_REBOOT           (15)
#define RPC_CMD_MAX              (16)

#define RPC_CIRCBUF_LEN (0x100000)


static struct tcp_pcb* rpc_pcb;
static u8 rpc_recv_buf[0x800];
static u8* rpc_read_buffer = NULL;
static u32 rpc_read_buffer_idx = 0;

static bool magic_read = false;
static bool size_read = false;
static u32 payload_read_left = 0;
static u32 payload_size = 0;
static u32 packet_recvd = 0;

static int rpc_timeout = 0;

void rpc_reset()
{
    payload_read_left = 0;
    payload_size = 0;
    rpc_read_buffer_idx = 0;
    packet_recvd = 0;
    magic_read = false;
    size_read = false;
}

void rpc_proc_buffer(struct tcp_pcb *tpcb)
{
    u8* payload_read = rpc_read_buffer + 8;
    
    rpc_reset();
    
    // Read and verify command
    u32 cmd = *(u32*)payload_read; payload_read += sizeof(u32);
    
    if (cmd >= RPC_CMD_MAX) {
        wifi_printlnf("rpc_recv bad cmd %02x", cmd);
        return;
    }
    
    if (cmd == RPC_CMD_CREATEFILE) {
        char* fpath = (char*)payload_read; payload_read += 0x40;
        u32 truncate = *(u32*)payload_read; payload_read += sizeof(u32);
        
        wifi_printlnf("RPC_CreateFile: `%s` %x", fpath, truncate);
        
        // CreateFile
        FILE* f = fopen(fpath,"w");
        if (f) {
            fclose(f);
        }
    }
    else if (cmd == RPC_CMD_WRITEFILE) {
        char* fpath = (char*)payload_read; payload_read += 0x40;
        u32 offs = *(u32*)payload_read; payload_read += sizeof(u32);
        u32 len = *(u32*)payload_read; payload_read += sizeof(u32);
        
        // WriteFile
        FILE* f = fopen(fpath,"r+");
        if (f) {
            fseek(f, offs, SEEK_SET);
            fwrite(payload_read, len, 1, f);
            fclose(f);
        }
        
        u8* payload_write = rpc_recv_buf;
        strcpy((char*)payload_write, "SLTR"); payload_write += 4;
        *payload_write = cmd; payload_write++;
        
        u32 payload_write_size = payload_write - rpc_recv_buf;
        tcp_write(tpcb, rpc_recv_buf, payload_write_size, 1);
    }
    else if (cmd == RPC_CMD_REBOOT) {
        while (1) {
	        fifoSendValue32(FIFO_USER_01, 1);
	    }
    }
    else {
        u8* payload_write = rpc_recv_buf;
        strcpy((char*)payload_write, "SLTR"); payload_write += 4;
        *payload_write = cmd; payload_write++;
        
        u32 payload_write_size = payload_write - rpc_recv_buf;
        tcp_write(tpcb, rpc_recv_buf, payload_write_size, 1);
    }
    
    //wifi_printlnf("rpc_recv cmd %02x", cmd);
    
    
    
    //
    
    tpcb->flags |= TF_ACK_NOW;
    //
    //tcp_output(tpcb);
}

err_t rpc_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (p == NULL && err == ERR_OK)
    {
        if (payload_read_left)
        {
            //rpc_proc_buffer(tpcb);
        }

        wifi_printlnf("rpc_recv close");
        goto no_ack_cleanup;
    }
    
    if (err != ERR_OK)
    {
        wifi_printlnf("rpc_recv err? %d", err);
        return ERR_OK;
    }
    
    // Make sure we have one buffer
    p = pbuf_coalesce(p, PBUF_RAW);
    if (p->next != NULL) {
        wifi_printlnf("rpc_recv pbuf_coalesce failed: %x", p->tot_len);
        goto no_ack_cleanup;
    }
    
    u8* payload_read = (u8*)p->payload;
    int packet_left = p->tot_len;
    
    rpc_timeout = 0;
    
    while (packet_left > 0)
    {
        if (rpc_read_buffer_idx < 8)
        {
            u32 to_copy = 8 - rpc_read_buffer_idx;
            if (to_copy > packet_left)
                to_copy = packet_left;

            memcpy(rpc_read_buffer + rpc_read_buffer_idx, payload_read, to_copy);
            rpc_read_buffer_idx += to_copy;
            payload_read += to_copy;
            packet_recvd += to_copy;
            packet_left -= to_copy;
        }
        
        if (!magic_read && packet_recvd >= 8)
        {
            if (memcmp(rpc_read_buffer, "SLTC", 4)) {
                wifi_printlnf("rpc_recv bad magic");
                hexdump(payload_read, 8);
                goto no_ack_cleanup;
            }
            magic_read = true;
            
            payload_read_left = *(u32*)(rpc_read_buffer+4);
            payload_size = payload_read_left;
            
            size_read = true;
        }
        
        if (packet_left <= 0) break;
        if (!magic_read || !size_read) break;

        u32 to_copy = packet_left;
        if (to_copy > payload_read_left)
            to_copy = payload_read_left;
        if (to_copy)
            memcpy(rpc_read_buffer + rpc_read_buffer_idx, payload_read, to_copy);
        rpc_read_buffer_idx += to_copy;
        payload_read += to_copy;
        packet_recvd += to_copy;
        payload_read_left -= to_copy;
        packet_left -= to_copy;
        
        if (!payload_read_left)
            rpc_proc_buffer(tpcb);
    }
    
    // Ack the packet
    tcp_recved(tpcb, p->tot_len);
    
    // Free received buffer
    pbuf_free(p);
    
    return ERR_OK;
    
no_ack_cleanup:
    if (p)
        pbuf_free(p);
    rpc_reset();
    tcp_close(tpcb);
    tcp_recv(tpcb, NULL);
    return ERR_OK;
}

void rpc_error(void *arg, err_t err)
{
    if (err == ERR_RST)
    {
        wifi_printlnf("rpc reset");
    }
    else
    {
        wifi_printlnf("rpc_error %d", err);
    }
    rpc_reset();
}

err_t rpc_poll(void *arg, struct tcp_pcb *tpcb)
{    
    rpc_timeout++;
    if (rpc_timeout > 15)
        tcp_abort(tpcb);
    
    return ERR_OK;
}

err_t rpc_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    err_t ret_err;
    
    wifi_printlnf("rpc_accept");

    tcp_setprio(newpcb, TCP_PRIO_MIN);
    //tcp_nagle_disable(newpcb);

    // Set up callbacks
    tcp_arg(newpcb, NULL);
    tcp_recv(newpcb, rpc_recv);
    tcp_err(newpcb, rpc_error);
    tcp_poll(newpcb, rpc_poll, 1);
    ret_err = ERR_OK;
    
    rpc_reset();
    
    return ret_err;  
}

void rpc_init(void)
{
    err_t err;
    
    rpc_read_buffer = malloc(RPC_CIRCBUF_LEN);
  
    rpc_pcb = tcp_new();
    if (!rpc_pcb) {
        wifi_printlnf("tcp_new failed...");
        return;
    }
  
    err = tcp_bind(rpc_pcb, IP_ADDR_ANY, RPC_PORT);
    if (err != ERR_OK) {
        wifi_printlnf("tcp_bind failed...");
        return;
    }
    
    rpc_pcb = tcp_listen(rpc_pcb);
    tcp_accept(rpc_pcb, rpc_accept); 
}

void rpc_deinit(void)
{
    tcp_close(rpc_pcb);
}

void rpc_tick(void)
{

}


