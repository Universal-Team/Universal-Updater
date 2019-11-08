#pragma once

#include <3ds.h>

/*! Loop status */
typedef enum
{
	LOOP_CONTINUE, /*!< Continue looping */
	LOOP_RESTART,  /*!< Reinitialize */
	LOOP_EXIT,     /*!< Terminate looping */
} loop_status_t;

bool isTransfering;
char ftp_accepted_connection[50], ftp_file_transfer[100];

int           ftp_init(void);
loop_status_t ftp_loop(void);
void          ftp_exit(void);
