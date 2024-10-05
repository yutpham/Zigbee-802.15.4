/*******************************************************************************/
//Module: OS                                                                   */   
//File Name: static_timer.c                                                    */
//Description: This file contains timer functions for timer, abstraction of    */
//             the OS timer                                                    */
//  -------------------------------------------------------------              */
//Revision History:                                                            */
//Date              Programmer      Details                                    */
//11/13/2006        pnthai          Create                                     */
//  -------------------------------------------------------------              */
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/


/************************************************************************/
/*                  Include files                                       */
/************************************************************************/
#include "types.h"
#include "mac_types.h"
#include "ucos_ii.h"
#include "static_timer.h"
#include "hal.h"
#include "schedul.h"

#define     MAX_ZIGBEE_STATIC_TIMER            16
/*---------------------------------------------------------------------
                        STATIC PERIODICAL TIMER
--------------------------------------------------------------------- */
/* Static callback function
*/
void mlme_scanTimeout();
void mac_forceOffTrx(void* par_pParam, void* args);
void mlme_runAssoDataReq(void* args,void* par_param);
void mlme_timeoutAssoDataRes(void* args,void* par_pParam);	
void beacon_createSyncBeaconTask(void*   par_pParam, void* args);
void beacon_createTimeoutSyncBncTask(void*,void*);
void csmaca_locateBeaconTimeout(void* args,void*    par_pParam);
void tx_timeoutAck(void*, void* par_pParam);
void csmaca_restartFromDelay(void* args1, void* args2);
void mlme_timeoutPoll(void*, void*);
void mlme_handleRxEnableTask(void*, void* );
void beacon_createBeaconTxTask(void* par_pParam, void* args);

typedef struct
{
    OS_TMR*     osTimer;
    P_FNC       pCallback;
} Z_TIMER;

Z_TIMER TimerList[MAX_ZIGBEE_STATIC_TIMER]=
{
    {NULL, mlme_scanTimeout},
    {NULL, mac_forceOffTrx},
    {NULL, mlme_runAssoDataReq},
    {NULL, mlme_timeoutAssoDataRes},
    {NULL, beacon_createSyncBeaconTask},
    {NULL, beacon_createTimeoutSyncBncTask},
    {NULL, csmaca_locateBeaconTimeout},
    {NULL, tx_timeoutAck},
    {NULL, csmaca_restartFromDelay},
    {NULL, mlme_timeoutPoll},
    {NULL, mlme_handleRxEnableTask}
#if  ZIGBEE_DEVICE_FFD == 1
    ,{NULL, beacon_createBeaconTxTask}
#endif  //!ZIGBEE_DEVICE_FFD == 1
};

/******************************************************************************/
//@FunctionName: timer_init                                                   */
//@Description:  Specific initialization of abstraction of the OS timer       */
//                                                                            */    
//@Param                                                                      */
//                                                                            */   
/******************************************************************************/
BOOL timer_init()
{
    /*********** Variable declaration ***********/
    UINT8   nErr;
    /*********** Function body       ***********/    
    /*Initialize static timer*/
    for (int i = 0; i< MAX_ZIGBEE_STATIC_TIMER; i++)
    {
        if (TimerList[i].pCallback != NULL)
        {
            TimerList[i].osTimer = OSTmrCreate(0,1,OS_TMR_OPT_ONE_SHOT,TimerList[i].pCallback
                                            ,NULL,NULL,&nErr);
        }
    }

    /* configure timer */
    TIMER1_INIT(TIMER1_CAPTURE_NOISE_CANCELLER | TIMER1_CAPTURE_ON_POSEDGE);
    TIMER1_SET_COMPA_VALUE(MAC_TIMER_OVERFLOW_VALUE);
    TIMER1_SET_COMPB_VALUE(MAC_TIMER_BACKOFF_SLOT_OFFSET);    

    /* start the timer 1*/
    TIMER1_START(TIMER1_CLK_DIV1);

    return TRUE;
} // End of functions

/******************************************************************************/
//@FunctionName: timer_startTimer                                             */
//@Description:                                                               */
//  Start a timer with specific duration and callback function                */    
//@Param                                                                      */
//                                                                            */   
/******************************************************************************/
BOOL timer_startTimer(STATIC_TIMER_CALLBACK         nIndex,
                      void*                         par_pParam,
                      UINT32                        par_Time)
{
    /*********** Variable declaration ***********/
    UINT8   nErr;
    /*********** Function body       ***********/
    //start the os timer   
    TimerList[nIndex].osTimer->OSTmrCallbackArg = par_pParam;
    TimerList[nIndex].osTimer->OSTmrPeriod      = par_Time;       
    return OSTmrStart(TimerList[nIndex].osTimer, &nErr);
} // End of functions MacTimer_startTimer

/******************************************************************************/
//@FunctionName: timer_endTimer                                               */
//@Description:                                                               */
//  Start a timer with specified duration and callback function               */    
//@Param                                                                      */
//                                                                            */   
/******************************************************************************/
BOOL timer_endTimer(STATIC_TIMER_CALLBACK         nIndex)
{
    /*********** Variable declaration ***********/
    UINT8   nErr;
    /*********** Function body       ***********/
    //Stop timer from os
    OSTmrStop(TimerList[nIndex].osTimer,OS_TMR_OPT_NONE ,NULL, &nErr); 
    return TRUE;
} // End of functions MacTimer_endTimer



