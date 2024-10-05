/*******************************************************************************/
//Module: 	APL                                                                */   
//File Name: aps_ib                                                            */
//Description: This file will implement the internal information block of APS  */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//07/02/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "types.h"
#include "zconf.h"
#include "utils.h"
#include "aps_ib.h"
#include "linklist.h"
#include "mac_types.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/
#define         ADDRMAP_SHORT_ADDR      0x02
#define         ADDRMAP_EXT_ADDR        0x03

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/



/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
/* Address mapping 
*/
LINK_LIST       AddrMap;
UINT8           nNumofAddrEntry;
/* Binding table
*/
LINK_LIST       BindingTbl;
UINT8           nNumofBindingEntry;
/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/
/******************************************************************************/
//@FunctionName: ib_cmpAddrMap								                  */
//@Description:	This function will compare two address entry                  */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL    ib_cmpAddrMap(ADDR_MAP_ENTRY* pAddrEntry, void*  Sample,UINT8 cmpOption )
{
	/*********** Variable declaration ***********/
    
	/*********** Function body		 ***********/   
    if(cmpOption == ADDRMAP_SHORT_ADDR)
        return (pAddrEntry->ShortAddr == *(UINT16*)Sample);
    if (cmpOption == ADDRMAP_EXT_ADDR)  
        return (pAddrEntry->ExtAddr == *(QWORD*)Sample);
	return FALSE;
} // End of functions


/******************************************************************************/
//@FunctionName:  ib_cmpBindingEntry										  */
//@Description:	  This function will compare to find an appropriate binding   */
//               entry. Value to compare will be corresponding to cmpOption   */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL    ib_cmpBindingEntry(BINDING_ENTRY*   pBindEntry, void*   pSample, UINT8  cmpOption)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
	return FALSE;
} // End of functions

/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/*-----------------------------------------------------------------------
                          ADDRESS MAPPING FUNCTION
------------------------------------------------------------------------*/
/******************************************************************************/
//@FunctionName: ib_initAddrMap              								  */
//@Description:	This function will initialize the addr mapping                */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    ib_initAddrMap()
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    llist_init(&AddrMap);
    nNumofAddrEntry= 0;
} // End of functions


/******************************************************************************/
//@FunctionName: ib_addAddrEntry      										  */
//@Description:	 This function will add a new entry in the addr mapping       */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL    ib_addAddrEntry(UINT16  ShortAddr,QWORD*    pExtAddr)
{
	/*********** Variable declaration ***********/
    ADDR_MAP_ENTRY*     pEntry;
	/*********** Function body		 ***********/
    pEntry  = (ADDR_MAP_ENTRY*)mem_getMem(sizeof(ADDR_MAP_ENTRY));
    if(pEntry)
    {
        pEntry->ExtAddr     = *pExtAddr;
        pEntry->ShortAddr   = ShortAddr;
        llist_add(&AddrMap,pEntry);
        nNumofAddrEntry++;
        return TRUE;
    }
    return FALSE;
} // End of functions

/******************************************************************************/
//@FunctionName:  ib_searchAddrEntry     									  */
//@Description:	 This function will search a entry in the addr mapping        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
ADDR_MAP_ENTRY*    ib_searchAddrEntry(UINT16  ShortAddr,QWORD*    pExtAddr)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    if (ShortAddr != INVALID_SHORT_ADDR)
        return llist_search(&AddrMap,ib_cmpAddrMap,&ShortAddr,ADDRMAP_SHORT_ADDR);
    if (pExtAddr != NULL)
        return llist_search(&AddrMap,ib_cmpAddrMap,pExtAddr,ADDRMAP_EXT_ADDR);
    return NULL;
} // End of functions

/******************************************************************************/
//@FunctionName:  ib_removeAddrEntry     									  */
//@Description:	 This function will remove a entry in the addr mapping        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    ib_removeAddrEntry(UINT16  ShortAddr,QWORD*    pExtAddr)
{
	/*********** Variable declaration ***********/
    ADDR_MAP_ENTRY*     pEntry;
	/*********** Function body		 ***********/
    if (ShortAddr != INVALID_SHORT_ADDR)
    {
        pEntry = llist_remove(&AddrMap,ib_cmpAddrMap,(void*)&ShortAddr,ADDRMAP_SHORT_ADDR);
        if (pEntry)
        {
            nNumofAddrEntry--;
            mem_freeMem(pEntry);
        }
    }
    if (pExtAddr != NULL)
    {
        pEntry = llist_remove(&AddrMap,ib_cmpAddrMap,pExtAddr,ADDRMAP_EXT_ADDR);
        if (pEntry)
        {
            nNumofAddrEntry--;
            mem_freeMem(pEntry);
        }
    }
} // End of functions


