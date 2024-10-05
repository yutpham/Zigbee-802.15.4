/*******************************************************************************/
//Module: APL/ZDO 	                                                           */
//File Name: zdo.c                                                             */
//Description: This file implement the behaviors of the ZDO of Coordinator,    */
//             Router or End Device in zigbee network                          */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//06/26/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "types.h"
#include "app_framework.h"
#include "zdp.h"
#include "zdo.h"
#include "aps.h"
#include "zdo_app_interface.h"
#include "zconf.h"
#include "schedul.h"
#include "ucos_ii.h"
#include "aps_ib.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/



/*Binding request*/
typedef struct  
{
    BYTE*   pInClusterList;
    BYTE    NumInClusters;
    BYTE*   pOutClusterList;
    BYTE    NumOutClusters;
    UINT16  SrcAddr;    
    UINT8   Endpoint;
    UINT16  ProfileId;
    BYTE    SrcTrans;
    UINT8   Status;
}BIND_REQUEST;
/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/

/*Network descriptor in Nwk layer*/
extern NWK_DESC*           nwk_desc[MAX_NWK_DESC_SIZE];

/* Configuration Attribute Definitions
*/
/*----Config_NWK_Mode_and_Params----*/
UINT8           confDeviceType      =   ZIGBEE_DEVICE_TYPE;
DWORD           confChannelList     =   conf_CHANNEL_LIST;
UINT8           confScanDuration    =   conf_SCAN_DURATION;
WORD            confPandId          =   conf_PAN_ID;
UINT8           confProtocolVers    =   conf_PROTOCOL_VERS;
WORD            confStackProfile    =   conf_PROFILE_ID;
UINT8           confBeaconOrder     =   conf_BEACON_ORDER;
UINT8           confSuperFrmOrder   =   conf_SUP_FRM_ORDER;
BOOL            confBattLifeExt     =   conf_BATT_EXT;
UINT8           confScanAttempts    =   conf_SCAN_ATTPEMPT;
/*--Config_NWK_Time_btwn_Scans--*/
UINT8           confTimeBetweenScans=   conf_TIME_BWT_SCANS;
/*--Config_Max_Bind--*/
UINT8           confMaxBind         =   conf_MAX_BINDING;
/*--Config_EndDev_Bind_Timeout--*/
UINT8           confBindingTimeout  =   conf_BINDING_TIMEOUT;
/*Config_Permit_Join_Duration*/
UINT8           confPermitJoinDuration= conf_PERMIT_JOINT_DUR;
/*Config_NWK_Leave_removeChildren*/
BOOL            confLeaveRemoveChildrens=conf_REMOVE_CHILDREN;
/*Config_NWK_BroadcastDeliveryTime*/
UINT8           confBroacastDelivTime;
/*Config_NWK_TransactionPersistenceTime*/
UINT8           confTransPersistenceTime=conf_TRANSACTION_PERSISTENCE_TIME;
/*Config_NWK_indirectPollRate*/
UINT8           confIndirectPollRate    =conf_INDIRECT_POLL_RATE;
/*Config_Max_Assoc*/
UINT8           confMaxAssoc            =conf_MAX_ASSOCIATION;
/*Config_NWK_Join_Direct_Addrs*/
void*           confJoinDirectAddrs     =NULL;

/* Device state
*/
DEV_STATE       DevState;
UINT8           NunRetries;

/*Variable for zdo command/response*/
BIND_REQUEST*   pCurrBindReq;
/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/


/************************************************************************/
/*                      Function                                        */
/************************************************************************/




#if ZIGBEE_DEVICE_TYPE == ZIGBEE_PAN_COOR
/******************************************************************************/
//@FunctionName:  zdo_startCoord 										  	  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void zdo_startCoord(ZAPP_MSG*     pMsg)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    switch(pMsg->MsgType)
    {
    case MSG_ZDO_START:
        /* ZDO now is in INITIALIZATION STATE */
		nlmeResetRequest();			            // Call NWK reset	
		break;
    case MSG_ZDO_CONFIRM_RESET: 
        nlmeNetworkFormationRequest(confChannelList & VALID_CHANNELS, confScanDuration,
                                    confBeaconOrder,confSuperFrmOrder,confPandId, confBattLifeExt);
		break;
    case MSG_ZDO_CONFIRM_FORMATION:
        /* Setting permit join */
        nlmePermitJoiningRequest(confPermitJoinDuration);
        /* Set internal parameter to nwk*/
        nlmeSetRequest(NWKNETWORKBROADCASTDELIVERYTIME,sizeof(confBroacastDelivTime),
            &confTransPersistenceTime);
        nlmeSetRequest(NWKTRANSACTIONPERSISTENCETIME,sizeof(confTransPersistenceTime),
            &confTransPersistenceTime);

        /* Device started */
        aps_init();             /* Initialize APS */
        zdo_app_DeviceStart();  /* Inform other endpoint */

        /* ZDO now is in NORMAL STATE */
        DevState = DEV_STARTED;
        break;
    default:
        break;
    }
} // End of functions
#endif //!ZIGBEE_DEVICE_TYPE == ZIGBEE_PAN_COOR

