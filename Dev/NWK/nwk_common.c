/*******************************************************************************/
//Module: NWK	                                                               */   
//File Name: _nwk_common                                                       */
//Description: 																   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//06/14/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "nwk_common.h"
#include "nwk.h"
#include "utils.h"
#include "ucos_ii.h"
#include "zconf.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/


/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/

/* Network layer information*/
NWK_INFO            NwkInfo;
// Declaration: Network Information Base
NWK_IB              nwk_ib;
// Declaration: Network Descriptor
NWK_DESC*           nwk_desc[MAX_NWK_DESC_SIZE];
// Declaration: Array of struct for Neighbor Table of size NWK_NEIGHBOR_TBLE_SIZE
NWK_NEIGHBOR_NODE*  nwk_neighborTable[NWK_NEIGHBOR_TBLE_SIZE];
	
/* Current child index
*/
UINT8               nCurrentChild = NWK_NEIGHBOR_TBLE_SIZE;

/* From Zdo */
extern          UINT8           confPermitJoinDuration;
/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/


/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/******************************************************************************/
//@FunctionName: This function will be called in the scanning procedure to    */
//               update the neighbor table and network table                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void  nwk_updateBeacon(MLME_BEACON_NOTIFY_INDICATION* BcnNotif)
{
	/*********** Variable declaration ***********/
	SUPER_FRAME*	superFrm; 
    UINT8           index;
	/*********** Function body		 ***********/	
    /*Check if neighbors exist */
    superFrm = (SUPER_FRAME*)&BcnNotif->PanDescriptor.SuperframeSpec;
    for (index = 0;index<NWK_NEIGHBOR_TBLE_SIZE;index++)
    {
        if (nwk_neighborTable[index] != NULL)
        {
            if (((BcnNotif->PanDescriptor.CoordAddrMode      == AM_SHORT_16) &&
                 (BcnNotif->PanDescriptor.CoordAddress.Short == nwk_neighborTable[index]->NetworkAddress)) ||
                 ((BcnNotif->PanDescriptor.CoordAddrMode     == AM_EXTENDED_64) &&
                  (BcnNotif->PanDescriptor.CoordAddress.Extended  == nwk_neighborTable[index]->ExtendedAddress )))
                    return; /*Existed*/
        }
    }

    /*Get free entry*/
    for (index = 0;index<NWK_NEIGHBOR_TBLE_SIZE;index++)
        if (nwk_neighborTable[index] == NULL)
            break;

    /* Check */
	if (index == NWK_NEIGHBOR_TBLE_SIZE){
		// omit the beacon
	}
	else {
		//Update Neighbor Table
		nwk_neighborTable[index] =	(NWK_NEIGHBOR_NODE*)mem_getMem(sizeof(NWK_NEIGHBOR_NODE));
        if (nwk_neighborTable[index])
        {		    
		    nwk_neighborTable[index]->PanID             =   BcnNotif->PanDescriptor.CoordPANId;			    //required
            if (BcnNotif->PanDescriptor.CoordAddrMode  == AM_SHORT_16)
            {
		        nwk_neighborTable[index]->NetworkAddress	=	BcnNotif->PanDescriptor.CoordAddress.Short;	//required                
                nwk_neighborTable[index]->ExtendedAddress   =   INVALID_EXT_ADDR;
            }
            if (BcnNotif->PanDescriptor.CoordAddrMode  == AM_EXTENDED_64)
            {
                nwk_neighborTable[index]->NetworkAddress    = INVALID_SHORT_ADDR;
                nwk_neighborTable[index]->ExtendedAddress   = BcnNotif->PanDescriptor.CoordAddress.Extended;
            }   
            if (superFrm->PanCoord == TRUE)
                nwk_neighborTable[index]->DeviceType	    =	ZIGBEE_PAN_COOR;
            else
                nwk_neighborTable[index]->DeviceType	    =	ZIGBEE_ROUTER;      //required			
            //RxOnWhenIdle only router in nonbeacon or coor.
		    nwk_neighborTable[index]->RxOnWhenIdle	    =	TRUE;//((superFrm->BeaconOrder == 15) || (superFrm->PanCoord == TRUE));  
            
		    nwk_neighborTable[index]->Relationship	    =	NWK_NO_RELATION;											//required
		    nwk_neighborTable[index]->Depth		        =	BcnNotif->Payload.DeviceDepth;
		    nwk_neighborTable[index]->BeaconOrder	    =   superFrm->BeaconOrder;	
		    nwk_neighborTable[index]->PermitJoining	    =	superFrm->AssoPermit;
#if ZIGBEE_DEVICE_TYPE == ZIGBEE_ROUTER
            nwk_neighborTable[index]->PotentialParent   =   ((BcnNotif->Payload.RouterCapacity == TRUE) 
                                                            || (BcnNotif->Payload.EndDeviceCapacity == TRUE));                                                            
#elif ZIGBEE_DEVICE_TYPE == ZIGBEE_ENDPOINT
            nwk_neighborTable[index]->PotentialParent   =   (BcnNotif->Payload.EndDeviceCapacity == TRUE);
#else
            nwk_neighborTable[index]->PotentialParent  = FALSE; /*Pan coord.*/
#endif

		    nwk_neighborTable[index]->LogicalChannel	        =	BcnNotif->PanDescriptor.LogicalChannel;
		    nwk_neighborTable[index]->IncomingBeaconTimestamp	=	BcnNotif->PanDescriptor.TimeStamp;
		    nwk_neighborTable[index]->BeaconTxTimeOffset	    =	BcnNotif->Payload.TxOffset;						   				    
        }
	}

    /* If we are in the discovery */
    if (NwkInfo.CurRequest == NWK_STATE_DISCOVERY)
    { 
		/* Update Network Description */
        for (index = 0;index<MAX_NWK_DESC_SIZE;index++)
            if (nwk_desc[index] != NULL && 
                nwk_desc[index]->PanID == BcnNotif->PanDescriptor.CoordPANId)
            {
                /*Update the network*/
                if (superFrm->AssoPermit)
		            nwk_desc[index]->PermitJoining	=	TRUE;
                return;
            }

        for (index = 0;index<MAX_NWK_DESC_SIZE;index++)
            if (nwk_desc[index] == NULL)
                break;
        if (index < MAX_NWK_DESC_SIZE)
        {
		    nwk_desc[index] = mem_getMem(sizeof(NWK_DESC));
            if (nwk_desc[index])
            {                   
		        nwk_desc[index]->PanID	=	BcnNotif->PanDescriptor.CoordPANId;
		        nwk_desc[index]->LogicalChannel	=	BcnNotif->PanDescriptor.LogicalChannel;
		        nwk_desc[index]->StackProfile	=	BcnNotif->Payload.StackProfile;
		        nwk_desc[index]->ZigbeeVersion	=	BcnNotif->Payload.nwkProtocolVersion;
		        nwk_desc[index]->BeaconOrder	=	superFrm->BeaconOrder;
		        nwk_desc[index]->SuperframeOrder= 	superFrm->SuperFrmOrder;
		        nwk_desc[index]->PermitJoining	=	superFrm->AssoPermit;
            }
        }
    }
	
} // End of functions

