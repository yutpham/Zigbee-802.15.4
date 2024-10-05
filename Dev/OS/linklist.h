/*******************************************************************************/
//Module: OS                                                                   */   
//File Name: linklist.h                                                        */
//Description: This component implement type definition for 				   */	
//				a simple one-way link list message     						   */
/*
            -------           -------           
           | pNext | ------> | pNext | ------>.....NULL
            -------           -------
           |       |         |       |   
            -------           -------
*/
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//05/11/2007        pnthai          Created                                    */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef LLIST_H
#define LLIST_H
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

/* Type of message using in the Message Queue*/
typedef     struct  
{
    void*      pHead;
    void*      pTail;
}LINK_LIST;           

typedef LINK_LIST*          P_LINK_LIST;           

/* Type of the compare function */
typedef BOOL (*CMP)(void *par_llElement, void *parg, UINT8  cmpOption);

/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/



/************************************************************************/
/*                      Function                                        */
/************************************************************************/
/******************************************************************************/
//@FunctionName: llist_init  										  	      */
//@Description:	 Initialize list                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void llist_init(P_LINK_LIST par_pLList);

/******************************************************************************/
//@FunctionName:  llist_add      						  	                  */
//@Description:	  This function will push the message to the head of 
//                the link list                                               */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL    llist_add(P_LINK_LIST par_pLList, void*   par_pMsg);

/******************************************************************************/
//@FunctionName:  llist_search 			    			  	                  */
//@Description:	  This function will search a message in the llist according  */
//                to the match of comparison function                         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void*    llist_search(P_LINK_LIST par_pLList, CMP     par_cmpFunc, 
                      void*  par_pParam,UINT8  cmpOption );

/******************************************************************************/
//@FunctionName:  llist_remove 			    			  	                  */
//@Description:	  This function will remove a message in the llist according 
//                to the match of comparison function                         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void*    llist_remove(P_LINK_LIST par_pLList, CMP     par_cmpFunc, 
                      void*  par_pParam, UINT8  cmpOption);

/******************************************************************************/
//@FunctionName:  llist_removeRec 			    			  	              */
//@Description:	  This function will remove a message in the l-list according 
//                to the match msg pointer                                    */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void*    llist_removeRec(P_LINK_LIST par_pLList, void*   par_pMsg);

#endif //!LLIST_H
