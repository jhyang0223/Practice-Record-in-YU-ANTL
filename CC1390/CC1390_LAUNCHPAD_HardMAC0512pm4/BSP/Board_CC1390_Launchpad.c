/*
 * Board_Launchpad.c
 *
 *  Created on: 2018. 2. 15.
 *      Author: Hello_DELL
 */

#include <BSP/Board_CC1390_Launchpad.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOCC26XX.h>

GPIO_PinConfig gpioPinConfigs[] = {
    // LED Pins
    BOARD_GPIO_LED0 | GPIO_DO_NOT_CONFIG,  /* Green LED */
    BOARD_GPIO_LED1 | GPIO_DO_NOT_CONFIG,  /* Red LED */
    BOARD_SPI_CTL_IF_INT | GPIO_DO_NOT_CONFIG,
    BOARD_RANGE_EXTENDER_HGM | GPIO_DO_NOT_CONFIG,
    BOARD_GPIO_DEBUG_PIN0 | GPIO_DO_NOT_CONFIG,
    BOARD_GPIO_DEBUG_PIN1 | GPIO_DO_NOT_CONFIG,
};

GPIO_CallbackFxn gpioCallbackFunctions[] = {
};

const GPIOCC26XX_Config GPIOCC26XX_config = {
    .pinConfigs = (GPIO_PinConfig *)gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *)gpioCallbackFunctions,
    .numberOfPinConfigs = Board_GPIO_GPIOCOUNT,
    .numberOfCallbacks  = sizeof(gpioCallbackFunctions)/sizeof(GPIO_CallbackFxn),
    .intPriority = (~0)
};

/*
 *  =============================== PIN ===============================
 */
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>