/******************************************************************************/
//@FunctionName: This function will check in the nwk Description table if
//               there exist such a network with the same PANID               */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL  nwk_checkNwkExisted(UINT16    PanId)
{
	/*********** Variable declaration ***********/
	UINT8 index;
	/*********** Function body		 ***********/
	
	for (index = 0 ; index < MAX_NWK_DESC_SIZE; index ++){
		if (nwk_desc[index] != NULL && 
            (UINT16)nwk_desc[index]->PanID	==  (UINT16)PanId){
			return TRUE;
		}
	}
		
    return FALSE;
} // End of functions

/******************************************************************************/
//@FunctionName: This function will check if we should chose the channel for  */
//               a new network. The choice depends on implementation
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT8  nwk_choseChannel(UINT32 ScanChannel)
{
	/*********** Variable declaration ***********/
    return  DEFAULT_CHANNEL;    
	/*********** Function body		 ***********/
} // End of functions

/******************************************************************************/
//@FunctionName: This function will search in the neighbor table a router 
//               to join								  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
NWK_NEIGHBOR_NODE*  nwk_findRouterInNT(UINT16   PanId, DWORD  ScanChannels)
{
	/*********** Variable declaration ***********/	
	/*********** Function body		 ***********/	
    ScanChannels = 0;
    /*TBD: consider the link quality*/
	for (UINT8  index = 0; index < NWK_NEIGHBOR_TBLE_SIZE; index++){
		if ( (nwk_neighborTable[index] != NULL)
             && (nwk_neighborTable[index]->PanID     == PanId)
             && (nwk_neighborTable[index]->PermitJoining) 
            && (nwk_neighborTable[index]->PotentialParent))
                return nwk_neighborTable[index];
	}	
    return NULL;
} // End of functions

