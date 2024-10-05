/*******************************************************************************/
//Module: NWK                                                                  */   
//File Name: nwk_data.c                                                        */
//Description: This file will implement network data handler
//              - Transmission
//              - Relay data												   */
//              - Broadcast data                                               */
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
#include "types.h"
#include "data_queue.h"
#include "nwk_common.h"
#include "nwk_data.h"
#include "zconf.h"
#include "nwk.h"
#include "utils.h"
#include "nwk_routing.h"
#include "nwk_msg.h"
#include "schedul.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/
#define APS_HANDLE_OFFSET           2*MAX_NWK_DATA_QUEUE_SIZE
#define NWK_HANDLE                  MAX_NWK_DATA_QUEUE_SIZE
#define BT_HANDLE                   (MAX_NWK_DATA_QUEUE_SIZE+1)
#define NWK_MAX_BTR                 3
/************************************************************************/
/*					Type Definition										*/
/************************************************************************/
typedef enum
{
    NWK_DATA_WAIT_SEND = 0x01,
    NWK_DATA_SENDING = 0x02
}NWK_DATA_STATUS;

/*Broadcasting record
*/
typedef struct  
{
    P_NPDU      pNpdu;
    UINT8       nLength;
    UINT16      NTIndexBitMap;                  /*Assume that neighbor table  size < 32*/
    UINT8       SrcIndex;
    UINT8       nwkDeliveryTimeElapsed;         /*In second*/
    UINT8       nwkAckElapsed;                  /*In second*/
    UINT8       nwkBrcRetries:3;              
    UINT8       Handle:5;
}BTR;

/* Broadcasting table
*/
typedef struct  
{
    BTR*        Btr[NWK_MAX_BTR];
    UINT8       nCount;
}BTT;

/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
/*Data queue*/
#if ZIGBEE_DEVICE_FFD == 1                  /* Code for FFD only */
/* Data queue
*/
DATA_REC            RecList[MAX_NWK_DATA_QUEUE_SIZE];
DATA_QUEUE          NwkDataQueue;
#endif //!ZIGBEE_DEVICE_FFD == 1                  /* Code for FFD only */
/* Broadcasting table
*/
BTT                 NwkBtt;
/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/


/************************************************************************/
/*                      Function                                        */
/************************************************************************/


/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void        nwk_dataInit()
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
#if ZIGBEE_DEVICE_FFD == 1                  /* Code for FFD only */
    /*init data queue*/
    dataqueue_init(&NwkDataQueue,RecList,MAX_NWK_DATA_QUEUE_SIZE);
#endif //!ZIGBEE_DEVICE_FFD == 1                  /* Code for FFD only */
} // End of functions

/*-----------------------------------------------------------------------------

                    Data management function

------------------------------------------------------------------------------*/

/******************************************************************************/
//@FunctionName: This function will 
//                  - Format the NPDU data at the beginning of the Nsdu
//                    this memory already spared for NWK header
//                  - Get new memory block (32 bytes) for NPDU command, format
//                    it  
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
P_NPDU nwk_makeNpdu(NWK_FRM_TYPE FrmType, BYTE NsduLength, BYTE *Nsdu)
{
	/*********** Variable declaration ***********/	
	P_NPDU npdu = NULL;	
	/*********** Function body		 ***********/
	if (FrmType == NWK_FRM_DATA){
        npdu = (P_NPDU)((BYTE*)Nsdu-NWK_NPDU_HEADER_SIZE);  /*Data convention: pay attention*/
        /* Header */
		npdu->FrmType	  =	NWK_FRM_DATA;		
        //PAYLOAD: already here	    
        return npdu;
	}
	if (FrmType == NWK_FRM_CMD)
    {
        /*TBD*/
    }	
	return npdu;
} // End of functions

