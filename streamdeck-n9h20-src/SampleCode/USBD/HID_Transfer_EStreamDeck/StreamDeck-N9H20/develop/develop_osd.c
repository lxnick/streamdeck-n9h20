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
	uint16_t c;
	
	struct stream_deck_fill* fill = (struct stream_deck_fill*) p;
	
	x = make_word(fill->x_hi, fill->x_low);
	y = make_word(fill->y_hi, fill->y_low);
	w = make_word(fill->size_x_hi, fill->size_x_low);
	h = make_word(fill->size_y_hi, fill->size_y_low);	
	c = make_word(fill->color_hi, fill->color_low);	

	osd_fill_rgb565(g_FrameBuffer,PANEL_WIDTH, PANEL_HEIGHT, x,y,w,h,c);

}


