/****************************************************************
 *                                                              *
 * Copyright (c) Chicony Electronics Co., Ltd                   *
 *                                                              *
 ****************************************************************/
#ifndef DEVELOP_UTILS_HEADER
#define DEVELOP_UTILS_HEADER

#include "stdint.h"
#include "stream_command.h"


struct stream_deck_draw_jpeg
{
	uint8_t report_id;
	uint8_t	command;	// 0xD0

};	

void osd_fill_rgb565(uint8_t* p, int width, int height, int x, int y, int size_x, int size_y,  uint16_t pattern);

void command_fill(uint8_t* p);

void command_download(uint8_t* p);
	
	

#endif // #ifndef DEVELOP_UTILS_HEADER
