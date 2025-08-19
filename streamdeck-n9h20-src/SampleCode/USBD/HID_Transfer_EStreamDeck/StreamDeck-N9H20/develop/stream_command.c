#include "stream_command.h"
#include "frame_buffer.h"
 #include <stdlib.h>
 
#include "N9H20_JPEG.h"

#include "develop_tick.h"
#include "develop_op_queue.h"

struct receive_session
{
	uint8_t report_id;
	uint8_t command;
	uint8_t icon_id;
	
	uint16_t total_length;
	
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
	
	bool	receive_done;		// To start decode
	uint8_t* 	buffer;
	uint32_t  collected;
}	;

struct  download_icon
{
	int icon_id;	//
	int data_count; 
	uint8_t* buffer;
};	

struct decode_info
{
		uint16_t width;
		uint16_t height;
		uint8_t* buffer;
};	


	

__align(32) uint8_t download_buffer[ FB_SIZE /4];
__align(32) uint8_t decode_buffer[ FB_SIZE ];

struct receive_session receive_data;

struct decode_info decode_data;


struct download_data download_data_info = 
{
	0,
	download_buffer,
	NULL
};	

bool icon_ready = false;

struct receive_session session;

void  reset_receive_session()
{
	session.report_id = 0;
	session.command = 0;
	session.icon_id = 0;
	session.buffer = download_buffer;
	session.collected = 0;
	session.total_length = 0;
	session.receive_done = false;
}	

BOOL decode_on_jpeg_header(void)
{
    JPEG_INFO_T jpegInfo;

    jpegGetInfo(&jpegInfo);

    if (jpegInfo.jpeg_width == 0 || jpegInfo.jpeg_height == 0)
        return FALSE;
		
		decode_data.width = jpegInfo.jpeg_width;
		decode_data.height = jpegInfo.jpeg_height;

#if 0
    /* DownScale size control */
    if (jpegInfo.jpeg_width > jpegInfo.jpeg_height)
    {
        if ((jpegInfo.jpeg_width > PANEL_WIDTH || jpegInfo.jpeg_height > PANEL_HEIGHT))
        {
            /* Set Downscale to QVGA */
            jpegIoctl(JPEG_IOCTL_SET_DECODE_DOWNSCALE, PANEL_HEIGHT, PANEL_WIDTH);
            u32TargetHeight = PANEL_HEIGHT;
            u32TargetWidth = PANEL_WIDTH;
        }
        else
        {
            u32TargetHeight = jpegInfo.jpeg_height;
            u32TargetWidth = jpegInfo.jpeg_width;
        }
    }
    else
    {
        if ((jpegInfo.jpeg_width > PANEL_WIDTH || jpegInfo.jpeg_height > PANEL_HEIGHT))
        {
            UINT32 ratio;
            ratio = jpegInfo.jpeg_height / PANEL_HEIGHT + 1;
            /* Set Downscale to QVGA */
            jpegIoctl(JPEG_IOCTL_SET_DECODE_DOWNSCALE, jpegInfo.jpeg_height / ratio, jpegInfo.jpeg_width / ratio);
            u32TargetHeight = jpegInfo.jpeg_height / ratio;
            u32TargetWidth = jpegInfo.jpeg_width / ratio;
        }
        else
        {
            u32TargetHeight = jpegInfo.jpeg_height;
            u32TargetWidth = jpegInfo.jpeg_width;
        }
    }
#endif

    /* The pixel offset for putting the image at the center of Frame Buffer */
//    shift_x = ((icon_index % ICON_ROW_COUNT) * ICON_SPACE_WIDTH) + 12;
//    shift_y = (icon_index / ICON_ROW_COUNT) * ICON_SPACE_HEIGHT + 8;

    /* Set Decode Stride to Panel width */
    jpegIoctl(JPEG_IOCTL_SET_DECODE_STRIDE, jpegInfo.jpeg_width, 0);

    /* Change Raw data Output address (Let JPEG engine output data to the center of Panel Buffer) */
    jpegIoctl(JPEG_IOCTL_SET_YADDR, (UINT32) decode_data.buffer, 0);


    return TRUE; /* Return TRUE to continue Decode operation, Otherwise, Stop Decode operation */
}

bool decode_jpeg(UINT32 u32SrcAddr, UINT32 u32DestAddr)
{
		decode_data.buffer = (uint8_t*) (u32DestAddr | 0x80000000);
		decode_data.width = 0;
		decode_data.height = 0;
	
    /* JPEG Init */
    jpegInit();

    /* Set Bit stream Address */
    jpegIoctl(JPEG_IOCTL_SET_BITSTREAM_ADDR, (UINT32)u32SrcAddr, 0);

    /* Decode mode */
    jpegIoctl(JPEG_IOCTL_SET_DECODE_MODE, JPEG_DEC_PRIMARY_PACKET_RGB565, 0);

    /* Set JPEG Header Decode End Call Back Function */
    jpegIoctl(JPEG_IOCTL_SET_HEADERDECODE_CALBACKFUN, (UINT32)decode_on_jpeg_header, 0);

    /* Trigger JPEG decoder */
    jpegIoctl(JPEG_IOCTL_DECODE_TRIGGER, 0, 0);

    /* Wait for complete */
    //    if(!jpegWait())
    //    {
    //        sysprintf("JPEG Decode Error!!\n");
    //        return 1;
    //    }

    jpegWait();
		
		if ( (decode_data.width == 0 ) || (decode_data.height == 0 ))
			return false;

    return true;
}