/******************************************************************************/
//@FunctionName: This function will search in the neighbor table a node that
//               has the same 64-bits address
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
NWK_NEIGHBOR_NODE*  nwk_findInNT(WORD   NwkAddr)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/	
	for (UINT8 counter = 0; counter < NWK_NEIGHBOR_TBLE_SIZE ; counter++ )
	{
		if ( nwk_neighborTable[counter]
            && nwk_neighborTable[counter]->NetworkAddress == NwkAddr){
			return nwk_neighborTable[counter];
		}
	}
    
    return NULL;
} // End of functions

/******************************************************************************/
//@FunctionName: This function will search in the neighbor table a node that
//               has the same 64-bits address
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
NWK_NEIGHBOR_NODE*  nwk_findNodeInNT(ADDRESS*    pExAddr)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/	
	for (UINT8 counter = 0; counter < NWK_NEIGHBOR_TBLE_SIZE ; counter++ )
	{
		if ( nwk_neighborTable[counter]
            && nwk_neighborTable[counter]->ExtendedAddress == pExAddr->Extended){
			return nwk_neighborTable[counter];
		}
	}
    
    return NULL;
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT16 nwk_findExtAddrInNT(ADDRESS*    pExAddr)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/	
	for (UINT8 counter = 0; counter < NWK_NEIGHBOR_TBLE_SIZE ; counter++ )
	{
		if ( nwk_neighborTable[counter]
            && nwk_neighborTable[counter]->ExtendedAddress == pExAddr->Extended){
			return nwk_neighborTable[counter]->NetworkAddress;
		}
	}    
    return INVALID_EXT_ADDR;
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT8 nwk_findIndexInNT(UINT8 AddrMode, ADDRESS*  pSrcAddr)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/	
	for (UINT8 counter = 0; counter < NWK_NEIGHBOR_TBLE_SIZE ; counter++ )
	{
		if ( nwk_neighborTable[counter])
        {
            if ((AddrMode == AM_SHORT_16)
                &&(nwk_neighborTable[counter]->NetworkAddress == pSrcAddr->Short))
			    return counter;
            if ((AddrMode == AM_EXTENDED_64)
                &&(nwk_neighborTable[counter]->ExtendedAddress == pSrcAddr->Extended))
			    return counter;
		}
	}    
    return NWK_NEIGHBOR_TBLE_SIZE;
} // End of functions


/******************************************************************************/
//@FunctionName: This function check if a  node is parent                     */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL nwk_IsParent(ADDRESS*  pExAddr)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
	for (UINT8 index = 0 ; index < NWK_NEIGHBOR_TBLE_SIZE;  index++){
		if (nwk_neighborTable[index]
            && ((nwk_neighborTable[index]->ExtendedAddress == pExAddr->Extended) || 
                (nwk_neighborTable[index]->NetworkAddress == pExAddr->Short)) 
            && (nwk_neighborTable[index]->Relationship == NWK_PARRENT))
			return  TRUE;
	}
	
    return FALSE;
} // End of functions
#if ZIGBEE_DEVICE_FFD == 1                  /* Code for FFD only */
/******************************************************************************/
//@FunctionName: This function check if a  node is child                      */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL nwk_IsChild(ADDRESS*  pExAddr)
{
	/*********** Variable declaration ***********/	
	UINT8 index;
	/*********** Function body		 ***********/
	for (index = 0 ; index < NWK_NEIGHBOR_TBLE_SIZE;  index++){
		if (nwk_neighborTable[index]
            && ((nwk_neighborTable[index]->ExtendedAddress   == pExAddr->Extended) || 
                (nwk_neighborTable[index]->NetworkAddress    == pExAddr->Short)) 
            && (nwk_neighborTable[index]->Relationship == NWK_CHILD))
			return  TRUE;
	}
	
    return FALSE;
} // End of functions
#endif //!ZIGBEE_DEVICE_FFD == 1                  /* Code for FFD only */
/******************************************************************************/
//@FunctionName: This function will get extended addr from short add of 
//               a node in the NT (neighbors table)                           */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
ADDRESS*    nwk_getExAddInNT(UINT16 ShortAddr)
{
	/*********** Variable declaration ***********/
	UINT8 index;
	/*********** Function body		 ***********/
	for (index = 0 ; index < NWK_NEIGHBOR_TBLE_SIZE;  index++){
		if (nwk_neighborTable[index]
            && nwk_neighborTable[index]->NetworkAddress == ShortAddr)
			return  (ADDRESS*)&nwk_neighborTable[index]->ExtendedAddress;
	}
	
    return NULL;
	
} // End of functions

