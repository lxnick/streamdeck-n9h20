#include "develop_osd.h"

extern char *g_FrameBuffer;

#define PANEL_WIDTH 480  /* PANEL Width (Raw data output width for Panel Test) */
#define PANEL_HEIGHT 272 /* PANEL Height (Raw data output height for Panel Test) */

uint16_t make_word(uint8_t hi, uint8_t low)
{
		uint16_t v = hi;
		v <<= 8;
		v |= low;
		return v;
}	

void osd_fill_rgb565(uint8_t* p, int width, int height, int x, int y, int size_x, int size_y,  uint16_t pattern)
{
    int i, j;

    int offset  =  (y*width*sizeof(uint16_t)+ x*sizeof(uint16_t));
    uint16_t* line;

    for( i=0; i < size_y; i ++)
    {
        line = (uint16_t*) ( p + offset + i* width * sizeof(uint16_t));
        for( j=0;  j < size_x; j ++)
            line[ j] = pattern;
    }
}


void command_fill(uint8_t* p)
{
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
//	uint32_t c;
	
	struct stream_deck_fill* fill = (struct stream_deck_fill*) p;
	
	x = fill->rect.x;
	y = fill->rect.y;
	w = fill->rect.w;
	h = fill->rect.h;	
//	c = fill->color;	

//	osd_fill_rgb565(g_FrameBuffer,PANEL_WIDTH, PANEL_HEIGHT, x,y,w,h,c);
}

