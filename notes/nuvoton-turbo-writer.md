# Nuvoton Turbo Writer
# ISP Introduction
N329/N9H2X boot option
    - Normal mode
    - Recovery mode
ISP (In System Programming) through USB in Recovery mode
## Boot Flow
N9H20
    - Normal mode
    SD card 0 boot -> NAND boot -> SPI boot -> SD card 1 boot -> USB boot
    - Recovery mode
    USB boot
## Turbo Writer Introduction
Files in Turbo Writer
    -ChangeLog.txt
    -TurboWriter.exe
    -NAND ID.ini
    -TurboWriter.ini
    -Turbo Writer Firmware (.bin)
    -Turbo Writer Firmware Version
        N9H20 version
        -Version Data + V1/V3/V5 -> DRAM 2M/8M/32M
    -Turbo Writer Tool verion
## SDRAM

## Preliminary Definition
Image Type
    1. System image, ID = 0x03
        - Required
        - NAND/SD/SPI Loader
    2. Execute image, ID = 0x01
        - Option
    3. Logo image, ID = 0x04
        - Option
    4. Data image, ID = 0x00
        - Option
    5. RomFS, ID = 0x02
        - Option

## Write to NAND Flash
## Write to SD Card
## Write to SPI Flash
## Write to SPI Flash - RAW Data

# Notice
由於 Boot Loader 有幾個來源 (NAND/SD/SPI) 因此需要逐步確認
    
      