const PIN_Config BoardGpioInitTable[] = {
    BOARD_GPIO_LED0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    BOARD_GPIO_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    BOARD_UART_DEBUG_RX | PIN_INPUT_EN | PIN_PULLDOWN,
    BOARD_UART_DEBUG_TX | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL,
    BOARD_SPI_CTL_IF_MOSI | PIN_INPUT_EN | PIN_PULLDOWN,
    BOARD_SPI_CTL_IF_MISO | PIN_INPUT_EN | PIN_PULLDOWN,
    BOARD_SPI_CTL_IF_CLK | PIN_INPUT_EN | PIN_PULLDOWN,
    BOARD_SPI_CTL_IF_CS | PIN_INPUT_EN,
    BOARD_SPI_CTL_IF_INT | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MIN,
    BOARD_RANGE_EXTENDER_HGM | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MIN,
    BOARD_GPIO_DEBUG_PIN0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    BOARD_GPIO_DEBUG_PIN1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

const PINCC26XX_HWAttrs PINCC26XX_hwAttrs = {
    .intPriority = ~0,
    .swiPriority = 0
};

/*
 *  =============================== Power ===============================
 */
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

const PowerCC26XX_Config PowerCC26XX_config = {
    .policyInitFxn      = NULL,
    .policyFxn          = &PowerCC26XX_standbyPolicy,
    .calibrateFxn       = &PowerCC26XX_calibrate,
    .enablePolicy       = true,
    .calibrateRCOSC_LF  = true,
    .calibrateRCOSC_HF  = true,
};

/*
 *  =============================== RF Driver ===============================
 */
#include <ti/drivers/rf/RF.h>

const RFCC26XX_HWAttrs RFCC26XX_hwAttrs = {
    .hwiCpe0Priority = ~0,
    .hwiHwPriority   = ~0,
    .swiCpe0Priority =  0,
    .swiHwPriority   =  0,
};

/*
 *  =============================== SPI DMA ===============================
 */
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPICC26XXDMA.h>

SPICC26XXDMA_Object spiCC26XXDMAObjects[Board_SPI_SPICOUNT];

/*
 * NOTE: The SPI instances below can be used by the SD driver to communicate
 * with a SD card via SPI.  The 'defaultTxBufValue' fields below are set to 0xFF
 * to satisfy the SDSPI driver requirement.
 */
const SPICC26XXDMA_HWAttrsV1 spiCC26XXDMAHWAttrs[Board_SPI_SPICOUNT] = {
    {
        .baseAddr           = SSI0_BASE,
        .intNum             = INT_SSI0_COMB,
        .intPriority        = ~0,
        .swiPriority        = 0,
        .powerMngrId        = PowerCC26XX_PERIPH_SSI0,
        .defaultTxBufValue  = 0x0,
        .rxChannelBitMask   = 1<<UDMA_CHAN_SSI0_RX,
        .txChannelBitMask   = 1<<UDMA_CHAN_SSI0_TX,
        .mosiPin            = BOARD_SPI_CTL_IF_MOSI,
        .misoPin            = BOARD_SPI_CTL_IF_MISO,
        .clkPin             = BOARD_SPI_CTL_IF_CLK,
        .csnPin             = BOARD_SPI_CTL_IF_CS,
        .minDmaTransferSize = 1
    },
};

const SPI_Config SPI_config[Board_SPI_SPICOUNT] = {
    {
         .fxnTablePtr = &SPICC26XXDMA_fxnTable,
         .object      = &spiCC26XXDMAObjects[Board_SPI_CTL_IF],
         .hwAttrs     = &spiCC26XXDMAHWAttrs[Board_SPI_CTL_IF]
    },
};

const uint_least8_t SPI_count = Board_SPI_SPICOUNT;

/*
 *  =============================== UART ===============================
 */
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26XX.h>

UARTCC26XX_Object uartCC26XXObjects[Board_UART_UARTCOUNT];

uint8_t uartCC26XXRingBuffer[Board_UART_UARTCOUNT][32];

const UARTCC26XX_HWAttrsV2 uartCC26XXHWAttrs[Board_UART_UARTCOUNT] = {
    {
        .baseAddr       = UART0_BASE,
        .powerMngrId    = PowerCC26XX_PERIPH_UART0,
        .intNum         = INT_UART0_COMB,
        .intPriority    = ~0,
        .swiPriority    = 0,
        .txPin          = BOARD_UART_DEBUG_TX,
        .rxPin          = BOARD_UART_DEBUG_RX,
        .ctsPin         = PIN_UNASSIGNED,
        .rtsPin         = PIN_UNASSIGNED,
        .ringBufPtr     = uartCC26XXRingBuffer[Board_UART_DEBUG],
        .ringBufSize    = sizeof(uartCC26XXRingBuffer[Board_UART_DEBUG]),
        .txIntFifoThr   = UARTCC26XX_FIFO_THRESHOLD_1_8,
        .rxIntFifoThr   = UARTCC26XX_FIFO_THRESHOLD_4_8
    }
};

const UART_Config UART_config[Board_UART_UARTCOUNT] = {
    {
        .fxnTablePtr = &UARTCC26XX_fxnTable,
        .object      = &uartCC26XXObjects[Board_UART_DEBUG],
        .hwAttrs     = &uartCC26XXHWAttrs[Board_UART_DEBUG]
    },
};

const uint_least8_t UART_count = Board_UART_UARTCOUNT;

/*
 *  =============================== UDMA ===============================
 */
#include <ti/drivers/dma/UDMACC26XX.h>

UDMACC26XX_Object udmaObjects[Board_UDMACOUNT];

const UDMACC26XX_HWAttrs udmaHWAttrs[Board_UDMACOUNT] = {
    {
        .baseAddr    = UDMA0_BASE,
        .powerMngrId = PowerCC26XX_PERIPH_UDMA,
        .intNum      = INT_DMA_ERR,
        .intPriority = ~0
    }
};

const UDMACC26XX_Config UDMACC26XX_config[Board_UDMACOUNT] = {
    {
         .object  = &udmaObjects[Board_UDMA0],
         .hwAttrs = &udmaHWAttrs[Board_UDMA0]
    },
};

/*
 *  ======== initGeneral ========
 */
void General_init(void)
{
    Power_init();

    if (PIN_init(BoardGpioInitTable) != PIN_SUCCESS) {
        /* Error with PIN_init */
        while (1);
    }

    /* Shut down external flash as default */
    //CC1310_LAUNCHXL_shutDownExtFlash();
}