/******************************************************************************/
//@FunctionName: This function will get extended addr from short add of 
//               a node in the NT (neighbors table)                           */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT16     nwk_getShortAddInNT(ADDRESS* pExAddr)
{
	/*********** Variable declaration ***********/
	UINT8 index;
	/*********** Function body		 ***********/
	for (index = 0 ; index < NWK_NEIGHBOR_TBLE_SIZE;  index++){
		if (nwk_neighborTable[index]
            && nwk_neighborTable[index]->ExtendedAddress == pExAddr->Extended)
			return  nwk_neighborTable[index]->NetworkAddress;
	}
	
    return INVALID_SHORT_ADDR;
} // End of functions

/******************************************************************************/
//@FunctionName: Set relationship to a node in the NT   	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void      nwk_setRelationship(QWORD* pExAddr, NWK_NEIG_RELATION Relationship)
{
	/*********** Variable declaration ***********/
	UINT8 index;
	/*********** Function body		 ***********/
	for (index = 0 ; index < NWK_NEIGHBOR_TBLE_SIZE;  index++){
		if (nwk_neighborTable[index]
            && nwk_neighborTable[index]->ExtendedAddress == *pExAddr)
		{
			nwk_neighborTable[index]->Relationship	  =	 Relationship;
			break;
		}
	}
	
} // End of functions

#if     ZIGBEE_DEVICE_FFD == TRUE
/******************************************************************************/
//@FunctionName: This function will remove a child in the NT due to a 
//               disassociation cmd                                           */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
NWK_STATUS nwk_removeChild(QWORD* pDevAddress)
{
	/*********** Variable declaration ***********/	
    UINT8           index;
	/*********** Function body		 ***********/

    /*Get the child */
    for (index = 0;index<NWK_NEIGHBOR_TBLE_SIZE;index++)
    {
        if (nwk_neighborTable[index] != NULL
            && nwk_neighborTable[index]->ExtendedAddress == *pDevAddress)
        {
		    mem_freeMem(nwk_neighborTable[index]);
            nwk_neighborTable[index] =	NULL;

            /* not FULL: Change permit joint */
            if (confPermitJoinDuration == 0x00)
            {
                index = 1;    
                mlmeSetRequest(MAC_ASSOCIATION_PERMIT,&index);
            }

            return nwk_SUCCESS;
        }
    }

    return nwk_UNKNOWN_DEVICE;
} // End of functions


/******************************************************************************/
//@FunctionName: Add new child in the NT
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
NWK_STATUS  nwk_addNewChild(QWORD* pDevAddress, WORD NwkAddr,CAPACITY_INFO CapInfor, 
                            BOOL securityUse, UINT8 aclEntry)
{
	/*********** Variable declaration ***********/	
    UINT8           nFreeIndex;
    UINT8           index;
	/*********** Function body		 ***********/
    nFreeIndex  = NWK_NEIGHBOR_TBLE_SIZE;

    /*Get free entry*/
    for (index = 0;index<NWK_NEIGHBOR_TBLE_SIZE;index++)
    {
        if (nFreeIndex  == NWK_NEIGHBOR_TBLE_SIZE
            && nwk_neighborTable[index] == NULL)
            nFreeIndex = index;
        if (nwk_neighborTable[index] != NULL
            && nwk_neighborTable[index]->ExtendedAddress == *pDevAddress)
        {
            return nwk_ALREADY_PRESENT;
        }
    }
    /* Check */
	if (nFreeIndex < NWK_NEIGHBOR_TBLE_SIZE)
    {	    
		nwk_neighborTable[nFreeIndex] =	(NWK_NEIGHBOR_NODE*)mem_getMem(sizeof(NWK_NEIGHBOR_NODE));
        if (nwk_neighborTable[nFreeIndex])
        {
		    nwk_neighborTable[nFreeIndex]->PanID             =	NwkInfo.NwkPANId;
		    nwk_neighborTable[nFreeIndex]->ExtendedAddress	=	*pDevAddress;		//required			
		    nwk_neighborTable[nFreeIndex]->NetworkAddress	=	NwkAddr;			//required
            if (CapInfor.DeviceType)
		        nwk_neighborTable[nFreeIndex]->DeviceType	    =	ZIGBEE_ROUTER;			//required			
            else
                nwk_neighborTable[nFreeIndex]->DeviceType	    =	ZIGBEE_ENDPOINT;			//required			
		    nwk_neighborTable[nFreeIndex]->RxOnWhenIdle	    =	CapInfor.RxOnWhenIdle;		
		    nwk_neighborTable[nFreeIndex]->Relationship	    =	NWK_CHILD;	
		    nwk_neighborTable[nFreeIndex]->Depth		    =	NwkInfo.BcnPayload.DeviceDepth+1;
		    nwk_neighborTable[nFreeIndex]->BeaconOrder	    =	NwkInfo.NwkBeaconOrder;
		    nwk_neighborTable[nFreeIndex]->PermitJoining	=	(CapInfor.DeviceType == TRUE); /*Need to check*/
		    nwk_neighborTable[nFreeIndex]->PotentialParent	=	FALSE;
		    nwk_neighborTable[nFreeIndex]->LogicalChannel	=   NwkInfo.NwkChannel;
		    nwk_neighborTable[nFreeIndex]->IncomingBeaconTimestamp	=	0x00;                  /*Not used for child*/
		    nwk_neighborTable[nFreeIndex]->BeaconTxTimeOffset	    =	0x00;                  /*Not used for child*/
            return nwk_SUCCESS;
        }
	}	
    
    return nwk_TABLE_FULL;
} // End of functions


