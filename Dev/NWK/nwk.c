/*******************************************************************************/
//Module: NWK                                                                  */   
//File Name: nwk.c		                                                       */
//Description: This file will implement intertask communication mechanism 
//             that conformed with the scheduler mechanism detailed in AS-OS   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//05/28/2007                                                   */
//------------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/

#include "types.h"
#include "utils.h"
#include "mac_types.h"
#include "schedul.h"
#include "mac_primitives.h"
#include "nwk_common.h"
#include "nwk_msg.h"
#include "nwk_routing.h"
#include "nwk_data.h"
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
//@FunctionName:  nwk_init   		 										  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nwk_init()
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    
    /*Init nwk stata */
    NwkInfo.CurRequest  =  NWK_STATE_INIT;
    NwkInfo.NwkState    =  NWK_STATE_INIT;

    /*Init data transmission*/
    nwk_dataInit();
} // End of functions


/******************************************************************************/
//@FunctionName: nwk_setNwkState											  */
//@Description:	 This function will control transaction on nwk layer          */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL nwk_setNwkState(NWK_STATE      State)
{
	/*********** Variable declaration ***********/
    BOOL        bReturn;
	/*********** Function body		 ***********/
    if (NwkInfo.NwkState == NWK_STATE_NORMAL_MODE)
    {
        if(State == NWK_STATE_SYNC)
            bReturn = TRUE;
    }
    return FALSE;
} // End of functions

/*-----------------------------------------------------------------------------

                Confirm/Indication function

------------------------------------------------------------------------------*/

/******************************************************************************/
//@FunctionName:  nwk_sendStatMsg 										  	  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nwk_sendStatMsg(NWK_MSG_TYPE     Type, 
                     MAC_STATUS       Status, 
                     WORD             Data)
{
	/*********** Variable declaration ***********/
    NWK_MSG    Msg;
	/*********** Function body		 ***********/
    Msg.MsgType = Type;
    Msg.Status  = Status;
    Msg.Data    = Data;
    schedul_sendMsg(TASK_APER_NWK, &Msg);
}

/*************************************************************************************/
//	Function Name: 	mcpsDataConfirm
//	Description: 	
//	Parameters:		
//	Return:			nothing
/*************************************************************************************/
void mcpsDataConfirm(BYTE       msduHandle,
                     MAC_STATUS status)
{
    nwk_sendStatMsg(MSG_MAC_CONFIRM_DATA,status,msduHandle);
} // End of functions mcpsDataConfirm

/*************************************************************************************/
//	Function Name: 	mcpsDataIndication
//	Description: 	
//	Parameters:		
//	Return:			nothing
/*************************************************************************************/
void mcpsDataIndication(MCPS_DATA_INDICATION* par_pDIndic)
{
	/*********** Variable declaration ***********/
    NWK_MSG              Msg;
	/*********** Function body		 ***********/
    Msg.MsgType         = MSG_MAC_INDI_DATA;
    Msg.Data            = (WORD)par_pDIndic;
    schedul_sendMsg(TASK_APER_NWK, &Msg);
} // mcpsDataIndication

/*************************************************************************************/
//	Function Name: 	mlmeAssociateConfirm
//	Description: 	
//	Parameters:		
//	Return:			nothing
/*************************************************************************************/
void mlmeAssociateConfirm(WORD AssocShortAddress,UINT8 status)
{
    nwk_sendStatMsg(MSG_MAC_CONFIRM_ASSOCIATION,status,AssocShortAddress);
} // mlmeAssociateConfirm

/*************************************************************************************/
//	Function Name: 	mlmeDisassociateConfirm
//	Description: 	
//	Parameters:		
//	Return:			nothing
/*************************************************************************************/
void mlmeDisassociateConfirm(MAC_STATUS status)
{
    nwk_sendStatMsg(MSG_MAC_CONFIRM_DISASSOCIATION,status,0);
}

/*************************************************************************************/
//	Function Name: 	mlmeDisassociateIndication
//	Description: 	
//	Parameters:		
//	Return:			nothing
/*************************************************************************************/
void mlmeDisassociateIndication(QWORD* par_pDevAddr, BYTE Reason, 
                                BOOL SecurityUse, BOOL AclEntry)
{
	/*********** Variable declaration ***********/
    MSG_DISASSO_INDI_DATA*      pDisAssoData;
	/*********** Function body		 ***********/
    pDisAssoData = mem_getMem(sizeof(MSG_DISASSO_INDI_DATA));
    if (pDisAssoData)
    {
        pDisAssoData->devAddr         = *par_pDevAddr;
        pDisAssoData->Reason          = Reason;
        pDisAssoData->Sercurity       = SecurityUse;
        pDisAssoData->AclEntry        = AclEntry;
                
        nwk_sendStatMsg(MSG_MAC_INDI_DISASSOC,SUCCESS,(WORD)pDisAssoData);     
    }
} // End of functions