#if ZIGBEE_DEVICE_TYPE == ZIGBEE_ROUTER      
/******************************************************************************/
//@FunctionName: zdo_startRouter  										  	  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void zdo_startRouter(ZAPP_MSG*     pMsg)
{
	/*********** Variable declaration ***********/
    DISCOR_CONFIRM*         pConf;
	/*********** Function body		 ***********/
    switch(pMsg->MsgType)
    {
    case MSG_ZDO_START:
        /* ZDO now is in INITIALIZATION STATE */
		nlmeResetRequest();			            // Call NWK reset	
		break;
    case MSG_ZDO_CONFIRM_RESET: 
		nlmeNetworkDiscoveryRequest(confChannelList & VALID_CHANNELS,confScanDuration);	 		                
        break;
    case MSG_ZDO_CONFIRM_DIS:
        /*TBD: rejoin or join ? depends on starting mode*/
        pConf = (DISCOR_CONFIRM*)pMsg->Data;
        if (pConf->Status == nwk_SUCCESS)
        {
            if (nwk_checkNwk(confPandId,confChannelList))
            {
	            nlmeJoinRequest(confPandId, TRUE, FALSE, confChannelList, confScanDuration,
                                !NodePowerDescriptor.CurrentPowerSource,
                                ((NodeDescriptor.CapabilityFlags&0x08) == 0x08), CAP_SECURITY_CAPABLE);		
                break;
            }
        }            
        zdo_retryInit(NULL,NULL);

		break;
    case MSG_ZDO_CONFIRM_JOIN:
        if (pMsg->Data != nwk_SUCCESS)
            zdo_retryInit(NULL,NULL);
        else
            nlmeStartRouterRequest(confBeaconOrder,confSuperFrmOrder,confBattLifeExt);
        break;
    case MSG_ZDO_CONFIRM_START_ROUTER:
        if (pMsg->Data != nwk_SUCCESS)
            zdo_retryInit(NULL,NULL);
        else
        {
            nlmeSyncRequest(TRUE);

            /* Device started */
            aps_init();             /* Initialize APS */
            zdo_app_DeviceStart();  /* Inform other endpoint */
            apf_sendAppMsg(EP_ID_ZDO,MSG_ZDO_ED_ANNCE,0x00);

            /* ZDO now is in NORMAL STATE */    
            DevState = DEV_STARTED;            
        }
        break;
    default:
        break;
    }
} // End of functions
#endif //!ZIGBEE_DEVICE_TYPE == ZIGBEE_ROUTER

#if ZIGBEE_DEVICE_TYPE == ZIGBEE_ENDPOINT  
/******************************************************************************/
//@FunctionName:   zdo_startEndpoint										  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void zdo_startEndpoint(ZAPP_MSG*     pMsg)
{
	/*********** Variable declaration ***********/
    
	/*********** Function body		 ***********/	
    switch(pMsg->MsgType)
    {
    case MSG_ZDO_START:
        /* ZDO now is in INITIALIZATION STATE */
		nlmeResetRequest();			            // Call NWK reset	
		break;
    case MSG_ZDO_CONFIRM_RESET: 
		nlmeNetworkDiscoveryRequest(confChannelList & VALID_CHANNELS,confScanDuration);	 		                
        break;
    case MSG_ZDO_CONFIRM_DIS:
        /*TBD: rejoin or join ? depends on starting mode*/
        if (pMsg->Data == nwk_SUCCESS)
        {
            if (nwk_checkNwk(confPandId,confChannelList))
            {
	            nlmeJoinRequest(confPandId, FALSE, FALSE, confChannelList, confScanDuration,
                                !NodePowerDescriptor.CurrentPowerSource,
                                ((NodeDescriptor.CapabilityFlags&0x08) == 0x08), CAP_SECURITY_CAPABLE);		
                break;
            }
        }            
        zdo_retryInit(NULL,NULL);
		break;
    case MSG_ZDO_CONFIRM_JOIN:
        if (pMsg->Data != nwk_SUCCESS)
            zdo_retryInit(NULL,NULL);
        else
        {   
            if (confBeaconOrder != NON_BEACON_NWK)
                nlmeSyncRequest(TRUE);

            /* Device started */
            aps_init();             /* Initialize APS */
            zdo_app_DeviceStart();  /* Inform other endpoint */
            apf_sendAppMsg(EP_ID_ZDO,MSG_ZDO_ED_ANNCE,0x00);
            
            /* ZDO now is in NORMAL STATE */    
            DevState = DEV_STARTED;            
        }
        break;
        break;
    default:
        break;
    }

} // End of functions
#endif //!ZIGBEE_DEVICE_TYPE == ZIGBEE_ENDPOINT

