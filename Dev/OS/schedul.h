/*******************************************************************************/
//Module: OS       	                                                           */   
//File Name: schedul.h                                                         */
//Description: This file contains functions for scheduling task in MAC layer   */
//  -------------------------------------------------------------              */
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//11/29/2006        pnthai          Create                                     */
//  -------------------------------------------------------------              */
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef SCHEDULER_H
#define SCHEDULER_H


/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "types.h"
/************************************************************************/
/*					Type Definition										*/
/************************************************************************/

/*Priority of a task */
typedef enum 
{
    /* Periodic task */
    TASK_PER_MAC_PERIODICAL = 0,    
    TASK_PER_MAC_TX,
    TASK_PER_MAC_RX,
    TASK_PER_MAC_TRANSACTION,
    /* Periodic task */
    TASK_APER_NWK,
    TASK_APER_APP,

    /* Undefined */
    TASK_PRIOR_UNDEFINED
}TASK_PRIORITY;


/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/


/******************************************************************************/
//@FunctionName:  schedul_init                                                */
//@Description:	Initialize scheduler declaration                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void schedul_init();

/******************************************************************************/
//@FunctionName: schedul_removeTask     				  	                  */
//@Description:	 Remove the first task in the appropriate queue  declarations */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL schedul_removeTask(TASK_PRIORITY   par_eTaskPior);

/******************************************************************************/
//@FunctionName: schedul_addTask                		  	                  */
//@Description:	 Add task to scheduler declaration                            */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL schedul_addTask(P_FNC par_pTask, TASK_PRIORITY   par_eTaskPior, WORD*   par_TaskData);




#endif  // !SCHEDULER_H
