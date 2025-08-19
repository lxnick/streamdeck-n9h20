#include "N9H20_VPOST.h"
#include "frame_buffer.h"

#include "stream_command.h"


// Set bit31=1 as cacheable alias
#define TO_CACHE_ADDR(p)    ((void *)((UINT32)(p) |  0x80000000))
#define TO_NC_ADDR(p)       ((void *)((UINT32)(p) & ~0x80000000))
	
// Set cache  alignemnt 32 bytes
#define CACHE_LINE_BYTES    32U
#define ALIGN_DOWN(v,a)     ((UINT32)(v) & ~((UINT32)(a) - 1U))
#define ALIGN_UP(v,a)       (((UINT32)(v) + ((UINT32)(a) - 1U)) & ~((UINT32)(a) - 1U))


#define RGB565(r,g,b)	(UINT16)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))


extern CHAR *g_FrameBuffer;

typedef struct {
#if DUAL_BUFFER		
    UINT8 *nc[2];     // non-cache buffer base address
#else
    UINT8 *nc[1];
#endif	
    int    draw_idx;  // current drawing  buffer, cacheable 
    int    show_idx;  // current displaying buffer, non-cache
} fb_system_t;

static fb_system_t s_fb;
#if DUAL_BUFFER
uint32_t fb0_buffer[ FB_SIZE / sizeof(uint32_t)];
uint32_t fb1_buffer[ FB_SIZE / sizeof(uint32_t)];
#else
uint32_t fb0_buffer[ FB_SIZE / sizeof(uint32_t)];
#endif

int fb_init(void)
{
#if DUAL_BUFFER	
    s_fb.nc[0]  = (UINT8 *) &fb0_buffer[0];
    s_fb.nc[1]  = (UINT8 *) &fb1_buffer[0];
    s_fb.show_idx = 0;     
    s_fb.draw_idx = 1;     

    vpostSetFrameBuffer((UINT32) s_fb.nc[s_fb.show_idx]);

    memset(TO_CACHE_ADDR(s_fb.nc[0]), 0x00, FB_SIZE);
    memset(TO_CACHE_ADDR(s_fb.nc[1]), 0x00, FB_SIZE);
#else
    s_fb.nc[0]  = (UINT8 *) &fb0_buffer[0];
    s_fb.show_idx = 0;     
    s_fb.draw_idx = 0; 
	
    vpostSetFrameBuffer((UINT32) s_fb.nc[0]);
    memset(TO_CACHE_ADDR(s_fb.nc[0]), 0x00, FB_SIZE);	
#endif	
    return 0;
}

uint8_t *fb_get_draw_buffer(void)
{
#if DUAL_BUFFER	
    return (uint8_t *)TO_CACHE_ADDR(s_fb.nc[s_fb.draw_idx]);
#else
    return (uint8_t *)TO_CACHE_ADDR( g_FrameBuffer );
#endif	
}

uint8_t *fb_get_display_buffer_nc(void)
{
#if DUAL_BUFFER		
    return s_fb.nc[s_fb.show_idx];
#else
    return s_fb.nc[0];
#endif	
}

void fb_flush_region(int x, int y, int w, int h)
{
//		uint32_t start;
//		uint32_t end;
//		uint32_t aligned_start;
//		uint32_t aligned_end;	
//		uint32_t len;	
	
	//	uint8_t* draw_c;

    if (w <= 0 || h <= 0) return;
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x >= FB_WIDTH || y >= FB_HEIGHT) return;
    if (x + w > FB_WIDTH)  w = FB_WIDTH  - x;
    if (y + h > FB_HEIGHT) h = FB_HEIGHT - y;

//    draw_c = fb_get_draw_buffer();

 //   start = (UINT32)(draw_c + (y * FB_WIDTH + x) * FB_PER_PIXEL);
 //   end   = (UINT32)(draw_c + ((y + h - 1) * FB_WIDTH + (x + w)) * FB_PER_PIXEL);
//    aligned_start = ALIGN_DOWN(start, CACHE_LINE_BYTES);
 //   aligned_end   = ALIGN_UP(end,   CACHE_LINE_BYTES);
//    len           = aligned_end - aligned_start;

//    sysFlushCache((UINT8 *)aligned_start, len);
}

void fb_swap(void)
{
#if DUAL_BUFFER		   
	uint8_t* draw_c = fb_get_draw_buffer();

	vpostSetFrameBuffer( (UINT32) s_fb.nc[s_fb.draw_idx]);
	s_fb.show_idx = s_fb.draw_idx;
  s_fb.draw_idx = 1 - s_fb.draw_idx;
#else
	vpostSetFrameBuffer( (UINT32) s_fb.nc[0]);

#endif	
}

void fb_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
		int i, j;
		uint8_t* buffer = fb_get_draw_buffer();

    int offset  =  (y*FB_LINE+ x*FB_PER_PIXEL);
    uint16_t* line;

    for( i=0; i < h; i ++)
    {
        line = (uint16_t*) ( buffer + offset + i* FB_LINE);
        for( j=0;  j < w; j ++)
            line[ j] = color;
    }
}	

void fb_copy_rect(uint8_t* src, uint16_t srcw, uint16_t srch, uint16_t destx, uint16_t desty)
{
		int i;
	
		uint8_t* dest = fb_get_draw_buffer();
		int dest_offset = (desty*FB_LINE+ destx*FB_PER_PIXEL);
	
		int line_size = srcw * FB_PER_PIXEL;
		uint16_t* src_ptr = (uint16_t*)  ((UINT32)src| 0x80000000) ;
		uint16_t* dest_ptr = (uint16_t*) ( (UINT32) (dest + dest_offset) | 0x80000000) ;
	
    for( i=0; i < srch; i ++)
    {
				memcpy(dest_ptr, src_ptr, line_size);
				dest_ptr += FB_LINE;	
				src_ptr += line_size;			
    }  	
}	

typedef struct {
    int x, y;
    int w, h;
    int vx, vy;
    UINT16 color;
} Sprite;

static Sprite box = 
{
	10, 10,
	60, 40,
	3,2,
	RGB565(255, 80, 80),
};

void fb_animate(void)
{
		
//	fb_fill_rect(0,0,FB_WIDTH,FB_HEIGHT, RGB565(16,16,16) );
	
	// Update physics
        box.x += box.vx;
        box.y += box.vy;
        if (box.x < 0)                  { box.x = 0; box.vx = -box.vx; }
        if (box.y < 0)                  { box.y = 0; box.vy = -box.vy; }
        if (box.x + box.w >= FB_WIDTH) { box.x = FB_WIDTH - box.w - 1; box.vx = -box.vx; }
        if (box.y + box.h >= FB_HEIGHT){ box.y = FB_HEIGHT - box.h - 1; box.vy = -box.vy; }

        // Draw sprite
        fb_fill_rect(box.x, box.y, box.w, box.h, box.color);


        // Present
 //       fb_swap();

        // Simple pacing (~60–70 fps on many boards; adjust as you like)
        sysDelay(16);
}	