/******************************************************************************/
//@FunctionName:   	zdo_startDevice									  	      */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    zdo_startDevice(ZAPP_MSG*     pMsg)
{
#if ZIGBEE_DEVICE_TYPE == ZIGBEE_ENDPOINT  
    zdo_startEndpoint(pMsg);
#endif
#if ZIGBEE_DEVICE_TYPE == ZIGBEE_ROUTER  
    zdo_startRouter(pMsg);
#endif
#if ZIGBEE_DEVICE_TYPE == ZIGBEE_PAN_COOR
    zdo_startCoord(pMsg);
#endif
} // End of functions

/******************************************************************************/
//@FunctionName:   	zdo_init									  	          */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void zdo_init(void*   pParam,void*  arg)
{
	/*********** Variable declaration ***********/
    ZAPP_MSG  Msg;     
	/*********** Function body		 ***********/
    pCurrBindReq = NULL;
    /* Initialize ZDO */
    Msg.MsgType     = MSG_ZDO_START;
    Msg.EndPointId  = EP_ID_ZDO;
    schedul_sendMsg(TASK_APER_NWK,&Msg);
} // End of functions

/******************************************************************************/
//@FunctionName:   zdo_retryInit   				  	                          */
//@Description:	This function will restart zigbee initialization after a      */
//                predefined duration                                         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void zdo_retryInit(void*   pOstimer,void*  pParam)
{
	/*********** Variable declaration ***********/
    UINT8   nErr;
	/*********** Function body		 ***********/
    if (NunRetries++ >= ZIGBEE_MAX_RETRIES)
    {
        err(ERR_UNRECOVERABLE,NULL,NULL,0); /*Report error: error in starting*/            
    }

    if (!pOstimer)
    {
        pOstimer = OSTmrCreate(0,ZIGBEE_RETRY_DELAY*3125,OS_TMR_OPT_ONE_SHOT,zdo_retryInit,NULL,NULL,&nErr);
        OSTmrStart(pOstimer,&nErr);
    }
    else
    {
        OSTmrDel(pOstimer,&nErr);
        zdo_init(NULL,NULL);
    }

} // End of functions

