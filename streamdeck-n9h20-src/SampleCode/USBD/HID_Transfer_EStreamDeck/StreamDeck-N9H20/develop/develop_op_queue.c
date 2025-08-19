#include "develop_op_queue.h"

#include "N9H20.h"

uint32_t op_sequence  = 1;
struct op_node op_table[ MAX_OP ];

#define MAX_OP      32

void op_queue_init(void)
{
    memset( op_table, 0, sizeof(op_table));
};

bool op_queue_add(struct op_node* node)
{
    int i;
    for( i=0; i < MAX_OP; i ++)
    {
        if ( op_table[i].active == 0 )
        {
            op_table[i] = *node;
            op_table[i].active = 1;
            op_table[i].seq = op_sequence;
            op_sequence ++;
            return  true;
        }
    }

    return false;
}

bool op_queue_release(struct op_node* node)
{
    int i;
    for( i=0; i < MAX_OP; i ++)
    {
        if ( op_table[i].active == 1 && op_table[i].seq == node-> seq)
        {
            if ( op_table[i].buffer != NULL)
                free( op_table[i].buffer );
            memset( &op_table[i], 0, sizeof(struct op_node));
            return  true;
        }
    }
		return  false;
}

bool op_queue_get(struct op_node* node)
{
    int i;
    for( i=0; i < MAX_OP; i ++)
    {
        if ( op_table[i].active == 1 )
        {
            *node = op_table[i];
            return  true;
        }
    }

    return false;
}
