/*******************************************************************************/
//Module: NWK                                                                  */   
//File Name: nwk_routing                                                       */
//Description: 	This file will implement the routing procedure defined for
//              Zigbee network  											   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//07/03/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/


/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "types.h"
#include "nwk_routing.h"
#include "nwk_common.h"
#include "mac_types.h"
#include "mac_primitives.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/


/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
#if ZIGBEE_DEVICE_FFD == 1                  /* Code for FFD only */
/* Cskip of the device for hierarchical routing
*/
UINT16              Cskip;
/*Index for router and device
*/
UINT16              RouterAllocAddrMap;
UINT16              DeviceAllocAddrMap;

// Declaration: Array of struct for Neighbor Table of size NWK_NEIGHBOR_TBLE_SIZE
extern NWK_NEIGHBOR_NODE*  nwk_neighborTable[];
/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/

/************************************************************************/
/*                      Function                                        */
/************************************************************************/
/*-----------------------------------------------------------------------------
                                ADDR ALLOCATION
-----------------------------------------------------------------------------*/
/******************************************************************************/
//@FunctionName: routing_init    						  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    routing_init()
{
	/*********** Variable declaration ***********/
    UINT8       nTem;
    UINT16      nPower;
	INT16		n1;
	INT16		n2;
	/*********** Function body		 ***********/
    nTem = nwk_ib.nwkMaxDepth-NwkInfo.BcnPayload.DeviceDepth-1;
    nPower = 1;
    for (UINT8 i = 0; i<nTem; i++)
        nPower = nPower*nwk_ib.nwkMaxRouters;

    if (nwk_ib.nwkMaxRouters == 1)
        Cskip = 1 + nwk_ib.nwkMaxChildren*(nTem);
    else
	{
        if (nwk_ib.nwkMaxRouters == 0)
            Cskip = -1 - nwk_ib.nwkMaxChildren + nwk_ib.nwkMaxRouters + nwk_ib.nwkMaxChildren*nPower;
        else
        {
            n1 = nwk_ib.nwkMaxRouters + nwk_ib.nwkMaxChildren*nPower-1 - nwk_ib.nwkMaxChildren;
            n2 = nwk_ib.nwkMaxRouters-1;
		    Cskip = (UINT16)n1/n2;
        }            
	}

    if (Cskip == 0)
    {
        /*not available for association*/
        mlmeSetRequest(MAC_ASSOCIATION_PERMIT,&nTem);            
    }

    RouterAllocAddrMap = 0x00;
    DeviceAllocAddrMap = 0x00;
} // End of functions

/******************************************************************************/
//@FunctionName: routing_allocDistAddr										  */
//                 										  	                  */
//@Description:	This function will implement the distributed addr assignment  */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT16   routing_allocDistAddr(BOOL   bRouter)
{
	/*********** Variable declaration ***********/    
	/*********** Function body		 ***********/
    if (bRouter)
    {
        /*Search for free slot*/
        for (UINT8 i = 0;i<nwk_ib.nwkMaxRouters;i++)
        {
            if(!((1<<i)&RouterAllocAddrMap))
            {
                RouterAllocAddrMap = RouterAllocAddrMap | (1<<i);
                return NwkInfo.NwkAddr+1+i*Cskip;    
            }
        }        
    }
    else
    {
        /*Search for free slot*/
        for (UINT8 i = 0;i<nwk_ib.nwkMaxChildren-nwk_ib.nwkMaxRouters;i++)
        {
            if(!((1<<i)&DeviceAllocAddrMap))
            {
                DeviceAllocAddrMap = DeviceAllocAddrMap | (1<<i);
                return NwkInfo.NwkAddr+nwk_ib.nwkMaxRouters*Cskip+i+1;    
            }
        }        
    }
    return 0; /*no address available*/
} // End of functions

/******************************************************************************/
//@FunctionName: routing_checkFreeAddr										  */
//                 										  	                  */
//@Description:	 This function will implement the distributed addr assignment */
//               Check the free addrs
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL   routing_checkFreeAddr(BOOL   bRouter)
{
	/*********** Variable declaration ***********/    
	/*********** Function body		 ***********/
    if (bRouter)
    {
        /*Search for free slot*/
        for (UINT8 i = 0;i<nwk_ib.nwkMaxRouters;i++)
            if(!((1<<i)&RouterAllocAddrMap))
                return TRUE;    
    }
    else
    {
        /*Search for free slot*/
        for (UINT8 i = 0;i<nwk_ib.nwkMaxChildren-nwk_ib.nwkMaxRouters;i++)
            if(!((1<<i)&DeviceAllocAddrMap))
                return TRUE;    
    }
    return FALSE; /*no address available*/
} // End of functions




/*-----------------------------------------------------------------------------
                                ROUTING DECISION
-----------------------------------------------------------------------------*/
/******************************************************************************/
//@FunctionName: routing_getNextNodeInHier									  */
//@Description:	This function will apply hierarchical routing to find the 
//               next network node for a given destination                    */
//               Calculation is based on neighbors table                      */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
NWK_NEIGHBOR_NODE*  routing_getNextNodeInHier(UINT16    DesAddr)
{
	/*********** Variable declaration ***********/    
    UINT8       SubTreeIndex;
	/*********** Function body		 ***********/
    /*Check if this addr is in range of child of the device*/
    if (DesAddr > NwkInfo.NwkAddr
        &&  DesAddr <= NwkInfo.NwkAddr+nwk_ib.nwkMaxRouters*Cskip+nwk_ib.nwkMaxChildren-nwk_ib.nwkMaxRouters)
    {
        SubTreeIndex = NWK_NEIGHBOR_TBLE_SIZE;
        /*Search for appropriate child in the NT*/
        for (UINT8  index = 0; index < NWK_NEIGHBOR_TBLE_SIZE; index++)
        {
            if ((nwk_neighborTable[index] != NULL)
                && (nwk_neighborTable[index]->Relationship == NWK_CHILD))
            {
                /*Destination is the child of the device*/
                if(nwk_neighborTable[index]->NetworkAddress == DesAddr) 
                    return nwk_neighborTable[index];
                 
                /*Destination not is child but within the  tree of the device*/
                if ((nwk_neighborTable[index]->NetworkAddress < DesAddr)
                    && (DesAddr < nwk_neighborTable[index]->NetworkAddress + Cskip))
                {
                    SubTreeIndex = index;
                }
            }
        }
        if (SubTreeIndex < NWK_NEIGHBOR_TBLE_SIZE)
            return nwk_neighborTable[SubTreeIndex];/*Subtree*/
        else
            /*Destination not found*/
            return NULL;
    }

    /*Destination is not in the range of children*/
    return NwkInfo.pParent;
} // End of functions


/******************************************************************************/
//@FunctionName: routing_getNextHop										      */
//@Description:	 This function will apply routing table to find the 
//               next network node for a given destination                    */
//               Calculation is based on routing table                        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
QWORD*  routing_getNextHop(UINT16    DesAddr)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    return 0;
} // End of functions

#endif //!ZIGBEE_DEVICE_FFD == 1                  /* Code for FFD only */
