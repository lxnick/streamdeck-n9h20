#include "develop_blt2d.h"
#include "develop_tick.h"
#include "develop_utils.h"

#include "N9H20.h"
#include "wblib.h"

#include "../Common.h"

#define TICON_WIDTH	 (96)
#define TICON_HEIGHT (96)

#define TPIXEL_SIZE		(2)

#define TFRAME_SIZE		(PANEL_WIDTH*PANEL_HEIGHT*TPIXEL_SIZE)

#define TBUFFER_ALIGNMENT	(32)

extern unsigned char demo_icon_96x96[];
extern unsigned int demo_icon_96x96_len ;

static void* aligned_malloc_c99(size_t size, size_t alignment)
{
	void* raw;
	uintptr_t addr;
	uintptr_t aligned;
    // alignment must be a power-of-two and >= sizeof(void*)
    if (alignment < sizeof(void*) || (alignment & (alignment - 1)) != 0) return NULL;


    // Portable fallback (works in plain C99 and Keil armcc):
    // Over-allocate, align, and stash original pointer just before the aligned block.
		raw = malloc(size + alignment - 1 + sizeof(void*));
    if (!raw) return NULL;
    addr = (uintptr_t)raw + sizeof(void*);
    aligned = (addr + (alignment - 1)) & ~(uintptr_t)(alignment - 1);
    ((void**)aligned)[-1] = raw;           // store original pointer
    return (void*)aligned;
}

static void aligned_free_c99(void* p)
{
    if (!p) return;

    free(((void**)p)[-1]);                  // matches the fallback path
}

#define DISP_WIDTH          480
#define DISP_STRIDE         (DISP_WIDTH * 2)
#define DISP_HEIGHT         272
#define FMT_DST             eDRVBLT_DEST_RGB565

#define COLOR_WHITE         0xFFFFFFFF
#define COLOR_BLACK         0xFF000000
#define COLOR_GRAY          0xFF808080
#define COLOR_RED           0xFFFF0000
#define COLOR_GREEN         0xFF00FF00
#define COLOR_BLUE          0xFF0000FF

void blt2d_clear_disp_buffer(uint32_t fill_color)
{
    S_FI_FILLOP clr_op;
    
    // Fill color is non-premultiplied alpha.
       
    clr_op.sRect.i16Xmin = 0;
    clr_op.sRect.i16Ymin = 0;  
    clr_op.sRect.i16Xmax = DISP_WIDTH;
    clr_op.sRect.i16Ymax = DISP_HEIGHT;      
    
    clr_op.sARGB8.u8Blue = (fill_color & 0x000000FF);
    clr_op.sARGB8.u8Green = (fill_color & 0x0000FF00) >> 8;
    clr_op.sARGB8.u8Red = (fill_color & 0x00FF0000) >> 16;
    clr_op.sARGB8.u8Alpha = (fill_color & 0xFF000000) >> 24;
    
    clr_op.u32FBAddr = (uint32_t) g_FrameBuffer;
    clr_op.i32Stride = DISP_STRIDE;
    clr_op.eDisplayFmt = FMT_DST;
    clr_op.i32Blend = 0;    // No alpha blending.
    
    bltFIFill(clr_op);
}


INT32 g_VdmaCh = 0;
volatile BOOL g_bVdmaInt = FALSE;

void EdmaIrqHandler(unsigned int arg)
{ 	
	EDMA_Free(g_VdmaCh);  	
	g_bVdmaInt = TRUE;
}

void vdma_memcpy(void* src, void* dest, int size )
{
	UINT32 src_addr, dest_addr;

	src_addr = (UINT32)src | NON_CACHE_BIT;
	dest_addr = (UINT32)dest;

	g_VdmaCh = VDMA_FindandRequest();
	
	if (g_VdmaCh < 0) 
	{
			sysprintf("Request VDMA fail.\n");
			return;
	}

	EDMA_SetupSingle(g_VdmaCh, src_addr, dest_addr, size);
	EDMA_SetupHandlers(g_VdmaCh, eDRVEDMA_BLKD_FLAG, EdmaIrqHandler, 0);

	EDMA_Trigger(g_VdmaCh);

	while(g_bVdmaInt == FALSE);
	
	g_bVdmaInt = FALSE;
}	