/*************************************************************************************/
//	Function Name: 	mlmeResetConfirm
//	Description: 	
//	Parameters:		
//	Return:			nothing
/*************************************************************************************/
void mlmeResetConfirm(MAC_STATUS status)
{
    nwk_sendStatMsg(MSG_MAC_CONFIRM_RESET,status,0);
}
/*************************************************************************************/
//	Function Name: 	mlmePollConfirm
//	Description: 	
//	Parameters:		
//	Return:			nothing
/*************************************************************************************/
void mlmePollConfirm(MAC_STATUS status)
{
    nwk_sendStatMsg(MSG_MAC_CONFIRM_POLL,status,0);
} // End of functions

/*************************************************************************************/
//	Function Name: 	mlmeScanConfirm
//	Description: 	
//	Parameters:		
//	Return:			nothing
/*************************************************************************************/
void mlmeScanConfirm(MAC_STATUS status, MAC_SCAN_RESULT* par_pScanResult)
{
    nwk_sendStatMsg(MSG_MAC_CONFIRM_SCAN,status,(WORD)par_pScanResult);
}

/*************************************************************************************/
//	Function Name: 	mlmeSyncLossIndication
//	Description: 	
//	Parameters:		
//	Return:			nothing
/*************************************************************************************/
void mlmeSyncLossIndication(MAC_STATUS lossReason)
{
    nwk_sendStatMsg(MSG_MAC_INDI_LOSS,lossReason,0);
}

/*************************************************************************************/
//	Function Name: 	mlmeCommStatusIndication
//	Description: 	
//	Parameters:		
//	Return:			nothing
/*************************************************************************************/
void mlmeCommStatusIndication(WORD panId, BYTE srcAddrMode, ADDRESS *pSrcAddr, BYTE dstAddrMode, ADDRESS *pDstAddr, 
                              BYTE status)
{
    MSG_COMM_STATUS_DATA*        pCommIndi;
	/*********** Function body		 ***********/
    pCommIndi = mem_getMem(sizeof(MSG_COMM_STATUS_DATA));
    if (pCommIndi)
    {
        pCommIndi->panId         = panId;
        pCommIndi->srcAddrMode   = srcAddrMode;
        pCommIndi->SrcAddr       = *pSrcAddr;
        pCommIndi->status        = status;
                
        nwk_sendStatMsg(MSG_MAC_INDI_ASSO,SUCCESS,pCommIndi);     
    }    
}

/*************************************************************************************/
//	Function Name: 	mlmeBeaconNotifyIndication
//	Description: 	
//	Parameters:		
//	Return:			nothing
/*************************************************************************************/
void mlmeBeaconNotifyIndication(MLME_BEACON_NOTIFY_INDICATION *pBcnNot)
{
    nwk_sendStatMsg(MSG_MAC_INDI_BEACON,0,(WORD)pBcnNot);
}

#if     ZIGBEE_DEVICE_FFD == 1
/*************************************************************************************/
//	Function Name: 	mlmeStartConfirm
//	Description: 	
//	Parameters:		
//	Return:			nothing
/*************************************************************************************/
void mlmeStartConfirm(MAC_STATUS     status)
{
    nwk_sendStatMsg(MSG_MAC_CONFIRM_START,status,0);
}

