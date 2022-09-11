/*
 * GPIO_LED.h
 *
 *  Created on: 2018. 2. 15.
 *      Author: Hello_DELL
 */

#ifndef BSP_GPIO_LED_H_
#define BSP_GPIO_LED_H_


/* Standard C Libraries */
/* TI Drivers */
/* Board Header files */
#include "Board_CC1390_Launchpad.h"
/* Thread Header */
/* RTOS header files */
/* TI RF Utils */
/* Driverlib Header files */


#define LED_ON          1
#define LED_OFF         0


void initLED();
void onLED0();
void offLED0();
void toggleLED0();
void onLED1();
void offLED1();
void toggleLED1();
void onDebugPIN0();
void offDebugPIN0();
void toggleDebugPIN0();
void onDebugPIN1();
void offDebugPIN1();
void toggleDebugPIN1();

#endif /* BSP_GPIO_LED_H_ */
