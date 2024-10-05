/*******************************************************************************/
//Module: Network Layer                                                        */   
//File Name: nwk_nlme                                                          */
//Description: This file implement the nwk management entity of the network    */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//02/26/2007                   Create                                     */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "nwk.h"
#include "mac_primitives.h"
#include "app_framework.h"
#include "nwk_msg.h"
#include "zdo.h"
#include "zconf.h"
#include "utils.h"
#include "ucos_ii.h"
#include "nwk_routing.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/
#if  ZIGBEE_DEVICE_FFD == 1
/* Leave transaction information */
typedef struct  
{

}NWK_LEAVE_TRANS_INFO;

#endif //!ZIGBEE_DEVICE_FFD == 1

/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
#if  ZIGBEE_DEVICE_TYPE == ZIGBEE_PAN_COOR
extern WORD confPandId;
#endif //!ZIGBEE_DEVICE_FFD == 1


/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/


/************************************************************************/
/*                      Function                                        */
/************************************************************************/



/*************************************************************************************/
//	Function Name: 	nlmeGetRequest						
//	Description: 	This primitive allows the next higher layer of a device can read 
//					and write attributes in the NIB
//	Parameters:		NIBAttribute	-	NWK_IB_ID
//	Return:			nothing
/*************************************************************************************/



NWK_STATUS nlmeGetRequest(BYTE NIBAttribute,void *pNIBAttribValue,UINT8* NIBAttribLength){
	switch (NIBAttribute){
		case NWKSEQUENCENUMBER:
			*((UINT8*)pNIBAttribValue) = nwk_ib.nwkSequenceNumber; 
			*NIBAttribLength = sizeof(nwk_ib.nwkSequenceNumber);	
			break;
		case NWKPASSIVEACKTIMEOUT:
			*((UINT8*)pNIBAttribValue) = nwk_ib.nwkPassiveAckTimeout; 
			*NIBAttribLength = sizeof(nwk_ib.nwkPassiveAckTimeout);	
			break;
		case NWKMAXBROADCASTRETRIES:
			*((UINT8*)pNIBAttribValue) = nwk_ib.nwkMaxBroadcastRetries; 
			*NIBAttribLength = sizeof(nwk_ib.nwkMaxBroadcastRetries);	
			break;	
		case NWKMAXCHILDREN:								
			*((UINT8*)pNIBAttribValue) = nwk_ib.nwkMaxChildren; 
			*NIBAttribLength = sizeof(nwk_ib.nwkMaxChildren);	
			break;
		case NWKMAXDEPTH:
			*((UINT8*)pNIBAttribValue) = nwk_ib.nwkMaxDepth; 
			*NIBAttribLength = sizeof(nwk_ib.nwkMaxDepth);	
			break;	
		case NWKMAXROUTERS:
			*((UINT8*)pNIBAttribValue) = nwk_ib.nwkMaxRouters; 
			*NIBAttribLength = sizeof(nwk_ib.nwkMaxRouters);	
			break;
		case NWKNEIGHBORTABLE:
			*((QWORD**)pNIBAttribValue) = nwk_ib.nwkNeighborTable; 
			*NIBAttribLength = sizeof(nwk_ib.nwkNeighborTable);	        /*TBD: update the length*/
			break;
		case NWKNETWORKBROADCASTDELIVERYTIME:
			*((UINT8*)pNIBAttribValue) = nwk_ib.nwkBroadcastDeliveryTime; 
			*NIBAttribLength = sizeof(nwk_ib.nwkBroadcastDeliveryTime);	
			break;
		case NWKREPORTCOSTANTCOST:		
			*((UINT8*)pNIBAttribValue) = nwk_ib.nwkReportConstantCost; 
			*NIBAttribLength = sizeof(nwk_ib.nwkReportConstantCost);	
			break;
		case NWKROUTEDISCOVERYRETRIESPERMITTED:
			*((UINT8*)pNIBAttribValue) = nwk_ib.nwkRouteDiscoveryRetriesPermitted; 
			*NIBAttribLength = sizeof(nwk_ib.nwkRouteDiscoveryRetriesPermitted);	
			break;
		case NWKROUTETABLE:
			*((QWORD**)pNIBAttribValue) = nwk_ib.nwkRouteTable;          /*TBD: update the length*/   
			*NIBAttribLength = sizeof(nwk_ib.nwkRouteTable);	
			break;
		case NWKSYMLINK:
			*((BOOL*)pNIBAttribValue) = nwk_ib.nwkSymLink; 
			*NIBAttribLength = sizeof(nwk_ib.nwkSymLink);	
			break;										
		case NWKCAPABILITYINFORMATION:
			*((BYTE*)pNIBAttribValue) = nwk_ib.nwkCapabilityInformation; 
			*NIBAttribLength = sizeof(nwk_ib.nwkCapabilityInformation);	
			break;
		case NWKUSETREEADDRALLOC:
			*((BOOL*)pNIBAttribValue)	= nwk_ib.nwkUseTreeAddrAlloc; 
			*NIBAttribLength = sizeof(nwk_ib.nwkUseTreeAddrAlloc);	
			break;
		case NWKUSETREEROUTING:
			*((BOOL*)pNIBAttribValue)	= nwk_ib.nwkUseTreeRouting; 
			*NIBAttribLength = sizeof(nwk_ib.nwkUseTreeRouting);	
			break;	
		case NWKNEXTADDRESS:
			*((UINT16*)pNIBAttribValue) = nwk_ib.nwkNextAddress; 
			*NIBAttribLength = sizeof(nwk_ib.nwkNextAddress);	
			break;
		case NWKAVAILABLEADDRESSES:
			*((UINT16*)pNIBAttribValue) = nwk_ib.nwkAvailableAddresses; 
			*NIBAttribLength = sizeof(nwk_ib.nwkAvailableAddresses);	
			break;
		case NWKADDRESSINCREMENT:
			*((UINT16*)pNIBAttribValue) = nwk_ib.nwkAddressIncrement; 
			*NIBAttribLength = sizeof(nwk_ib.nwkAddressIncrement);	
			break;
		case NWKTRANSACTIONPERSISTENCETIME:
			*((UINT16*)pNIBAttribValue)	= nwk_ib.nwkTransactionPersistenceTime;
			*NIBAttribLength = sizeof(nwk_ib.nwkTransactionPersistenceTime);	
			break;
		default:
            *NIBAttribLength = 0; /*mean UNSUPPORTED_ATTRIBUTE */ 
            return nwk_UNSUPPORTED_ATTRIBUTE;
			break;
		}	
    return nwk_SUCCESS;
}


