/*******************************************************************************/
//Module: NWK                                                                  */   
//File Name: data_queue.c                                                      */
//Description: This file will implement function to handle the data queue      */
//             Will be used in NWK and APS
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//07/16/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "data_queue.h"

/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/
/************************************************************************/
/*					Type Definition										*/
/************************************************************************/

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
//@FunctionName: dataqueue_init												  */		
//@Description:	This function will init data queue                            */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void dataqueue_init(DATA_QUEUE* pQueue,DATA_REC*   pRecList, UINT8 Size)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    pQueue->RecList = pRecList;
    pQueue->Size    = Size;

    for (UINT8 index = 0; index <Size; index++ )
    {
        pQueue->RecList[index].pData       = NULL;
        pQueue->RecList[index].NextIndex   = Size;
    }
    pQueue->FirstIndex          = Size;
    pQueue->LastIndex           = Size;
} // End of functions


/******************************************************************************/
//@FunctionName: dataqueue_getFreeSlot										  */
//@Description:	 This function will add a new record into the queue           */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT8 dataqueue_getFreeSlot(DATA_QUEUE* pQueue)
{
	/*********** Variable declaration ***********/
    UINT8                   index;
 	/*********** Function body		 ***********/
    /*Search for free slot */
    for (index = 0; index <pQueue->Size; index++ )
        if (pQueue->RecList[index].pData == NULL)
            return index;

    /*Not available to add*/
    return pQueue->Size;    
} // End of functions

/******************************************************************************/
//@FunctionName: dataqueue_addRec                                             */
//@Description:	This function will add a new record into the queue            */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT8 dataqueue_addRec(DATA_QUEUE* pQueue, BYTE*   pData, 
                       UINT8  nLength, UINT8 Index, UINT8 Handle,UINT8 Status)
{
	/*********** Variable declaration ***********/
 	/*********** Function body		 ***********/
    /*Search for free slot */
    if (pQueue->RecList[Index].pData == NULL)
    {
            pQueue->RecList[Index].pData            = pData;
            pQueue->RecList[Index].Handle           = Handle;        
            pQueue->RecList[Index].nLength          = nLength;
            pQueue->RecList[Index].Status           = Status;
    
            /*Update next index*/
            /*First packet*/
            if (pQueue->FirstIndex == pQueue->Size)             
                pQueue->FirstIndex = Index;
            else
                pQueue->RecList[pQueue->LastIndex].NextIndex = Index;

            pQueue->LastIndex = Index;
            return Handle;
    }

    /*Not available to add*/
    return pQueue->Size;    
} // End of functions

/******************************************************************************/
//@FunctionName: dataqueue_getRec											  */
//@Description:	 This function will get the oldest record from the queue      */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT8 dataqueue_getRec(DATA_QUEUE* pQueue, UINT8 Status)
{
	/*********** Variable declaration ***********/
    UINT8            Index;
	/*********** Function body		 ***********/
    Index       = pQueue->FirstIndex;
    while (Index != pQueue->Size && pQueue->RecList[Index].pData) /* is the valid packet*/
    {
        if (pQueue->RecList[Index].Status == Status)
            return Index;                       

        Index = pQueue->RecList[Index].NextIndex;
    }

    return pQueue->Size;    
} // End of functions


/******************************************************************************/
//@FunctionName: dataqueue_removeRec										  */
//@Description:	This function will remove a record from the queue             */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL dataqueue_removeRec(DATA_QUEUE* pQueue, UINT8  RemovedIndex)
{
	/*********** Variable declaration ***********/
    UINT8            Index;   
	/*********** Function body		 ***********/
    Index = pQueue->FirstIndex;
    /*First packet is removed */
    if (RemovedIndex == pQueue->FirstIndex)
    {
        pQueue->FirstIndex = pQueue->RecList[RemovedIndex].NextIndex;
        pQueue->RecList[RemovedIndex].pData         = NULL;
        pQueue->RecList[RemovedIndex].NextIndex     = pQueue->Size;

        /*Last packet is removed */
        if (RemovedIndex == pQueue->LastIndex)
            pQueue->LastIndex = pQueue->Size;

        return TRUE;
    }
    else
    {
        while (Index != pQueue->Size && pQueue->RecList[Index].pData) /* is the valid data*/
        {
            if (pQueue->RecList[Index].NextIndex == RemovedIndex)      
                break;         
            Index       = pQueue->RecList[Index].NextIndex;
        }

        if ((RemovedIndex == pQueue->RecList[Index].NextIndex) 
            && (Index != pQueue->Size))
        {
            pQueue->RecList[Index].NextIndex = pQueue->RecList[RemovedIndex].NextIndex;
        
            pQueue->RecList[RemovedIndex].pData         = NULL;
            pQueue->RecList[RemovedIndex].NextIndex     = pQueue->Size;

            /*Last packet is removed */
            if (RemovedIndex == pQueue->LastIndex)
                pQueue->LastIndex = Index;

            return TRUE;
        }
    }

    return FALSE;
} // End of functions

/******************************************************************************/
//@FunctionName: dataqueue_findRec  										  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT8   dataqueue_findRec(DATA_QUEUE* pQueue, UINT8 nHandler)
{
	/*********** Variable declaration ***********/
    UINT8            Index;
	/*********** Function body		 ***********/
    Index       = pQueue->FirstIndex;
    while (Index != pQueue->Size && pQueue->RecList[Index].pData) /* is the valid packet*/
    {
        if (pQueue->RecList[Index].Handle == nHandler)
            return Index;                       

        Index = pQueue->RecList[Index].NextIndex;
    }

    return pQueue->Size;    
} // End of functions