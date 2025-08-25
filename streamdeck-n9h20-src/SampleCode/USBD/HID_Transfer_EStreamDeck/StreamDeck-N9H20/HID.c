#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wblib.h"
#include "N9H20_reg.h"
#include "N9H20_USBD.h"
#include "HID.h"
#include "Common.h"
#include "N9H20.h"

#include "develop/develop_osd.h"
#include "develop/stream_command.h"

// #define __FORCE_FULLSPEED__

/* Mass_Storage command base address */
extern volatile USBD_INFO_T usbdInfo;

/* USB Device Property */
extern USB_CMD_T _usb_cmd_pkt;

UINT32 g_u32EPA_MXP, g_u32EPB_MXP;
UINT32 volatile u32Ready = 0;
UINT32 volatile g_u32ReadWriteSize;
unsigned int g_Version = 0x20230112;
unsigned int g_Status = 0xA5;
unsigned int g_Parameter[50][50] = {0};

#if defined(__GNUC__)
UINT8 image_buffer[512] __attribute__((aligned(32)));
#else
UINT8 __align(32) image_buffer[512];
#endif

unsigned char *imagebuf;

// Eden
__align(4) UINT8 usb_out_buf[1024];
__align(4) UINT8 jpeg_buf[ICON_BUFFER_NUM][ICON_BUFFER_SIZE]; // icons * 12k jpeg buf
INT32 jpeg_data_idx = 0;
UINT8 jpeg_store_idx = 0;
UINT8 jpeg_decode_idx = 0;
UINT8 icon_index = 0;
UINT8 send_usb_in_report = 0;

UINT8 *g_usb_out_buf;
UINT8(*g_jpeg_buf)
[ICON_BUFFER_SIZE];

