/*
 * CC1310_Control_Interface.c
 *
 *  Created on: 2018. 2. 19.
 *      Author: Hello_DELL
 */


#include <BSP/HCI.h>


SPI_Handle spi_handle;
SPI_Params spi_params;
SPI_Transaction spi_transac;

void initHCI()
{
    SPI_Params_init(&spi_params);
    spi_params.mode = SPI_SLAVE;
    spi_params.dataSize = 8;
    spi_params.transferMode = SPI_MODE_BLOCKING;
    spi_params.frameFormat = SPI_POL0_PHA1;
    spi_handle = SPI_open(Board_SPI_CTL_IF, &spi_params);

    GPIO_setConfig(Board_GPIO_CTL_INT, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH | GPIO_CFG_OUT_STR_LOW);
}

void resetHCI()
{
    SPI_close(spi_handle);
    initHCI();
}

void rwHCI(char *tx_buff, char *rx_buff, int size)
{
    spi_transac.count = size;
    spi_transac.txBuf = (Ptr)tx_buff;
    spi_transac.rxBuf = (Ptr)rx_buff;

    SPI_transfer(spi_handle, &spi_transac);
}

void pudnHCI()
{
    GPIO_write(Board_GPIO_CTL_INT, 0);
    GPIO_write(Board_GPIO_CTL_INT, 1);
}
