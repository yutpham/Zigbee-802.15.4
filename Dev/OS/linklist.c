/*******************************************************************************/
//Module: OS	                                                               */   
//File Name: linklist.c                                                        */
//Description: This component implement a simple one-way message               */
/*
             head                   tail
              |                       |
            -------               -------           
           | pNext | --->...---> | pNext | ------>NULL
            -------               -------
           |       |             |       |   
            -------               -------
Specific functions are implemented on this queue
    - Push
    - Pop
    - Search/Remove a msg according to the comparison function
*/
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//05/11/2007        pnthai          Created                                    */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "linklist.h"
#include "utils.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/

/* Typedef for a msg in llist*/
typedef struct  
{
    void*   pNext;
}LL_MSG;
typedef     LL_MSG*        P_LL_MSG;

/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/



/************************************************************************/
/*                      Function                                        */
/************************************************************************/
/******************************************************************************/
//@FunctionName: llist_init  										  	      */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void llist_init(P_LINK_LIST par_pLList)
{
	/*********** Variable declaration ***********/
    LL_MSG*        pRec;
    LL_MSG*        pTem;
	/*********** Function body		 ***********/
    /* Free the records */
    pRec = par_pLList->pHead;
    while (pRec !=NULL)
    {
        pTem = (LL_MSG*)pRec->pNext;
        mem_freeMem(pRec);  /*TBD: assume all element has size smaller than 32*/
        pRec = pTem;
    }    
    par_pLList->pHead = NULL;
    par_pLList->pTail = NULL;
} // End of functions

/******************************************************************************/
//@FunctionName:  llist_add      						  	                  */
//@Description:	  This function will push the message to the head of 
//                the link list                                               */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL    llist_add(P_LINK_LIST par_pLList, void*   par_pMsg)
{
	/*********** Variable declaration ***********/
    P_LL_MSG        pMsg;    
	/*********** Function body		 ***********/
    pMsg = (P_LL_MSG)par_pMsg;
    if(pMsg != NULL)
    {
        pMsg->pNext   = NULL;
        if (par_pLList->pHead == NULL)
        {
            par_pLList->pHead = par_pMsg;               /*Update the head if empty*/
            par_pLList->pTail = par_pMsg;
        }
        else
        {   
            pMsg                = par_pLList->pTail;
            pMsg->pNext         = par_pMsg;        /*Update the tail*/
            par_pLList->pTail   = par_pMsg;
        }
        return TRUE;
    }
    else
        return FALSE;

} // End of functions

/******************************************************************************/
//@FunctionName:  llist_search 			    			  	                  */
//@Description:	  This function will search a message in the llist according 
//                to the match of comparison function                         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void*    llist_search(P_LINK_LIST par_pLList, CMP     par_cmpFunc, 
                      void*  par_pParam,UINT8  cmpOption )
{
	/*********** Variable declaration ***********/
    P_LL_MSG     pCurrentMsg;
	/*********** Function body		 ***********/
    pCurrentMsg = par_pLList->pHead;
    while (pCurrentMsg!=NULL)                       /* Loop through the queue*/   
    {        
        if (par_cmpFunc(pCurrentMsg,par_pParam, cmpOption))
            return pCurrentMsg;
        pCurrentMsg = pCurrentMsg->pNext;
    }
    return  NULL;
} // End of functions

/******************************************************************************/
//@FunctionName:  llist_search 			    			  	                  */
//@Description:	  This function will remove a message in the llist according 
//                to the match of comparison function                         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void*    llist_remove(P_LINK_LIST par_pLList, CMP     par_cmpFunc, 
                      void*  par_pParam, UINT8  cmpOption)
{
	/*********** Variable declaration ***********/
    P_LL_MSG     pCurrentMsg;
    P_LL_MSG     pPrev;
	/*********** Function body		 ***********/
    pCurrentMsg = par_pLList->pHead;
    pPrev       = NULL;
    while (pCurrentMsg!=NULL)                       /* Loop through the queue*/   
    {        
        if (par_cmpFunc(pCurrentMsg,par_pParam, cmpOption))
            break;
        pPrev       = pCurrentMsg;
        pCurrentMsg = pCurrentMsg->pNext;
    }
    if(pCurrentMsg == NULL)                             /* No msg found or no msg in queue */
        return NULL;
    if((pPrev == NULL) || (pCurrentMsg->pNext == NULL))     /* One msg in queue, remove it*/                 
    {
        if (pCurrentMsg->pNext == NULL)                      /* Remove tail */
        {
            par_pLList->pTail   = pPrev;
            pPrev->pNext        = NULL;
        }
        if (pPrev == NULL)                                   /* Remove head */
            par_pLList->pHead = pCurrentMsg->pNext;         /*Remove the msg from queue*/        
    }
    else
        pPrev->pNext = pCurrentMsg->pNext;                  /*Remove the msg from queue*/
    return  pCurrentMsg;
} // End of functions

/******************************************************************************/
//@FunctionName:  llist_search 			    			  	                  */
//@Description:	  This function will remove a message in the l-list according 
//                to the match msg pointer                                    */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void*    llist_removeRec(P_LINK_LIST par_pLList, void*   par_pMsg)
{
	/*********** Variable declaration ***********/
    P_LL_MSG     pCurrentMsg;
    P_LL_MSG     pPrev;
	/*********** Function body		 ***********/
    pCurrentMsg = par_pLList->pHead;
    pPrev       = NULL;
    while (pCurrentMsg!=NULL)                       /* Loop through the queue*/   
    {        
        if (pCurrentMsg == par_pMsg)
            break;
        pPrev       = pCurrentMsg;
        pCurrentMsg = pCurrentMsg->pNext;
    }
    if(pCurrentMsg == NULL)                             /* No msg found or no msg in queue */
        return NULL;
    if((pPrev == NULL) || (pCurrentMsg->pNext == NULL))     /* One msg in queue, remove it*/                 
    {
        if (pCurrentMsg->pNext == NULL)                      /* Remove tail */
        {
            par_pLList->pTail   = pPrev;
            pPrev->pNext        = NULL;
        }
        if (pPrev == NULL)                                  /* Remove head */
            par_pLList->pHead = pCurrentMsg->pNext;         /*Remove the msg from queue*/        
    }
    else
        pPrev->pNext = pCurrentMsg->pNext;                  /*Remove the msg from queue*/
    return  pCurrentMsg;
} // End of functions
