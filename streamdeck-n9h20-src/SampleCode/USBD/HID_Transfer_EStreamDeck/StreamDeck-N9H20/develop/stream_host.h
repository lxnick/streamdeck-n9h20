#ifndef STREAM_HOST_HEADER
#define STREAM_HOST_HEADER

#include "stdint.h"

#pragma pack (push, 1)

#define STREAM_DECK_OUTPUT_BLOCK	1024

#define REPORT_ID			(0x02)


#define COMMAND_ELGATO	(0x07)		//	Elgato Stream Deck download  icon
#define COMMAND_FILL		(0xD0)
#define COMMAND_DRAW_JPEG	(0xD1)

#define RGB565_RED			(0xF800)
#define RGB565_GREEN		(0x07D0)
#define RGB565_BLUE			(0x001F)
#define RGB565_YELLOW		(0xFFD0)

#define IMAGE_END			0x01


struct  stream_header
{
	uint8_t report_id;
	uint8_t	command;
};

struct  stream_rect
{
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
};

struct  stream_image
{
	uint32_t total_length;
	uint16_t block_size;
	uint16_t data_flag;
};

struct stream_elgato
{
	struct  stream_header header;
	uint8_t icon_id;
	uint8_t image_end;
	uint16_t data_length;
	uint16_t image_chunk;
	uint8_t data[1];
};

struct stream_deck_output
{
	uint8_t report_id;
	uint8_t	command;
	uint8_t icon_id;
	uint8_t image_end;
	uint8_t data_length_low;
	uint8_t data_length_hi;
	uint8_t image_trunk_low;
	uint8_t image_trunk_hi;
	uint8_t data[1];
};

struct stream_deck_fill
{
	uint8_t report_id;
	uint8_t	command;	// 0xD0
	uint8_t x_low;
	uint8_t x_hi;
	uint8_t y_low;
	uint8_t y_hi;
	uint8_t size_x_low;
	uint8_t size_x_hi;
	uint8_t size_y_low;
	uint8_t size_y_hi;
	uint8_t color_low;
	uint8_t color_hi;
	uint8_t data[1];
};

struct stream_deck_draw
{
	struct stream_header header;		
	struct stream_rect rect;
	struct stream_image image;

	uint8_t data[1];
};


#pragma pack (pop)

#endif
