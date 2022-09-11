/*
 * Range_Extender.c
 *
 *  Created on: 2018. 3. 14.
 *      Author: Hello_DELL
 */

#include "Range_Extender.h"

PIN_Handle range_extender_pin_handle;
PIN_State range_extender_pin_state;

PIN_Config range_extender_pin_table[] =
{
     BOARD_RANGE_EXTENDER_PA | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,     // CC1190 PA
     BOARD_RANGE_EXTENDER_LNA | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,    // CC1190 LNA
     PIN_TERMINATE
};

void initRangeExtender()
{
    range_extender_pin_handle = PIN_open(&range_extender_pin_state, range_extender_pin_table);
    PINCC26XX_setMux(range_extender_pin_handle, BOARD_RANGE_EXTENDER_PA, PINCC26XX_MUX_RFC_GPO1);
    PINCC26XX_setMux(range_extender_pin_handle, BOARD_RANGE_EXTENDER_LNA, PINCC26XX_MUX_RFC_GPO0);

    GPIO_setConfig(Board_RANGE_EXTENDER_HGM, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
}

void enableHGMRangeExtender()
{
    GPIO_write(Board_RANGE_EXTENDER_HGM, true);
}

void disableHGMRangeExtender()
{
    GPIO_write(Board_RANGE_EXTENDER_HGM, false);
}
