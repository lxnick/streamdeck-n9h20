#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wbtypes.h"
#include "wblib.h"
#include "N9H20_JPEG.h"
#include "Common.h"
#include "HID.h"

UINT32 shift_x, shift_y;

void image_rotate_transfer(UINT16 panel_width, UINT16 panel_height, UINT16 x, UINT16 y, UINT16 width, UINT16 height)
{
    UINT16 i, j;
    UINT16 temp1, temp2;
    UINT32 index1, index2;
    UINT16 *g_p16JpegBuffer = (UINT16 *)g_JpegBuffer;

    // rotate the image 180 degree
    for (i = 0; i < height / 2; i++)
    {
        for (j = 0; j < width; j++)
        {
            index1 = (i * width + j);
            index2 = ((height - 1 - i) * width + (width - 1 - j));

            // Swap the two bytes of the RGB565 pixel
            temp1 = g_p16JpegBuffer[index1];
            g_p16JpegBuffer[index1] = g_p16JpegBuffer[index2];
            g_p16JpegBuffer[index2] = temp1;
        }
    }

    // Copy the image to the frame buffer
    for (i = 0; i < height; i++)
    {
        index1 = width * i * 2;
        index2 = ((y + i) * panel_width + x) * 2;

        memcpy(&g_FrameBuffer[index2], &g_JpegBuffer[index1], width * 2);
    }
}

/*-----------------------------------------------------------------------*/
/*  Header Decode Complete Callback function                             */
/*-----------------------------------------------------------------------*/
BOOL JpegDecHeaderComplete(void)
{
    JPEG_INFO_T jpegInfo;
    UINT32 u32TargetWidth, u32TargetHeight;
    UINT32 u32OutputOffset = 0;
    // UINT32 shift_x, shift_y;

    /* Get the JPEG information(jpeg_width, jpeg_height, and yuvformat are valid) */
    jpegGetInfo(&jpegInfo);

    if (jpegInfo.jpeg_width == 0 || jpegInfo.jpeg_height == 0)
        return FALSE;

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
    shift_x = ((icon_index % ICON_ROW_COUNT) * ICON_SPACE_WIDTH) + 12;
    shift_y = (icon_index / ICON_ROW_COUNT) * ICON_SPACE_HEIGHT + 8;

#ifdef ICON_ROTATE_180
    /* Set Decode Stride to Panel width */
    jpegIoctl(JPEG_IOCTL_SET_DECODE_STRIDE, jpegInfo.jpeg_width, 0);

    /* Change Raw data Output address (Let JPEG engine output data to the center of Panel Buffer) */
    jpegIoctl(JPEG_IOCTL_SET_YADDR, (UINT32)g_JpegBuffer, 0);
#else
    /* Set Decode Stride to Panel width */
    jpegIoctl(JPEG_IOCTL_SET_DECODE_STRIDE, PANEL_WIDTH, 0);

    /* Change Raw data Output address (Let JPEG engine output data to the center of Panel Buffer) */
    u32OutputOffset = (UINT32)(PANEL_WIDTH * shift_y) + (UINT32)shift_x;
    jpegIoctl(JPEG_IOCTL_SET_YADDR, (UINT32)g_FrameBuffer + u32OutputOffset * 2, 0);
#endif

    return TRUE; /* Return TRUE to continue Decode operation, Otherwise, Stop Decode operation */
}
/*-----------------------------------------------------------------------*/
/*  Decode Function                                                      */
/*-----------------------------------------------------------------------*/
INT JpegDec(UINT32 u32SrcAddr, UINT32 u32DestAddr)
{
    /* JPEG Init */
    jpegInit();

    /* Set Bit stream Address */
    jpegIoctl(JPEG_IOCTL_SET_BITSTREAM_ADDR, (UINT32)u32SrcAddr, 0);

    /* Decode mode */
    jpegIoctl(JPEG_IOCTL_SET_DECODE_MODE, JPEG_DEC_PRIMARY_PACKET_RGB565, 0);

    /* Set JPEG Header Decode End Call Back Function */
    jpegIoctl(JPEG_IOCTL_SET_HEADERDECODE_CALBACKFUN, (UINT32)JpegDecHeaderComplete, 0);

    /* Trigger JPEG decoder */
    jpegIoctl(JPEG_IOCTL_DECODE_TRIGGER, 0, 0);

    /* Wait for complete */
    //    if(!jpegWait())
    //    {
    //        sysprintf("JPEG Decode Error!!\n");
    //        return 1;
    //    }

#ifdef ICON_ROTATE_180
    jpegWait();
    image_rotate_transfer(PANEL_WIDTH, PANEL_HEIGHT, shift_x, shift_y, 72, 72);
#endif

    return 0;
}