/*************************************************************************************/
//	Function Name: 	mlmeStartConfirm
//	Description: 	
//	Parameters:		
//	Return:			nothing
/*************************************************************************************/
void mlmeAssociateIndication(ADDRESS* par_pDevAddr, BYTE CapInfor, 
                        BOOL SecurityUse, UINT8 AclEntry)
{
	/*********** Variable declaration ***********/
    MSG_ASSO_INDI_DATA*        pAssIndication;
	/*********** Function body		 ***********/
    pAssIndication = mem_getMem(sizeof(MSG_ASSO_INDI_DATA));
    if (pAssIndication)
    {
        pAssIndication->devAddr         = par_pDevAddr->Extended;
        pAssIndication->CapacityInfor   = CapInfor;
        pAssIndication->Sercurity       = SecurityUse;
        pAssIndication->aclEntry        = AclEntry;
                
        nwk_sendStatMsg(MSG_MAC_INDI_ASSO,SUCCESS,pAssIndication);     
    }
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mlmeOrphanIndication(QWORD* pOrphanAddress, BOOL securityUse, UINT8 aclEntry)
{
    nwk_sendStatMsg(MSG_MAC_INDI_ORPHAN,SUCCESS,pOrphanAddress);     
}
#endif  //!ZIGBEE_DEVICE_FFD == 1

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nwk_processAssConfirm(NWK_MSG*   pMsg)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
	if( pMsg->Status == SUCCESS){            
        if (NwkInfo.CurRequest == NWK_STATE_JOIN) 
        {
            if (pMsg->Data != INVALID_SHORT_ADDR)
            {
                /*UPdated on association*/
		        NwkInfo.pParent->Relationship    = NWK_PARRENT;    /*Set parent */
                NwkInfo.pParent->PermitJoining   = TRUE; 		
                NwkInfo.NwkAddr                  = pMsg->Data;
                NwkInfo.NwkState                 = NWK_STATE_JOIN_SUCESS;
                mlmeGetRequest(MAC_COORD_EXTENDED_ADDRESS,&NwkInfo.pParent->ExtendedAddress);

                if (NwkInfo.NwkJoinedDeviveType == ZIGBEE_ENDPOINT)
                    NwkInfo.NwkState             = NWK_STATE_NORMAL_MODE; /*Ready*/

	            nlmeJoinConfirm(NwkInfo.NwkPANId, nwk_SUCCESS);                
                return;
            }

        }
	}

    NwkInfo.CurRequest = NWK_STATE_INIT;
	NwkInfo.pParent->Relationship    = NWK_NO_RELATION;    /*Set no relationship */
    NwkInfo.pParent->PermitJoining   = FALSE; 		
    nlmeJoinConfirm(NwkInfo.NwkPANId, pMsg->Status);
} // End of functions


/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nwk_processScanConfirm(NWK_MSG*   pMsg)
{
	/*********** Variable declaration ***********/
    MAC_SCAN_RESULT                 *pScanResult;
	/*********** Function body		 ***********/
    pScanResult = (MAC_SCAN_RESULT*)pMsg->Data;
	if (pMsg->Status ==	SUCCESS)
	{
#if ZIGBEE_DEVICE_TYPE == ZIGBEE_PAN_COOR   /* Code for PAN Coordinator only */
        if (NwkInfo.CurRequest == NWK_STATE_FORMATION)
           nlme_formNwk();
#endif //! ZIGBEE_DEVICE_TYPE == ZIGBEE_PAN_COOR   

        if (NwkInfo.CurRequest== NWK_STATE_DISCOVERY)
        {
		    nlmeNetworkDiscoveryConfirm(nwk_desc, nwk_SUCCESS); 
        }

        if (NwkInfo.CurRequest== NWK_STATE_REJOIN)
        {
            nlmeJoinConfirm(NwkInfo.NwkPANId,nwk_SUCCESS);
            NwkInfo.NwkState = NWK_STATE_JOIN_SUCESS;
        }
	}
	else 
	{
#if ZIGBEE_DEVICE_TYPE == ZIGBEE_PAN_COOR   /* Code for PAN Coordinator only */
        if (NwkInfo.CurRequest == NWK_STATE_FORMATION)
           nlme_formNwk();
#endif //!ZIGBEE_DEVICE_TYPE == ZIGBEE_PAN_COOR   
        if (NwkInfo.CurRequest== NWK_STATE_DISCOVERY)
		    nlmeNetworkDiscoveryConfirm(0, NULL, pMsg->Status); 

        if (NwkInfo.CurRequest== NWK_STATE_REJOIN)
            nlmeJoinConfirm(NwkInfo.NwkPANId,nwk_NO_NETWORKS);
	}	
    /*Free the memory*/
    mem_freeMem(pScanResult);
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nwk_processDisAssConfirm(NWK_MSG*   pMsg)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
	if(pMsg->Status == SUCCESS){
		nlmeLeaveConfirm(NULL, nwk_SUCCESS);        /*TBD*/
	}
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nwk_processPollConfirm(NWK_MSG*   pMsg)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
	if (pMsg->Status == SUCCESS)
		nlmeSyncConfirm(nwk_SUCCESS);
	else 
		nlmeSyncConfirm(nwk_SYNC_FAILURE);    
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
#if  ZIGBEE_DEVICE_FFD == TRUE
void nwk_processStartConfirm(NWK_MSG*   pMsg)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
	if (pMsg->Status == SUCCESS)
	{
		if (NwkInfo.CurRequest == NWK_STATE_FORMATION)
        {
            NwkInfo.NwkState            = NWK_STATE_NORMAL_MODE;
            NwkInfo.NwkJoinedDeviveType = ZIGBEE_PAN_COOR;
            nlmeNetworkFormationConfirm(nwk_SUCCESS);
        }
        if (NwkInfo.CurRequest == NWK_STATE_START_ROUTER)
        {
            NwkInfo.NwkState            = NWK_STATE_NORMAL_MODE;
            NwkInfo.NwkJoinedDeviveType = ZIGBEE_ROUTER;
            nlmeStartRouterConfirm(nwk_SUCCESS);
        }
	}
    else
        nlmeNetworkFormationConfirm(nwk_STARTUP_FAILURE);
} // End of functions
#endif

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nwk_processBeaconIndication(NWK_MSG*   pMsg)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    if (NwkInfo.CurRequest == NWK_STATE_DISCOVERY)
        nwk_updateBeacon((MLME_BEACON_NOTIFY_INDICATION*)pMsg->Data);
} // End of functions