/******************************************************************************/
//@FunctionName: This function will return next child in the NT               */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
NWK_NEIGHBOR_NODE*      nwk_getFirstChild(BOOL bFirstChild)
{
	/*********** Variable declaration ***********/
    
	/*********** Function body		 ***********/
    /* Reset current child index */
    nCurrentChild = NWK_NEIGHBOR_TBLE_SIZE;
    for (UINT8  index = 0; index <NWK_NEIGHBOR_TBLE_SIZE; index++)
    {
        if (nwk_neighborTable[index] != NULL
            && nwk_neighborTable[index]->Relationship == NWK_CHILD)
        {
            nCurrentChild = index;
            return nwk_neighborTable[index];
        }
    }
    return NULL;
} // End of functions

/******************************************************************************/
//@FunctionName: This function will return next child in the NT               */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
NWK_NEIGHBOR_NODE*      nwk_getNextChild(BOOL bFirstChild)
{
	/*********** Variable declaration ***********/
    
	/*********** Function body		 ***********/
    for (UINT8  index = nCurrentChild; index <NWK_NEIGHBOR_TBLE_SIZE; index++)
    {
        if (nwk_neighborTable[index] != NULL
            && nwk_neighborTable[index]->Relationship == NWK_CHILD)
        {
            nCurrentChild = index;
            return nwk_neighborTable[index];
        }
    }
    return NULL;
} // End of functions


#endif     //!ZIGBEE_DEVICE_FFD == TRUE

/******************************************************************************/
//@FunctionName: This function will check if there exist such a nwk in the 
//               network desctiptor. After that, it will release the memory
//               holding by nwk desc    				  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL nwk_checkNwk(WORD  PanID, DWORD    ChannelList)
{
	/*********** Variable declaration ***********/
    BOOL        bReturn;
	/*********** Function body		 ***********/
    ChannelList = 0;
    bReturn     = FALSE;
	for (UINT8 index = 0 ; index < MAX_NWK_DESC_SIZE; index ++){
		if (nwk_desc[index] != NULL)
        {
            if(nwk_desc[index]->PanID ==  PanID)
		        bReturn =  TRUE;
            
            /*End of the discovery, remove the memory*/
            mem_freeMem(nwk_desc[index]);
		}
	}       
    
    return bReturn;
} // End of functions

#if     ZIGBEE_DEVICE_FFD == TRUE
/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nwk_inhibitJoin(BOOL  bInhibit)
{
	/*********** Variable declaration ***********/
    UINT8   nValue;
	/*********** Function body		 ***********/
    NwkInfo.BcnPayload.EndDeviceCapacity = 0;
    NwkInfo.BcnPayload.RouterCapacity    = 0;    
    /* FULL: Change permit joint */
    nValue = 0;
    mlmeSetRequest(MAC_ASSOCIATION_PERMIT,&nValue);
} // End of functions



