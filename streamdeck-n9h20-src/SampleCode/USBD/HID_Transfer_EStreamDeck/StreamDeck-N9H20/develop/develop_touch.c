#include "develop_app.h"

#include "N9H20.h"
#include "stream_host.h"

#define TOUCH_ADC_MIN_X		(100)
#define TOUCH_ADC_MAX_X		(915)
#define TOUCH_ADC_MIN_Y		(180)
#define TOUCH_ADC_MAX_Y		(810)

#define TOUCH_ADC_X				(480)
#define TOUCH_ADC_Y				(272)

extern void  HID_SendInputReport(uint8_t* data, int count);

static struct stream_event event;

static void touch_adc_hard_compensate(UINT16 srcx, UINT16 srcy, UINT16* px, UINT16* py)
{
		UINT16 x, y;
		
		if ( srcx > TOUCH_ADC_MAX_X)
			srcx = TOUCH_ADC_MAX_X;
		if ( srcx < TOUCH_ADC_MIN_X)
			srcx = TOUCH_ADC_MIN_X;
		if ( srcy > TOUCH_ADC_MAX_Y)
			srcy = TOUCH_ADC_MAX_Y;
		if ( srcy < TOUCH_ADC_MIN_Y)
			srcy = TOUCH_ADC_MIN_Y;
		
		*px = (srcx-TOUCH_ADC_MIN_X)*TOUCH_ADC_X / (TOUCH_ADC_MAX_X - TOUCH_ADC_MIN_X);
		*py = (srcy-TOUCH_ADC_MIN_Y)*TOUCH_ADC_Y / (TOUCH_ADC_MAX_Y - TOUCH_ADC_MIN_Y);
}		
		



void develop_touch_adc(void)
{
	UINT16 x, y;
	UINT16 fx, fy;
	
	int i;
	
	if(adc_read(ADC_NONBLOCK, &x, &y))
	{
		sysprintf("TP[ADC]x = %d, y = %d\n", x, y);	
		
		touch_adc_hard_compensate(x,y,&fx,&fy);
		sysprintf("TP[ADC-FX]x = %d, y = %d\n", fx, fy);			
				
		memset(&event,0, sizeof(struct stream_event));
		event.header.report_id = REPORT_ID_INPUT;
		event.header.command = INPUT_UHPB;
		event.event = TOUCH_EVENT;
		event.mask = 0xFE;
		event.count = MAX_TOUCH_POINT;
			
		event.touch[0].track_id = 0;
		event.touch[0].x = fx;			
		event.touch[0].y = fy;
		event.touch[0].size = 12;
			
		for( i=1; i < MAX_TOUCH_POINT; i++)
		{
			event.touch[i].track_id = i;
			event.touch[i].x = i;			
			event.touch[i].y = i;
			event.touch[i].size = i;				
		}		
			
		HID_SendInputReport( (uint8_t*) &event, sizeof( struct stream_event) );
	
	}
}	