#if  ZIGBEE_DEVICE_FFD == TRUE
/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nwk_processAssoIndication(NWK_MSG*   pMsg)
{
	/*********** Variable declaration ***********/
    CAPACITY_INFO*              pCap;
    MSG_ASSO_INDI_DATA*         pAssoIndi;
    WORD                        NwkAddr;
    NWK_STATUS                  Status;
	/*********** Function body		 ***********/
    pAssoIndi = (MSG_ASSO_INDI_DATA*)pMsg->Data;
    /* Check if device can be associated */
    /* Add to neighbor table */

    pCap    = &pAssoIndi->CapacityInfor;
    
    if (((pCap->DeviceType==FALSE) && !NwkInfo.BcnPayload.EndDeviceCapacity)
        || ((pCap->DeviceType==TRUE) && !NwkInfo.BcnPayload.RouterCapacity))
    {
        NwkAddr  = INVALID_SHORT_ADDR;
        Status   = DEV_NOT_PERMIT;
    }
    else
    {
        NwkAddr = nwk_getShortAddInNT(&pAssoIndi->devAddr);    
        if (NwkAddr == INVALID_SHORT_ADDR)
        {            
            NwkAddr = routing_checkFreeAddr(pCap->DeviceType);
            if (NwkAddr)
            {
                NwkAddr = routing_allocDistAddr(pCap->DeviceType); 
                Status  = nwk_addNewChild(  &pAssoIndi->devAddr, NwkAddr, *pCap,
                                            pAssoIndi->Sercurity, pAssoIndi->aclEntry);
            }
            if(Status == nwk_TABLE_FULL || NwkAddr == INVALID_SHORT_ADDR)  
            {
                NwkAddr = INVALID_SHORT_ADDR;
                Status  = DEV_AT_CAP;
                nwk_inhibitJoin(TRUE);
            }
            else
            {
                nlmeJoinIndication(NwkAddr,&pAssoIndi->devAddr,pAssoIndi->CapacityInfor);
            }
            }
        else
            Status = nwk_SUCCESS;
    }

    mlmeAssociateResponse(&pAssoIndi->devAddr,(WORD)NwkAddr,Status,FALSE);        
    /* Table full or no available nwk addr: Discard the association cmd */
    mem_freeMem(pMsg->Data);        
} // End of functions
#endif  //!ZIGBEE_DEVICE_FFD == 1

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nwk_processNwkEvent(NWK_MSG*   pMsg, void* arg)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    switch(pMsg->MsgType)
    {
   /* Confirmation */
    case MSG_MAC_CONFIRM_ASSOCIATION:
        nwk_processAssConfirm(pMsg);
    	break;
    case MSG_MAC_CONFIRM_SCAN:
        nwk_processScanConfirm(pMsg);
        break;
    case MSG_MAC_CONFIRM_DATA:
        nwk_processDataConfirm(pMsg);
        break;
    case MSG_MAC_CONFIRM_DISASSOCIATION:
        nwk_processDisAssConfirm(pMsg);            
        break;        
    case MSG_MAC_CONFIRM_RESET:
        nlme_resetNwk(pMsg->Status);
    	break;
    case MSG_MAC_CONFIRM_POLL:
        nwk_processPollConfirm(pMsg);
    	break;

    case MSG_MAC_CONFIRM_START:
#if  ZIGBEE_DEVICE_FFD == TRUE
        nwk_processStartConfirm(pMsg);
#endif
    	break;                
   /* Indication */
    case MSG_MAC_INDI_DATA:
        nwk_processDataIndication(pMsg);
        break;
    case MSG_MAC_INDI_BEACON:
        nwk_processBeaconIndication(pMsg);
        break;
    case MSG_MAC_INDI_ASSO:
#if  ZIGBEE_DEVICE_FFD == TRUE
        nwk_processAssoIndication(pMsg);
#endif
        break;
    case MSG_MAC_INDI_LOSS:
        if (pMsg->Data == BEACON_LOSS)
            nlmeSyncIndication();
        break;        
    case MSG_MAC_INDI_DISASSOC:
        /*TBD*/
        break;
    case MSG_BT_JITER_TIMEOUT:
    case MSG_BT_1s_TIMEOUT:
        nwk_processBtEvent(pMsg);
        break;
    default:
        break;
    }
} // End of functions


