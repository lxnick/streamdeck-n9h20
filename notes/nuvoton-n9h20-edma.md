# Nuvoton N9H20 EDMA
    Channel 0：
        VDMA (Video DMA) → Memory-to-Memory 
    Channel 1~4：
        PDMA (Peripheral DMA) → Peripheral-to-Memory,Memory-to-Peripheral 

# VDMA 
    -Memory to Memory DMA
    -Support Color space transform on the fly
## VDMA Flow
    1. Enable DMA
    2. Config source/dest address
    3. Congig transfer count
    4. Trigger transfer

# PDMA
    -SDRAM and APB peripheral (UART0/1,SPI Master 0/1, ADC) from/to
## PDMA Flow
    1. Select Mode 
        MODE_SEL = 0 : SDRAM->APB
        MODE_SEL = 1 : APD->SDRAM
    2. Config address
        APB : Fixed Address
        SDRAM : Increment       
    3. Enable APB peripheral PDMA

# Scatter Gather Transfer
    -Transfer multiple memory block
## Scatter Gather Descriptor
    1. Physical Source Address (32 bits)
    2. Physical Destination Address (32 bits)
    3. Byte Count (17 bits)
    4. Stride Transfer Byte Count (15 bits)
    5. EOT (End of Table, 1 bit)
    6. Source Offset Byte Length (15 bits)
    7. Destination Offset Byte Length (15 bits)
    8. Order (1 bit)
    9. Next Scatter Gather Table Address (32 bits)

# APIs Specification Functions
    -EDMA_Init
    -EDMA_Exit
    -EDMA_Enable
    -EDMA_Disable
    -EDMA_Request
    -VDMA_FindandRequest
    -PDMA_FindandRequest
    -EDMA_SetupHandlers
    -EDMA_SetupSingle
    -EDMA_Free
    -EDMA_SetupSG
    -EDMA_FreeSG
    -EDMA_SetupCST
    -EDMA_ClearCST
    -EDMA_Trigger
    -EDMA_TriggerDone
    -EDMA_IsBusy
    -EDMA_SetAPB
    -EDMA_SetWrapINTType
    -EDMA_SetDirection
    