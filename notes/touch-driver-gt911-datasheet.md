# Touch Driver 911 data sheet
5-Point Capacitive Touch Controller for Small-Sized MID

# Features
Built-in capacitive sensing circuit and high-performance MPU
    - Report rate: 100Hz
    - Outputs touch coordinates in real time
    - Unified software applicable to capacitive touch screens of various sizes
    - Single power supply, internal 1.8V LDO
    - Flash embedded; In-system reprogrammable
    - HotKnot integrated
Capacitive touch sensor
    - Channels: 26 Tx × 14 Rx
    - Supports capacitive touch screen sizes: 7” ~ 8”
    - Supports touch key design on FPC
    - Supports ITO glass and ITO film
    - Cover Lens thickness supported
    - Adaptive frequency hopping
    - Supports OGS full lamination
HotKnot
    - Transmission rate：7.0Kbps(max)
    - Data frame maximum capacity：128 bytes
    - Applicable sensor types: OGS/traditional GFF/GG/GF
Environmental adaptability
    - Self-calibration during initialization
    - Automatic drift compensation
    - Operating temperature: -40℃ to +85℃; humidity: ≦95%RH
    - Storage temperature: -60℃ to +125℃; humidity: ≦95%RH
Host interface
    - Standard I2C interface
    - Works in slave mode
    - Supports 1.8V to 3.3V host interface voltage
Response time
    - Green mode: <48ms
    - Sleep mode: <200ms
    - Initialization: <200ms
Tools provided to support application development:
    - Touch panel module parameter detector and auto-generator
    - Touch panel module performance test tool
    - MP Test Tool
    - Reference driver code and documentary guide for host software development
# Block  Diagram
# Pin Configuration
# Sensor Design
## Layout of Rx Channels
## Layout of Tx Channels
## Sensor Design Specifications
## Touch Key Design
# 6.I2C Communication
GT911 supports two I2C slave addresses: 0xBA/0xBB and 0x28/0x29.
addresses were selected by power on timing
## Power-on Timing
## Writing  to GP911
## Reading Data from  GT911

# Description on Functions
## Operating Modes
### Normal Mode
    - minimum coordinate refresh cycle is 7ms-10ms
### Green Mode
    - scanning cycle is about 40ms
### Gesture Mode 
    - Send  0x08 to 0x8046, enable 0x8040
    - Detect mode
        - Swipe 
        - Double-Tap
        - Letter Gesture
    - Pulse out to INT 250 us High        
### Sleep Mode
    - Enter
        Host pull INT low then send I2C command to
    - Wakeup
        Host pull INT high for 2~5 ms
        Host release INT as gloating input
### Approach Mode
### Receive Mode
### Send Mode
## Interrupt Triggering Mechanism
## Sleep Mode
## Stationary Configuration
## Adaptive Frequency Hopping
### Self-calibration during Initialization
### Automatic Drift Compensation
GT911 will update detection reference capacitance within the first 200ms of initialization