const UINT8 reportin_buf_05[] = {0x05, 0x0C, 0xF4, 0x5F, 0xED, 0xA6, 0x31, 0x2E, 0x30, 0x31, 0x2E, 0x30, 0x30, 0x30, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const UINT8 reportin_buf_06[] = {0x06, 0x0C, 0x44, 0x4C, 0x35, 0x32, 0x4C, 0x32, 0x41, 0x33, 0x35, 0x37, 0x33, 0x39, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/***************************************************************/
#define HID_CMD_SIGNATURE 0x43444948

/* HID Transfer Commands */
#define HID_CMD_NONE 0x00
#define HID_CMD_GET_VER 0xD3
#define HID_CMD_GET_STS 0xD4
#define HID_CMD_IMAGE_WRITE 0xC4
#define HID_CMD_SET_PARAM 0xC5
#define HID_CMD_GET_PARAM 0xD6
#define HID_CMD_IMAGE_READ 0xD7
#define HID_CMD_EXIT 0xB1
#define HID_CMD_TEST 0xB4

#ifdef __GNUC__

typedef struct __attribute__((__packed__))
{
    UINT8 u8Cmd;
    UINT8 u8Size;
    UINT32 u32Arg1;
    UINT32 u32Arg2;
    UINT32 u32Signature;
    UINT32 u32Checksum;
} CMD_T;

CMD_T gCmd __attribute__((aligned(4)));
static UINT8 gu8PageBuff[EPA_MAX_PKT_SIZE] __attribute__((aligned(4))) = {0}; /* Page buffer to upload/download through HID report */
static UINT8 gu8JPEGBuff[0x100000] __attribute__((aligned(4))) = {0};         /* JPEG buffer to download through HID report */
static UINT8 gTemp[EPA_MAX_PKT_SIZE] __attribute__((aligned(4)));             /* Page buffer to upload/download through HID report */
#else
typedef __packed struct
{
    UINT8 u8Cmd;
    UINT8 u8Size;
    UINT32 u32Arg1;
    UINT32 u32Arg2;
    UINT32 u32Signature;
    UINT32 u32Checksum;
} CMD_T;

__align(4) CMD_T gCmd;
__align(4) static UINT8 gu8PageBuff[EPA_MAX_PKT_SIZE] = {0}; /* Page buffer to upload/download through HID report */
__align(4) static UINT8 gu8JPEGBuff[0x100000] = {0};         /* JPEG buffer to download through HID report */
__align(4) static UINT8 gTemp[EPA_MAX_PKT_SIZE];             /* Page buffer to upload/download through HID report */
#endif

//static UINT32 g_u32BytesInJPEGBuf = 0; /* The bytes of data in g_u8JPEGBuff */

CMD_T *pCmd;
UINT8 *g_u8PageBuff, *g_u8TestPages, *g_Temp, *g_u8JPEGBuff;
INT32 gi32CmdTestCnt = 0;

#if defined(__GNUC__)
UINT8 HID_DeviceReportDescriptor[] __attribute__((aligned(4))) =
#else
__align(4) UINT8 HID_DeviceReportDescriptor[] =
#endif
    {
        0x05, 0x0C,       /* Usage Page (Consumer Devices) */
        0x09, 0x01,       /* Usage (Consumer Control) */
        0xA1, 0x01,       /* Collection (Application) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0xFF, 0x03, /* Report Count (1023) */
        0x85, 0x02,       /* Report ID (2) */
        0x91, 0x02,       /* Output (Data, Var, Abs) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0xFF, 0x01, /* Report Count (511) */
        0x85, 0x01,       /* Report ID (1) */
        0x81, 0x02,       /* Input (Data, Var, Abs) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x03,       /* Report ID (3) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x07,       /* Report ID (7) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x05,       /* Report ID (5) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x04,       /* Report ID (4) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x06,       /* Report ID (6) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x08,       /* Report ID (8) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x09,       /* Report ID (9) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x0A,       /* Report ID (10) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x0B,       /* Report ID (11) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x0C,       /* Report ID (12) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0xC0              /* End Collection */
};

#if defined(__GNUC__)
UINT8 HID_DeviceReportDescriptor_FS[] __attribute__((aligned(4))) =
#else
__align(4) UINT8 HID_DeviceReportDescriptor_FS[] =
#endif
    {
        0x05, 0x0C,       /* Usage Page (Consumer Devices) */
        0x09, 0x01,       /* Usage (Consumer Control) */
        0xA1, 0x01,       /* Collection (Application) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x3F, 0x00, /* Report Count (63) */
        0x85, 0x02,       /* Report ID (2) */
        0x91, 0x02,       /* Output (Data, Var, Abs) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x3F, 0x00, /* Report Count (63) */
        0x85, 0x01,       /* Report ID (1) */
        0x81, 0x02,       /* Input (Data, Var, Abs) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x03,       /* Report ID (3) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x07,       /* Report ID (7) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x05,       /* Report ID (5) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x04,       /* Report ID (4) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x06,       /* Report ID (6) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x08,       /* Report ID (8) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x09,       /* Report ID (9) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x0A,       /* Report ID (10) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x0B,       /* Report ID (11) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0x0A, 0x00, 0xFF, /* Usage (Vendor Defined) */
        0x15, 0x00,       /* Logical Minimum (0) */
        0x26, 0xFF, 0x00, /* Logical Maximum (255) */
        0x75, 0x08,       /* Report Size (8 bits) */
        0x96, 0x1F, 0x00, /* Report Count (31) */
        0x85, 0x0C,       /* Report ID (12) */
        0xB1, 0x04,       /* Feature (Data, Var, Abs, Buffered Bytes) */
        0xC0              /* End Collection */
};

/* HID Descriptor */
#if defined(__GNUC__)
UINT8 gu8DeviceDescriptor[] __attribute__((aligned(4))) =
#else
__align(4) UINT8 gu8DeviceDescriptor[] =
#endif
    {
        LEN_DEVICE,       /* bLength */
        DESC_DEVICE,      /* bDescriptorType */
        0x00, 0x02,       /* bcdUSB */
        0x00,             /* bDeviceClass */
        0x00,             /* bDeviceSubClass */
        0x00,             /* bDeviceProtocol */
        CEP_MAX_PKT_SIZE, /* bMaxPacketSize0 */
        /* idVendor */
        USB_VID & 0x00FF,
        ((USB_VID & 0xFF00) >> 8),
        /* idProduct */
        USB_PID & 0x00FF,
        ((USB_PID & 0xFF00) >> 8),
        0x20, 0x04, /* bcdDevice */
        0x01,       /* iManufacture */
        0x02,       /* iProduct */
        0x03,       /* iSerialNumber - no serial */
        0x01        /* bNumConfigurations */
};

#if defined(__GNUC__)
static UINT8 HID_ConfigurationBlock[] __attribute__((aligned(4))) =
#else
__align(4) static UINT8 HID_ConfigurationBlock[] =
#endif
    {
        LEN_CONFIG,  /* bLength */
        DESC_CONFIG, /* bDescriptorType */
        /* wTotalLength */
        (LEN_CONFIG + LEN_INTERFACE + LEN_HID + LEN_ENDPOINT * 2) & 0x00FF,
        (((LEN_CONFIG + LEN_INTERFACE + LEN_HID + LEN_ENDPOINT * 2) & 0xFF00) >> 8),
        0x01,                                                                  /* bNumInterfaces */
        0x01,                                                                  /* bConfigurationValue */
        0x00,                                                                  /* iConfiguration */
        0xA0, /*0x80 | (USBD_SELF_POWERED << 6) | (USBD_REMOTE_WAKEUP << 5),*/ /* bmAttributes */
        USBD_MAX_POWER,                                                        /* MaxPower */

        /* I/F descr: HID */
        LEN_INTERFACE,  /* bLength */
        DESC_INTERFACE, /* bDescriptorType */
        0x00,           /* bInterfaceNumber */
        0x00,           /* bAlternateSetting */
        0x02,           /* bNumEndpoints */
        0x03,           /* bInterfaceClass */
        0x00,           /* bInterfaceSubClass */
        0x00,           /* bInterfaceProtocol */
        0x00,           /* iInterface */

        /* HID Descriptor */
        LEN_HID,      /* Size of this descriptor in UINT8s. */
        DESC_HID,     /* HID descriptor type. */
        0x10, 0x01,   /* HID Class Spec. release number. */
        0x00,         /* H/W target country. */
        0x01,         /* Number of HID class descriptors to follow. */
        DESC_HID_RPT, /* Descriptor type. */
        /* Total length of report descriptor. */
        sizeof(HID_DeviceReportDescriptor) & 0x00FF,
        ((sizeof(HID_DeviceReportDescriptor) & 0xFF00) >> 8),

        /* EP Descriptor: interrupt in. */
        LEN_ENDPOINT,               /* bLength */
        DESC_ENDPOINT,              /* bDescriptorType */
        (INT_IN_EP_NUM | EP_INPUT), /* bEndpointAddress */
        EP_INT,                     /* bmAttributes */
        /* wMaxPacketSize */
        EPA_MAX_PKT_SIZE & 0x00FF,
        ((EPA_MAX_PKT_SIZE & 0xFF00) >> 8),
        HID_DEFAULT_INT_IN_INTERVAL, /* bInterval */

        /* EP Descriptor: interrupt out. */
        LEN_ENDPOINT,                 /* bLength */
        DESC_ENDPOINT,                /* bDescriptorType */
        (INT_OUT_EP_NUM | EP_OUTPUT), /* bEndpointAddress */
        EP_INT,                       /* bmAttributes */
        /* wMaxPacketSize */
        EPB_MAX_PKT_SIZE & 0x00FF,
        ((EPB_MAX_PKT_SIZE & 0xFF00) >> 8),
        HID_DEFAULT_INT_IN_INTERVAL /* bInterval */
};

#if defined(__GNUC__)
static UINT8 HID_ConfigurationBlock_FS[] __attribute__((aligned(4))) =
#else
__align(4) static UINT8 HID_ConfigurationBlock_FS[] =
#endif
    {
        LEN_CONFIG,  /* bLength */
        DESC_CONFIG, /* bDescriptorType */
        /* wTotalLength */
        (LEN_CONFIG + LEN_INTERFACE + LEN_HID + LEN_ENDPOINT * 2) & 0x00FF,
        (((LEN_CONFIG + LEN_INTERFACE + LEN_HID + LEN_ENDPOINT * 2) & 0xFF00) >> 8),
        0x01,                                                                  /* bNumInterfaces */
        0x01,                                                                  /* bConfigurationValue */
        0x00,                                                                  /* iConfiguration */
        0xA0, /*0x80 | (USBD_SELF_POWERED << 6) | (USBD_REMOTE_WAKEUP << 5),*/ /* bmAttributes */
        USBD_MAX_POWER,                                                        /* MaxPower */

        /* I/F descr: HID */
        LEN_INTERFACE,  /* bLength */
        DESC_INTERFACE, /* bDescriptorType */
        0x00,           /* bInterfaceNumber */
        0x00,           /* bAlternateSetting */
        0x02,           /* bNumEndpoints */
        0x03,           /* bInterfaceClass */
        0x00,           /* bInterfaceSubClass */
        0x00,           /* bInterfaceProtocol */
        0x00,           /* iInterface */

        /* HID Descriptor */
        LEN_HID,      /* Size of this descriptor in UINT8s. */
        DESC_HID,     /* HID descriptor type. */
        0x10, 0x01,   /* HID Class Spec. release number. */
        0x00,         /* H/W target country. */
        0x01,         /* Number of HID class descriptors to follow. */
        DESC_HID_RPT, /* Descriptor type. */
        /* Total length of report descriptor. */
        sizeof(HID_DeviceReportDescriptor) & 0x00FF,
        ((sizeof(HID_DeviceReportDescriptor) & 0xFF00) >> 8),

        /* EP Descriptor: interrupt in. */
        LEN_ENDPOINT,               /* bLength */
        DESC_ENDPOINT,              /* bDescriptorType */
        (INT_IN_EP_NUM | EP_INPUT), /* bEndpointAddress */
        EP_INT,                     /* bmAttributes */
        /* wMaxPacketSize */
        EPA_OTHER_MAX_PKT_SIZE & 0x00FF,
        ((EPA_OTHER_MAX_PKT_SIZE & 0xFF00) >> 8),
        HID_DEFAULT_INT_IN_INTERVAL, /* bInterval */

        /* EP Descriptor: interrupt out. */
        LEN_ENDPOINT,                 /* bLength */
        DESC_ENDPOINT,                /* bDescriptorType */
        (INT_OUT_EP_NUM | EP_OUTPUT), /* bEndpointAddress */
        EP_INT,                       /* bmAttributes */
        /* wMaxPacketSize */
        EPB_OTHER_MAX_PKT_SIZE & 0x00FF,
        ((EPB_OTHER_MAX_PKT_SIZE & 0xFF00) >> 8),
        HID_DEFAULT_INT_IN_INTERVAL /* bInterval */
};

/* Identifier Language */
#if defined(__GNUC__)
static UINT8 HID_StringDescriptor0[4] __attribute__((aligned(4))) =
#else
__align(4) static UINT8 HID_StringDescriptor0[4] =
#endif
    {
        4,             /* bLength */
        USB_DT_STRING, /* bDescriptorType */
        0x09, 0x04};

/* iManufacturer */
#if defined(__GNUC__)
UINT8 HID_StringDescriptor1[] __attribute__((aligned(4))) =
#else
__align(4) UINT8 HID_StringDescriptor1[] =
#endif
    {
        0x0E, /* bLength */
        0x03, /* bDescriptorType */
        'E', 0, 'l', 0, 'g', 0, 'a', 0, 't', 0, 'o', 0};

/* iProduct */
#if defined(__GNUC__)
UINT8 HID_StringDescriptor2[] __attribute__((aligned(4))) =
#else
__align(4) UINT8 HID_StringDescriptor2[] =
#endif
    {
        0x22, /* bLength */
        0x03, /* bDescriptorType */
        'S', 0, 't', 0, 'r', 0, 'e', 0, 'a', 0, 'm', 0, ' ', 0,
        'D', 0, 'e', 0, 'c', 0, 'k', 0, ' ', 0, 'M', 0, 'K', 0, '.', 0, '2', 0};

/* iSerialNumber */
#if defined(__GNUC__)
UINT8 HID_StringDescriptor3[] __attribute__((aligned(4))) =
#else
__align(4) UINT8 HID_StringDescriptor3[] =
#endif
    {
        0x1A, /* bLength */
        0x03, /* bDescriptorType */
        'D', 0, 'L', 0, '5', 0, '2', 0, 'L', 0, '2', 0, 'A', 0, '3', 0, '5', 0, '7', 0, '3', 0, '9', 0};

//==================================================================================================
/* HID High Speed Init */
void hidHighSpeedInit(void)
{
    usbdInfo.usbdMaxPacketSize = 0x40;
    outp32(EPA_MPS, 0x40); /* mps */
    while (inp32(EPA_MPS) != 0x40)
        ; /* mps */

    /* Interrupt in */
    outp32(EPA_IRQ_ENB, 0x00000008);   /* tx transmitted */
    outp32(EPA_RSP_SC, 0x000000000);   /* auto validation */
    outp32(EPA_MPS, EPA_MAX_PKT_SIZE); /* mps 512 */
    outp32(EPA_CFG, 0x0000001D);       /* Interrupt in ep no 1 */
    outp32(EPA_START_ADDR, EPA_BASE);
    outp32(EPA_END_ADDR, EPA_BASE + EPA_MAX_PKT_SIZE - 1);

    /* Interrupt out */
    outp32(EPB_IRQ_ENB, 0x00000010);   /* data pkt received */
    outp32(EPB_RSP_SC, 0x00000000);    /* auto validation */
    outp32(EPB_MPS, EPB_MAX_PKT_SIZE); /* mps 512 */
    outp32(EPB_CFG, 0x00000025);       /* Interrupt out ep no 2 */
    outp32(EPB_START_ADDR, EPB_BASE);
    outp32(EPB_END_ADDR, EPB_BASE + EPB_MAX_PKT_SIZE - 1);

    g_u32EPA_MXP = EPA_MAX_PKT_SIZE;
    g_u32EPB_MXP = EPB_MAX_PKT_SIZE;

    g_u32ReadWriteSize = EPA_MAX_PKT_SIZE;

    usbdInfo.pu32HIDRPTDescriptor[0] = (PUINT32)&HID_DeviceReportDescriptor;
    usbdInfo.u32HIDRPTDescriptorLen[0] = sizeof(HID_DeviceReportDescriptor);
}

//==================================================================================================
/* HID Full Speed Init */
void hidFullSpeedInit(void)
{
    usbdInfo.usbdMaxPacketSize = 0x40;
    outp32(EPA_MPS, 0x40); /* mps */
    while (inp32(EPA_MPS) != 0x40)
        ; /* mps */

    /* Interrupt in */
    outp32(EPA_IRQ_ENB, DATA_TxED_IE);       /* tx transmitted */
    outp32(EPA_RSP_SC, 0x000000000);         /* auto validation */
    outp32(EPA_MPS, EPA_OTHER_MAX_PKT_SIZE); /* mps 64 */
    outp32(EPA_CFG, 0x0000001D);             /* Interrupt in ep no 1 */
    outp32(EPA_START_ADDR, EPA_OTHER_BASE);
    outp32(EPA_END_ADDR, EPA_OTHER_BASE + EPA_OTHER_MAX_PKT_SIZE - 1);

    /* Interrupt out */
    outp32(EPB_IRQ_ENB, 0x00000010);         /* data pkt received */
    outp32(EPB_RSP_SC, 0x00000000);          /* auto validation */
    outp32(EPB_MPS, EPB_OTHER_MAX_PKT_SIZE); /* mps 64 */
    outp32(EPB_CFG, 0x00000025);             /* Interrupt out ep no 2 */
    outp32(EPB_START_ADDR, EPB_OTHER_BASE);
    outp32(EPB_END_ADDR, EPB_OTHER_BASE + EPB_OTHER_MAX_PKT_SIZE - 1);

    g_u32EPA_MXP = EPA_OTHER_MAX_PKT_SIZE;
    g_u32EPB_MXP = EPB_OTHER_MAX_PKT_SIZE;

#ifdef __FORCE_FULLSPEED__
    outp32(OPER, 0);
#endif
    usbdInfo.pu32HIDRPTDescriptor[0] = (PUINT32)&HID_DeviceReportDescriptor_FS;
    usbdInfo.u32HIDRPTDescriptorLen[0] = sizeof(HID_DeviceReportDescriptor_FS);
}

//==================================================================================================
void hidClassOUT(void)
{
    if (_usb_cmd_pkt.bRequest == HID_SET_IDLE)
    {
        //        sysprintf("\rSet IDLE\n");
    }
    else if (_usb_cmd_pkt.bRequest == HID_SET_REPORT)
    {
        u32Ready = 1;
        sysprintf("\rSET_REPORT 0x%X\n", inp8(CEP_DATA_BUF));
    }
}

//==================================================================================================
void hidClassIN(void)
{
    const unsigned char *buf_p;
    int class_in_len = 0;
    int i = 0;

    // get feature report
    if ((_usb_cmd_pkt.bmRequestType == 0xA1) || (_usb_cmd_pkt.bRequest == 0x01))
    {
        if (_usb_cmd_pkt.wValue == 0x0305)
        {
            buf_p = reportin_buf_05;
            class_in_len = sizeof(reportin_buf_05);
        }
        else if (_usb_cmd_pkt.wValue == 0x0306)
        {
            buf_p = reportin_buf_06;
            class_in_len = sizeof(reportin_buf_06);
        }

        for (i = 0; i < class_in_len; i++)
            outp8(CEP_DATA_BUF, buf_p[i]);
    }

    outp32(IN_TRNSFR_CNT, class_in_len);
}

//==================================================================================================
/* send key_press command by usb in report */
void HID_SetInReport(void)
{
    while (inp32(DMA_CTRL_STS) & DMA_EN)
        ; /* Wait DMA Ready */

    /* Prepare the data for next HID IN transfer */
    outp32(DMA_CTRL_STS, 0x11);                         /* bulk in, dma read, ep1 */
    outp32(AHB_DMA_ADDR, (UINT32)&g_u8PageBuff[0]);     /*Address for DMA */
    outp32(DMA_CNT, EPA_MAX_PKT_SIZE);                  /* DMA length */
    outp32(DMA_CTRL_STS, inp32(DMA_CTRL_STS) | DMA_EN); /* Trigger DMA */

    while (inp32(DMA_CTRL_STS) & DMA_EN)
        ; /* Wait DMA Complete */
}

//==================================================================================================
void key_press(void)
{
    static UINT32 key_scan_interval = 0;
    UINT32 key;
    static UINT32 key_last = 0;

    if (key_scan_interval++ < 10000)
        return;

    key_scan_interval = 0;

    key = kpi_read(KPI_NONBLOCK);
    if (key != key_last)
    {
        memset(&g_u8PageBuff[4], 0, ICON_NUM);
        switch (key)
        {
        case 1:
            g_u8PageBuff[4] = 0x01;
            vpostSetFrameBuffer((UINT32)g_FrameBuffer_test);
            break;
        case 2:
            g_u8PageBuff[5] = 0x01;
            vpostSetFrameBuffer((UINT32)g_FrameBuffer);
            break;
        case 4:
            g_u8PageBuff[6] = 0x01;
            break;
        case 8:
            g_u8PageBuff[7] = 0x01;
            break;
        case 32:
            g_u8PageBuff[8] = 0x01;
            break;
        case 16:
            memset(&g_u8PageBuff[4], 0x01, ICON_NUM); // Press all keys
            break;
        }
        key_last = key;
        // send_usb_in_report = 1;
        HID_SetInReport();
    }
}

//==================================================================================================
/* Interrupt IN handler */
void EPA_Handler(UINT32 u32IntEn, UINT32 u32IntStatus)
{
}

//==================================================================================================
/* Interrupt OUT handler */
void EPB_Handler(UINT32 u32IntEn, UINT32 u32IntStatus)
{
    if (u32IntStatus & (DATA_RxED_IS | SHORT_PKT_IS))
    {
        UINT32 len;
//        UINT32 jpeg_len;

        len = inp32(EPB_DATA_CNT) & 0xffff; /* Get data from Endpoint FIFO */
        // sysprintf("len: %d\n", len);

        while (inp32(DMA_CTRL_STS) & DMA_EN)
            ;
        /* Process the data phase of write command */
        outp32(DMA_CTRL_STS, 0x02); /* bulk out, dma write, ep2 */
        outp32(AHB_DMA_ADDR, (UINT32)g_usb_out_buf);
        outp32(DMA_CNT, len);
        outp32(DMA_CTRL_STS, inp32(DMA_CTRL_STS) | DMA_EN);
        while (inp32(DMA_CTRL_STS) & DMA_EN)
            ;
				
				on_receive_data(g_usb_out_buf, 1024);
				
	
    }
}

void  HID_SendInputReport(uint8_t* data, int count)
{
		memset( &g_u8PageBuff[0], 0, EPA_MAX_PKT_SIZE);
		memcpy( &g_u8PageBuff[0], data, count );

    while (inp32(DMA_CTRL_STS) & DMA_EN)
        ; /* Wait DMA Ready */

    /* Prepare the data for next HID IN transfer */
    outp32(DMA_CTRL_STS, 0x11);                         /* bulk in, dma read, ep1 */
    outp32(AHB_DMA_ADDR, (UINT32)&g_u8PageBuff[0]);     /*Address for DMA */
    outp32(DMA_CNT, EPA_MAX_PKT_SIZE);                  /* DMA length */
    outp32(DMA_CTRL_STS, inp32(DMA_CTRL_STS) | DMA_EN); /* Trigger DMA */

    while (inp32(DMA_CTRL_STS) & DMA_EN)
        ; /* Wait DMA Complete */
}


//==================================================================================================
void draw_icon(void)
{
    if (jpeg_decode_idx != jpeg_store_idx)
    {
        icon_index = g_jpeg_buf[jpeg_decode_idx][0];
        if (icon_index < ICON_NUM)
        {
#ifdef ICON_ROTATE_180
            JpegDec((UINT32)&g_jpeg_buf[jpeg_decode_idx][1], (UINT32)g_JpegBuffer);
#else
            JpegDec((UINT32)&g_jpeg_buf[jpeg_decode_idx][1], (UINT32)g_FrameBuffer);
#endif
        }

        jpeg_decode_idx++;
        if (jpeg_decode_idx >= ICON_BUFFER_NUM)
            jpeg_decode_idx = 0;
    }
}

//==================================================================================================
/* Reset handler */
void Reset_Handler(VOID)
{
    // isConnectToHost = 1;
}

//==================================================================================================
/* HID Init */
void hidInit(void)
{
    /* Set Endpoint map */
    usbdInfo.i32EPA_Num = INT_IN_EP_NUM;  /* Endpoint 1 */
    usbdInfo.i32EPB_Num = INT_OUT_EP_NUM; /* Endpoint 2 */
    usbdInfo.i32EPC_Num = -1;             /* Not use */
    usbdInfo.i32EPD_Num = -1;             /* Not use */

    /* Set Callback Function */
    /* Set MSC initialize function */
    usbdInfo.pfnFullSpeedInit = hidFullSpeedInit;
    usbdInfo.pfnHighSpeedInit = hidHighSpeedInit;

    /* Set Descriptor pointer */
    usbdInfo.pu32DevDescriptor = (PUINT32)&gu8DeviceDescriptor;
    usbdInfo.pu32HSConfDescriptor = (PUINT32)&HID_ConfigurationBlock;
    usbdInfo.pu32FSConfDescriptor = (PUINT32)&HID_ConfigurationBlock_FS;
    usbdInfo.pu32HIDDescriptor = (PUINT32)((UINT32)&HID_ConfigurationBlock + LEN_CONFIG + LEN_INTERFACE);

    usbdInfo.pfnClassDataOUTCallBack = hidClassOUT;
    usbdInfo.pfnClassDataINCallBack = hidClassIN; // Eden

    /* Set the HID report descriptor */
    usbdInfo.pu32HIDRPTDescriptor[0] = (PUINT32)&HID_DeviceReportDescriptor;
    usbdInfo.u32HIDRPTDescriptorLen[0] = sizeof(HID_DeviceReportDescriptor);
    usbdInfo.pfnEPACallBack = EPA_Handler;
    usbdInfo.pfnEPBCallBack = EPB_Handler;
    usbdInfo.pfnReset = Reset_Handler;

    usbdInfo.pu32StringDescriptor[0] = (PUINT32)&HID_StringDescriptor0;
    usbdInfo.pu32StringDescriptor[1] = (PUINT32)&HID_StringDescriptor1;
    usbdInfo.pu32StringDescriptor[2] = (PUINT32)&HID_StringDescriptor2;
    usbdInfo.pu32StringDescriptor[3] = (PUINT32)&HID_StringDescriptor3;

    /* Set Descriptor length */
    usbdInfo.u32DevDescriptorLen = LEN_DEVICE;
    usbdInfo.u32HSConfDescriptorLen = LEN_CONFIG_AND_SUBORDINATE;
    usbdInfo.u32FSConfDescriptorLen = LEN_CONFIG_AND_SUBORDINATE;
    usbdInfo.u32StringDescriptorLen[0] = HID_StringDescriptor0[0] = sizeof(HID_StringDescriptor0);
    usbdInfo.u32StringDescriptorLen[1] = HID_StringDescriptor1[0] = sizeof(HID_StringDescriptor1);
    usbdInfo.u32StringDescriptorLen[2] = HID_StringDescriptor2[0] = sizeof(HID_StringDescriptor2);
    usbdInfo.u32StringDescriptorLen[3] = HID_StringDescriptor3[0] = sizeof(HID_StringDescriptor3);

    pCmd = (CMD_T *)((UINT32)&gCmd | BIT31);
    g_u8PageBuff = (UINT8 *)((UINT32)gu8PageBuff | BIT31);
    g_u8JPEGBuff = (UINT8 *)((UINT32)gu8JPEGBuff | BIT31);
    g_Temp = (UINT8 *)((UINT32)gTemp | BIT31);

    // Eden
    g_usb_out_buf = (UINT8 *)((UINT32)usb_out_buf | BIT31);
    g_jpeg_buf = (UINT8(*)[ICON_BUFFER_SIZE])((UINT32)jpeg_buf | BIT31);

    // usb_class_in buffer header initial
    g_u8PageBuff[0] = 0x01; // report id
    g_u8PageBuff[1] = 0x00; // command
    g_u8PageBuff[2] = 0x0F; // icon number for Elgato Stream Deck MK.2 must be 0x0F
    g_u8PageBuff[3] = 0x00; // icon number for Elgato Stream Deck MK.2 must be 0x00
}

//==================================================================================================
void HIDStart(void)
{
    /* Enable USB */
    udcOpen();
    hidInit();
    udcInit();
}

//==================================================================================================