/******************************************************************************/
//@FunctionName:  This function will responsible for sending a single packet  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS    nwk_sendUni(P_NPDU pNpdu,UINT8 nLength,UINT8 npduHandle, NWK_NEIGHBOR_NODE*  pNextHop)
{
	/*********** Variable declaration ***********/
    UINT8               TxOption;
    Z_STATUS            Status;
    UINT16              nPanId;
    QWORD*              pAddr;
    UINT8               AddrMode;
	/*********** Function body		 ***********/
    if (pNextHop != NULL)
    {
        if (pNextHop->RxOnWhenIdle)
            TxOption = ACK_TX;
        else if(pNextHop->Relationship == NWK_CHILD)
            TxOption = ACK_TX|IND_TX;  
        else
            return nwk_UNKNOWN_DEVICE;

        if (npduHandle == BT_HANDLE) /*No ACK*/
            TxOption &= ~ACK_TX;

        if (NwkInfo.NwkAddr != INVALID_SHORT_ADDR)
        {
            AddrMode    = AM_SHORT_16;
            pAddr       = &NwkInfo.NwkAddr;
        }
        else
        {
            AddrMode    = AM_SHORT_16;
            pAddr       = &aExtendedAddress;
        }


        mlmeGetRequest(MAC_PAN_ID,&nPanId);
        if (pNextHop->NetworkAddress != INVALID_SHORT_ADDR)
            Status =  mcpsDataRequest ( AddrMode, nPanId, pAddr, 
                                        AM_SHORT_16, nPanId, (ADDRESS*)&pNextHop->NetworkAddress, 
                                       nLength, (BYTE*) pNpdu, npduHandle, TxOption);
        else
            Status =  mcpsDataRequest ( AddrMode, nPanId, pAddr, 
                                        AM_EXTENDED_64, nPanId, (ADDRESS*)&pNextHop->ExtendedAddress, 
                                       nLength, (BYTE*) pNpdu, npduHandle, TxOption);
            return Status;
    }
    else
        return  nwk_UNKNOWN_DEVICE;
} // End of functions
/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nwk_sendBroadcast(BTR*     pBtr)
{
	/*********** Variable declaration ***********/
    UINT16  wTemp;
    UINT16  BitMap;
	/*********** Function body		 ***********/
    if (pBtr != NULL)
    {
        BitMap = nwk_getNtBitMap();
        for (UINT8 Index = 0; Index < NWK_NEIGHBOR_TBLE_SIZE; Index++)
        {
            wTemp = 1<<Index;
            if ((!(pBtr->NTIndexBitMap&wTemp)) && (BitMap&wTemp))
            {                
                /*Check Source*/
                if (Index == pBtr->SrcIndex)
                {
                    wTemp = (UINT16)(1<<pBtr->SrcIndex);
                    pBtr->NTIndexBitMap |= wTemp;
                    nwk_sendUni(pBtr->pNpdu, pBtr->nLength, 
                    BT_HANDLE,nwk_neighborTable[Index]);
                }
                else
                    nwk_sendUni(pBtr->pNpdu, pBtr->nLength, 
                    BT_HANDLE,nwk_neighborTable[Index]);
            }
        }
    }
} // End of functions

/******************************************************************************/
//@FunctionName:  This function will responsible for sending a single packet  */
//                  - For relaying data: get a new slot handler and transmit
//                  - For sending data from APS: send it with handler from APS*/
//                  - With NsduHandle = 0: get queued relaying packet and send it*/
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS    nwk_send(P_NPDU pNpdu,UINT8 nLength, UINT8    npduHandle)
{
	/*********** Variable declaration ***********/
    NWK_NEIGHBOR_NODE*  pNextHop;
	/*********** Function body		 ***********/
	/*Routing procedure: if node has no routing capacity or
      no path, use tree routing if available: Need to correct this  */      
    if (nwk_ib.nwkUseTreeRouting)
    {
#if ZIGBEE_DEVICE_FFD == 1                  /* Code for FFD only */
        pNextHop  = routing_getNextNodeInHier(pNpdu->DesAddr);
#else
        pNextHop  = NwkInfo.pParent;
#endif
        if (pNextHop)
            return nwk_sendUni(pNpdu,nLength,npduHandle, pNextHop);
    }
    TOGGLE_OLED();
    /* TBD: Routing table, buffer to wait the route ...*/
    return  nwk_UNKNOWN_DEVICE;
} // End of functions



/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
#if ZIGBEE_DEVICE_FFD == 1                  /* Code for FFD only */
void nwk_transmitQueuedPkt()
{
	/*********** Variable declaration ***********/
    Z_STATUS            Status;
    UINT8               npduHandle;
	/*********** Function body		 ***********/
    npduHandle = dataqueue_getRec(&NwkDataQueue,NWK_DATA_WAIT_SEND);
    if (npduHandle < NwkDataQueue.Size)
    {
        Status = nwk_send(NwkDataQueue.RecList[npduHandle].pData,
                          NwkDataQueue.RecList[npduHandle].nLength,npduHandle/*Old packet*/);
        if (Status  == SUCCESS)
            NwkDataQueue.RecList[npduHandle].Status = NWK_DATA_SENDING;
    }
} // End of functions
#endif //!ZIGBEE_DEVICE_FFD == 1                  /* Code for FFD only */


