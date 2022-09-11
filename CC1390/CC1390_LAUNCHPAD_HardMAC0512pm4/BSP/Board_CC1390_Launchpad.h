/*
 * Board_Launchpad.h
 *
 *  Created on: 2018. 2. 15.
 *      Author: Hello_DELL
 */

#ifndef BSP_BOARD_CC1390_LAUNCHPAD_H_
#define BSP_BOARD_CC1390_LAUNCHPAD_H_


#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>

#include <ti/drivers/PIN.h>
#include <ti/devices/cc13x0/driverlib/ioc.h>

#include <ti/sysbios/knl/Clock.h>

#define Clock_convertMsToTicks(milliseconds) \
    (((milliseconds) * 1000) / Clock_tickPeriod)


#define BOARD_GPIO_LED0             IOID_7  // Green
#define BOARD_GPIO_LED1             IOID_6  // Red

#define BOARD_UART_DEBUG_TX         IOID_3
#define BOARD_UART_DEBUG_RX         IOID_2

#define BOARD_SPI_CTL_IF_CLK        IOID_24
#define BOARD_SPI_CTL_IF_MISO       IOID_10
#define BOARD_SPI_CTL_IF_MOSI       IOID_21
#define BOARD_SPI_CTL_IF_CS         IOID_4
#define BOARD_SPI_CTL_IF_INT        IOID_5

#define BOARD_RANGE_EXTENDER_PA     IOID_30
#define BOARD_RANGE_EXTENDER_LNA    IOID_29
#define BOARD_RANGE_EXTENDER_HGM    IOID_28

#define BOARD_GPIO_DEBUG_PIN0       IOID_12
#define BOARD_GPIO_DEBUG_PIN1       IOID_11

typedef enum {
    Board_GPIO_LED_GREEN = 0,
    Board_GPIO_LED_RED,
    Board_GPIO_CTL_INT,     // Interrupt
    Board_RANGE_EXTENDER_HGM,
    Board_GPIO_DEBUG_PIN0,
    Board_GPIO_DEBUG_PIN1,

    Board_GPIO_GPIOCOUNT
} Board_GPIO_Name;

typedef enum {
    Board_SPI_CTL_IF = 0,   // CC1310 Control Interface

    Board_SPI_SPICOUNT
} Board_SPI_Name;

typedef enum {
    Board_UART_DEBUG = 0,

    Board_UART_UARTCOUNT
} Board_UART_Name;

typedef enum {
    Board_UDMA0 = 0,

    Board_UDMACOUNT
} Board_UDMAName;


void General_init(void);

#endif /* BSP_BOARD_CC1390_LAUNCHPAD_H_ */