void develop_draw_icon_rgb565(uint16_t *fb_base, int fb_w, int fb_h,
                      uint16_t *icon, int iw, int ih,
                      int dst_x, int dst_y)
{
    int sx = 0, sy = 0;              // icon start position
    int w  = iw, h = ih;             // drawkinfg rect
		int y;
		uint16_t *src;
		uint16_t *dst;
	
    if (dst_x < 0) 
			{	sx = -dst_x; w -= sx; dst_x = 0; }
    if (dst_y < 0) 
			{ sy = -dst_y; h -= sy; dst_y = 0; }
    if (dst_x + w > fb_w)  
			w = fb_w  - dst_x;
    if (dst_y + h > fb_h)  
			h = fb_h  - dst_y;
    if (w <= 0 || h <= 0) 
			return;    

		src = icon + sy * iw + sx;
    dst = fb_base + dst_y * fb_w + dst_x;

    for ( y = 0; y < h; ++y) {
        memcpy(dst, src, (size_t)w * sizeof(uint16_t));
        src += iw;       
        dst += fb_w;     
    }
}

static void image_transform_out_size(int src_w, int src_h, img_op_t op,
                                          int *out_w, int *out_h)
{
    if (op == IMG_OP_ROT_90 || op == IMG_OP_ROT_270) {
        *out_w = src_h; *out_h = src_w;
    } else {
        *out_w = src_w; *out_h = src_h;
    }
}

static void compute_out_buffer_size(int src_w, int src_h, int bytes_per_pixel,
                                           img_op_t op, int *out_w, int *out_h, int *out_bytes)
{
    image_transform_out_size(src_w, src_h, op, out_w, out_h);
    if (out_bytes) *out_bytes = (*out_w) * (*out_h) * bytes_per_pixel;
}

int image_transform_rgb565(const uint16_t *src, int src_w, int src_h, int src_stride_bytes,
                         uint16_t *dst, int dst_stride_bytes, img_op_t op)
{
    int out_w, out_h;
		int x,y;
		int sx,  sy;
		int src_pitch_px;
		int dst_pitch_px;
		
    if (!src || !dst || src_w <= 0 || src_h <= 0) return -1;

    if (src_stride_bytes == 0) src_stride_bytes = src_w * 2;
		src_pitch_px = src_stride_bytes >> 1;

    image_transform_out_size(src_w, src_h, op, &out_w, &out_h);
    if (dst_stride_bytes == 0) dst_stride_bytes = out_w * 2;
		dst_pitch_px = dst_stride_bytes >> 1;

    switch (op) {
    case IMG_OP_FLIP: { // ????:????(??)
        for ( y = 0; y < src_h; ++y) {
            const uint16_t *srow = src + (src_h - 1 - y) * src_pitch_px;
            uint16_t *drow       = dst + y * dst_pitch_px;
            memcpy(drow, srow, (size_t)src_w * 2);
        }
        break;
    }
    case IMG_OP_MIRROR: { // ????:?????
        for ( y = 0; y < src_h; ++y) {
            const uint16_t *srow = src + y * src_pitch_px;
            uint16_t *drow       = dst + y * dst_pitch_px;
            for ( x = 0; x < src_w; ++x) {
                drow[x] = srow[src_w - 1 - x];
            }
        }
        break;
    }
    case IMG_OP_ROT_180: { // 180°:?? + ??
        for ( y = 0; y < src_h; ++y) {
            const uint16_t *srow = src + (src_h - 1 - y) * src_pitch_px;
            uint16_t *drow       = dst + y * dst_pitch_px;
            for ( x = 0; x < src_w; ++x) {
                drow[x] = srow[src_w - 1 - x];
            }
        }
        break;
    }
    case IMG_OP_ROT_90: {  // ??? 90°:dst(x,y)=src(y, src_h-1-x)
        for ( y = 0; y < out_h; ++y) {              // out_h = src_w
            uint16_t *drow = dst + y * dst_pitch_px;
            for ( x = 0; x < out_w; ++x) {          // out_w = src_h
                sx = y;
                sy = src_h - 1 - x;
                drow[x] = src[sy * src_pitch_px + sx];
            }
        }
        break;
    }
    case IMG_OP_ROT_270: { // ??? 270°:dst(x,y)=src(src_w-1-y, x)
        for ( y = 0; y < out_h; ++y) {              // out_h = src_w
            uint16_t *drow = dst + y * dst_pitch_px;
            for ( x = 0; x < out_w; ++x) {          // out_w = src_h
                sx = src_w - 1 - y;
                 sy = x;
                drow[x] = src[sy * src_pitch_px + sx];
            }
        }
        break;
    }
    default: return -1;
    }
    return 0;
}

void develop_image_rotate90_rgb565(void* src, int w, int h, void* dest)
{
	image_transform_rgb565(src,w,h,0,dest, 0, IMG_OP_ROT_90);
}	

void develop_image_rotate180_rgb565(void* src, int w, int h, void* dest)
{
	image_transform_rgb565(src,w,h,0,dest, 0, IMG_OP_ROT_180);
}

void develop_image_rotate270_rgb565(void* src, int w, int h, void* dest)
{
	image_transform_rgb565(src,w,h,0,dest, 0, IMG_OP_ROT_270);
}