/******************************************************************************/
//@FunctionName:  This function will handle broadcasting process              */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS    nwk_broadcast(P_NPDU pNpdu,UINT8 nLength, UINT8 AddrMode, ADDRESS*  pSrcAddr, UINT8 nHandle)
{
	/*********** Variable declaration ***********/
    UINT8       i;
    UINT16      nTemp;
    UINT8       nFreeIndex;
    NWK_MSG     Msg;
	/*********** Function body		 ***********/
    /*This function is called for new incoming broadcast packet*/
    /*Check in the BTT*/
    nFreeIndex = NWK_MAX_BTR;
    for (i = 0; i< NWK_MAX_BTR; i++)
    {
        if ((NwkBtt.Btr[i] != NULL)
            && (NwkBtt.Btr[i]->pNpdu->SeqNumber  == pNpdu->SeqNumber)
            && (NwkBtt.Btr[i]->pNpdu->SrcAddr    == pNpdu->SrcAddr))
        {
            /*Btr existed*/
            break;
        }
        else if ((NwkBtt.Btr[i] == NULL)
                && (nFreeIndex == NWK_MAX_BTR))
            nFreeIndex = i;
    }

    if (i < NWK_MAX_BTR)
    {            
        /*Mark device have relayed */
        nFreeIndex = nwk_findIndexInNT(AddrMode, pSrcAddr);
        nTemp = (UINT16)(1<<nFreeIndex);
        NwkBtt.Btr[i]->NTIndexBitMap |= nTemp;
        return nwk_ALREADY_PRESENT;
    }

    if ( (nFreeIndex < NWK_MAX_BTR)&&(i == NWK_MAX_BTR) )   /*record is not existed*/
    {
        /* Create new record*/
        if (pNpdu->Radius >0)
        {
            NwkBtt.Btr[nFreeIndex] = mem_getMem(sizeof(BTR));
            if (NwkBtt.Btr[nFreeIndex])
            {
                /*Set data for new record*/
                NwkBtt.Btr[nFreeIndex]->nLength         = nLength;
                NwkBtt.Btr[nFreeIndex]->pNpdu           = pNpdu;
                NwkBtt.Btr[nFreeIndex]->NTIndexBitMap   = 0;
                NwkBtt.Btr[nFreeIndex]->nwkAckElapsed   = 0;
                NwkBtt.Btr[nFreeIndex]->nwkBrcRetries   = 0;

                /*Handle*/
                NwkBtt.Btr[nFreeIndex]->Handle = nHandle;

                /*If from others*/
                if (pSrcAddr != NULL)
                    NwkBtt.Btr[nFreeIndex]->SrcIndex  = nwk_findIndexInNT(AddrMode, pSrcAddr);
                else
                    NwkBtt.Btr[nFreeIndex]->SrcIndex = NWK_NEIGHBOR_TBLE_SIZE;

                /*Create timer*/
                Msg.MsgType = MSG_BT_JITER_TIMEOUT;
                Msg.Status  = nFreeIndex;
                Msg.Data    = 0;
                mtimer_creatTmr(&Msg,(UINT32)ramdom_getRandomByte()%nwkcMaxBroadcastJitter,FALSE,TASK_APER_NWK);
                NwkBtt.Btr[nFreeIndex]->nwkDeliveryTimeElapsed   = 0;
            
                /*Create the periodical timer*/
                if (NwkBtt.nCount == 0) 
                {
                    Msg.MsgType = MSG_BT_1s_TIMEOUT;
                    Msg.Data    = 0;
                    Msg.Status  = 0;
                    mtimer_creatTmr(&Msg,(UINT32)1000,TRUE,TASK_APER_NWK);
                }
                NwkBtt.nCount++;
                i= nFreeIndex;

                return SUCCESS;  /*New broadcasting packet*/
            }
            else
                return TRANSACTION_OVERFLOW;
        }        
        else if (pNpdu->Radius == 0)
            return nwk_NOT_PERMITTED;
    }
    else 
        return TRANSACTION_OVERFLOW;
} // End of functions

