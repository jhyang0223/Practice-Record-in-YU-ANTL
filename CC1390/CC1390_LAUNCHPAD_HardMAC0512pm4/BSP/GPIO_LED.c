/*
 * GPIO_LED.c
 *
 *  Created on: 2018. 2. 15.
 *      Author: Hello_DELL
 */

#include "GPIO_LED.h"

void initLED()
{
    GPIO_setConfig(Board_GPIO_LED_GREEN, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);
    GPIO_setConfig(Board_GPIO_LED_RED, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);
    GPIO_setConfig(Board_GPIO_DEBUG_PIN0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_GPIO_DEBUG_PIN1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    offLED0();
    offLED1();

    offDebugPIN0();
    offDebugPIN1();
}

void onLED0()
{
    GPIO_write(Board_GPIO_LED_GREEN, LED_ON);
}

void offLED0()
{
    GPIO_write(Board_GPIO_LED_GREEN, LED_OFF);
}

void toggleLED0()
{
    GPIO_toggle(Board_GPIO_LED_GREEN);
}

void onLED1()
{
    GPIO_write(Board_GPIO_LED_RED, LED_ON);
}

void offLED1()
{
    GPIO_write(Board_GPIO_LED_RED, LED_OFF);
}

void toggleLED1()
{
    GPIO_toggle(Board_GPIO_LED_RED);
}

void onDebugPIN0()
{
    GPIO_write(Board_GPIO_DEBUG_PIN0, true);
}

void offDebugPIN0()
{
    GPIO_write(Board_GPIO_DEBUG_PIN0, false);
}

void toggleDebugPIN0()
{
    GPIO_toggle(Board_GPIO_DEBUG_PIN0);
}

void onDebugPIN1()
{
    GPIO_write(Board_GPIO_DEBUG_PIN1, true);
}

void offDebugPIN1()
{
    GPIO_write(Board_GPIO_DEBUG_PIN1, false);
}

void toggleDebugPIN1()
{
    GPIO_toggle(Board_GPIO_DEBUG_PIN1);
}
