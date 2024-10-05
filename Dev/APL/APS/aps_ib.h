/*******************************************************************************/
//Module: APL	                                                               */   
//File Name: aps_ib                                                            */
//Description: This file will implement the internal information block of APS  */

//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//07/02/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef APS_IB_H
#define APS_IB_H

/************************************************************************/
/*					Include files										*/
/************************************************************************/


/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/
/* Binding entry
*/
typedef struct  
{
  void*     pNext;
  WORD     srcAddr;
  UINT8     srcEP;
  WORD     dstAddr;
  UINT8     dstEP;
  UINT8     numClusterIds;
  UINT8     clusterIdList[MAX_BINDING_CLUSTER_IDS];  
}BINDING_ENTRY;

/* Address mapping entry
*/
typedef struct  
{
    void*       pNext;
    UINT16      ShortAddr;
    QWORD       ExtAddr;
}ADDR_MAP_ENTRY;


/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/


/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/

/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/*-----------------------------------------------------------------------
                          ADDRESS MAPPING FUNCTION
------------------------------------------------------------------------*/
/******************************************************************************/
//@FunctionName:  ib_initAddrMap              								  */
//@Description:	 This function will initialize the addr mapping               */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    ib_initAddrMap();

/******************************************************************************/
//@FunctionName:  ib_addAddrEntry      										  */
//@Description:	This function will add a new entry in the addr mapping        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL    ib_addAddrEntry(UINT16  ShortAddr,QWORD*    pExtAddr);


/******************************************************************************/
//@FunctionName: ib_searchAddrEntry											  */
//@Description:	This function will search a entry in the addr mapping         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
ADDR_MAP_ENTRY*    ib_searchAddrEntry(UINT16  ShortAddr,QWORD*    pExtAddr);


/******************************************************************************/
//@FunctionName: ib_removeAddrEntry       									  */
//@Description:	 This function will remove a entry in the addr mapping        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    ib_removeAddrEntry(UINT16  ShortAddr,QWORD*    pExtAddr);


/******************************************************************************/
//@FunctionName: ib_mappedToNwkAddr     				  	                  */
//@Description:	 This function will map from extended address to nwk addr     */
//               with information in the NT table and address mapping table   */    
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT16  ib_mappedToNwkAddr(ADDRESS*     pExtAddr);

/******************************************************************************/
//@FunctionName: ib_mappedToNwkAddr     				  	                  */
//@Description:	 This function will map from nwk addr to extended address     */
//               with information in the NT table and address mapping table   */    
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
QWORD*  ib_mappedToIEEEAddr(UINT16      NwkAddr);

/*-----------------------------------------------------------------------
                          BINDING TABLE
------------------------------------------------------------------------*/
/******************************************************************************/
//@FunctionName: ib_initBindingTbl              							  */
//@Description:	 This function will initialize the binding table              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    ib_initBindingTbl();

/******************************************************************************/
//@FunctionName: ib_addBindingEntry                                           */
//@Description:	This function will add a binding entry into the binding table */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL ib_addBindingEntry(WORD   SrcAddr,        UINT8 srcEP, 
                        WORD   DstAddr,        UINT8 dstEP, 
                        BYTE   numClusterIds,  BYTE*    clusterIds);

/******************************************************************************/
//@FunctionName: ib_removeBindingEntry  									  */
//@Description: This function will remove and binding entry in the binding    */
//              table                                                         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void ib_removeBindingEntry(BINDING_ENTRY*   pEntry);


/******************************************************************************/
//@FunctionName: ib_searchBindingEntry      								  */
//@Description:	This function will search an binding link matched the given  */
//            source addr & end point and dest addr and endpoint and cluster. 
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BINDING_ENTRY*  ib_searchBindingLink(WORD    SrcAddr, UINT8   srcEP,UINT8 ClusterId,
                                     WORD    DestAddr, UINT8  dstEP);

/******************************************************************************/
//@FunctionName: ib_searchBindingEntry      								  */
//@Description:	This function will search an binding entry matched the given  */
//            source addr & end point and dest addr and endpoint and cluster. 
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BINDING_ENTRY*  ib_searchBindingEntry(WORD    SrcAddr, UINT8   srcEP,
                                      WORD    DestAddr, UINT8  dstEP);

/******************************************************************************/
//@FunctionName: ib_searchIterativeBindingEntry								  */
//@Description: This function will search an binding entry matched the given  */
//            source addr and end point. If srcAddr & srcEP matched again     */
//            the last call of this function, the next entry will be returned */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BINDING_ENTRY*  ib_searchIterativeBindingEntry(WORD    SrcAddr, UINT8  srcEP,
                                              BINDING_ENTRY*   pCurEntry);

#endif //!APS_IB_H