/*************************************************************************************/
//	Function Name: 	nlmeSetRequest						
//	Description: 	This primitive allows the next higher layer to write the value of 
//					an attribute into the NIB*/
//	Parameters:		nlmeSetRequest_s - NLME_SETREQUEST
//	Return:			nothing
/*************************************************************************************/
NWK_STATUS nlmeSetRequest(BYTE NIBAttribute, UINT8	NIBAttributeLength, void *NIBAttributeValue){
	
	/*********** Function body		 ***********/
    NWK_STATUS      SetStatus;
	/*********** Function body		 ***********/			
    SetStatus = nwk_SUCCESS;
    if(NIBAttribute == NWKSEQUENCENUMBER){
	    nwk_ib.nwkSequenceNumber = *(WORD*)NIBAttributeValue; 
    }
    else if(NIBAttribute == NWKPASSIVEACKTIMEOUT) {
        nwk_ib.nwkPassiveAckTimeout = *(UINT8*)NIBAttributeValue; 
    }	 
     else if(NIBAttribute == NWKMAXBROADCASTRETRIES) {
		    nwk_ib.nwkMaxBroadcastRetries = *(UINT8*)NIBAttributeValue; 
    }	
	else if(NIBAttribute == NWKMAXCHILDREN) {
        nwk_ib.nwkMaxChildren = *(UINT8*)NIBAttributeValue; 
    }
	else if(NIBAttribute == NWKMAXDEPTH) {
        nwk_ib.nwkMaxDepth = *(UINT8*)NIBAttributeValue; 
    }
	else if(NIBAttribute == NWKMAXROUTERS) {
        nwk_ib.nwkMaxRouters = *(UINT8*)NIBAttributeValue; 
	}
	else if(NIBAttribute == NWKNEIGHBORTABLE) {
		nwk_ib.nwkNeighborTable = (void*)NIBAttributeValue;
	}
	else if(NIBAttribute == NWKNETWORKBROADCASTDELIVERYTIME) {
        nwk_ib.nwkBroadcastDeliveryTime = *(UINT8*)NIBAttributeValue;
	}	
	else if(NIBAttribute == NWKREPORTCOSTANTCOST) {
        nwk_ib.nwkReportConstantCost = *(UINT8*)NIBAttributeValue; 
	}		
	else if(NIBAttribute == NWKROUTEDISCOVERYRETRIESPERMITTED) {
        nwk_ib.nwkRouteDiscoveryRetriesPermitted = *(UINT8*)NIBAttributeValue; 
	}	
	else if(NIBAttribute == NWKSYMLINK) {
		nwk_ib.nwkRouteTable = (void*)NIBAttributeValue; 
	}		
	else if(NIBAttribute == NWKROUTEDISCOVERYRETRIESPERMITTED) {
        nwk_ib.nwkSymLink = *(UINT8*)NIBAttributeValue; 
	}	
		
	else if(NIBAttribute == NWKCAPABILITYINFORMATION) {
        nwk_ib.nwkCapabilityInformation = *(UINT8*)NIBAttributeValue; 
	}		
		
	else if(NIBAttribute == NWKUSETREEADDRALLOC) {
        nwk_ib.nwkUseTreeAddrAlloc = *(UINT8*)NIBAttributeValue; 
	}	
	
	else if(NIBAttribute == NWKUSETREEROUTING) {
        nwk_ib.nwkUseTreeRouting = *(UINT8*)NIBAttributeValue; 
	}			
		
	else if(NIBAttribute == NWKNEXTADDRESS) {
        nwk_ib.nwkNextAddress = *(UINT16*)NIBAttributeValue; 
	}	
	
	else if(NIBAttribute == NWKAVAILABLEADDRESSES) {
        nwk_ib.nwkAvailableAddresses = *(UINT16*)NIBAttributeValue; 
	}	
			
	else if(NIBAttribute == NWKADDRESSINCREMENT) {
        nwk_ib.nwkAddressIncrement = *(UINT16*)NIBAttributeValue; 
	}	
		
	else if(NIBAttribute == NWKTRANSACTIONPERSISTENCETIME) {
        nwk_ib.nwkTransactionPersistenceTime = *(UINT8*)NIBAttributeValue; 
	}
	else{
		SetStatus = nwk_UNSUPPORTED_ATTRIBUTE; 
	}	
    
	return SetStatus;
}