/*----------------------------------------------------------------------------
                    Function for processing ZDO command
-----------------------------------------------------------------------------*/
/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void zdo_processEDBindTimeout(ZAPP_MSG*        pMsg)
{
	/*********** Variable declaration ***********/
    AF_ADDR     SrcAddr;
	/*********** Function body		 ***********/
    SrcAddr.Addr.Short    = pCurrBindReq->SrcAddr;
    SrcAddr.EP            = pCurrBindReq->Endpoint;
    SrcAddr.AddrMode      = APS_ADDR_SHORT;
    zdp_EDBindRes(&SrcAddr,pCurrBindReq->SrcTrans,
                  pCurrBindReq->Status);

    /*Remove the binding request*/
    mem_freeMem(pCurrBindReq);
    pCurrBindReq = NULL;    
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BYTE zdo_cmpClusterLists( BYTE numList1, BYTE *list1,
                              BYTE numList2, BYTE *list2, BYTE *pMatches )
{
	/*********** Variable declaration ***********/
    BYTE x, y, z;
    BYTE numMatches = 0;
	/*********** Function body		 ***********/  
    // Check the first in against the seconds
    for ( x = 0; x < numList1; x++ )
    {
        for ( y = 0; y < numList2; y++ )
        {
          z = list2[y];
          if ( list1[x] == z )
            pMatches[numMatches++] = z;
        }
    }
    return ( numMatches );
}                          

/******************************************************************************/
//@FunctionName: zdo_processEDBind                                            */
//@Description:	 This function will process the ED Bind request and send      */
//               response to the source of the request
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void zdo_processEDBind(AF_MSG* pAfMsg, AF_ADDR*    pSrcAddr)
{
	/*********** Variable declaration ***********/
    ADDR_MAP_ENTRY* pAddrEntry;   
    BINDING_ENTRY*  pBindingEntry;
    ZAPP_MSG        Msg;
    BYTE*           pMatch;
    BYTE            nNumMatch;
    BYTE            nNum;
    BYTE            nNumOut;
    Z_STATUS        Status;
	/*********** Function body		 ***********/
    /*If binding is in progress, check the match immediately*/
    if (pCurrBindReq)
    {
        /*Check the source of the binding*/
        if (pCurrBindReq->SrcAddr == pSrcAddr->Addr.Short)
        {
            return;/*omit it*/
        }

        pMatch = mem_getMem(MAX(pCurrBindReq->NumInClusters,pCurrBindReq->NumOutClusters));
        if (pMatch)
        {
            /*Check the match of the in-cluster of previous request with current*/
            nNum = pAfMsg->pData[5];  /*Num In Cluster ID*/
            nNumMatch = zdo_cmpClusterLists(pCurrBindReq->NumOutClusters,pCurrBindReq->pOutClusterList,
                                            nNum,&pAfMsg->pData[6],pMatch);
            if (nNumMatch)
            {
                /*Update the binding*/
                pBindingEntry = ib_searchBindingEntry(pCurrBindReq->SrcAddr,pCurrBindReq->Endpoint,
                                                      pSrcAddr->Addr.Short, pAfMsg->pData[2]/*End point*/);
                if (pBindingEntry)
                {
                    /*Existed, remove it*/
                    ib_removeBindingEntry(pBindingEntry);
                    Status = SUCCESS;
                }
                /*Add new one*/
                else if (ib_addBindingEntry(pCurrBindReq->SrcAddr,pCurrBindReq->Endpoint,
                                       pSrcAddr->Addr.Short, pAfMsg->pData[2]/*End point*/,
                                       nNumMatch,pMatch))
                {
                    Status = SUCCESS;
                }
                else
                    Status = ZDP_TABLE_FULL;
            }

            /*Check the match of the in-cluster of previous request with current*/
            nNum    = pAfMsg->pData[5];  /*Num In Cluster ID*/
            nNumOut = pAfMsg->pData[6+nNum]; /*Num out-cluster*/
            nNumMatch = zdo_cmpClusterLists(nNumOut,&pAfMsg->pData[7+nNum],
                                            pCurrBindReq->NumInClusters,pCurrBindReq->pInClusterList,pMatch);
            if (nNumMatch)
            {
                /*Update the binding*/
                pBindingEntry = ib_searchBindingEntry(pSrcAddr->Addr.Short, pAfMsg->pData[2]/*End point*/,
                                                        pCurrBindReq->SrcAddr,pCurrBindReq->Endpoint);
                if (pBindingEntry)
                {
                    /*Existed, remove it*/
                    ib_removeBindingEntry(pBindingEntry);
                    Status = SUCCESS;
                }
                /*Add new one*/
                else if (ib_addBindingEntry(pSrcAddr->Addr.Short, pAfMsg->pData[2]/*End point*/,
                                       pCurrBindReq->SrcAddr,pCurrBindReq->Endpoint,
                                       nNumMatch,pMatch))
                    Status = SUCCESS;
                else
                    Status = ZDP_TABLE_FULL;
            }

            /*Free the memory*/
            mem_freeMem(pMatch);
        }
        else
        {
            /*Not enough memory*/
            Status = ZDP_TABLE_FULL;
        }

        pCurrBindReq->Status = Status;
        /*End of the binding*/
        /*Notify the requester*/
        zdp_EDBindRes(pSrcAddr,pAfMsg->TransSeqNumber,Status);

        if (Status == SUCCESS)
            TOGGLE_RLED();/*Success of a binding*/
    }
    else
    /*No binding in progress, keep the request to process later*/
    {
        pCurrBindReq = mem_getMem(sizeof(BIND_REQUEST)+pAfMsg->DataLen - 7 /*not used element*/);
        if (pCurrBindReq)
        {
            pCurrBindReq->Endpoint      = pAfMsg->pData[2];             /*End point*/
            pCurrBindReq->ProfileId     = *(WORD*) &pAfMsg->pData[3];    /*Cluster ID*/
            pCurrBindReq->NumInClusters = pAfMsg->pData[5];             /*Num In Cluster ID*/
            pCurrBindReq->NumOutClusters= 
                pAfMsg->pData[6+pCurrBindReq->NumInClusters];             /*Num out Cluster ID*/
            pCurrBindReq->pInClusterList    =(BYTE*)pCurrBindReq+sizeof(BIND_REQUEST);
            pCurrBindReq->pOutClusterList   =(BYTE*)pCurrBindReq+sizeof(BIND_REQUEST)+pCurrBindReq->NumInClusters;
            mem_cpy(pCurrBindReq->pInClusterList,&pAfMsg->pData[6],pCurrBindReq->NumInClusters);
            mem_cpy(pCurrBindReq->pOutClusterList,&pAfMsg->pData[7+pCurrBindReq->NumInClusters],
                    pCurrBindReq->NumOutClusters);
            pCurrBindReq->SrcAddr       = pSrcAddr->Addr.Short;         /*source of the request*/ 
            pCurrBindReq->SrcTrans      = pAfMsg->TransSeqNumber;
            pCurrBindReq->Status        = ZDP_TIMEOUT;

            /*Start binding time out*/
            Msg.MsgType     = MSG_ZDO_BINDING_TIMEOUT;
            Msg.EndPointId  = EP_ID_ZDO;
            Msg.Data        = 0;
            mtimer_creatTmr(&Msg,
                            (UINT32)(1000*apscBindingTimeout), 
                            FALSE,TASK_APER_NWK);
        }
        else
        {
            /*Not enough memory, omit the request*/
            err(ERR_UNRECOVERABLE,0,0,0);
        }
    }
} // End of functions


/******************************************************************************/
//@FunctionName: zdo_processBind_UnBind                                       */
//@Description:	 This function will process bind/unbind request and send 
//               response to the source of the request                        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void zdo_processBind_UnBind(AF_MSG* pAfMsg, AF_ADDR*    pSrcAddr)
{
	/*********** Variable declaration ***********/
    ADDR_MAP_ENTRY* pAddrEntry;
    Z_STATUS        stat;
    WORD            SrcAddr;
    WORD            DstAddr;
	/*********** Function body		 ***********/
    SrcAddr = INVALID_SHORT_ADDR;
    DstAddr = INVALID_SHORT_ADDR;
#if defined ( ZDO_NWKADDR_REQUEST )  
    // Make sure the addresses are in the address mapping
    pAddrEntry = ib_searchAddrEntry(INVALID_SHORT_ADDR,(QWORD*)pAfMsg->pData);
    if ( pAddrEntry == NULL )
        zdp_NwkAddrReq((QWORD*)pAfMsg->pData, ZDP_ADDR_REQ_SINGLE, 0, FALSE ); 
    else
        SrcAddr     = pAddrEntry->ShortAddr;

    //  Check the destination
    pAddrEntry = ib_searchAddrEntry(INVALID_SHORT_ADDR,(QWORD*)(pAfMsg->pData+ADDR_LEN_EXTENDED_64+2));
    if ( pAddrEntry == NULL )
        zdp_NwkAddrReq((QWORD*)(pAfMsg->pData+ADDR_LEN_EXTENDED_64+2), 
                    ZDP_ADDR_REQ_SINGLE, 0, FALSE ); 
    else
        DstAddr     = pAddrEntry->ShortAddr;
#endif  
    if (ZIGBEE_DEVICE_TYPE != ZIGBEE_PAN_COOR)
        stat = ZDP_NOT_SUPPORTED;
    else if ((SrcAddr == INVALID_SHORT_ADDR) || (DstAddr == INVALID_SHORT_ADDR))
    {
        stat = ZDP_NOT_SUPPORTED;           /*Not supported*/
    }
    else
    {
        if ( apsmeBindRequest( SrcAddr, pAfMsg->pData[ADDR_LEN_EXTENDED_64], 
                            pAfMsg->pData[ADDR_LEN_EXTENDED_64+1],
                            DstAddr, pAfMsg->pData[2*ADDR_LEN_EXTENDED_64+2]) == SUCCESS )
            stat = ZDP_SUCCESS;
        else
            stat = ZDP_TABLE_FULL;
    }

    // response message
    zdp_BindUnBindRes(  pSrcAddr,pAfMsg->TransSeqNumber, stat, 
                        (pAfMsg->pData[ADDR_LEN_EXTENDED_64+1]==Bind_req));
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void zdo_processData(ZAPP_MSG*     pMsg)
{
	/*********** Variable declaration ***********/
    AF_IMCOM_FRM*   pAfFrm;
    AF_MSG          AfMsg;
    ADDR_MAP_ENTRY* pAddrEntry;
	/*********** Function body		 ***********/
    pAfFrm = pMsg->Data;   
    if (!apf_getMSGTrans(pAfFrm,&AfMsg,1))
        return;

    switch(pAfFrm->ClusterId) 
    {    
        /*--------------*/
        /*  Response    */
        /*--------------*/
        case NWK_addr_rsp:
            /*Check the status*/
            if (AfMsg.pData[0] == SUCCESS)
                ib_addAddrEntry(*(WORD*)(AfMsg.pData+1/*Status*/+ADDR_LEN_EXTENDED_64/*Ext addr*/),
                                (QWORD*)&AfMsg.pData[1]/*Status*/);
            break;
        case IEEE_addr_rsp:
            break;         
            
        /*--------------*/
        /*  Request     */
        /*--------------*/
        case NWK_addr_req:
            if (*(QWORD*)AfMsg.pData == aExtendedAddress)
            {
                zdp_NwkAddrRes(FALSE, &pAfFrm->SrcAddr, SUCCESS,&aExtendedAddress,NwkInfo.NwkAddr,
                               ZDP_ADDR_REQ_SINGLE, 0, 0,NULL,FALSE);
            }
            break;
        case IEEE_addr_req:
            break;        

        case End_Device_Bind_req:
            zdo_processEDBind(&AfMsg,&pAfFrm->SrcAddr);
            break;        

        case Bind_req:
        case Unbind_req:
            zdo_processBind_UnBind(&AfMsg,&pAfFrm->SrcAddr);
            break;        

        case End_Device_annce:
            /*Check in the address mapping, only care in the mapping table*/
            pAddrEntry = ib_searchAddrEntry(INVALID_SHORT_ADDR,(QWORD*)&AfMsg.pData[2]/*IEEE address*/);
            if (pAddrEntry) /*Existed, update the Nwk address*/
            {
                //pAddrEntry->ShortAddr = *(WORD*)AfMsg.pData; /*Check again TBD*/
            }
            else
            {
                ib_addAddrEntry(*(WORD*)(AfMsg.pData) /*Nwk address*/,
                                (QWORD*)&AfMsg.pData[2]/*IEEE address*/);
            }
                
            break;        
        default:
            break;
    }
    apf_releaseInComFrm(pAfFrm);
} // End of functions

/******************************************************************************/
//@FunctionName:  zdo_processEvent 										  	  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void zdo_processEvent(ZAPP_MSG*     pMsg)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    if (pMsg->MsgType == MSG_ZDO_START)
        DevState = DEV_INIT;
    if (DevState == DEV_INIT)
        zdo_startDevice(pMsg); /* Start device from beginning*/
    else
    {
        switch(pMsg->MsgType)
        {
        case MSG_ZDO_START:
            break;
        case MSG_ZDO_CONFIRM_RESET:
        case MSG_ZDO_CONFIRM_FORMATION:
        case MSG_ZDO_CONFIRM_JOIN:            
            err(ERR_UNRECOVERABLE,NULL,0);
            break;
        case MSG_ZDO_CONFIRM_DIS:
            break;
        case MSG_ZDO_INDI_JOIN:        
            mem_freeMem(pMsg->Data);
            break;
        case APF_DATA_INDICATION:
            zdo_processData(pMsg);
            break;
        case MSG_ZDO_BINDING_TIMEOUT:
            zdo_processEDBindTimeout(pMsg);
            break;            
        case MSG_ZDO_ED_ANNCE:
//             if (ZIGBEE_DEVICE_TYPE != ZIGBEE_PAN_COOR)                
//                 if (zdp_EndDeviceAnnce( NwkInfo.NwkAddr, &aExtendedAddress, FALSE) != SUCCESS)
//                     apf_sendAppMsg(EP_ID_ZDO,MSG_ZDO_ED_ANNCE,0x00);
            break;
        default:
            break;
        }
    }
       
} // End of functions
