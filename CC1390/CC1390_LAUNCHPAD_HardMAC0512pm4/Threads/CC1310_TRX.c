/*
 * CC1310_TRX.c
 *
 *  Created on: 2018. 3. 1.
 *      Author: Hello_DELL
 */

#include "CC1310_TRX.h"

typedef enum {
    MAC_Res,
    CMD_Res,
    DUMMY
} HCI_RX_State;

void *SPITRXThread(void *arg)
{
    Thread_Params thread_params = *(Thread_Params*)arg;

    HCI_Frame_Queue cmd_req_queue = thread_params.cmd_req_queue;
    HCI_Frame_Queue cmd_res_queue = thread_params.cmd_res_queue;
    HCI_Frame_Queue mac_req_queue = thread_params.mac_req_queue;
    HCI_Frame_Queue mac_res_queue = thread_params.mac_res_queue;
    Event_Handle cmd_proc_th_wake_evt = thread_params.cmd_proc_th_wake_evt;
    Event_Handle queue_moni_th_wake_evt = thread_params.queue_moni_th_wake_evt;
    Shared_MAC_Params *shared_mac_params = thread_params.shared_mac_params;

    HCI_Frame_Queue_Entry *ret_hci_frame_entry = NULL;
    HCI_Frame hci_tx_frame, hci_rx_frame;
    unsigned char hci_rx_state;

    Task_sleep(Clock_convertMsToTicks(1));

    memset(&hci_tx_frame, 0x00, sizeof(HCI_Frame));
    memset(&hci_rx_frame, 0x00, sizeof(HCI_Frame));

    while(1)
    {
        if(!(Queue_empty(mac_res_queue.handle)))
        {
            Semaphore_pend(mac_res_queue.sema, BIOS_WAIT_FOREVER);
            ret_hci_frame_entry = Queue_dequeue(mac_res_queue.handle);
            hci_rx_state = MAC_Res;
            Semaphore_post(mac_res_queue.sema);
            memcpy(&hci_tx_frame.fc, &(ret_hci_frame_entry->hci_frame.fc), HCI_FRAME_FC_SIZE + HCI_FRAME_LENGTH_SIZE + (ret_hci_frame_entry->hci_frame.length));
            free(ret_hci_frame_entry);
        }
        else if(!(Queue_empty(cmd_res_queue.handle)))
        {
            Semaphore_pend(cmd_res_queue.sema, BIOS_WAIT_FOREVER);
            ret_hci_frame_entry = Queue_dequeue(cmd_res_queue.handle);
            hci_rx_state = CMD_Res;
            Semaphore_post(cmd_res_queue.sema);
            memcpy(&hci_tx_frame.fc, &(ret_hci_frame_entry->hci_frame.fc), HCI_FRAME_FC_SIZE + HCI_FRAME_LENGTH_SIZE + (ret_hci_frame_entry->hci_frame.length));
            free(ret_hci_frame_entry);
        }
        else
        {
            hci_tx_frame.fc.frame_type = HCI_FRAME_FC_FT_DUMMY;
            hci_rx_state = DUMMY;
        }

        hci_tx_frame.sof[0] = HCI_FRAME_START_BYTE1;
        hci_tx_frame.sof[1] = HCI_FRAME_START_BYTE2;
        rwHCI((char*)&hci_tx_frame, (char*)&hci_rx_frame, HCI_FRAME_SIZE);

        if(hci_rx_state == MAC_Res)
            (*(mac_res_queue.size))--;
        else if(hci_rx_state == CMD_Res)
            (*(cmd_res_queue.size))--;

        if((hci_rx_frame.sof[0] == HCI_FRAME_START_BYTE1) && (hci_rx_frame.sof[1] == HCI_FRAME_START_BYTE2))
        {
            if((*(cmd_req_queue.size) + *(mac_req_queue.size)) < CONTOL_REQ_BUF_SIZE)
            {
                if(hci_rx_frame.fc.frame_type == HCI_FRAME_FC_FT_REQ_GET)
                {
                    if(hci_rx_frame.fc.cmd_type == HCI_FRAME_FC_CT_MAC_PARAM)
                    {
                        ret_hci_frame_entry = (HCI_Frame_Queue_Entry *)malloc(sizeof(HCI_Frame_Queue_Entry));

                        memcpy(&(ret_hci_frame_entry->hci_frame), &hci_rx_frame, HCI_FRAME_SOF_SIZE + HCI_FRAME_FC_SIZE + HCI_FRAME_LENGTH_SIZE + hci_rx_frame.length);

                        Semaphore_pend(cmd_req_queue.sema, BIOS_WAIT_FOREVER);
                        Queue_enqueue(cmd_req_queue.handle, &(ret_hci_frame_entry->elem));
                        (*(cmd_req_queue.size))++;
                        Semaphore_post(cmd_req_queue.sema);

                        Event_post(cmd_proc_th_wake_evt, CMD_PROCESS_WAKE_EVT);
                    }
                }
                else if(hci_rx_frame.fc.frame_type == HCI_FRAME_FC_FT_REQ_SET)
                {
                    /**********************************************************************************
                                                    COM_STA_STATE_RF_IDLE
                    **********************************************************************************/
                    if(shared_mac_params->mac_exe == MAC_EXE_IDLE)
                    {
                        if((hci_rx_frame.fc.cmd_type == HCI_FRAME_FC_CT_SYS) || (hci_rx_frame.fc.cmd_type == HCI_FRAME_FC_CT_MAC_PARAM))
                        {
                            ret_hci_frame_entry = (HCI_Frame_Queue_Entry *)malloc(sizeof(HCI_Frame_Queue_Entry));

                            memcpy(&(ret_hci_frame_entry->hci_frame), &hci_rx_frame, HCI_FRAME_SOF_SIZE + HCI_FRAME_FC_SIZE + HCI_FRAME_LENGTH_SIZE + hci_rx_frame.length);

                            Semaphore_pend(cmd_req_queue.sema, BIOS_WAIT_FOREVER);
                            Queue_enqueue(cmd_req_queue.handle, &(ret_hci_frame_entry->elem));
                            (*(cmd_req_queue.size))++;
                            Semaphore_post(cmd_req_queue.sema);

                            Event_post(cmd_proc_th_wake_evt, CMD_PROCESS_WAKE_EVT);
                        }
                    }
                    /**********************************************************************************
                                                    COM_STA_STATE_RF_BUSY
                    **********************************************************************************/
                    else if(shared_mac_params->mac_exe == MAC_EXE_BUSY)
                    {
                        if(((hci_rx_frame.fc.cmd_type == HCI_FRAME_FC_CT_SYS) && (hci_rx_frame.fc.cmd_sub_type == HCI_FRAME_FC_CST_SYS_RESET)) ||
                                ((hci_rx_frame.fc.cmd_type == HCI_FRAME_FC_CT_MAC_PARAM) && (hci_rx_frame.fc.cmd_sub_type == HCI_FRAME_FC_CST_MAC_EXE)))
                        {
                            ret_hci_frame_entry = (HCI_Frame_Queue_Entry *)malloc(sizeof(HCI_Frame_Queue_Entry));

                            memcpy(&(ret_hci_frame_entry->hci_frame), &hci_rx_frame, HCI_FRAME_SOF_SIZE + HCI_FRAME_FC_SIZE + HCI_FRAME_LENGTH_SIZE + hci_rx_frame.length);

                            Semaphore_pend(cmd_req_queue.sema, BIOS_WAIT_FOREVER);
                            Queue_enqueue(cmd_req_queue.handle, &(ret_hci_frame_entry->elem));
                            (*(cmd_req_queue.size))++;
                            Semaphore_post(cmd_req_queue.sema);

                            Event_post(cmd_proc_th_wake_evt, CMD_PROCESS_WAKE_EVT);
                        }
                        else if(hci_rx_frame.fc.cmd_type == HCI_FRAME_FC_CT_MAC_TRX)
                        {
                            if(hci_rx_frame.fc.cmd_sub_type == HCI_FRAME_FC_CST_MAC_TX)
                            {
                                ret_hci_frame_entry = (HCI_Frame_Queue_Entry *)malloc(sizeof(HCI_Frame_Queue_Entry));

                                memcpy(&(ret_hci_frame_entry->hci_frame), &hci_rx_frame, HCI_FRAME_SOF_SIZE + HCI_FRAME_FC_SIZE + HCI_FRAME_LENGTH_SIZE + hci_rx_frame.length);

                                Semaphore_pend(mac_req_queue.sema, BIOS_WAIT_FOREVER);
                                Queue_enqueue(mac_req_queue.handle, &(ret_hci_frame_entry->elem));
                                (*(mac_req_queue.size))++;
                                Semaphore_post(mac_req_queue.sema);

                                ret_hci_frame_entry = (HCI_Frame_Queue_Entry *)malloc(sizeof(HCI_Frame_Queue_Entry));
                                ret_hci_frame_entry->hci_frame.fc.frame_type = HCI_FRAME_FC_FT_RES_SET;
                                ret_hci_frame_entry->hci_frame.fc.cmd_type = HCI_FRAME_FC_CT_MAC_TRX;
                                ret_hci_frame_entry->hci_frame.fc.cmd_sub_type = HCI_FRAME_FC_CST_MAC_TX;
                                ret_hci_frame_entry->hci_frame.length = sizeof(unsigned char);
                                ret_hci_frame_entry->hci_frame.payload[0] = HCI_FRAME_RES_OK;

                                Semaphore_pend(mac_res_queue.sema, BIOS_WAIT_FOREVER);
                                Queue_enqueue(mac_res_queue.handle, &(ret_hci_frame_entry->elem));
                                (*(mac_res_queue.size))++;
                                Semaphore_post(mac_res_queue.sema);

                                Event_post(queue_moni_th_wake_evt, QUEUE_MONITOR_WAKE_EVT);
                            }
                        }
                    }
                }
            }
            else if(hci_rx_frame.fc.frame_type != HCI_FRAME_FC_FT_DUMMY)
            {
                ret_hci_frame_entry = (HCI_Frame_Queue_Entry *)malloc(sizeof(HCI_Frame_Queue_Entry));
                ret_hci_frame_entry->hci_frame.fc.frame_type = HCI_FRAME_FC_FT_NACK;
                ret_hci_frame_entry->hci_frame.fc.cmd_type = hci_rx_frame.fc.cmd_type;
                ret_hci_frame_entry->hci_frame.fc.cmd_sub_type = hci_rx_frame.fc.cmd_sub_type;
                ret_hci_frame_entry->hci_frame.length = 1;
                ret_hci_frame_entry->hci_frame.payload[0] = HCI_FRAME_NACK_BUF_FULL;

                Semaphore_pend(cmd_res_queue.sema, BIOS_WAIT_FOREVER);
                Queue_enqueue(cmd_res_queue.handle, &(ret_hci_frame_entry->elem));
                (*(cmd_res_queue.size))++;
                Semaphore_post(cmd_res_queue.sema);

                Event_post(queue_moni_th_wake_evt, QUEUE_MONITOR_WAKE_EVT);
            }
        }
        else
        {
            Task_sleep(Clock_convertMsToTicks(10));
            resetHCI();
            SysCtrlSystemReset();
        }
    }
}

void initSPITRXThread(Thread_Params *thread_params)
{
    pthread_t           thread;
    pthread_attr_t      attrs;
    struct sched_param  pri_param;
    int                 retc;
    int                 detachState;

    /* Set priority and stack size attributes */
    pthread_attr_init(&attrs);
    pri_param.sched_priority = 2;

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&attrs, detachState);
    if (retc != 0) {
        /* pthread_attr_setdetachstate() failed */
        while (1);
    }

    pthread_attr_setschedparam(&attrs, &pri_param);

    retc |= pthread_attr_setstacksize(&attrs, 1664);    // THREADSTACKSIZE = 1280 1536
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }

    // Create MACThread
    retc = pthread_create(&thread, &attrs, SPITRXThread, (void*)thread_params);
    if (retc != 0) {
        // pthread_create() failed
        while (1);
    }
}

