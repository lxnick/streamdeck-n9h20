# N9H20 Non-OS Library Reference Guide

# 1 ADC Libary
# 2 AVI Library
# 3 BLT Library
# 4 GNAND Library
# 5 GPIO Library
- gpio_open : 
    enables GPIO port A, D, and E
    Open port D
    gpio_open (GPIO_PORTD);
- gpio_configure :
    specified pin as GPIO 
    Configure the pin 0 of port D as GPIO
    gpio_configure (GPIO_PORTD, 0)
- gpio_readport    
    reads back all pin value of a GPIO port
    Read PORT C value
    gpio_readport(GPIO_PORTC, &val);
- gpio_setportdir
    set the pin direction of GPIO port
    Set PORT C pin 1 to output mode, and pin 0 to input mode
    gpio_setportdir (GPIO_PORTC, 0x3, 0x2);
- gpio_setportval        
    set the output value of GPIO port
    Set PORT C pin 1 to output high, and pin 0 to low
    gpio_setportval (GPIO_PORTC, 0x3, 0x2);
- gpio_setportpull
    set the pull up resistor of GPIO port
    Enable PORT C pin 1 pull up resistor, and disable pin 0 pull up resistor
    gpio_setportpull (GPIO_PORTC, 0x3, 0x2);
- gpio_setdebounce
    configure external interrupt de-bounce time
    Set nIRQ0 debounce sampling clock to 128 clocks    
    gpio_setdebounce (128, 1);
- gpio_getdebounce
    gets current external interrupt de-bounce time setting
    gpio_getdebounce (&clk, &src)
- gpio_setsrcgrp
    set external interrupt source group
    Set GPIO port C pin 0 as source of nIRQ3
    gpio_setsrcgrp (GPIO_PORTC, 1, 3);
- gpio_getsrcgrp
    get current external interrupt source setting
    Read GPIO port C interrupt group status 
    gpio_setsrcgrp (GPIO_PORTC, &val);
- gpio_setintmode
    sets the interrupt trigger mode of GPIO port
    Set PORT C pin 0 triggers on both falling and rising edge
    gpio_setintmode (GPIO_PORTC, 1, 1, 1);
- gpio_getintmode
    get interrupt trigger mode of GPIO port
    Get PORT C trigger mode 
    gpio_getintmode (GPIO_PORTC, &falling, &rising);
- gpio_setlatchtrigger
    set latch trigger source
    Enable latch for nIRQ0 and nIRQ3
    gpio_setlatchtrigger (9);
- gpio_getlatchtrigger
    get latch trigger source
    Get latch trigger source
    gpio_getlatchtrigger (&src);
- gpio_getlatchval
    get interrupt latch value
    Get port C latch value     
    gpio_getlatchval (GPIO_PORTC, &val);
- gpio_gettriggersrc
    get interrupt trigger source
    Get port C interrupt trigger source
    gpio_gettriggersrc (GPIO_PORTC, &src);
- gpio_cleartriggersrc    
    clear interrupt trigger source
    Clear port C interrupt trigger source
    gpio_cleartriggersrc (GPIO_PORTC);
#  6 I2C Library    
- i2cInit    
    configures GPIO to I2C mode
    i2cInit();
- i2cOpen   
    initializes the software resource and sets the clock frequency to 100 kHz.
    status = i2cOpen();
- i2cClose
    This function disables I2C engine clock.
    i2cClose();
- i2cRead
    reads data from I2C slave.
    len = i2cRead(buf, 8);
- i2cRead_OV
    reads data from OmniVision sensor
    len = i2cRead_OV(buf, 1); 
- i2cWrite    
    writes data to I2C slave
    len = i2cWrite(buf, 5);
- i2cIoctl    
    programmer configure I2C interface
    - I2C_IOC_SET_DEV_ADDRESS
        set the slave address
    - I2C_IOC_SET_SPEED
        set the clock frequency
    - I2C_IOC_SET_SUB_ADDRESS
        set the sub-address and its length
    i2cIoctl(I2C_IOC_SET_SPEED, 400, 0);
- i2cExit
    does nothing.

