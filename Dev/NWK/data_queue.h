/*******************************************************************************/
//Module: NWK	                                                               */   
//File Name: data_queue.h                                                      */
//Description: This file will implement type difinitions to handle the data queue*/
//             Will be used in NWK and APS
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//07/16/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef DATAQUEUE
#define DATAQUEUE
/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "types.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/
/************************************************************************/
/*					Type Definition										*/
/************************************************************************/

/* Data record
*/
typedef struct  
{
    UINT8       Status:3;
    UINT8       Reserved:5;
    UINT8       Handle;
    void*       pData;    
    UINT8       nLength;
    UINT8       NextIndex;
}DATA_REC;


/*Data queue*/
typedef struct  
{
    DATA_REC*   RecList;
    UINT8       FirstIndex:4;
    UINT8       LastIndex :4;
    UINT8       Size;
}DATA_QUEUE;
/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/

/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/


/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/******************************************************************************/
//@FunctionName: dataqueue_init 											  */
//@Description:	 This function will init data queue                           */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void dataqueue_init(DATA_QUEUE* pQueue,DATA_REC*   pRecList, UINT8 Size);


/******************************************************************************/
//@FunctionName: dataqueue_addRec											  */	
//@Description:	  This function will add a new record into the queue          */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT8 dataqueue_addRec(DATA_QUEUE* pQueue, BYTE*   pData, 
                       UINT8  nLength, UINT8 Index, UINT8 Handle,UINT8 Status);

/******************************************************************************/
//@FunctionName: dataqueue_getRec											  */		
//@Description:	  This function will get the oldest record from the queue     */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT8 dataqueue_getRec(DATA_QUEUE* pQueue,UINT8 Status);


/******************************************************************************/
//@FunctionName: dataqueue_removeRec										  */	
//@Description:	This function will remove a record from the queue             */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL dataqueue_removeRec(DATA_QUEUE* pQueue, UINT8  RemovedIndex);

#endif  //!DATA_QUEUE
