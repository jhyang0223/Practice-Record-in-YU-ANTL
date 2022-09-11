/*
 * UART_Debug.c
 *
 *  Created on: 2018. 2. 15.
 *      Author: Hello_DELL
 */


#include "UART_Debug.h"

UART_Handle uart_handler;
UART_Params uart_params;

void intDebug()
{
    UART_Params_init(&uart_params);
    uart_params.writeDataMode = UART_DATA_BINARY;
    uart_params.readDataMode = UART_DATA_BINARY;
    uart_params.readReturnMode = UART_RETURN_FULL;
    uart_params.readEcho = UART_ECHO_OFF;
    uart_params.baudRate = UART_BAUDRATE;

    uart_handler = UART_open(Board_UART_DEBUG, &uart_params);

    if (uart_handler == NULL) {
        /* UART_open() failed */
        while (1);
    }
}

void writeDebug(char *buff, int size)
{
    UART_write(uart_handler, buff, size);
}

void readDebug(char* buff, int size)
{
    UART_read(uart_handler, buff, size);
}