/*************************************************************************************/
//	Function Name: 	nlmeResetRequest						
//	Description: 	This primitive allows the next higher layer to request that the 
//					NWk layer performs a reset operation
//	Parameters:		nothing
//	Return:			nothing 
/*************************************************************************************/

void nlmeResetRequest(){
	// Initiate MAC RESET 
	mlmeResetRequest(TRUE); 
}

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nlme_resetNwk(NWK_STATUS   Status)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
	if (Status == nwk_SUCCESS)
	{
		// Set the current state
		NwkInfo.NwkState = NWK_STATE_INIT;				

		// Set NIB to default values
		nwk_ib.nwkSequenceNumber 					= 	(BYTE)ramdom_getRandomByte();	// set to random number
		nwk_ib.nwkPassiveAckTimeout 				= 	0x03;
		nwk_ib.nwkMaxBroadcastRetries 				= 	0x03;
		nwk_ib.nwkMaxChildren 						= 	NWK_MAX_CHILD;
		nwk_ib.nwkMaxDepth 							= 	0x05;
		nwk_ib.nwkMaxRouters 						= 	NWK_MAX_ROUTER;
		nwk_ib.nwkNeighborTable 					= 	NULL;
		nwk_ib.nwkBroadcastDeliveryTime 			= 	nwk_ib.nwkPassiveAckTimeout*nwk_ib.nwkMaxBroadcastRetries; //need to initialize
		nwk_ib.nwkReportConstantCost 				= 	0x00;
		nwk_ib.nwkRouteDiscoveryRetriesPermitted 	= 	nwkcDiscoveryRetryLimit;
		nwk_ib.nwkRouteTable 						= 	NULL;
		nwk_ib.nwkSymLink 							= 	FALSE;					
		nwk_ib.nwkCapabilityInformation				= 	0x00,		      
		nwk_ib.nwkUseTreeAddrAlloc 					= 	USE_TREE_ADDR_ALLOC;
		nwk_ib.nwkUseTreeRouting 					= 	USE_TREE_ROUTING;
		nwk_ib.nwkNextAddress						= 	0x0000;
		nwk_ib.nwkAvailableAddresses 				= 	0x0000;
		nwk_ib.nwkAddressIncrement 					= 	0x0001;
		nwk_ib.nwkTransactionPersistenceTime		= 	0x01f4;

        //inform upper layer
        nlmeResetConfirm(nwk_SUCCESS); 
	}
 	else 
        nlmeResetConfirm(nwk_DISABLE_TRX_FAILURE); 
} // End of functions



