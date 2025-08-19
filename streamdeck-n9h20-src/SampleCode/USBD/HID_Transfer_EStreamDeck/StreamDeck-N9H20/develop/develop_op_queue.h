/****************************************************************
 *                                                              *
 * Copyright (c) Chicony Electronics Co., Ltd                   *
 *                                                              *
 ****************************************************************/
#ifndef DEVELOP_OP_QUEUE_HEADER
#define DEVELOP_OP_QUEUE_HEADER

#include "stdint.h"
#include "stdbool.h"

#define OP_FILE     0x01
#define OP_RECT     0x02
#define OP_BALL     0x03
#define OP_ANIMATE  0x04     

#define MAX_OP      32

#define ROTATE_0					0
#define ROTATE_90				90
#define ROTATE_180				180
#define ROTATE_270				270

struct op_node
{
	uint8_t active;
	uint8_t op;
	uint8_t rotate;
	
	uint16_t icon;
	

	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
	uint32_t seq;
	uint32_t length;
	uint8_t* 	buffer;
}	;

void op_queue_init(void);
bool op_queue_add(struct op_node* node);
bool op_queue_release(struct op_node* node);
bool op_queue_get(struct op_node* node);

#endif // #ifndef DEVELOP_TASK_QUEUE_HEADER