/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nwk_schedulBeacon()
{
	/*********** Variable declaration ***********/
    UINT32          nOffset;
    UINT32          nBeaconDuration;
    UINT32          nSFDuration;
    UINT32          nNumSlot;
    UINT32          EarliestStamp;
    UINT8           nTemp;
    UINT32*         Slots;
	/*********** Function body		 ***********/
    /*Calculate start time*/
    DISABLE_GLOBAL_INT();
    nOffset = OSTmrTime;
    ENABLE_GLOBAL_INT();

    Slots = (UINT32*)mem_getMem(NWK_NEIGHBOR_TBLE_SIZE*sizeof(UINT32));
    if (Slots == NULL)  /*Unrecoverable error*/
        err(ERR_UNRECOVERABLE,NULL,NULL,0);
    

    /*Get the earliest beacon*/
    /*Calculate the current slot*/
    EarliestStamp = nOffset;
    for (UINT8  index = 0; index <NWK_NEIGHBOR_TBLE_SIZE; index++)
    {
        if (nwk_neighborTable[index] != NULL
            && (nwk_neighborTable[index]->DeviceType == ZIGBEE_ROUTER || 
                nwk_neighborTable[index]->DeviceType == ZIGBEE_PAN_COOR ))
        {
            /*Offset from beginning of beacon of the parent*/
            if ( EarliestStamp >= nwk_neighborTable[index]->IncomingBeaconTimestamp)
                EarliestStamp = nwk_neighborTable[index]->IncomingBeaconTimestamp;
        }
    }    

    nBeaconDuration = ((UINT32)(aBaseSuperframeDuration / aUnitBackoffPeriod) << NwkInfo.NwkBeaconOrder);
    nSFDuration     = ((UINT32)(aBaseSuperframeDuration / aUnitBackoffPeriod) << NwkInfo.NwkSuperFrmOrder);

    /*Calculate the current slot*/
    for (UINT8  index = 0; index <NWK_NEIGHBOR_TBLE_SIZE; index++)
    {
        if (nwk_neighborTable[index] != NULL
            && (nwk_neighborTable[index]->DeviceType == ZIGBEE_ROUTER || 
                nwk_neighborTable[index]->DeviceType == ZIGBEE_PAN_COOR ))
        {
            /*Offset from beginning of beacon of the parent*/
            Slots[index] = nwk_neighborTable[index]->IncomingBeaconTimestamp - EarliestStamp;
            Slots[index] = Slots[index]%nBeaconDuration; 
            Slots[index] = Slots[index]/nSFDuration+1;
        }
        else
            Slots[index] = 0;
    }    

    /*Search for free slot*/
    nNumSlot = (UINT32)nBeaconDuration/nSFDuration;
    if (nNumSlot == 0)
        err(ERR_UNRECOVERABLE,NULL,NULL,0);
    for (UINT8  index = 1; index <=nNumSlot; index++)
    {
        nTemp = index;
        for (UINT8  i = 0; i <NWK_NEIGHBOR_TBLE_SIZE; i++)
        {
            if (Slots[i] == nTemp) /*Slot used in the neighbors*/
            {
                nTemp = 0;
                break;
            }
        }
        if (nTemp != 0) /*Slot is not used*/
            break;
    }
    
    if (nTemp == 0)
        err(ERR_UNRECOVERABLE,NULL,NULL,0);

    /*Get offset with earliest stamp*/
    nOffset         = nOffset - EarliestStamp;    
    nOffset         = nOffset % nBeaconDuration;

    /*Check current time with free slot*/
    nTemp -=1;
    if(nOffset > (UINT32)nSFDuration*nTemp)
        nOffset = (nBeaconDuration - nOffset) + (UINT32)nSFDuration*nTemp;
    else
        nOffset = (UINT32)nSFDuration*nTemp - nOffset;

    mem_freeMem(Slots);

    NwkInfo.BeaconOffset = nOffset;
} // End of functions
#endif     //!ZIGBEE_DEVICE_FFD == TRUE


/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	  Assume that maximum is 16 record in the NT                  */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT16 nwk_getNtBitMap() 
{
	/*********** Variable declaration ***********/
    UINT16      Bitmap;
	/*********** Function body		 ***********/
    Bitmap = 0;
    for (UINT8  index = 0; index <NWK_NEIGHBOR_TBLE_SIZE; index++)
        if((nwk_neighborTable[index] != NULL)
            && ((nwk_neighborTable[index]->Relationship == NWK_CHILD) || 
            (nwk_neighborTable[index]->Relationship ==NWK_PARRENT)))
            Bitmap |= (1<<index);

    return Bitmap;
} // End of functions   