/******************************************************************************/
//@FunctionName:    									  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS nldeDataRequest(UINT16 DstAddr, BYTE NsduLength, BYTE *Nsdu, 
                           BYTE NsduHandle, BYTE Radius, BYTE DiscoverRoute, BOOL SecurityEnable)
{
	/*********** Variable declaration ***********/
    P_NPDU      pNpdu;  
    Z_STATUS    Status;
	/*********** Function body		 ***********/
    if(SecurityEnable)
        err(ERR_UNRECOVERABLE,0,0,0);
	if (NwkInfo.NwkState != NWK_STATE_NORMAL_MODE)
		return nwk_INVALID_REQUEST;
	else 
	{ 
		// TBD: Check NIB if Security is enabled: if NIB = 0 then ignore SecurityEnable
        
		/* Form the NPDU */
        pNpdu = nwk_makeNpdu(NWK_FRM_DATA,NsduLength,Nsdu);
        
		if (!pNpdu)
            return nwk_TABLE_FULL;

        /* Header */
		pNpdu->ProtocolVer	=	nwkcProtocolVersion;
        pNpdu->DiscoverRoute	=	nwk_ib.nwkUseTreeRouting ? NWK_DISCO_SUPPRESS:DiscoverRoute;	/*Use tree routing*/
		if((nwkcDefaultSecurityLevel ==	0x00) || (NwkInfo.SecurityEnable == FALSE) || (SecurityEnable == FALSE)){
			pNpdu->Security	= 0x00;
		}	
		else {
			pNpdu->Security	= 0x00; 	/* security not implemented */
		}

		//ADDRESS FIELD
		pNpdu->DesAddr	    =	DstAddr;
		pNpdu->SrcAddr	    =	NwkInfo.NwkAddr;
		pNpdu->Radius	    =	Radius;
		pNpdu->SeqNumber	=	nwk_ib.nwkSequenceNumber++;

        mlmeGetRequest(MAC_SHORT_ADDRESS,&pNpdu->SrcAddr);
        pNpdu->DiscoverRoute    = DiscoverRoute;
        pNpdu->Security         = FALSE;    /*Not implemented*/
        if (Radius)
            pNpdu->Radius = Radius;
        else
            pNpdu->Radius = 2*nwk_ib.nwkMaxDepth;   

        if (DstAddr != BROADCAST_ADDR)
            Status = nwk_send(pNpdu,NsduLength+NWK_NPDU_HEADER_SIZE, NsduHandle+APS_HANDLE_OFFSET);
        else if (DstAddr == BROADCAST_ADDR && pNpdu->Radius > 0)
        {
            Status = nwk_broadcast(pNpdu,NsduLength+NWK_NPDU_HEADER_SIZE, AM_SHORT_16, 
                                   NULL,NsduHandle+APS_HANDLE_OFFSET);
        }
        else
            Status =  nwk_INVALID_PARAMETER;

        return Status;
        /*No memory handling needed here*/
	}
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nwk_processDataIndication(NWK_MSG*   pMsg)
{
	/*********** Variable declaration ***********/
    MCPS_DATA_INDICATION*           pIndiData;
    P_NPDU                          pNpdu;
    UINT8                           npduHandle;
	Z_STATUS						Status;
	/*********** Function body		 ***********/
    pIndiData   = pMsg->Data;       
    pNpdu       = (P_NPDU)pIndiData->pMsdu;

    /* Decrement the radius*/
    pNpdu->Radius--;
    
    /*send packet to upper layer*/
    if(pNpdu->DesAddr == NwkInfo.NwkAddr)
    {
        if (pNpdu->FrmType == NWK_FRM_DATA)
        {
	        nldeDataIndication(AM_SHORT_16,&pNpdu->SrcAddr, pIndiData->msduLength-NWK_NPDU_HEADER_SIZE, 
                                &pNpdu->Payload, pIndiData->mpduLinkQuality);
        }
        else if(pNpdu->FrmType == NWK_FRM_CMD)
        {
        }
        mem_freeMem(pNpdu);
    }
    /*Broadcast the packet*/
    else if(pNpdu->DesAddr == BROADCAST_ADDR)
    {
        Status = nwk_broadcast( pNpdu,pIndiData->msduLength, 
                                pIndiData->srcAddrMode, &pIndiData->srcAddr,NWK_HANDLE);

        if (Status == SUCCESS ||Status == nwk_NOT_PERMITTED)
        {
            if (pNpdu->FrmType == NWK_FRM_DATA)
                nldeDataIndication(AM_SHORT_16,&pNpdu->SrcAddr, pIndiData->msduLength-NWK_NPDU_HEADER_SIZE, 
                        &pNpdu->Payload, pIndiData->mpduLinkQuality);
            else if(pNpdu->FrmType == NWK_FRM_CMD)
            {
                /*Later*/
            }
        }

        if(Status != SUCCESS)
            mem_freeMem(pIndiData->pMsdu); /*no resource or already existed, discard data*/
    }
#if  ZIGBEE_DEVICE_FFD == TRUE
    else if (pNpdu->Radius > 0)
    {   /* Relay packet */
        npduHandle = dataqueue_getFreeSlot(&NwkDataQueue);
        if (npduHandle < NwkDataQueue.Size) /*Not available*/
            Status = nwk_send(pNpdu,pIndiData->msduLength,npduHandle);

        if (npduHandle == NwkDataQueue.Size)
            /*Can not relay packet, discard it*/
            mem_freeMem(pNpdu);
        else
        {
            Status = (Status == SUCCESS)? NWK_DATA_SENDING:NWK_DATA_WAIT_SEND;
            dataqueue_addRec(&NwkDataQueue,pNpdu,pIndiData->msduLength,npduHandle,NWK_HANDLE,Status);
        }
    }        
#else
    else
        /*This is end-device, discard it*/
        mem_freeMem(pNpdu);
#endif        
    mem_freeMem(pIndiData);/*Free indication msg*/
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nwk_processDataConfirm(NWK_MSG*   pMsg)
{
	/*********** Variable declaration ***********/
    DATA_REC*       pRec;
    P_NPDU          pNpdu;
	/*********** Function body		 ***********/
    /*This packet is on nwk duty*/
    if (pMsg->Data < NWK_HANDLE)
    {
        if (pNpdu->SrcAddr == NwkInfo.NwkAddr 
            && pNpdu->FrmType == NWK_FRM_CMD)
        {
            /* Process end of transmission of cmd */   
            /*Cmd need to be handled in the handler tabld*/
            return;
        }
#if  ZIGBEE_DEVICE_FFD == TRUE	
        pRec    = &NwkDataQueue.RecList[pMsg->Data];
        pNpdu   = pRec->pData;
        /*if broadcast packet*/
        if (pNpdu->DesAddr == BROADCAST_ADDR)
        {
            /*Process broadcast packet*/
        }
        else if (pNpdu->SrcAddr != NwkInfo.NwkAddr 
            && pNpdu->FrmType != NWK_FRM_CMD)
        {
            /* Finished relaying data */   
            /*remove data handle*/
            dataqueue_removeRec(&NwkDataQueue,pMsg->Data);    

            /*release data*/
            mem_freeMem(pNpdu);

            /* Transmit queued relayed packet*/
            nwk_transmitQueuedPkt();
        }
        else
        {
            /*release data*/
            mem_freeMem(pNpdu);
        }
#endif  //!ZIGBEE_DEVICE_FFD == TRUE
    }
    else    /*This packet is on APS duty*/
        nldeDataConfirm((UINT8)pMsg->Data-APS_HANDLE_OFFSET, pMsg->Status);
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nwk_processBtEvent(NWK_MSG*    pMsg)
{
	/*********** Variable declaration ***********/
    UINT8       npduHandle; 
    UINT8       index;
    NWK_MSG     Msg;
	/*********** Function body		 ***********/
    if (pMsg->MsgType == MSG_BT_JITER_TIMEOUT)
    {
        nwk_sendBroadcast(NwkBtt.Btr[Msg.Status]);
    }
    if (pMsg->MsgType == MSG_BT_1s_TIMEOUT) /*Update the BTT*/
    {        
        for (UINT8 i = 0; i< NWK_MAX_BTR; i++)
        {
            if (NwkBtt.Btr[i] != NULL)        
            {
                NwkBtt.Btr[i]->nwkAckElapsed++;
                NwkBtt.Btr[i]->nwkDeliveryTimeElapsed++;

                if (NwkBtt.Btr[i]->nwkAckElapsed == nwk_ib.nwkPassiveAckTimeout)
                { 
                    /*resend*/
                    nwk_sendBroadcast(NwkBtt.Btr[i]);
                    NwkBtt.Btr[i]->nwkAckElapsed = 0;
                    NwkBtt.Btr[i]->nwkBrcRetries++;
                }
                if ((NwkBtt.Btr[i]->nwkDeliveryTimeElapsed == nwk_ib.nwkBroadcastDeliveryTime)
                    || (NwkBtt.Btr[i]->nwkBrcRetries == nwk_ib.nwkMaxBroadcastRetries))
                    /* remove */
                {
                    /*remove the record*/
                    if (NwkBtt.Btr[i]->Handle >= APS_HANDLE_OFFSET)
                        nldeDataConfirm(NwkBtt.Btr[i]->Handle-APS_HANDLE_OFFSET,SUCCESS);
                    else
                        mem_freeMem(NwkBtt.Btr[i]->pNpdu);

                    mem_freeMem(NwkBtt.Btr[i]);
                    NwkBtt.Btr[i] = NULL;

                    NwkBtt.nCount--;
                    if (NwkBtt.nCount == 0) /*End the periodical timer*/
                    {
                        Msg.MsgType = MSG_BT_1s_TIMEOUT;
                        Msg.Data    = 0;
                        Msg.Status  = 0;
                        mtimer_endTmr(&Msg);
                    }
                }
            }
        }
    }
} // End of functions
