/*
 * UART_Debug.h
 *
 *  Created on: 2018. 2. 15.
 *      Author: Hello_DELL
 */

#ifndef BSP_UART_DEBUG_H_
#define BSP_UART_DEBUG_H_

/* Standard C Libraries */
/* TI Drivers */
/* Board Header files */
#include "Board_CC1390_Launchpad.h"
/* Thread Header */
/* RTOS header files */
/* TI RF Utils */
/* Driverlib Header files */



#define UART_BAUDRATE           512000


void intDebug();
void writeDebug(char* buff, int size);
void readDebug(char* buff, int size);


#endif /* BSP_UART_DEBUG_H_ */
