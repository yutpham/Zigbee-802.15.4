/*******************************************************************************/
//Module: OS   	                                                               */   
//File Name: schedul.c                                                         */
//Description: This file contains functions for scheduling task in MAC layer   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//11/29/2006        pnthai          Create                                     */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "static_timer.h"
#include "schedul.h"
#include "hal.h"
#include "ucos_ii.h"
#include "utils.h"
#include "queue.h"
#include "hal_cc2420.h"
#include "mac_types.h"

/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/
#define     NUM_PER_TASK_LEVEL              4
#define     MAX_NUM_PER_TASK                12
#define     MAX_NUM_APER_TASK               2

#define     MAC_ZIGBEE_EVENT                12
#define     MAX_APP_MSG_NUM                 6    

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/
typedef struct  
{
    P_FNC                pFunc;
    WORD                 dwTaskData;
    BOOL                 bSuspended;
}TASK_TYPE;

typedef TASK_TYPE*       P_TASK_TYPE;

typedef struct 
{
    TASK_TYPE                       sTask;
    void*                           pNext;
}TASK_QUEUE_ELEM;

typedef TASK_QUEUE_ELEM*    P_TASK_QUEUE_ELEM;

/* Periodical task 
*/
typedef struct 
{
    P_TASK_QUEUE_ELEM       pFirst;
    P_TASK_QUEUE_ELEM       pLast;
}TASK_QUEUE;

typedef TASK_QUEUE*             P_TASK_QUEUE;

typedef struct 
{
    P_TASK_QUEUE_ELEM           pFreeElems;
    TASK_QUEUE                  TaskQueue[NUM_PER_TASK_LEVEL];
}TASK_QUEUE_LIST;

/* A-periodical task 
*/
typedef struct  
{
    TASK_TYPE       Task;
    Q_QUEUE         MsgQueue;
}APERIODICAL_TASK;

/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/

/* Spool for periodic task
*/
TASK_QUEUE_ELEM     PerTaskSpool[MAX_NUM_PER_TASK];
TASK_QUEUE_LIST     PerTaskQueue;


/*  A-periodical  task is static task, defined here
*/
void    zigbee_task(void*, void*);
void    app_task(void*, void*);

/* Array contains event msg for the task*/
DWORD               ZigbeeEventList[MAC_ZIGBEE_EVENT];
DWORD               AppMsgQueue[MAX_APP_MSG_NUM];

/* Task*/
APERIODICAL_TASK    AperTaskQueue[MAX_NUM_APER_TASK]=
{
    {{zigbee_task, NULL, FALSE},{NULL,NULL,NULL,NULL,0,0}}, /*TASK_APER_NWK*/
    {{app_task, NULL, FALSE},{NULL,NULL,NULL,NULL,0,0}}     /*TASK_APER_APP*/
};

/*Current a-periodical task*/
TASK_PRIORITY       CurrAperTask;   
/******************************************************************************/
//@FunctionName:   schedul_runTasks  					  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void schedul_runPerTasks(void)
{
	/*********** Variable declaration ***********/
    P_TASK_QUEUE_ELEM                pCurrentPos;  
    UINT8                            i;
	/*********** Function body		 ***********/

    /*Run periodical task*/
    for (i = 0; i <= TASK_PER_MAC_TRANSACTION; i++)
    {        
        /* start task */
        DISABLE_GLOBAL_INT();
        pCurrentPos = PerTaskQueue.TaskQueue[i].pFirst;
        ENABLE_GLOBAL_INT();
        if (pCurrentPos != NULL) 
        {
            if (pCurrentPos->sTask.bSuspended == FALSE)
            {
                ((P_FNC) pCurrentPos->sTask.pFunc)((void*)pCurrentPos->sTask.dwTaskData, NULL);

                /* High privilege for RX and TX task */
                if(i > TASK_PER_MAC_TX )
                    break;
            }
        }
    }    
    return;
} // End of functions