/*-----------------------------------------------------------------------------
                        A-PERIODICAL TIMER

This kind of timer is used for a-periodical task. This timer should 
created for a-periodical task: NWK, APS, APP to handle its own periodical action
----------------------------------------------------------------------------- */
/* Timer record
*/
#define             MSG_TIMER_PRECISION       30        /*In mimisecond*/
typedef struct  
{
    DWORD           Msg;
    UINT32          TmrMatch;                           /*In 10 mini second*/    
    UINT32          Period;
    BOOL            Periodic;
    UINT8           Task;            
}TIMER_REC;

/* Timer table
*/
typedef struct  
{
    TIMER_REC*      TmrList[MAX_MSG_TIMER];
    UINT32          TmrTime;
}TIMER_TABLE;
/*Timer list*/
TIMER_TABLE         MsgTimer;
OS_TMR*             pMsOsTmr;

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will reset the msg timer                        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mtimer_reset()
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    DISABLE_GLOBAL_INT();
    MsgTimer.TmrTime = 0;
    for (UINT8 i=0;i<MAX_MSG_TIMER;i++)
        if (MsgTimer.TmrList[i] != NULL)
        {
            mem_freeMem(MsgTimer.TmrList[i]);
             MsgTimer.TmrList[i] = NULL;
        }
    ENABLE_GLOBAL_INT();
    mtimer_handler(NULL,NULL);
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mtimer_handler(void*   pTimer, void*   arg)
{
	/*********** Variable declaration ***********/
    UINT8       nErr;
	/*********** Function body		 ***********/
    if (pTimer == NULL)
    {
        /*Create timer*/
        pTimer = OSTmrCreate(0, (UINT32)(MSG_TIMER_PRECISION*1000)/(320)/*Converted to back-off slot*/,
                                OS_TMR_OPT_PERIODIC,mtimer_handler,NULL,NULL,&nErr);
        if (pTimer == NULL)
            err(ERR_UNRECOVERABLE,0,0,0);

        pMsOsTmr=  pTimer;
        OSTmrStart(pMsOsTmr,&nErr);
    }    
    mtimer_dispatchTmrMsg(NULL,NULL);
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mtimer_dispatchTmrMsg(void*   pTimer, void*   arg)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    DISABLE_GLOBAL_INT();
    MsgTimer.TmrTime++;
    for (UINT8 i=0;i<MAX_MSG_TIMER;i++) 
    {
        if (MsgTimer.TmrList[i] != NULL)
        {
            /*Send msg to appropriate task*/
            if(MsgTimer.TmrList[i]->TmrMatch == MsgTimer.TmrTime)
            {
                schedul_sendMsg(MsgTimer.TmrList[i]->Task,&MsgTimer.TmrList[i]->Msg);
            
                if (MsgTimer.TmrList[i]->Periodic)
                    MsgTimer.TmrList[i]->TmrMatch = MsgTimer.TmrTime + MsgTimer.TmrList[i]->Period;
                else
                {
                    mem_freeMem(MsgTimer.TmrList[i]);
                    MsgTimer.TmrList[i] = NULL;
                }
            }
        }
    }        
    ENABLE_GLOBAL_INT();
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will create a msg timer                         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL mtimer_creatTmr(DWORD*     pMsg, UINT32    Period/*In 1ms*/,BOOL bPeriodical, UINT8     Task)
{
	/*********** Variable declaration ***********/
	UINT8 			i;
	/*********** Function body		 ***********/
    DISABLE_GLOBAL_INT();
    /*Search for free slot*/
    for (i=0;i<MAX_MSG_TIMER;i++)
        if (MsgTimer.TmrList[i] == NULL)
            break;

    if (i == MAX_MSG_TIMER)
    {
        ENABLE_GLOBAL_INT();
        return FALSE;
    }
    else
    {
        MsgTimer.TmrList[i]= mem_getMem(sizeof(TIMER_REC));
        if (MsgTimer.TmrList[i] == NULL)
        {
            ENABLE_GLOBAL_INT();
            return FALSE;
        }
        MsgTimer.TmrList[i]->Msg         = *pMsg;
        MsgTimer.TmrList[i]->Period      = (UINT32)Period/MSG_TIMER_PRECISION;
        if (MsgTimer.TmrList[i]->Period == 0)
            MsgTimer.TmrList[i]->Period = 1; /*Timing step*/
        MsgTimer.TmrList[i]->TmrMatch    = MsgTimer.TmrList[i]->Period +MsgTimer.TmrTime;
        MsgTimer.TmrList[i]->Periodic    = bPeriodical; 
        MsgTimer.TmrList[i]->Task        = Task;
        ENABLE_GLOBAL_INT();
        return TRUE;
    }
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will create a msg timer                         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mtimer_endTmr(DWORD*     pMsg)
{
	/*********** Variable declaration ***********/
	UINT8 			i;
	/*********** Function body		 ***********/
    DISABLE_GLOBAL_INT();
    for (i=0;i<MAX_MSG_TIMER;i++)
        if (MsgTimer.TmrList[i] != NULL
            && MsgTimer.TmrList[i]->Msg == *pMsg)
            break;    
    if (i < MAX_MSG_TIMER)
    {
        mem_freeMem(MsgTimer.TmrList[i]);
        MsgTimer.TmrList[i] = NULL;
    }
    ENABLE_GLOBAL_INT();
} // End of functions