/*************************************************************************************/
//	Function Name: 	nlmeSyncRequest
//	Description: 	
//	Parameters:		Track - BOOL
//	Return:			nothing
/*************************************************************************************/
Z_STATUS nlmeSyncRequest(BOOL Track)
{
	/*********** Variable declaration ***********/
    UINT8       nSetValue;
	/*********** Function body		 ***********/
    /*Set network state*/
    if (!nwk_setNwkState(NWK_STATE_SYNC))
        return nwk_NOT_PERMITTED;
		
	if ((Track == FALSE) && (NwkInfo.NwkBeaconOrder == NON_BEACON_NWK))
    {
        if (NwkInfo.pParent->NetworkAddress == INVALID_SHORT_ADDR)
		    return mlmePollRequest(AM_EXTENDED_64,&NwkInfo.pParent->ExtendedAddress,
                                    NwkInfo.NwkPANId,FALSE);
        else
		    return mlmePollRequest(AM_SHORT_16,&NwkInfo.pParent->NetworkAddress,
                                    NwkInfo.NwkPANId,FALSE);
    }
		
	if ((Track == FALSE) && (NwkInfo.NwkBeaconOrder != NON_BEACON_NWK))
	{	
        nSetValue = TRUE;
        mlmeSetRequest(MAC_AUTO_REQUEST, &nSetValue);
        return mlmeSyncRequest(NwkInfo.NwkChannel, FALSE);
	}

	if ((Track == TRUE) && (NwkInfo.NwkBeaconOrder == NON_BEACON_NWK))
		return INVALID_PARAMETER;	

	if ((Track == TRUE) && (NwkInfo.NwkBeaconOrder != NON_BEACON_NWK))
	{
        nSetValue = TRUE;
        mlmeSetRequest(MAC_AUTO_REQUEST, &nSetValue);
        return mlmeSyncRequest(NwkInfo.NwkChannel, TRUE);
	}
    return INVALID_PARAMETER;
} // End of functions

/*************************************************************************************/
//	Function Name: 	nlmeNetworkDiscoveryRequest
//	Description: 	This primitive allows the next higher layer to request that 
//                  the NWK layer discover networks currently operating within the POS
//	Parameters:		ScanChannels - DWORD
//					ScanDuration - UINT8
//	Return:			nothing
/*************************************************************************************/
void nlmeNetworkDiscoveryRequest(DWORD ScanChannels, UINT8 ScanDuration)
{	
    /* Set current request */
    NwkInfo.CurRequest =  NWK_STATE_DISCOVERY;

    /* perform the request */
	if (nwkcCoordinatorCapable == 1) 
    {
		mlmeScanRequest(ACTIVE_SCAN, ScanChannels, ScanDuration); //active	
    }
	else 
		if (MAC_RFD_ACTIVE_SCAN_ENABLE == 0)
        {
			mlmeScanRequest(PASSIVE_SCAN, ScanChannels, ScanDuration); //passive
        }
		else 
			mlmeScanRequest(ACTIVE_SCAN, ScanChannels, ScanDuration); //active
}

/*************************************************************************************/
//	Function Name: 	nlmeJoinRequest
//	Description: 	This primitive allows the next higher layer to request that the NWK layer to JOIN or associate to a parent in NEiGHBOR table
//	Parameters:		nlmeJoinRequest_s - NLME_JOINREQUEST
//	Return:			nothing
/*************************************************************************************/
void nlmeJoinRequest(UINT16	PanID,          BOOL	JoinAsRouter,   BOOL	RejoinNetwork,	
                     DWORD  ScanChannels,   UINT8	ScanDuration,   UINT8	PowerSource, 
                     BOOL	RxOnWhenIdle,	UINT8	MACSecurity)
{	
    
	/*********** Variable declaration ***********/
    NWK_NEIGHBOR_NODE*      pRouter;
    CAPACITY_INFO*          pCapInfo;
    UINT8                   nTemp;
	/*********** Function body		 ***********/
    /* Check if currently joined nwk */
    if ((NwkInfo.NwkState == NWK_STATE_NORMAL_MODE)
        || (NwkInfo.NwkState == NWK_STATE_JOIN_SUCESS))
        nlmeJoinConfirm(PanID,nwk_INVALID_REQUEST);
	else 
	{
        /* Rejoin the network or not */
        if (!RejoinNetwork) /* Join new network */
        {
            pCapInfo              = (CAPACITY_INFO*)&nwk_ib.nwkCapabilityInformation;

            if (JoinAsRouter)
            {
                NwkInfo.NwkJoinedDeviveType = ZIGBEE_ROUTER;
                pCapInfo->DeviceType        = TRUE;                    
                pCapInfo->AllocAddr         = TRUE;
            }
            else
            {
                NwkInfo.NwkJoinedDeviveType = ZIGBEE_ENDPOINT;
                pCapInfo->DeviceType        = FALSE;
                pCapInfo->AllocAddr         = FALSE;                    
            }
            
            pRouter = nwk_findRouterInNT(PanID,ScanChannels);
            if (pRouter)
            {       
                NwkInfo.pParent       = pRouter;
                NwkInfo.NwkPANId      = PanID;
                NwkInfo.NwkChannel    = pRouter->LogicalChannel;

                pCapInfo->PwrSrc      = PowerSource;  
                pCapInfo->RxOnWhenIdle= RxOnWhenIdle;
                pCapInfo->SecurityCap = 0;                
                NwkInfo.CurRequest    = NWK_STATE_JOIN;

                pCapInfo   = &nwk_ib.nwkCapabilityInformation;
                nTemp      = pCapInfo->RxOnWhenIdle;
                mlmeSetRequest(MAC_RX_ON_WHEN_IDLE, &nTemp);

                if (pRouter->NetworkAddress >= INVALID_SHORT_ADDR)
                    mlmeAssociateRequest(pRouter->LogicalChannel,AM_EXTENDED_64,PanID,
                                        (ADDRESS*)&pRouter->ExtendedAddress,
                                        nwk_ib.nwkCapabilityInformation,FALSE);
                else
                    mlmeAssociateRequest(pRouter->LogicalChannel,AM_SHORT_16,PanID,
                                        (ADDRESS*)&pRouter->NetworkAddress,
                                        nwk_ib.nwkCapabilityInformation,FALSE);

            }
            else /*No device exists*/
            {
                nlmeJoinConfirm(PanID,nwk_NOT_PERMITTED);
                return;
            }
        }

        if (RejoinNetwork 
            && NwkInfo.NwkState != NWK_STATE_NORMAL_MODE)   /* Rejoin nwk*/
        {
            NwkInfo.CurRequest = NWK_STATE_REJOIN;
            mlmeScanRequest(ORPHAN_SCAN, ScanChannels,ScanDuration);
        }            
	}    
} // End of functions

