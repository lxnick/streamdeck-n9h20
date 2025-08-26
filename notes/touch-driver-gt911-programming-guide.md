# GT911 Programming Guide - GOODIX
Applicable to firmware of version 1040 or later
 
# Description on Interface
GT911 interfaces with the host via 6 pins: VDD, GND, SCL, SDA, INT and RESET
    - INT should be floating at input state
    - RESET pin outputs low for longer 100 us
    - I2C maximum transmission rate 400K bps
    - transmission less than  200K is recommented
    - Select I2C address from  I2C at startup

 # Communication Timings   
 ## Timing for Write Operation
 ## Timing for Read Operation

 # Register Map
 