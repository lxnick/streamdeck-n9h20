#ifndef STREAM_HOST_HEADER
#define STREAM_HOST_HEADER

#include "stdint.h"

#pragma pack (push, 1)

/* Stream HID Command format*/

/* HID report size */
#define STREAM_DECK_OUTPUT_BLOCK	1024
#define STREAM_DECK_INPUT_BLOCK		512
#define STREAM_DECK_FEATURE_BLOCK	32

/* HID report ID */
#define REPORT_ID_INPUT			(0x01)
#define REPORT_ID_OUTPUT		(0x02)



#define REPORT_ID_FEATURE_3		(0x03)
#define REPORT_ID_FEATURE_12	(0x0C)
#define REPORT_ID_KEYBOARD		(0x0D)

struct  stream_header
{
	uint8_t report_id;
	uint8_t	command;
};

/* ---------------------------*/
/* HID Input :REPORT_ID_INPUT */
#define INPUT_ELGATO			(0x00)	
/*
+----+----------+---------------+------------------------------------------ - +
|Byte| Field    | Value / Notes |
+----+----------+---------------+------------------------------------------ - +
|  0 | ReportId | 0x01          |
|  1 | Command  | 0x00          | 0
| 2/3| Icon     | B2[0x0F]+B3[] |
|  3 | Reserved | 0x00          |
|  4 | Key State Bytes | One byte per key : 0x01 = Pressed, 0x00 = Not. |
|     |                           | Pad 0x00 for unused bytes. |
+---- - +-------------------------- - +------------------------------------------ - +
*/
#define INPUT_UHPB				(0x01)

#define TOUCH_EVENT				(0x01)
#define MAX_TOUCH_POINT			(10)
/*
+-------+----------+---------------+-----------------------------------------
|Byte   | Field    | Value / Notes |
+-------+----------+---------------+-----------------------------------------
|  0    | ReportId | 0x01          |
|  1    | Command  | 0x01          | 
|  2    | Event    | 0x01          | TOUCH_EVENT	
|  3    | Mask     | 0xFE          | 0xFF - TOUCH_EVENT
|  4    | pt-count |               | uint8_t 
|  5    | pad      |               | not used
|  6    | track-id |               | uint8_t
|  7    | space    |               | not used
|  8/9  | x        |               | uint16_t 
|  10/11| y        |               | uint16_t
|  12/13| size     |               | uint16_t

*/

struct strem_touch
{
	uint8_t track_id;
	uint8_t space;
	uint16_t x;
	uint16_t y;
	uint16_t size;
};

struct stream_event
{
	struct  stream_header header;
	uint8_t event;
	uint8_t mask;
	uint8_t count;
	uint8_t pad;
	struct strem_touch touch[MAX_TOUCH_POINT];
};

/* ---------------------------*/
/* HID Input :REPORT_ID_OUTPUT */



struct  stream_rect
{
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
};

struct  stream_color
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

struct  stream_image
{
	uint32_t total_length;
	uint16_t block_size;
	uint16_t data_flag;
};

struct stream_job_key
{
	uint16_t delay;
	uint8_t	key[6];
};

struct  stream_job_data
{
	uint32_t total_length;
	uint16_t block_size;
	uint16_t data_flag;
};

#define OUTPUT_ELGATO		(0x07)
#define IMAGE_END			0x01

/*
+----+----------+---------------+------------------------------------------ - +
|Byte| Field    | Value / Notes |
+----+----------+---------------+------------------------------------------ - +
|  0 | ReportId | 0x02          |
|  1 | Command  | 0x07          |
|  2 | Icon  ID |               | 0~
|  3 | ImgEndTag|               | 0x00: Normal, 0x01 : End
| 4/5| Length   |               | data count in this report
| 6/7| Trunk    |               | image trunk
|  8 | data     |               | payload
+----+--------------------------+------------------------------------------ - +
*/

struct stream_elgato
{
	struct  stream_header header;
	uint8_t icon_id;
	uint8_t image_end;
	uint16_t data_length;
	uint16_t image_chunk;
	uint8_t data[1];
};

#define OUTPUT_FILL			(0x10)
/*
+----+----------+---------------+------------------------------------------ - +
|Byte | Field    | Value / Notes |
+-----+----------+---------------+------------------------------------------ - +
|  0  | ReportId | 0x02          |
|  1  | Command  | 0x10          |
| 2/3 | x        |               | Panel x coordinat
| 4/5 | y        |               | Panel y coordinat
| 6/7 | w        |               | Draw width ( not used)
| 8/9 | h        |               | Draw height ( not used)
|  10 | r        |               | uint32_t Image Length
|  11 | g        |               | data in this report (block)
|  12 | b        |               | data block flag
|  13 | a        |               | payload
+----+--------------------------+------------------------------------------ - +
*/
struct stream_fill
{
	struct  stream_header header;
	struct  stream_rect rect;
	struct  stream_color color;
	uint8_t data[1];
};


#define OUTPUT_JPEG			(0x11)
/*
+----+----------+---------------+------------------------------------------ - +
|Byte | Field    | Value / Notes |
+-----+----------+---------------+------------------------------------------ - +
|  0  | ReportId | 0x02          |
|  1  | Command  | 0x11          |
| 2/3 | x        |               | Panel x coordinat
| 4/5 | y        |               | Panel y coordinat
| 6/7 | w        |               | Draw width ( not used)
| 8/9 | h        |               | Draw height ( not used)
|     | Length   |               | uint32_t Image Length
|14/15| Size     |               | data in this report (block)
|16/17| Falg     |               | data block flag
|     | data     |               | payload
+----+--------------------------+------------------------------------------ - +
*/
struct stream_jpeg
{
	struct stream_header header;		
	struct stream_rect rect;
	struct stream_image image;
	uint8_t data[1];
};

#define OUTPUT_JOB			(0x12)
/*
+----+----------+---------------+------------------------------------------ - +
|Byte | Field    | Value / Notes |
+-----+----------+---------------+------------------------------------------ - +
|  0  | ReportId | 0x02          |
|  1  | Command  | 0x12          |
| 2/3 | length   |               | data size for stream_job_item  
| 4/5 | count    |               | key count to bo sent
| 6/7 | reserved |               | reserved 
| 8/9 | Key-Delay|               | delay time to send key (in ms )
|10/  | Key-Mod  |               | HID Key[0] Modifier
|11/  | Key-Res  |               | HID Key[1] Reserved
|10/  | Key-6    |               | HID Key[2~7] Key pattern
More  Keys
+----+--------------------------+------------------------------------------ - +
*/
struct stream_job_item
{
	uint16_t length;
	uint16_t count;	
	uint16_t reserved;			
	struct stream_job_key key[1];
};

struct stream_job
{	
	struct stream_header header;	
	struct stream_job_item item;
};

#define RGB565_RED			(0xF800)
#define RGB565_GREEN		(0x07D0)
#define RGB565_BLUE			(0x001F)
#define RGB565_YELLOW		(0xFFD0)

#pragma pack (pop)

#endif