#if 	ZIGBEE_DEVICE_FFD == 1

#if     ZIGBEE_DEVICE_TYPE == ZIGBEE_ROUTER

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nlmeJoinDirectRequest(ADDRESS*  pDeviceAddr, CAPACITY_INFO CapInfor)
{
	/*********** Variable declaration ***********/
    NWK_STATUS      status;
	/*********** Function body		 ***********/
    status = nwk_addNewChild(pDeviceAddr,0x00,CapInfor,NwkInfo.SecurityEnable,0x00);    
	nlmeJoinDirectConfirm(pDeviceAddr,  status);
} // End of functions

#endif 

#endif 


/*************************************************************************************/
//	Function Name: 	nlmeLeaveRequest
//	Description: 	
//	Parameters:		
//	Return:			nothing
/*************************************************************************************/

void nlmeLeaveRequest(ADDRESS* pDeviceAddress, BOOL RemoveChildren, BOOL MACSecurityEnable)
{
	/*********** Variable declaration ***********/
    NWK_NEIGHBOR_NODE*	pNode;
	NWK_FRM_TYPE FrameType 	=  NWK_FRM_CMD;
	NWK_CMD_TYPE CmdType	=  NWK_CMD_LEAVE;
	P_NPDU       npdu;
    BYTE         Nsdu;
    BYTE         NsduLength;   
	/*********** Function body		 ***********/
	if (NwkInfo.NwkState != NWK_STATE_NORMAL_MODE){
		nlmeLeaveConfirm(pDeviceAddress, nwk_INVALID_REQUEST);
	}
	else
    {
        /* Remove devices from the network */
        if (pDeviceAddress->Extended != 0 )
        {
            /* Remove this child from the network 2.7.1.7.2*/
            pNode = nwk_findNodeInNT(pDeviceAddress);
            if (!pNode) /* Node does not exist in the NT*/
            {
                nlmeLeaveConfirm(pDeviceAddress,nwk_UNKNOWN_DEVICE);
                return;
            }
            else /* Device found, remove it */   
			{
				//	Check if the device initiating the Leave is a coord or a router
				if ((ZIGBEE_DEVICE_TYPE == ZIGBEE_PAN_COOR) || (ZIGBEE_DEVICE_TYPE == ZIGBEE_ROUTER)){

				/* Begin the transaction */			
                Nsdu        = 0;
                NsduLength  = sizeof(Nsdu);
				npdu = nwk_makeNpdu(NWK_FRM_DATA,NsduLength,Nsdu);
				
				/* Send th LEAVE.request */
				mcpsDataRequest(AM_SHORT_16, NwkInfo.NwkPANId, NwkInfo.NwkAddr, pNode->NetworkAddress,  AM_SHORT_16,
								pNode->PanID,   
								NsduLength, 
								npdu,      
								0, 
								0);								
				}
                else{
					nlmeLeaveConfirm(pDeviceAddress, nwk_INVALID_REQUEST);
				}
            }
        }

        /* Remove itself from the network following 2.7.1.7.1*/
        if (pDeviceAddress->Extended == 0 && !RemoveChildren)
        {            
            /* Initiate self removal procedure */


            /* Clear routing table */ 
            /* TBD */            
            /* Reset the MAC */
            mlmeResetRequest(TRUE);
        }

    }

} // End of functions

