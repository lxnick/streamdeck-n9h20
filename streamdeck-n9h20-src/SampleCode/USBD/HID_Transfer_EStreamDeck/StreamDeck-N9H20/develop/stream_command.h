/****************************************************************
 *                                                              *
 * Copyright (c) Chicony Electronics Co., Ltd                   *
 *                                                              *
 ****************************************************************/
#ifndef STREAM_COMMAND_HANDLER
#define STREAM_COMMAND_HANDLER

#include "stdint.h"
#include "stdbool.h"
#include <string.h>

#include "stream_host.h"

typedef void (*ON_COMPLETE)();


struct download_data
{
	int data_collected;
	uint8_t* buffer;	
	ON_COMPLETE	OnComplete;
};

void OnDownloadDrawComplete(void);


void stream_command_icon(uint8_t* data);


void stream_command_draw(uint8_t* data);


void on_receive_data(uint8_t* buffer, int length);

void receive_elgato(uint8_t* data, int length);

#endif // STREAM_COMMAND_HANDLER