void develop_image_mirror_rgb565(void* src, int w, int h, void* dest)
{
	image_transform_rgb565(src,w,h,0,dest, 0, IMG_OP_MIRROR);
}	

void develop_image_flip_rgb565(void* src, int w, int h, void* dest)
{
	image_transform_rgb565(src,w,h,0,dest, 0, IMG_OP_FLIP);
}	



/* memo: on 480x272xRGB565 frame  buffer
	memcpy 							8,320 us
	blt2d_clear_buffer 10,770 us
	memset							3,510 us
	vdma_memcpy					5,940 us
*/


void develop_test_rotate90(void)
{
	uint8_t* rotate  =  malloc(demo_icon_96x96_len);
	
	develop_image_rotate90_rgb565(demo_icon_96x96,96,96,rotate);
	develop_draw_icon_rgb565(MAKE_NONE_CACHE(g_FrameBuffer), 480, 272, (uint16_t*) rotate, 96, 96,  96, 0 );
	
	free( rotate );
}

void develop_test_rotate180(void)
{
	uint8_t* rotate  =  malloc(demo_icon_96x96_len);
	
	develop_image_rotate180_rgb565(demo_icon_96x96,96,96,rotate);
	develop_draw_icon_rgb565(MAKE_NONE_CACHE(g_FrameBuffer), 480, 272, (uint16_t*) rotate, 96, 96,  96, 96 );
	
	free( rotate );
}

void develop_test_rotate270(void)
{
	uint8_t* rotate  =  malloc(demo_icon_96x96_len);
	
	develop_image_rotate270_rgb565(demo_icon_96x96,96,96,rotate);
	develop_draw_icon_rgb565(MAKE_NONE_CACHE(g_FrameBuffer), 480, 272, (uint16_t*) rotate, 96, 96,  0, 96 );
	
	free( rotate );
}

void develop_test_mirror(void)
{
	uint8_t* mirror  =  malloc(demo_icon_96x96_len);
	
	develop_image_mirror_rgb565(demo_icon_96x96,96,96,mirror);
	develop_draw_icon_rgb565(MAKE_NONE_CACHE(g_FrameBuffer), 480, 272, (uint16_t*) mirror, 96, 96,  192, 0 );
	
	free( mirror );
}	

void develop_test_flip(void)
{
	uint8_t* flip  =  malloc(demo_icon_96x96_len);
	
	develop_image_flip_rgb565(demo_icon_96x96,96,96,flip);
	develop_draw_icon_rgb565(MAKE_NONE_CACHE(g_FrameBuffer), 480, 272, (uint16_t*) flip, 96, 96,  192, 96 );
	
	free( flip );
}	

void develop_blt2d_test(void)
{
		UINT32 start_tick;
		UINT32 end_tick;
	
		uint8_t* pBackup;
    sysprintf("Demo Blt2d Start\n");   
	
		// Backup Main Frame
		pBackup = aligned_malloc_c99( TFRAME_SIZE, TBUFFER_ALIGNMENT);
	
		start_tick = develop_tick_get_count(TIMER0);
		memcpy(pBackup, g_FrameBuffer, TFRAME_SIZE);
		end_tick = develop_tick_get_count(TIMER0);
		sysprintf("\tmemcpy %d us\n", (end_tick - start_tick)*10 );	
	
		// Clear Main Frame
		start_tick = develop_tick_get_count(TIMER0);		
		blt2d_clear_disp_buffer(COLOR_GREEN);
		end_tick = develop_tick_get_count(TIMER0);
		sysprintf("\tblt2d_clear_disp_buffer %d us\n", (end_tick - start_tick)*10 );

		start_tick = develop_tick_get_count(TIMER0);		
		memset(g_FrameBuffer, 0x00, TFRAME_SIZE);
		end_tick = develop_tick_get_count(TIMER0);
		sysprintf("\tmemset %d us\n", (end_tick - start_tick)*10 );
		
		develop_draw_icon_rgb565(MAKE_NONE_CACHE(g_FrameBuffer), 480, 272, (uint16_t*) demo_icon_96x96, 96, 96,  0, 0 );
		
		develop_test_rotate90();	
		develop_test_rotate180();	
		develop_test_rotate270();			
		develop_test_mirror();
		develop_test_flip();
		
	
#if 0	
		start_tick = develop_tick_get_count(TIMER0);		
		vdma_memcpy(pBackup, g_FrameBuffer, TFRAME_SIZE);
		end_tick = develop_tick_get_count(TIMER0);
		sysprintf("\tvdma %d us\n", (end_tick - start_tick)*10 );	
#endif

    sysprintf("Stop Here!!!\n"); 	
		while(1)
			;
		
}