#if ZIGBEE_DEVICE_FFD == 1                  /* Code for FFD only */

#if ZIGBEE_DEVICE_TYPE == ZIGBEE_PAN_COOR   /* Code for PAN Coordinator only */

/*************************************************************************************/
//	Function Name: 	nlmeNetworkFormationRequest						
//	Description: 	
//	Parameters:		
//	Return:			
/*************************************************************************************/
void nlmeNetworkFormationRequest(DWORD ScanChannels, UINT8 ScanDuration, UINT8 BeaconOrder, 
                                 UINT8 SuperframeOrder, UINT16 PandId, BOOL BatteryLifeExtension)
{
	if ((ZIGBEE_DEVICE_FFD ==  FALSE)  || 
        ((NwkInfo.NwkState  == NWK_STATE_NORMAL_MODE) && (ZIGBEE_DEVICE_TYPE ==ZIGBEE_PAN_COOR))){
		nlmeNetworkFormationConfirm(nwk_INVALID_REQUEST);
	}
	else{
        /*Set the current state*/ /*TBD: omit ED_SCAN */
        NwkInfo.NwkState        = NWK_STATE_FORMATION;
        NwkInfo.CurRequest      = NWK_STATE_FORMATION;
        NwkInfo.NwkBeaconOrder  = BeaconOrder;
        NwkInfo.NwkSuperFrmOrder= SuperframeOrder;
        NwkInfo.NwkPANId        = PandId;           /* Pre-assigned PanID*/
        NwkInfo.NwkChannelList  = ScanChannels;
        NwkInfo.BatLifeExt      = BatteryLifeExtension;

        NwkInfo.BcnPayload.DeviceDepth          = 0;
        NwkInfo.BcnPayload.EndDeviceCapacity    = ENDDEVICE_CAP;
        NwkInfo.BcnPayload.nwkExtendedPANID     = aExtendedAddress;
        NwkInfo.BcnPayload.RouterCapacity       = ROUTER_CAP;
        NwkInfo.BcnPayload.nwkProtocolVersion   = confProtocolVers;/*Zigbee protocol*/
        NwkInfo.BcnPayload.ProtocolId           = 0;
        NwkInfo.BcnPayload.StackProfile         = confStackProfile;

		mlmeScanRequest(ACTIVE_SCAN, ScanChannels, ScanDuration);
	}	
}

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nlme_formNwk()
{
	/*********** Variable declaration ***********/
    UINT8        setAttributeValue;
	/*********** Function body		 ***********/
    if (NwkInfo.NwkPANId == 0)
    {
        /*TBD*/
    }

    if(!nwk_checkNwkExisted(NwkInfo.NwkPANId))
    {
        /* Not exist such PanId, then, get a channel */
        NwkInfo.NwkChannel = nwk_choseChannel(NwkInfo.NwkChannelList);
        if (NwkInfo.NwkChannel >= ZIGBEE_MIN_CHANNEL && NwkInfo.NwkChannel <= ZIGBEE_MAX_CHANNEL)
        {
            routing_init();

            NwkInfo.NwkAddr = 0x0000;
            mlmeSetRequest(MAC_SHORT_ADDRESS, &NwkInfo.NwkAddr);

            /* Modify necessary PIB attributes */
            setAttributeValue = TRUE;
            mlmeSetRequest(MAC_ASSOCIATION_PERMIT, &setAttributeValue);
            mlmeSetRequest(MAC_RX_ON_WHEN_IDLE, &setAttributeValue);
            setAttributeValue = sizeof(BEACON_PAYLOAD);
            mlmeSetRequest(MAC_BEACON_PAYLOAD_LENGTH, &setAttributeValue);
            mlmeSetRequest(MAC_BEACON_PAYLOAD, &NwkInfo.BcnPayload);
            setAttributeValue = TRUE;
            mlmeSetRequest(MAC_RX_ON_WHEN_IDLE, &setAttributeValue);
            mlmeStartRequest(NwkInfo.NwkPANId,NwkInfo.NwkChannel,NwkInfo.NwkBeaconOrder,
                            NwkInfo.NwkSuperFrmOrder,TRUE,NwkInfo.BatLifeExt,FALSE,FALSE,0);
            return;
        }
    }
                
    nlmeNetworkFormationConfirm(nwk_STARTUP_FAILURE);
} // End of functions


