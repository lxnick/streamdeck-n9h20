#ifndef KEYBOARD_REPORT_LAYOUT_HEADER6
#define KEYBOARD_REPORT_LAYOUT_HEADER6

#include "stdint.h"

#pragma pack (push, 1)

#define MAX_KEY_CODE    (6)
#define MAX_KEY_LED     (1)

#define KMOD_LEFT_CTRL      (1<<0)  //  E0
#define KMOD_LEFT_SHIFT     (1<<1)  //  E1
#define KMOD_LEFT_ALT       (1<<2)  //  E2
#define KMOD_LEFT_GUI       (1<<3)  //  E3
#define KMOD_RIGHT_CTRL     (1<<4)  //  E4
#define KMOD_RIGHT_SHIFT    (1<<5)  //  E5
#define KMOD_RIGHT_ALT      (1<<6)  //  E6
#define KMOD_RIGHT_GUI      (1<<7)  //  E7

#define KLED_NUM_LOCK       (1<<0)
#define KLED_CAP_LOCK       (1<<1)
#define KLED_SCROLL_LOCK    (1<<2)
#define KLED_COMPOSE        (1<<3)
#define KLED_KANA           (1<<4)

struct keyboard_in_report
{
    uint8_t modifier;
    uint8_t reserved;
    uint8_t keycode[6];
}   ;     



#endif
