/*
 * MAC.c
 *
 *  Created on: 2018. 3. 1.
 *      Author: Hello_DELL
 */

#include "MAC.h"

/***************************************
             Global Variable
 ***************************************/
//625  2.5 50 100 200 300 400 500 4M
//0, 0, 440, 360, 310, 300, 300, 290, 500
unsigned char cc1310_TxOverheadByte[9] = {11, 11, 11, 11, 11, 11};
unsigned short cc1310_TxOverheadUs[9] = {440, 360, 360, 300, 300, 290};
double IndexToBPuS[9] = {0.05, 0.1, 0.2, 0.3, 0.4, 0.5};

unsigned int Super_Frame_Size = 0;
unsigned short mBeacon_Interval = 0;
unsigned short mGaurd_Interval = 0;

unsigned short mTDMA_Start_Time = 0;
unsigned short mTDMA_Period = 0;
unsigned short mTDMA_End_Time = 0;
unsigned short mTDMA_Slot_Size = 0;
unsigned short numTDMA_Slot = 0;

/***************************************
                 Macro
 ***************************************/
#define getTransUsDelay(len, bps) (unsigned int)(((len + cc1310_TxOverheadByte[bps]) * 8 / IndexToBPuS[bps]) + cc1310_TxOverheadUs[bps])
#define UsToMiniSlot(millisecond) (unsigned short)(ceil((millisecond) / 500.0))

//#define TX_TEST
//#define RX_TEST

/*
char buf[254];

#ifdef TX_TEST
   memset(buf, 0xAA, sizeof(buf));

   RF_Params_Phy phy_params_ex;
   phy_params_ex.freq_tbl_idx = RF_PARAM_PHY_FREQ_923MHZ;
   phy_params_ex.dr_tbl_idx = RF_PARAM_PHY_DR_4MBPS;
   phy_params_ex.tx_pwr_tbl_idx = RF_PARAM_PHY_RE_TXPWR_26DBM;
   setPhyRFCtlIF(phy_params_ex);

   while(1)
   {
       onDebugPIN0();
       txRFCtlIF((char*)&buf, sizeof(buf), NULL);
       offDebugPIN0();
       dealyRFCtlIF(RF_getCurrentTime() + RF_convertMsToRatTicks(200));
       toggleLED0();
   }
#endif
#ifdef RX_TEST
   RF_Packet_Recv_Queue_Entry *recv_packet_entry;

   RF_Params_Phy phy_params_ex;
   phy_params_ex.freq_tbl_idx = RF_PARAM_PHY_FREQ_923MHZ;
   phy_params_ex.dr_tbl_idx = RF_PARAM_PHY_DR_4MBPS;
   phy_params_ex.tx_pwr_tbl_idx = RF_PARAM_PHY_RE_TXPWR_26DBM;
   setPhyRFCtlIF(phy_params_ex);

   while(1)
   {
       rxRFCtlIF(NULL, NULL, RUN);
       while(!(Queue_empty(pakcet_recv_queue.handle)))
       {
           Semaphore_pend(pakcet_recv_queue.sema, BIOS_WAIT_FOREVER);
           recv_packet_entry = Queue_dequeue(pakcet_recv_queue.handle);
           pakcet_recv_queue.size--;
           Semaphore_post(pakcet_recv_queue.sema);

           if(recv_packet_entry->packet_len == sizeof(buf))
           {
               toggleLED1();
           }
           free(recv_packet_entry->packet);
           free(recv_packet_entry);
       }
   }
#endif
*/

void *MACThread(void *arg)
{

}

void initMACThread(Thread_Params *thread_params)
{
    pthread_t           thread;
    pthread_attr_t      attrs;
    struct sched_param  pri_param;
    int                 retc;
    int                 detachState;

    /* Set priority and stack size attributes */
    pthread_attr_init(&attrs);
    pri_param.sched_priority = 1;

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&attrs, detachState);
    if (retc != 0) {
        /* pthread_attr_setdetachstate() failed */
        while (1);
    }

    pthread_attr_setschedparam(&attrs, &pri_param);

    retc |= pthread_attr_setstacksize(&attrs, 2176);    // THREADSTACKSIZE = 2048 1792
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }

    // Create MACThread
    retc = pthread_create(&thread, &attrs, MACThread, (void*)thread_params);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }
}