#endif //! ZIGBEE_DEVICE_TYPE == ZIGBEE_PAN_COOR

#if     ZIGBEE_DEVICE_TYPE == ZIGBEE_ROUTER
/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nlmeStartRouterRequest(UINT8 BeaconOrder, UINT8 SuperframeOrder, BOOL BatLifeExt)
{
	/*********** Variable declaration ***********/
    UINT8           nTemp;
    CAPACITY_INFO*  pCapInfor;              
	/*********** Function body		 ***********/
    /* Check if it joined as router */
    if (NwkInfo.NwkJoinedDeviveType != ZIGBEE_ROUTER)
    {
        nlmeStartRouterConfirm(nwk_INVALID_REQUEST);
    }
    else
    {
        /*If already created and now change the config or initializing new router */
        if (((NwkInfo.NwkState == NWK_STATE_NORMAL_MODE) && ((NwkInfo.NwkBeaconOrder  != BeaconOrder) || 
                (NwkInfo.NwkSuperFrmOrder != SuperframeOrder) || (NwkInfo.BatLifeExt != BatLifeExt)))
             || (NwkInfo.NwkState == NWK_STATE_JOIN_SUCESS) )
        {

            NwkInfo.NwkBeaconOrder  = BeaconOrder;
            NwkInfo.NwkSuperFrmOrder= SuperframeOrder;
            NwkInfo.BatLifeExt      = BatLifeExt;
            NwkInfo.CurRequest      = NWK_STATE_START_ROUTER;
            
            NwkInfo.BcnPayload.EndDeviceCapacity    = ENDDEVICE_CAP;
            NwkInfo.BcnPayload.nwkExtendedPANID     = 0x00; /*TBD*/
            NwkInfo.BcnPayload.RouterCapacity       = ROUTER_CAP;

#if ZIGBEE_DEVICE_FFD == 1
            NwkInfo.BcnPayload.DeviceDepth   = NwkInfo.pParent->Depth+1;                
            /*Init routing*/
            routing_init();
#endif

            /* Modify necessary PIB attributes */
            nTemp       = TRUE;
            mlmeSetRequest(MAC_ASSOCIATION_PERMIT, &nTemp);

            nTemp = sizeof(BEACON_PAYLOAD);
            mlmeSetRequest(MAC_BEACON_PAYLOAD_LENGTH, &nTemp);
            mlmeSetRequest(MAC_BEACON_PAYLOAD, &NwkInfo.BcnPayload);

            if (NwkInfo.NwkBeaconOrder != NON_BEACON_NWK)
                nwk_schedulBeacon();        /*Beacon scheduling*/
            else
                NwkInfo.BeaconOffset = 0;   /*Without beacon scheduling*/

            mlmeStartRequest(NwkInfo.NwkPANId,NwkInfo.NwkChannel,
                            BeaconOrder,SuperframeOrder,FALSE,BatLifeExt,
                            NwkInfo.NwkState == NWK_STATE_NORMAL_MODE, FALSE, NwkInfo.BeaconOffset);
        }
    }

} // End of functions



#endif  //!ZIGBEE_DEVICE_TYPE == ZIGBEE_ROUTER

/******************************************************************************/
//@FunctionName:  This is call back function to disable the PermitJoin        */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nlme_timeoutPermitJoin(void* pTimer, void* pArg)
{
	/*********** Variable declaration ***********/
    UINT32      nErr;    
	/*********** Function body		 ***********/
    OSTmrDel(pTimer,&nErr);     /* Delete the timer */
    nErr = FALSE;
    mlmeSetRequest(MAC_ASSOCIATION_PERMIT,&nErr);
} // End of functions


/******************************************************************************/
//@FunctionName: This function will enable permitjoint att. in MAC in 
//               a PermitDuration                                             */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nlmePermitJoiningRequest(UINT8 PermitDuration)
{
	/*********** Variable declaration ***********/
    OS_TMR*     PermitJoinTimer;
    UINT8       nErr;
    UINT32      nDur;
	/*********** Function body		 ***********/
    /* Check device */
    if (nwkcCoordinatorCapable || (Cskip == 0))
        nlmePermitJoiningConfirm(nwk_INVALID_REQUEST);
    else
    {
        /* Create a timer for reset the JoinPermition */
        nDur = (UINT32)PermitDuration*1000000/(MAC_SYMBOL_DURATION*aUnitBackoffPeriod);
        PermitJoinTimer = OSTmrCreate(0,nDur,OS_TMR_OPT_ONE_SHOT,nlme_timeoutPermitJoin,NULL,NULL,&nErr);
        nErr = TRUE;
        mlmeSetRequest(MAC_ASSOCIATION_PERMIT,&nErr);
        OSTmrStart(PermitJoinTimer,&nErr);
    }
} // End of functions