/******************************************************************************/
//@FunctionName:   schedul_runAperTasks  					  	              */
//@Description:	This function will implement the policy for a-periodical task */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void schedul_runAperTasks(void)
{
	/*********** Variable declaration ***********/
    DWORD           Msg; 
	/*********** Function body		 ***********/
    while (TRUE)
    {
        if(AperTaskQueue[0].MsgQueue.QEntries != 0)    /*Check the highest priority*/
        {
            CurrAperTask = TASK_APER_NWK;
            /* run it */
            DISABLE_GLOBAL_INT();
            queue_pop(&AperTaskQueue[0].MsgQueue, &Msg);
            ENABLE_GLOBAL_INT();
            AperTaskQueue[0].Task.pFunc(&Msg,NULL);
        }
        else                        /* Run the next available task to process ONE msg */
        {
            if(AperTaskQueue[1].MsgQueue.QEntries != 0)
            {
                CurrAperTask = TASK_APER_APP;
                /* run it */
                DISABLE_GLOBAL_INT();
                queue_pop(&AperTaskQueue[1].MsgQueue, &Msg);
                ENABLE_GLOBAL_INT();
                AperTaskQueue[1].Task.pFunc(&Msg,NULL);
            }
        }
    }    
    return;
} // End of functions
/******************************************************************************/
//@FunctionName:  schedul_sendMsg 										  	  */
//@Description:	 This function will be used to send msg to a specific 		  */
//               a-periodic task                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void schedul_sendMsg(TASK_PRIORITY  par_ePrior, void*   par_pMsg)   
{
    DISABLE_GLOBAL_INT();
    if (par_ePrior - TASK_APER_NWK >= 0 && par_ePrior - TASK_APER_NWK < MAX_NUM_APER_TASK)
        queue_push(&AperTaskQueue[par_ePrior - TASK_APER_NWK].MsgQueue, par_pMsg);
    ENABLE_GLOBAL_INT();
}

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT8 schedul_getTask()
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    return CurrAperTask;
} // End of functions
/******************************************************************************/
//@FunctionName:  schedul_init                                                */
//@Description:	  Initialize Scheduler                                        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void schedul_init()
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    //initialize task queue;
    for (int i = 0; i<MAX_NUM_PER_TASK; i++)
    {
        PerTaskSpool[i].sTask.dwTaskData     = 0;
        PerTaskSpool[i].sTask.pFunc          = NULL;

        if( i == MAX_NUM_PER_TASK -1)
        {
            PerTaskSpool[i].pNext = NULL;
        }
        else
        {
            PerTaskSpool[i].pNext = &PerTaskSpool[i+1];
        }
    }    
    for (int i = 0;i< NUM_PER_TASK_LEVEL; i++)
    {
        PerTaskQueue.TaskQueue[i].pFirst = NULL;
        PerTaskQueue.TaskQueue[i].pLast = NULL;
    }
    PerTaskQueue.pFreeElems = &PerTaskSpool[0];

    /* Initialize the msg queue for a-periodical task*/
    queue_init(&AperTaskQueue[0].MsgQueue,ZigbeeEventList,MAC_ZIGBEE_EVENT);
    queue_init(&AperTaskQueue[TASK_APER_APP-TASK_APER_NWK].MsgQueue,AppMsgQueue,MAX_APP_MSG_NUM);

} // End of functions

/******************************************************************************/
//@FunctionName:  schedul_resetPerTask                                        */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void schedul_resetPerTask()
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/

    /*Remove all periodical task*/
    DISABLE_GLOBAL_INT();
    for (int i = TASK_PER_MAC_RX; i <= TASK_PER_MAC_TRANSACTION; i++)
    {        
        while(schedul_removeTask(i));
    }
    ENABLE_GLOBAL_INT();
    return;

} // End of functions


