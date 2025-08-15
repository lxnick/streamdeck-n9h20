#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wblib.h"
#include "N9H20_reg.h"
#include "HID.h"
#include "Common.h"
#include "N9H20_VPOST.h"
#include "N9H20_JPEG.h"
#include "peripheral.h"
#include "N9H20.h"

#include "develop/frame_buffer.h"
#include "develop/develop_tick.h"
#include "develop/develop_op_queue.h"

void HIDStart(void);

#if defined(__GNUC__)
CHAR g_pu8FrameBuffer[PANEL_WIDTH * PANEL_HEIGHT * 2] __attribute__((aligned(32)));
#else
__align(32) CHAR g_pu8FrameBuffer[PANEL_WIDTH * PANEL_HEIGHT * 2];
__align(32) CHAR g_pu8FrameBuffer_test[PANEL_WIDTH * PANEL_HEIGHT * 2];
__align(32) CHAR g_pu8JpegBuffer[120 * 120 * 2];
#endif

// Eden
CHAR *g_FrameBuffer;
CHAR *g_JpegBuffer;
CHAR *g_FrameBuffer_test;

LCDFORMATEX lcdInfo;

void delay(int delay)
{
    int volatile count = 100 * delay;
    int volatile i;
    for (i = 0; i < count; i++)
        ;
}



volatile BOOL new_frame_flag = FALSE;

void tick_callback_10ms(void)
{
		static int tick_count = 0;
	
		tick_count++;
		if ( tick_count >= 100 )
		{
//			sysprintf("pass 1 second\n");
			tick_count = 0;
			new_frame_flag = TRUE;
		}		
//	new_frame_flag = TRUE;		
}


//==================================================================================================
int main(void)
{
    WB_UART_T uart;
    unsigned int volatile status = 0;
    UINT32 u32ExtFreq;
		UINT32 u32Tick;
    sysUartPort(1);
    u32ExtFreq = sysGetExternalClock(); /* Hz unit */
    uart.uiFreq = u32ExtFreq * 1000;
    uart.uiBaudrate = 115200;
    uart.uiDataBits = WB_DATA_BITS_8;
    uart.uiStopBits = WB_STOP_BITS_1;
    uart.uiParity = WB_PARITY_NONE;
    uart.uiRxTriggerLevel = LEVEL_1_BYTE;
    sysInitializeUART(&uart);

    sysInvalidCache();
    sysEnableCache(CACHE_WRITE_THROUGH);
    sysFlushCache(I_D_CACHE);

    /*-----------------------------------------------------------------------*/
    /*  CPU/HCLK/APB:  192/96/48                                             */
    /*-----------------------------------------------------------------------*/
    sysSetSystemClock(eSYS_UPLL,   // E_SYS_SRC_CLK eSrcClk,
                      192000,      // UINT32 u32PllKHz,
                      192000,      // UINT32 u32SysKHz,
                      192000,      // UINT32 u32CpuKHz,
                      192000 / 2,  // UINT32 u32HclkKHz,
                      192000 / 4); // UINT32 u32ApbKHz

    sysprintf("Demo code Start\n");

    // shift the frame buffer to 0x80000000
    g_FrameBuffer = (CHAR *)((UINT32)g_pu8FrameBuffer | 0x80000000);
    g_JpegBuffer = (CHAR *)((UINT32)g_pu8JpegBuffer | 0x80000000);
//    g_FrameBuffer_test = (CHAR *)((UINT32)g_pu8FrameBuffer_test | 0x80000000);

    /* Init Panel */
    lcdInfo.ucVASrcFormat = DRVVPOST_FRAME_RGB565;
    lcdInfo.nScreenWidth = PANEL_WIDTH;
    lcdInfo.nScreenHeight = PANEL_HEIGHT;

    vpostLCMInit(&lcdInfo, (UINT32 *)g_FrameBuffer);
		
//		fb_init();
		develop_tick_start(TIMER0,TICK_10US);
		develop_tick_set_event(TIMER0, 1000, tick_callback_10ms );
		
		op_queue_init();
		
    // Eden
    // gpio_interrupt_init();
    kpi_init();
    kpi_open(3); // use nIRQ0 as external interrupt source

    jpegOpen();

    HIDStart();
		


    while (1)
    {

				

//				sysprintf("Tick  %d\n",u32Tick );				
				
//				draw_icon();
//        key_press();
			
				main_task();
		
//				fb_animate();
			
			
//				if ( ! new_frame_flag)
//						continue;
//				new_frame_flag = FALSE;
    }
}

//==================================================================================================
