/*
 * Queue_Monitor.c
 *
 *  Created on: 2018. 3. 1.
 *      Author: Hello_DELL
 */


#include "Queue_Monitor.h"


void *QueueMonitorThread(void *arg)
{
    Thread_Params thread_params = *(Thread_Params*)arg;

    HCI_Frame_Queue cmd_res_queue = thread_params.cmd_res_queue;
    HCI_Frame_Queue mac_res_queue = thread_params.mac_res_queue;
    Event_Handle queue_moni_th_wake_evt = thread_params.queue_moni_th_wake_evt;

    int last_cmd_res_queue_size, last_mac_res_queue_size;
    int cur_cmd_res_queue_size, cur_mac_res_queue_size;

    unsigned int evt;

    while(1)
    {
        evt = Event_pend(queue_moni_th_wake_evt, Event_Id_NONE, QUEUE_MONITOR_WAKE_EVT, BIOS_WAIT_FOREVER);
        if(evt & QUEUE_MONITOR_WAKE_EVT)
        {
            last_cmd_res_queue_size = *(cmd_res_queue.size);
            last_mac_res_queue_size = *(mac_res_queue.size);
            while(1)
            {
                cur_cmd_res_queue_size = *(cmd_res_queue.size);
                cur_mac_res_queue_size = *(mac_res_queue.size);
                if(Queue_empty(mac_res_queue.handle) && Queue_empty(cmd_res_queue.handle))
                    break;
                else if((last_cmd_res_queue_size != cur_cmd_res_queue_size) && (cur_cmd_res_queue_size > 0))
                    last_cmd_res_queue_size = cur_cmd_res_queue_size;
                else if((last_mac_res_queue_size != cur_mac_res_queue_size) && (cur_mac_res_queue_size > 0))
                    last_mac_res_queue_size = cur_mac_res_queue_size;
                else
                {
                    //toggleLED1();
                    pudnHCI();
                }
                Task_sleep(Clock_convertMsToTicks(5));
            }
        }
    }
}

void initQueueMonitor(Thread_Params *thread_params)
{
    pthread_t           thread;
    pthread_attr_t      attrs;
    struct sched_param  pri_param;
    int                 retc;
    int                 detachState;

    /* Set priority and stack size attributes */
    pthread_attr_init(&attrs);
    pri_param.sched_priority = 4;

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&attrs, detachState);
    if (retc != 0) {
        /* pthread_attr_setdetachstate() failed */
        while (1);
    }

    pthread_attr_setschedparam(&attrs, &pri_param);

    retc |= pthread_attr_setstacksize(&attrs, 768);    // THREADSTACKSIZE = 512
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }

    // Create MACThread
    retc = pthread_create(&thread, &attrs, QueueMonitorThread, (void*)thread_params);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }
}


