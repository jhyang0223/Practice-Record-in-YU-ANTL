/*
 * RF_Control_Interface.h
 *
 *  Created on: 2018. 3. 2.
 *      Author: Hello_DELL
 */

#ifndef BSP_RF_CONTROL_INTERFACE_H_
#define BSP_RF_CONTROL_INTERFACE_H_


/* Standard C Libraries */
#include <stdlib.h>
/* TI Drivers */
#include <ti/drivers/rf/RF.h>
/* Board Header files */
#include "BSP/GPIO_LED.h"
#include "BSP/Range_Extender.h"
#include "smartrf_settings/smartrf_settings.h"
#include "smartrf_settings/application_settings.h"
/* Thread Header */
/* RTOS header files */
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>
/* TI RF Utils */
#include "Utils/RFQueue.h"
/* Driverlib Header files */
#include DeviceFamily_constructPath(driverlib/rf_prop_mailbox.h)


#define CS_RETRIES_WHEN_BUSY        20      /* Number of times the CS command should run in the case where the channel is BUSY */
#define RSSI_THRESHOLD_DBM          -80     /* The channel is reported BUSY is the RSSI is above this threshold */
#define IDLE_TIME_US                550
#define PROP_DONE_OK                0x3400  /* Proprietary Radio Operation Status Codes Number: Operation ended normally */

#define RF_PACKET_MAX_SIZE          255
#define DATA_ENTRY_HEADER_SIZE      8   // Constant header size of a Generic Data Entry
#define NUM_TX_DATA_ENTRIES         1   // NOTE: Only two data entries supported at the moment
#define NUM_TX_APPENDED_BYTES       0
#define NUM_RX_DATA_ENTRIES         2   // NOTE: Only two data entries supported at the moment
#define NUM_RX_APPENDED_BYTES       3   // RF_cmdRxHS.rxConf.bIncludeLen = 1: Include the received length
                                        // field (2 bytes) in the stored packet
typedef enum
{
    RUN,
    POST
} RX_RF_Mode;

typedef struct {
    Queue_Elem elem;
    int packet_len;
    char *packet;
    signed char rssi;
} RF_Packet_Recv_Queue_Entry;

typedef struct {
    Queue_Handle handle;
    Semaphore_Handle sema;
    int size;
} RF_Packet_Recv_Queue;

typedef struct {
    unsigned short crc_ok;
    unsigned short crc_nok;
} RF_Packet_Recv_Info;

extern RF_Packet_Recv_Queue pakcet_recv_queue;
extern unsigned char csma_tx_ok;

/*
typedef struct {
    unsigned char len;
    char data[255];
} Packet_Recv_Buf;
extern Packet_Recv_Buf packet_recv_buf;
*/

/**********************************************************************************
                        RF Params Phy Center Frequency Table
                        RF Params Phy Data rate
                        RF Params Phy TX Power Table
**********************************************************************************/
typedef enum {
    RF_PARAM_PHY_FREQ_917MHZ = 0,
    RF_PARAM_PHY_FREQ_918MHZ,
    RF_PARAM_PHY_FREQ_919MHZ,
    RF_PARAM_PHY_FREQ_920MHZ,
    RF_PARAM_PHY_FREQ_921MHZ,
    RF_PARAM_PHY_FREQ_922MHZ,
    RF_PARAM_PHY_FREQ_923MHZ
} RF_Params_Phy_Freq_Table_Index;

typedef enum {
    RF_PARAM_PHY_DR_50KBPS = 0,
    RF_PARAM_PHY_DR_100KBPS,
    RF_PARAM_PHY_DR_200KBPS,
    RF_PARAM_PHY_DR_300KBPS,
    RF_PARAM_PHY_DR_400KBPS,
    RF_PARAM_PHY_DR_500KBPS,
    RF_PARAM_PHY_DR_4MBPS
} RF_Params_Phy_DR_Table_Index;


typedef enum {
    RF_PARAM_PHY_RE_TXPWR_7DBM = 0,
    RF_PARAM_PHY_RE_TXPWR_14DBM,
    RF_PARAM_PHY_RE_TXPWR_18DBM,
    RF_PARAM_PHY_RE_TXPWR_20DBM,
    RF_PARAM_PHY_RE_TXPWR_22DBM,
    RF_PARAM_PHY_RE_TXPWR_23DBM,
    RF_PARAM_PHY_RE_TXPWR_24DBM,
    RF_PARAM_PHY_RE_TXPWR_25DBM,
    RF_PARAM_PHY_RE_TXPWR_26DBM
} RF_Params_Phy_TXPWR_Table_Index;


typedef struct {
    unsigned char freq_tbl_idx;
    unsigned char dr_tbl_idx;
    unsigned char tx_pwr_tbl_idx;
} RF_Params_Phy;

typedef struct {
    unsigned short retry_num;
    char rssi_threshold;
    unsigned int idle_time;
} RF_Params_CS;



void initRFCtlIF();
void setPhyRFCtlIF(RF_Params_Phy phy_param);
void txRFCtlIF(char *buf, int buf_size, unsigned int start_delay);
void txCSMARFCtlIF(char *buf, int buf_size, unsigned int start_delay);
void rxRFCtlIF(RF_CmdHandle *cmd_handle, unsigned int end_time, RX_RF_Mode rx_mode);
char getRSSIRfCtlIF();
void rxRFCancelCtlIF(RF_CmdHandle *cmd_handle);
void dealyRFCtlIF(unsigned int delay);
void initRXResultRFCtlIF();
RF_Packet_Recv_Info getRXResultRFCtlIF();
unsigned char getLQI();
signed char isChannelIdle(unsigned int time);
#endif /* BSP_RF_CONTROL_INTERFACE_H_ */