#endif // !ZIGBEE_DEVICE_FFD == 1


/********************************************************************************
                    CONFIRM AND INDICATION FUNCTIONS
*********************************************************************************/
/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nlmeJoinConfirm(UINT16 PanID,	NWK_STATUS Status)
{    
	/*********** Variable declaration ***********/
    ZAPP_MSG  Msg;     
	/*********** Function body		 ***********/    
    Msg.MsgType       = MSG_ZDO_CONFIRM_JOIN;
    Msg.EndPointId    = EP_ID_ZDO;
    Msg.Data          = Status;
    /*Do not care the PanId, access through NwkInfo*/
    apf_sendMsg(&Msg); 
} // End of functions


/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nlmeJoinIndication(    WORD    ShortAddr,
                            QWORD*  pExtAddr,
                            BYTE    CapInfor,
                            BOOL    Sercurity)
{    
	/*********** Variable declaration ***********/
    ZAPP_MSG        Msg;     
    MSG_JOIN_INDI*  pInidcation;
	/*********** Function body		 ***********/    
    pInidcation = (MSG_JOIN_INDI*)mem_getMem(sizeof(MSG_JOIN_INDI));
    if(pInidcation)
    {
        pInidcation->ExtAddr    = *pExtAddr;
        pInidcation->ShortAddr  = ShortAddr;
        pInidcation->Sercurity  = Sercurity;
        pInidcation->CapInfor   = CapInfor;

        Msg.MsgType       = MSG_ZDO_INDI_JOIN;
        Msg.EndPointId    = EP_ID_ZDO;
        Msg.Data          = (WORD)pInidcation;
        /*Do not care the PanId, access through NwkInfo*/
        apf_sendMsg(&Msg); 
    }
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nlmeLeaveConfirm(ADDRESS* NWK_DeviceAddress, NWK_STATUS Status)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/

} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nlmeNetworkDiscoveryConfirm(BYTE NetworkCount, NWK_DESC* NetworkDescriptor, 
                                 NWK_STATUS Status)
{
	/*********** Variable declaration ***********/
    ZAPP_MSG  Msg;     
	/*********** Function body		 ***********/
    /* Initialize ZDO */
    Msg.MsgType       = MSG_ZDO_CONFIRM_DIS;
    Msg.EndPointId    = EP_ID_ZDO;
    Msg.Data          = Status;
    /*we will use function to access nwk descriptor, omit it here*/
    apf_sendMsg(&Msg); 
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nlmeNetworkFormationConfirm(NWK_STATUS status)
{
	/*********** Variable declaration ***********/
    ZAPP_MSG  Msg;     
	/*********** Function body		 ***********/   
    Msg.MsgType       = MSG_ZDO_CONFIRM_FORMATION;
    Msg.EndPointId    = EP_ID_ZDO;
    apf_sendMsg(&Msg); 
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nlmeStartRouterConfirm(NWK_STATUS  Status)
{
	/*********** Variable declaration ***********/
    ZAPP_MSG  Msg;     
	/*********** Function body		 ***********/    
    Msg.MsgType       = MSG_ZDO_CONFIRM_START_ROUTER;
    Msg.EndPointId    = EP_ID_ZDO;
    Msg.Data          = Status;
    apf_sendMsg(&Msg); 
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nlmeResetConfirm(NWK_STATUS status)
{
    ZAPP_MSG  Msg;     
	/*********** Function body		 ***********/    
    Msg.MsgType       = MSG_ZDO_CONFIRM_RESET;
    Msg.EndPointId    = EP_ID_ZDO;
    apf_sendMsg(&Msg); 
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nlmeSyncConfirm(BYTE Status)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    /*No need to implement*/
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nlmeSyncIndication()
{
	/*********** Variable declaration ***********/
    ZAPP_MSG  Msg;     
	/*********** Function body		 ***********/    
    Msg.MsgType       = MSG_ZDO_INDI_SYNC;
    Msg.EndPointId    = EP_ID_ZDO;
    apf_sendMsg(&Msg); 
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nlmePermitJoiningConfirm(NWK_STATUS Status)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    /*No need to implement*/
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nlmeJoinDirectConfirm(ADDRESS*  pDeviceAddress, NWK_STATUS Status)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    /*No need, return directly from the call to the Request-function*/
} // End of functions