struct  download_icon download_icon = 
{
	-1,
	0,
	download_buffer
};	

#if 0
void stream_command_icon(uint8_t* data)
{

	struct stream_command_icon* icon = (struct stream_command_icon*) data;
	
	if  ( download_icon.icon_id == -1 || download_icon.data_count == 0 )
		download_icon.icon_id = icon->icon_id;
	

	if ( icon->data_length == 0 )
		goto ERROR_EXIT;
	
	memcpy(download_icon.buffer + download_icon.data_count, icon->data, icon->data_length);
	download_icon.data_count += icon->data_length;

	if ( icon->image_end )
	{	
		icon_ready = true;
	}	
		
  return ;				
		
	ERROR_EXIT:
		download_icon.icon_id = -1;
		download_icon.data_count = 0;
		return ;
}	
#endif

#if 0
void stream_command_draw(uint8_t* data)
{
	struct stream_deck_draw* draw = (struct stream_deck_draw*) data;
	
	draw_icon = *draw;
			
	if ( draw->image.block_size == 0 )
		goto  ERROR_EXIT;
	

	memcpy(download_data_info.buffer + download_data_info.data_collected, draw->data, draw->image.block_size);
	download_icon.data_count += draw->image.block_size;

	if ( draw->image.data_flag == IMAGE_END )
	{	
		icon_ready = true;
	}	
		
  return ;				
		
	ERROR_EXIT:
		download_icon.icon_id = -1;
		download_icon.data_count = 0;
		return ;
}		
#endif


#define ICON_ROW_COUNT 5
#define ICON_COL_COUNT 3
#define ICON_SPACE_WIDTH 96
#define ICON_SPACE_HEIGHT 88

void receive_elgato(uint8_t* buffer, int length)
{
	uint16_t  data_length;
	if (buffer[0] != session.report_id)
			reset_receive_session();
		else if (buffer[1] != session.command)	
			reset_receive_session();			
		else if (buffer[2] != session.icon_id)
			reset_receive_session();
		
	session.report_id = buffer[0];
	session.command = buffer[1];	
	session.icon_id = buffer[2];
		
	data_length = 	buffer[4] + (buffer[5] << 8);
	memcpy(	session.buffer + 	session.collected, buffer + 8, data_length);
	session.collected += data_length;
	
	if ( 	buffer[3] == 1)
	{	
		struct op_node node;
		
		node.active = 1;
		node.op = OP_FILE;
		node.icon = session.icon_id;
		node.rotate = ROTATE_180;
		node.buffer = (uint8_t*) malloc( session.collected );
		memcpy( node.buffer,session.buffer, session.collected);
		node.length = session.collected;
		node.x = ((session.icon_id % ICON_ROW_COUNT) * ICON_SPACE_WIDTH) + 12;
		node.y = (session.icon_id / ICON_ROW_COUNT) * ICON_SPACE_HEIGHT + 8;	
		
		op_queue_add(&node);
		reset_receive_session();
	}	
}

void receive_fill(uint8_t* buffer, int length)
{
}	

void receive_draw(uint8_t* buffer, int length)
{
	uint16_t  data_length;	
	struct stream_deck_draw* draw = (struct stream_deck_draw*) buffer;
	
	if ( session.total_length != draw->image.total_length )
			reset_receive_session();	
	
	session.x = draw->rect.x;
	session.y = draw->rect.y;
	

	session.total_length = draw->image.total_length;
	data_length = draw->image.block_size;
	
	memcpy(	session.buffer + 	session.collected, draw->data, data_length);
	session.collected += data_length;
	
	if ( session.collected == session.total_length )
		session.receive_done = true;
}	

void on_receive_data(uint8_t* buffer, int length)
{
	if ( buffer[0] != REPORT_ID )
		return ;
				
	switch ( buffer[1] )
	{
		case COMMAND_ELGATO:
			receive_elgato(buffer,length);
			break;
		case COMMAND_FILL:
			receive_fill(buffer,length);
			break;
		case COMMAND_DRAW:
			receive_draw(buffer,length);
			break;
		default:
			break;
	}	
}	

void  TestCopyRect(uint16_t x, uint16_t y,  uint16_t w,  uint16_t  h)
{
		int i;
		uint16_t* drawn = (uint16_t* ) malloc( w * h * 2 );

		for( i=0; i < w * h; i ++)
			drawn[i] = RGB565_BLUE;
	
		fb_copy_rect( (uint8_t* )drawn, x, y, w, h );
	
		free( drawn);
	
}



void  main_task(void)
{
	struct op_node node;
	
	if ( op_queue_get(&node) )
	{
			sysprintf("Get Node: %d\n", node.seq);
			sysprintf("\tIcon ID %d\n",node.icon );
		
			sysprintf("\tLength %d\n",node.length );		
		
			if ( ! decode_jpeg((UINT32) node.buffer, (UINT32) decode_buffer))
					return;

			sysprintf("\tRect (%d,%d,%d,%d) \n",node.x, node.y, decode_data.width, decode_data.height);			
//			fb_copy_rect(decode_buffer, decode_data.width, decode_data.height, node.x, node.y);
			
//			fb_fill_rect(node.x, node.y, decode_data.width, decode_data.height, RGB565_YELLOW);
			fb_copy_rect(decode_buffer, decode_data.width, decode_data.height, node.x, node.y);
//			TestCopyRect(0, 0, decode_data.width, decode_data.height);
		
			op_queue_release(&node);
	}
	


}	