/******************************************************************************/
//@FunctionName: schedul_addTask                		  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL schedul_addTask(P_FNC par_pTask, TASK_PRIORITY   par_eTaskPior, WORD*   par_TaskData)
{
	/*********** Variable declaration ***********/
    P_TASK_QUEUE_ELEM            pCurrentPos;   
    BOOL                             bReturn;
	/*********** Function body		 ***********/
    bReturn = FALSE;
    DISABLE_GLOBAL_INT();
    if (par_eTaskPior < TASK_APER_NWK)  /* Periodical task */
    {
        //get free slot
        pCurrentPos =  PerTaskQueue.pFreeElems;
        PerTaskQueue.pFreeElems = pCurrentPos->pNext;

        if(pCurrentPos != NULL)
        {
            //add data to the element
            pCurrentPos->sTask.dwTaskData   = (WORD)par_TaskData;
            pCurrentPos->sTask.pFunc        = par_pTask;
            pCurrentPos->sTask.bSuspended   = FALSE;   
            pCurrentPos->pNext = NULL;

            //add to the appropriate queue
            if(PerTaskQueue.TaskQueue[par_eTaskPior].pLast == NULL)
            {
                PerTaskQueue.TaskQueue[par_eTaskPior].pLast = pCurrentPos;
                PerTaskQueue.TaskQueue[par_eTaskPior].pFirst = pCurrentPos;

            }
            else
            {
                PerTaskQueue.TaskQueue[par_eTaskPior].pLast->pNext = pCurrentPos;
                PerTaskQueue.TaskQueue[par_eTaskPior].pLast        = pCurrentPos;
            }
            bReturn = TRUE;
        }
        else
        {
            bReturn = FALSE;
        }
    }    
    ENABLE_GLOBAL_INT();

    return bReturn;
} // End of functions

/******************************************************************************/
//@FunctionName:  schedul_suspend											  */
//@Description:	  suspend the current task to perform later,                  */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void schedul_suspend(TASK_PRIORITY   par_eTaskPior)
{
    if(PerTaskQueue.TaskQueue[par_eTaskPior].pFirst != NULL)   
        PerTaskQueue.TaskQueue[par_eTaskPior].pFirst->sTask.bSuspended = TRUE;
}

/******************************************************************************/
//@FunctionName: schedul_activate											  */
//@Description:	active the current task                                       */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void schedul_activate(TASK_PRIORITY   par_eTaskPior)
{
    if(PerTaskQueue.TaskQueue[par_eTaskPior].pFirst != NULL)   
        PerTaskQueue.TaskQueue[par_eTaskPior].pFirst->sTask.bSuspended = FALSE;
}

/******************************************************************************/
//@FunctionName: schedul_getParam                   	  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void* schedul_getParam(TASK_PRIORITY par_eTaskPior)
{
    if(PerTaskQueue.TaskQueue[par_eTaskPior].pFirst != NULL)   
        return PerTaskQueue.TaskQueue[par_eTaskPior].pFirst->sTask.dwTaskData;                
	return NULL;
} // End of functions

/******************************************************************************/
//@FunctionName: schedul_removeTask     				  	                  */
//@Description:	 Remove the first task in the appropriate queue               */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL schedul_removeTask(TASK_PRIORITY   par_eTaskPior)
{
	/*********** Variable declaration ***********/
    P_TASK_QUEUE_ELEM                pCurrentPos;   
    BOOL                             bReturn;
	/*********** Function body		 ***********/
    DISABLE_GLOBAL_INT();
    //remove the first task in the queue
    pCurrentPos =  PerTaskQueue.TaskQueue[par_eTaskPior].pFirst;
    if(pCurrentPos != NULL)
    {
        
        if(pCurrentPos->pNext == NULL)
        {
            PerTaskQueue.TaskQueue[par_eTaskPior].pLast = NULL;
            PerTaskQueue.TaskQueue[par_eTaskPior].pFirst = NULL;
        }
        else
        {
            PerTaskQueue.TaskQueue[par_eTaskPior].pFirst= pCurrentPos->pNext;
        }

        //add the released task to task-spool
        pCurrentPos->pNext = PerTaskQueue.pFreeElems;
        PerTaskQueue.pFreeElems = pCurrentPos;

        bReturn = TRUE;
    }
    else
    {
        bReturn = FALSE;
    }

    ENABLE_GLOBAL_INT();
    
    return bReturn;
} // End of functions


