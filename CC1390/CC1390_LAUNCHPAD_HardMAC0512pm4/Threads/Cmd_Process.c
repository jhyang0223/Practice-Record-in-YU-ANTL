/*
 * Cmd_Process.c
 *
 *  Created on: 2018. 3. 1.
 *      Author: Hello_DELL
 */


#include "Cmd_Process.h"


void *CmdProcessThread(void *arg)
{
    Thread_Params thread_params = *(Thread_Params*)arg;

    HCI_Frame_Queue cmd_req_queue = thread_params.cmd_req_queue;
    HCI_Frame_Queue cmd_res_queue = thread_params.cmd_res_queue;
    Event_Handle cmd_proc_th_wake_evt = thread_params.cmd_proc_th_wake_evt;
    Event_Handle queue_moni_th_wake_evt = thread_params.queue_moni_th_wake_evt;
    Event_Handle mac_th_wake_evt = thread_params.mac_th_wake_evt;
    Shared_MAC_Params *shared_mac_params = thread_params.shared_mac_params;

    HCI_Frame ret_hci_frame;
    HCI_Frame_Queue_Entry *ret_hci_frame_entry = NULL;

    unsigned int evt;
    bool valid_cmd_sub_type_flag;

    while(1)
    {
        evt = Event_pend(cmd_proc_th_wake_evt, Event_Id_NONE, CMD_PROCESS_WAKE_EVT, BIOS_WAIT_FOREVER);
        if(evt & CMD_PROCESS_WAKE_EVT)   // Post Cmd Process Event
        {
            while(!(Queue_empty(cmd_req_queue.handle)))
            {
                memset(&ret_hci_frame, 0x00, sizeof(HCI_Frame));
                Semaphore_pend(cmd_req_queue.sema, BIOS_WAIT_FOREVER);
                ret_hci_frame_entry = Queue_dequeue(cmd_req_queue.handle);
                (*(cmd_req_queue.size))--;
                Semaphore_post(cmd_req_queue.sema);
                memcpy(&ret_hci_frame.fc, &(ret_hci_frame_entry->hci_frame.fc), HCI_FRAME_FC_SIZE + HCI_FRAME_LENGTH_SIZE + (ret_hci_frame_entry->hci_frame.length));
                free(ret_hci_frame_entry);

                valid_cmd_sub_type_flag = false;

                if(ret_hci_frame.fc.frame_type == HCI_FRAME_FC_FT_REQ_GET)
                {
                    ret_hci_frame_entry = (HCI_Frame_Queue_Entry *)malloc(sizeof(HCI_Frame_Queue_Entry));
                    if(ret_hci_frame.fc.cmd_type == HCI_FRAME_FC_CT_MAC_PARAM)
                    {
                        ret_hci_frame_entry->hci_frame.fc.frame_type = HCI_FRAME_FC_FT_RES_GET;
                        ret_hci_frame_entry->hci_frame.fc.cmd_type = HCI_FRAME_FC_CT_MAC_PARAM;
                        switch(ret_hci_frame.fc.cmd_sub_type)
                        {
                        case HCI_FRAME_FC_CST_MAC_EXE:
                            ret_hci_frame_entry->hci_frame.fc.cmd_sub_type = HCI_FRAME_FC_CST_MAC_EXE;
                            ret_hci_frame_entry->hci_frame.length = sizeof(unsigned char);
                            memcpy(ret_hci_frame_entry->hci_frame.payload, &(shared_mac_params->mac_exe), ret_hci_frame_entry->hci_frame.length);
                            valid_cmd_sub_type_flag = true;
                            break;
                        case HCI_FRAME_FC_CST_MAC_RF:
                            ret_hci_frame_entry->hci_frame.fc.cmd_sub_type = HCI_FRAME_FC_CST_MAC_RF;
                            ret_hci_frame_entry->hci_frame.length = sizeof(RF_Params_Phy);
                            memcpy(ret_hci_frame_entry->hci_frame.payload, &(shared_mac_params->mac_rf), ret_hci_frame_entry->hci_frame.length);
                            valid_cmd_sub_type_flag = true;
                            break;
                        case HCI_FRAME_FC_CST_MAC_MODE:
                        {
                            ret_hci_frame_entry->hci_frame.fc.cmd_sub_type = HCI_FRAME_FC_CST_MAC_MODE;
                            ret_hci_frame_entry->hci_frame.length = sizeof(unsigned char);
                            memcpy(ret_hci_frame_entry->hci_frame.payload, &(shared_mac_params->mac_mode), ret_hci_frame_entry->hci_frame.length);
                            valid_cmd_sub_type_flag = true;
                            break;
                        }
                        default:
                            // Invalid Command Sub-Type
                            free(ret_hci_frame_entry);
                            break;
                        }

                        if(valid_cmd_sub_type_flag)
                        {
                            Semaphore_pend(cmd_res_queue.sema, BIOS_WAIT_FOREVER);
                            Queue_enqueue(cmd_res_queue.handle, &(ret_hci_frame_entry->elem));
                            (*(cmd_res_queue.size))++;
                            Semaphore_post(cmd_res_queue.sema);

                            Event_post(queue_moni_th_wake_evt, QUEUE_MONITOR_WAKE_EVT);
                        }
                    }
                }
                else if(ret_hci_frame.fc.frame_type == HCI_FRAME_FC_FT_REQ_SET)
                {
                    ret_hci_frame_entry = (HCI_Frame_Queue_Entry *)malloc(sizeof(HCI_Frame_Queue_Entry));
                    if(ret_hci_frame_entry->hci_frame.fc.cmd_type == HCI_FRAME_FC_CT_SYS)
                    {
                        switch(ret_hci_frame.fc.cmd_sub_type)
                        {
                        case HCI_FRAME_FC_CST_SYS_RESET:
                            SysCtrlSystemReset();
                            break;
                        }
                    }
                    else if(ret_hci_frame_entry->hci_frame.fc.cmd_type == HCI_FRAME_FC_CT_MAC_PARAM)
                    {
                        ret_hci_frame_entry->hci_frame.fc.frame_type = HCI_FRAME_FC_FT_RES_SET;
                        ret_hci_frame_entry->hci_frame.fc.cmd_type = HCI_FRAME_FC_CT_MAC_PARAM;
                        switch(ret_hci_frame.fc.cmd_sub_type)
                        {
                        case HCI_FRAME_FC_CST_MAC_EXE:
                            ret_hci_frame_entry->hci_frame.fc.cmd_sub_type = HCI_FRAME_FC_CST_MAC_EXE;
                            unsigned char req_mac_exe;
                            memcpy(&req_mac_exe, ret_hci_frame.payload, sizeof(unsigned char));

                            if(req_mac_exe != shared_mac_params->mac_exe)
                            {
                                memcpy(&(shared_mac_params->mac_exe), ret_hci_frame.payload, sizeof(shared_mac_params->mac_exe));
                                if(req_mac_exe == MAC_EXE_IDLE)
                                    Event_post(mac_th_wake_evt, MAC_THREAD_IDLE_EVT);
                                else if(req_mac_exe == MAC_EXE_BUSY)
                                    Event_post(mac_th_wake_evt, MAC_THREAD_BUSY_MAC_WAKE_EVT);
                                valid_cmd_sub_type_flag = true;
                            }
                            break;
                        case HCI_FRAME_FC_CST_MAC_RF:
                            ret_hci_frame_entry->hci_frame.fc.cmd_sub_type = HCI_FRAME_FC_CST_MAC_RF;
                            memcpy(&(shared_mac_params->mac_rf), ret_hci_frame.payload, sizeof(shared_mac_params->mac_rf));
                            valid_cmd_sub_type_flag = true;
                            break;
                        case HCI_FRAME_FC_CST_MAC_MODE:
                            ret_hci_frame_entry->hci_frame.fc.cmd_sub_type = HCI_FRAME_FC_CST_MAC_MODE;
                            memcpy(&(shared_mac_params->mac_mode), ret_hci_frame.payload, sizeof(shared_mac_params->mac_mode));
                            valid_cmd_sub_type_flag = true;
                            break;
                        default:
                            free(ret_hci_frame_entry);
                            break;
                        }

                        if(valid_cmd_sub_type_flag)
                        {
                            ret_hci_frame_entry->hci_frame.length = sizeof(unsigned char);
                            ret_hci_frame_entry->hci_frame.payload[0] = HCI_FRAME_RES_OK;

                            Semaphore_pend(cmd_res_queue.sema, BIOS_WAIT_FOREVER);
                            Queue_enqueue(cmd_res_queue.handle, &(ret_hci_frame_entry->elem));
                            (*(cmd_res_queue.size))++;
                            Semaphore_post(cmd_res_queue.sema);

                            Event_post(queue_moni_th_wake_evt, QUEUE_MONITOR_WAKE_EVT);
                        }
                    }
                }
                Task_sleep(Clock_convertMsToTicks(1));
            }
        }
    }

}

void initCmdProcess(Thread_Params *thread_params)
{
    pthread_t           thread;
    pthread_attr_t      attrs;
    struct sched_param  pri_param;
    int                 retc;
    int                 detachState;

    /* Set priority and stack size attributes */
    pthread_attr_init(&attrs);
    pri_param.sched_priority = 3;

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&attrs, detachState);
    if (retc != 0) {
        /* pthread_attr_setdetachstate() failed */
        while (1);
    }

    pthread_attr_setschedparam(&attrs, &pri_param);

    retc |= pthread_attr_setstacksize(&attrs, 1024);    // THREADSTACKSIZE = 512
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }

    // Create MACThread
    retc = pthread_create(&thread, &attrs, CmdProcessThread, (void*)thread_params);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }
}


