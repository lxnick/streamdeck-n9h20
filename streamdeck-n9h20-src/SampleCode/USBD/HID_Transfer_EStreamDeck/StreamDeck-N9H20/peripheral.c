#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "peripheral.h"

//==================================================================================================
void gpio_interrupt_init(void)
{
    gpio_setportdir(GPIO_PORTB, BIT2, 0x0);     /* Set GPB2 input mode */
    gpio_setportpull(GPIO_PORTB, BIT2, BIT2);   /* Set GPB2 pull-high */
    gpio_setsrcgrp(GPIO_PORTB, BIT2, 0x0);      /* set to IRQ_EXTINT0 */
    gpio_setintmode(GPIO_PORTB, BIT2, BIT2, 0); /* Set GPB2 falling edge trigger */

    sysSetLocalInterrupt(ENABLE_IRQ);
}

//==================================================================================================
void EXTINT0_IRQHandler(void)
{
    unsigned int read0;

    outpw(REG_IRQTGSRC0, inpw(REG_IRQTGSRC0) & BIT18); /* Clear GPB2 interrupt flag */
    sysprintf("EXTINT0 INT occurred.\n");

    gpio_readport(GPIO_PORTB, (unsigned short *)&read0);
    sysprintf("GPB2 pin status %d\n", (read0 & BIT2) >> 2);
}
//==================================================================================================