/******************************************************************************/
//@FunctionName: ib_mappedToNwkAddr     				  	                  */
//@Description:	 This function will map from extended address to nwk addr     */
//               with information in the NT table and address mapping table   */    
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT16  ib_mappedToNwkAddr(ADDRESS*     pExtAddr)
{
	/*********** Variable declaration ***********/
    UINT16              NwkAddr;
    ADDR_MAP_ENTRY*     pAddMap;
	/*********** Function body		 ***********/
    /*Search for coor.*/
    NwkAddr = nwk_getShortAddInNT(pExtAddr);
    if (NwkAddr != INVALID_SHORT_ADDR)
        return NwkAddr;
    else /*Search in the mapping table*/
    {
        pAddMap = ib_searchAddrEntry(INVALID_SHORT_ADDR,pExtAddr);
        if (pAddMap)
            return pAddMap->ShortAddr;
    }

    return INVALID_SHORT_ADDR;
} // End of functions

/******************************************************************************/
//@FunctionName: ib_mappedToNwkAddr     				  	                  */
//@Description:	 This function will map from nwk addr to extended address     */
//               with information in the NT table and address mapping table   */    
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
QWORD*  ib_mappedToIEEEAddr(UINT16      NwkAddr)
{
	/*********** Variable declaration ***********/
    QWORD*              pExtAddr;
    ADDR_MAP_ENTRY*     pAddMap;
	/*********** Function body		 ***********/
    /*Search for coor.*/
    pExtAddr = nwk_getExAddInNT(pExtAddr);
    if (NwkAddr != NULL)
        return pExtAddr;
    else /*Search in the mapping table*/
    {
        pAddMap = ib_searchAddrEntry(NwkAddr,NULL);
        if (pAddMap)
            return &pAddMap->ExtAddr;
    }

    return NULL;
} // End of functions


/*-----------------------------------------------------------------------
                          BINDING TABLE
------------------------------------------------------------------------*/
/******************************************************************************/
//@FunctionName:   ib_initBindingTbl     							          */
//@Description:	 This function will initialize the binding table              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    ib_initBindingTbl()
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    llist_init(&BindingTbl);
    nNumofBindingEntry  = 0;
} // End of functions

/******************************************************************************/
//@FunctionName:  ib_addBindingEntry                                          */
//@Description:	This function will add a binding entry into the binding table */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL ib_addBindingEntry(WORD   SrcAddr,       UINT8 srcEP, 
                        WORD   DstAddr,       UINT8 dstEP, 
                        BYTE    numClusterIds,  BYTE*    clusterIds)
{
	/*********** Variable declaration ***********/
    BINDING_ENTRY*  pBindingEntry;
	/*********** Function body		 ***********/
    if (numClusterIds > MAX_BINDING_CLUSTER_IDS)
        return FALSE;

    pBindingEntry = mem_getMem(sizeof(BINDING_ENTRY));
    if (pBindingEntry)
    {
        nNumofBindingEntry++;
        pBindingEntry->srcAddr  = SrcAddr;
        pBindingEntry->srcEP    = srcEP;
        pBindingEntry->dstAddr  = DstAddr;
        pBindingEntry->dstEP    = dstEP;
        pBindingEntry->numClusterIds = numClusterIds;
        for (int i = 0; i< numClusterIds; i++)
            pBindingEntry->clusterIdList[i] = clusterIds[i];
        llist_add(&BindingTbl,pBindingEntry);
        return TRUE;
    }
    return FALSE;
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL  ib_addCluster(BINDING_ENTRY*   pEntry, UINT16     ClusterId)
{
	/*********** Variable declaration ***********/
    UINT8       index;
	/*********** Function body		 ***********/
    /*Check the cluster id*/
    for (index = 0;index<pEntry->numClusterIds; index++)
    {
        if (pEntry->clusterIdList[index] == ClusterId)
            break;
    }
    if (pEntry->clusterIdList[index] != ClusterId) /*Not existed cluster id*/
    {
        /*Add*/
        if (pEntry->numClusterIds < MAX_BINDING_CLUSTER_IDS)
        {
            pEntry->clusterIdList[pEntry->numClusterIds] = ClusterId;
            pEntry->numClusterIds++;
            return TRUE;
        }
        else
        /*Full*/
            return FALSE;
    }    
    else
        return TRUE;
} // End of functions


/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL  ib_removeCluster(BINDING_ENTRY*   pEntry, UINT16     ClusterId)
{
	/*********** Variable declaration ***********/
    UINT8       index;
    UINT8       nTemp;;
	/*********** Function body		 ***********/
    /*Check the cluster id*/
    for (index = 0;index<pEntry->numClusterIds; index++)
    {
        if (pEntry->clusterIdList[index] == ClusterId)
            break;
    }
    if (pEntry->clusterIdList[index] == ClusterId) /* existed cluster id*/
    {
        /*Remove*/
        nTemp = index;
        for (index = nTemp;index<pEntry->numClusterIds; index++)
            pEntry->clusterIdList[index] = pEntry->clusterIdList[index+1];
        pEntry->numClusterIds--;
        return TRUE;
    }
    else /*Not existed */
        return TRUE;
} // End of functions

/******************************************************************************/
//@FunctionName:   ib_removeBindingEntry									  */
//@Description: This function will remove and binding entry in the binding    */
//              table                                                         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void ib_removeBindingEntry(BINDING_ENTRY*   pEntry)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    if (pEntry)
    {
        nNumofBindingEntry--;
        llist_removeRec(&BindingTbl,(void*)pEntry);
        mem_freeMem(pEntry);
    }
} // End of functions


