# Setup Development for Stream Deck
* Setup IDE (Keil-C)
* Setup SDK

# Setup keil-C
Execute  NuMicro ARM9 Device Database for Keil 1.02.exe
## Update Device Database
    -Get device database from Nuvoton release
    -Execute NuMicro ARM9 Device Database for Keil 1.02.exe
## Setup N9H20_HMI_NonOS_BSP_V1.04.000
[HMI NonOS](https://www.nuvoton.com/products/gui-solution/gui-platform/numaker-hmi-n9h20/?group=Software&tab=2)
    -Download N9H20_HMI_NonOS_BSP_V1.04.000
    -Extract zip
    -Open project N9H20_HMI_NonOS_BSP_V1.04.000/BSP/SampleCode/JPEG/KEIL/JPEG.uvproj

## Setup N9H20_emWin_Package
[N9H20-emWin](https://www.nuvoton.com/products/gui-solution/gui-platform/numaker-hmi-n9h20/?group=Software&tab=2)
    -Download en-us--N9H20_emWin_Package_20241111.zip
    -Extract zip
    -File Tree
        en-us--N9H20_emWin_Package_20241111
        ├── N9H20 emWin Quick Start Guide.pdf
        ├── N9H20_Linux_emWin_Package_20241111.tar.gz
        ├── N9H20_emWin_NonOS
        │   ├── GUIDemo
        │   ├── N9H20 emWin AppWizard Quick Start Guide.pdf
        │   ├── SimpleDemo
        │   ├── SimpleDemoAppWizard
        │   └── emWin
        ├── N9H20_emWin_NonOS.zip
        └── README.txt
    -Copy N9H20_emWin_NonOS/* to
### Merge 20_emWin_Package to N9H20_HMI_NonOS
    -Copy en-us--N9H20_emWin_Package_20241111\N9H20_emWin_NonOS\GUIDemo to
        N9H20_HMI_NonOS_BSP_V1.04.000\BSP\SampleCode\emWin\GUIDemo
    -Copy en-us--N9H20_emWin_Package_20241111\N9H20_emWin_NonOS\SimpleDemo to
        N9H20_HMI_NonOS_BSP_V1.04.000\BSP\SampleCode\emWin\SimpleDemo
    -Copy en-us--N9H20_emWin_Package_20241111\N9H20_emWin_NonOS\SimpleDemoAppWizard to
        N9H20_HMI_NonOS_BSP_V1.04.000\BSP\SampleCode\emWin\SimpleDemoAppWizard
    -Copy en-us--N9H20_emWin_Package_20241111\N9H20_emWin_NonOS\emWin to
        \N9H20_HMI_NonOS_BSP_V1.04.000\BSP\ThirdParty\emWin
### Test Sample Project
    -N9H20_HMI_NonOS_BSP_V1.04.000\BSP\SampleCode\emWin\GUIDemo
    -N9H20_HMI_NonOS_BSP_V1.04.000\BSP\SampleCode\emWin\SimpleDemo

## Test  Armani
    -N9H20_Armani_Stream_Deck\SampleCode\USBD\HID_Transfer_EStreamDeck\Armani_StreamDeck_480_272
    -N9H20_Armani_Stream_Deck\SampleCode\USBD\HID_Transfer_EStreamDeck\Armani_StreamDeck_800_480