/******************************************************************************/
//@FunctionName: ib_searchBindingEntry      								  */
//@Description:	This function will search an binding entry matched the given  */
//            source addr & end point and dest addr and endpoint. 
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BINDING_ENTRY*  ib_searchBindingEntry(WORD    SrcAddr, UINT8   srcEP,
                                      WORD    DestAddr, UINT8  dstEP)
{
	/*********** Variable declaration ***********/
    BINDING_ENTRY*      pCurrentMsg;
	/*********** Function body		 ***********/
    pCurrentMsg = (BINDING_ENTRY*)BindingTbl.pHead;
    while (pCurrentMsg!=NULL)                    /* Loop through the queue*/   
    {        
        if ((pCurrentMsg->srcAddr    == SrcAddr)
            && (pCurrentMsg->srcEP      == srcEP)
            && (pCurrentMsg->dstAddr    == DestAddr)
            && (pCurrentMsg->dstEP      == dstEP))
        {
            return pCurrentMsg;
        }
        pCurrentMsg = pCurrentMsg->pNext;
    }
    return NULL;
} // End of functions

/******************************************************************************/
//@FunctionName: ib_searchBindingEntry      								  */
//@Description:	This function will search an binding link matched the given  */
//            source addr & end point and dest addr and endpoint and cluster. 
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BINDING_ENTRY*  ib_searchBindingLink(WORD    SrcAddr, UINT8   srcEP,UINT8 ClusterId,
                                      WORD   DestAddr, UINT8  dstEP)
{
	/*********** Variable declaration ***********/
    BINDING_ENTRY*      pCurrentMsg;
    UINT8               index;
	/*********** Function body		 ***********/
    pCurrentMsg = (BINDING_ENTRY*)BindingTbl.pHead;
    while (pCurrentMsg!=NULL)                    /* Loop through the queue*/   
    {        
        if ((pCurrentMsg->srcAddr    == SrcAddr)
            && (pCurrentMsg->srcEP      == srcEP)
            && (pCurrentMsg->dstAddr    == DestAddr)
            && (pCurrentMsg->dstEP      == dstEP))
        {
            for (index = 0;index<pCurrentMsg->numClusterIds; index++)
                if (pCurrentMsg->clusterIdList[index] == ClusterId)
                    return pCurrentMsg;
        }
        pCurrentMsg = pCurrentMsg->pNext;
    }
    return NULL;
} // End of functions


/******************************************************************************/
//@FunctionName: ib_searchIterativeBindingEntry								  */
//@Description:	This function will search an binding entry matched the given  */
//            source addr and end point. If srcAddr & srcEP matched again	  */
//            the last call of this function, the next entry will be returned */                                                              
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BINDING_ENTRY*  ib_searchIterativeBindingEntry(WORD    SrcAddr, UINT8  srcEP,
                                               BINDING_ENTRY*   pCurEntry)
{
	/*********** Variable declaration ***********/
    BINDING_ENTRY*      pCurrentMsg;
	/*********** Function body		 ***********/
    /* If this search matched the previous search */
    if ((pCurEntry != NULL) 
        && (pCurEntry->srcAddr    == SrcAddr)
        && (pCurEntry->srcEP      == srcEP))
        /* Continue the search */
        pCurrentMsg = (BINDING_ENTRY*)pCurEntry->pNext;        
    else
        /* Restart the search */
        pCurrentMsg = (BINDING_ENTRY*)BindingTbl.pHead;

    while (pCurrentMsg!=NULL)                       /* Loop through the queue*/   
    {        
        if ((pCurrentMsg->srcAddr == SrcAddr)
            && (pCurrentMsg->srcEP == srcEP))
        {
            pCurEntry =  pCurrentMsg;
            return  pCurrentMsg;
        }
        pCurrentMsg = pCurrentMsg->pNext;
    }
    return NULL;
} // End of functions
