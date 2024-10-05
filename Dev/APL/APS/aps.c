/*******************************************************************************/
//Module: APL	                                                               */   
//File Name:   APS.C														   */
//Description: This file implements the Application Data Entity Primitives     */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//06/25/2007                                                   */
//------------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "types.h"
#include "utils.h"
#include "aps.h"
#include "aps_ib.h"
#include "app_framework.h"
#include "zdo.h"
#include "data_queue.h"
#include "ucos_ii.h"
#include "schedul.h"
#include "zdp.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/
#define         APS_INVALID_HANDLE          MAX_APS_DATA_QUEUE_SIZE

/*Status of data in the handler*/
typedef enum
{
    APS_DATA_BUF             = 0x01,         
    APS_DATA_WAIT_ACK
}APS_DATA_STATE;

typedef enum
{
    APS_INDIRECT_NONE       = 0x01,
    APS_INDIRECT_WAIT,
    APS_INDIRECT_SENDING       
};

typedef struct
{
    void*   pData;
    UINT8   nLen;
    UINT8   SourceEp;
    UINT8   DestEp;
    UINT16  ShortDesAddr;
    UINT8   Radius:6;
    UINT8   Indirect:1;
    UINT8   DiscoverRoute:1;
}ACK_RECORD;

/*Indirect transmission record*/
typedef struct  
{
    BYTE*           pData;
    UINT8           nLen;
    UINT8           Status;
    BINDING_ENTRY*  pCurBinding;
}INDIRECT_RECORD;

/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
/*Data queue*/
DATA_REC            ApsRecList[MAX_APS_DATA_QUEUE_SIZE];
DATA_QUEUE          ApsDataHandler;
UINT8               ApsCounter;

/*Current indirect transmission record*/
INDIRECT_RECORD     CurIndirectRec[MAX_APS_INDIREC_QUEUE_SIZE];
/******************************************************************************/
//@FunctionName:  aps_init											          */
//@Description:	  This function will initialize the APS. Called by ZDO        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void aps_init()
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    /*init data queue*/
    dataqueue_init(&ApsDataHandler,ApsRecList,MAX_APS_DATA_QUEUE_SIZE);
    /*Init aps counter*/
    ApsCounter              = ramdom_getRandomByte();
	
    for (UINT8 i = 0; i< MAX_APS_INDIREC_QUEUE_SIZE;i++)
        CurIndirectRec[i].Status = APS_INDIRECT_NONE;

    /*Init ib*/
    ib_initAddrMap();
    ib_initBindingTbl();

    aps_checkAck(NULL,NULL);
} // End of functions

/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/
/******************************************************************************/
//@FunctionName:   	aps_checkAck						  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void aps_checkAck(void* arg1, void* arg2)
{
	/*********** Variable declaration ***********/
    NWK_MSG     Msg;
	/*********** Function body		 ***********/
    /*Send msg to the queue*/
    Msg.MsgType = APS_ACK_TIMER;
    mtimer_creatTmr(&Msg,
                    (UINT32)(1000*apscMaxAckWaitDureation), 
                    TRUE,TASK_APER_NWK);
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void aps_sendApsMsg(UINT8       MsgType, UINT16 Data)
{
	/*********** Variable declaration ***********/
    NWK_MSG     Msg;
	/*********** Function body		 ***********/
    /*Send msg to the queue*/
    Msg.MsgType = MsgType;
    Msg.Data    = Data;
    schedul_sendMsg(TASK_APER_NWK, &Msg);
} // End of functions

/******************************************************************************/
//@FunctionName: aps_resend								  	                  */
//@Description:	                                                              */
//	                                                                          */
//@Param                                                                      */
//	                                                                          */
/******************************************************************************/
void aps_resend(UINT8 nHandle)
{
	/*********** Variable declaration ***********/
    ACK_RECORD*               pRec;
	/*********** Function body		 ***********/
    pRec = ApsDataHandler.RecList[nHandle].pData;
    nldeDataRequest(pRec->ShortDesAddr,pRec->nLen, pRec->pData,nHandle,
                    pRec->Radius,pRec->DiscoverRoute,FALSE);
    /*Do not care the status, just request*/
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function wil do task to finish transmission of a packet  */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void aps_endTrans(P_APS_FRM pFrm, ACK_RECORD*  pRec, UINT8  Index)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    /*If this is indirect transmission, inform to do next*/
    if (pRec->Indirect == 1)
        aps_processNextIndirecRec();           
    else
        /*Inform the requester*/
        apsdeDataConfirm(0,NULL,pRec->DestEp,pRec->SourceEp,SUCCESS);

    /*remove the frames and ack record*/
    mem_freeMem((BYTE*)(pRec->pData) - NWK_NPDU_HEADER_SIZE);/*Data convention: pay attention*/
    mem_freeMem(pRec);

    /*Remove data handler*/
    dataqueue_removeRec(&ApsDataHandler,Index);                    
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS  aps_sendData(ADDRESS* pDestAddr, UINT8 DestAddrMode,                        
                        BYTE    SrcEndpoint,BYTE DstEndpoint, 
                        BYTE    *par_pFrm, BYTE  nLen, BYTE RadiusCounter, 
                        BYTE    DiscoverRoute, BOOL SecurityEnable,
                        BYTE    TxOptions, BOOL bIndirectRelated)
{
	/*********** Variable declaration ***********/
    ADDR_MAP_ENTRY*     pAddrMap;
    ACK_RECORD*         pAckRec;
    Z_STATUS            Status;
    UINT8               tHandle;
	/*********** Function body		 ***********/
    tHandle = dataqueue_getFreeSlot(&ApsDataHandler);                      
    if (tHandle==ApsDataHandler.Size)
        return TRANSACTION_OVERFLOW;

    if(DestAddrMode == APS_ADDR_INDIRECT) /*Indirect msg*/
    {
        DestAddrMode        = APS_ADDR_SHORT;
        pDestAddr->Short    = 0x00;   /*TBD: change to binding capacity device*/
    }

    if(DestAddrMode == APS_ADDR_SHORT)
    {
        Status = nldeDataRequest(pDestAddr->Short,nLen,par_pFrm,tHandle,
                                RadiusCounter,DiscoverRoute,FALSE);
    }
    else if(DestAddrMode == APS_ADDR_EXT)
    {
        pAddrMap = ib_searchAddrEntry(INVALID_SHORT_ADDR,pDestAddr);
        if (pAddrMap)
        {
            /*Put data in the handler*/
            Status = nldeDataRequest(pAddrMap->ShortAddr,nLen,par_pFrm,tHandle,
                                    RadiusCounter,DiscoverRoute,FALSE);
            pDestAddr->Short  =  pAddrMap->ShortAddr; /*For latter use*/
        }
        else
            Status = APS_NO_BOUND_DEVICE;
    }

    if (Status == SUCCESS)
    {
        /*Add packet to the queue*/
        pAckRec = mem_getMem(sizeof(ACK_RECORD));
        if (pAckRec)
        {
            pAckRec->Indirect       = bIndirectRelated;
            pAckRec->SourceEp       = SrcEndpoint;
            pAckRec->DestEp         = DstEndpoint;
            pAckRec->DiscoverRoute  = DiscoverRoute;
            pAckRec->Radius         = RadiusCounter;
            pAckRec->ShortDesAddr   = pDestAddr->Short;
            pAckRec->nLen           = nLen;
            pAckRec->pData          = par_pFrm;
            dataqueue_addRec(&ApsDataHandler,pAckRec,sizeof(ACK_RECORD),tHandle,
                            ApsCounter - 1,APS_DATA_BUF);

            /*If ACK*/
            if (TxOptions&APS_TX_ACK_REQ)
                ApsDataHandler.RecList[tHandle].Status  = APS_DATA_WAIT_ACK;

            ApsDataHandler.RecList[tHandle].Reserved    = 0; /*The first time, init it to 0*/
            Status                                      = SUCCESS;
        }
        else
            Status = TRANSACTION_OVERFLOW;
    }
#ifdef ZIGBEE_STUB
    nldeDataConfirm(tHandle,SUCCESS);
#endif
#ifdef ZIGBEE_STUB
    nldeDataIndication(DestAddrMode, pDestAddr, nLen, par_pFrm, 0x00);
#endif
    return Status;
} // End of functions 

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will be call by APS framework to process indirect
//              record                                                        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void aps_processIndirectRec()
{
	/*********** Variable declaration ***********/
    P_APS_FRM           pFrm;
    P_APS_FRM_FROMCOOR  pFromCoorFrm;
    BINDING_ENTRY*      pBEntry;
    UINT8               index;
    INDIRECT_RECORD*    pIndirectRec;
	/*********** Function body		 ***********/
    /*Search if no sending packet, get the next to send*/
    for (index = 0; index <MAX_APS_INDIREC_QUEUE_SIZE; index++)
        if (CurIndirectRec[index].Status == APS_INDIRECT_SENDING)
            return;

    /*get the next to send*/
    for (index = 0; index <MAX_APS_INDIREC_QUEUE_SIZE; index++)
    {
        if (CurIndirectRec[index].Status == APS_INDIRECT_WAIT)
        {
            pIndirectRec = &CurIndirectRec[index];
            /*Check the type of the msg*/   
            pFrm = pIndirectRec->pData;
            if (pFrm->IndirectAddMode == 1) /*from coor: Indirect msg*/
            {
                /*Update the msg with new endpoint*/
                pFromCoorFrm                = (P_APS_FRM_FROMCOOR)pFrm;
                pFromCoorFrm->DestEndpoint  = pIndirectRec->pCurBinding->dstEP;
                pFrm->IndirectAddMode       = 0;
            }
            else
            {
                /*Direct message: just send it*/            
                pFrm->DestEndpoint  = pIndirectRec->pCurBinding->dstEP;
            }
            pIndirectRec->Status    =   APS_INDIRECT_SENDING;
            pBEntry = pIndirectRec->pCurBinding;
            aps_sendData(&pBEntry->dstAddr,APS_ADDR_SHORT,pBEntry->srcEP,pBEntry->dstEP,
                        pFrm,pIndirectRec->nLen,DEFAULT_RADIUS,FALSE,FALSE,
                        (pFrm->AckRequest==1)?APS_TX_ACK_REQ:0,TRUE);        
        }
    }
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	 This function will inform APS to process next indirect record*/
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void aps_processNextIndirecRec()
{
	/*********** Variable declaration ***********/
    BINDING_ENTRY*      pBindingEntry;
    INDIRECT_RECORD*    pIndirectRec;
    UINT8               index;
	/*********** Function body		 ***********/
    /*Check the current pending indirect record*/
    /*And then get the next free to send*/

    for (index = 0; index <MAX_APS_INDIREC_QUEUE_SIZE; index++)
        if (CurIndirectRec[index].Status == APS_INDIRECT_SENDING)
            break;

    if (index < MAX_APS_INDIREC_QUEUE_SIZE )
    {
        pBindingEntry = CurIndirectRec[index].pCurBinding;
        pBindingEntry = ib_searchIterativeBindingEntry(pBindingEntry->srcAddr,
                                                       pBindingEntry->srcEP,
                                                       pBindingEntry);
        if (pBindingEntry)
        {
            CurIndirectRec[index].pCurBinding = pBindingEntry;   
            CurIndirectRec[index].Status = APS_INDIRECT_WAIT;
        }
        else
        {
            /*Remove the current indirect record*/
            CurIndirectRec[index].pData = CurIndirectRec[index].pData - NWK_NPDU_HEADER_SIZE;
            //mem_freeMem(CurIndirectRec[index].pData);
            CurIndirectRec[index].Status    = APS_INDIRECT_NONE;
            CurIndirectRec[index].pData     = NULL;
            CurIndirectRec[index].pCurBinding = NULL;
            
            /*next indirect record*/
            aps_sendApsMsg(APS_INDIRECT_PROCESS_REQ, (UINT16)index);
        }
    }
} // End of functions


/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/******************************************************************************/
//@FunctionName:   apsdeDataRequest						  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
#define     FORMAT_APS_FRM(pFrm,FrType,DelivMode,IndirectMode,ProfileID,ClusterId,TxOptions,Ser)      \
                        pFrm = (BYTE*)pFrm+NWK_NPDU_HEADER_SIZE; \
                        pFrm->DeliveryMode      =   DelivMode; \
                        pFrm->FrmType           =   FrType;     \
                        pFrm->IndirectAddMode   =   IndirectMode; \
                        pFrm->AckRequest        =   (TxOptions&APS_TX_ACK_REQ); \
                        pFrm->Sercurity         =   Ser;                  \
                        pFrm->ProfileId         = ProfileID;                    \
                        pFrm->ClusterId         = ClusterId;                    \
                        pFrm->ApsCounter        = ApsCounter++;                            


Z_STATUS apsdeDataRequest( UINT8 DestAddrMode,ADDRESS* pDestAddr, BYTE DstEndpoint, 
                           UINT16 ProfileID, BYTE ClusterId, BYTE SrcEndpoint,
                           BYTE asduLength, BYTE *asdu, BYTE TxOptions, 
                           BYTE DiscoverRoute, BYTE RadiusCounter )
{
	/*********** Variable declaration ***********/
    UINT8               tHandle;
    P_APS_FRM           par_pFrm;
    P_APS_FRM_TOCOOR    par_pToCorFrm;
    UINT8               nLen;
    ADDR_MAP_ENTRY*     pAddrMap;
    Z_STATUS            Status;
    ACK_RECORD*         pAckRec;
    UINT8               index;
	/*********** Function body		 ***********/    
    /* Keep the transmission to evaluate ACK or resend*/
    tHandle = dataqueue_getFreeSlot(&ApsDataHandler);                      
    if (tHandle==ApsDataHandler.Size)
        return TRANSACTION_OVERFLOW;

    Status = SUCCESS;
    if (DestAddrMode == APS_ADDR_INDIRECT)
    {   
        Status = APS_NO_BOUND_DEVICE;
#if ZIGBEE_DEVICE_TYPE == ZIGBEE_PAN_COOR /*TBD*/
        /*Check there is space for the current node*/
        for (index = 0; index<MAX_APS_INDIREC_QUEUE_SIZE;index++)
            if (CurIndirectRec[index].Status == APS_INDIRECT_NONE)
                break;

        if (index == MAX_APS_INDIREC_QUEUE_SIZE)
            Status = TRANSACTION_OVERFLOW;      /*Support only number of indirect request at a time*/
        else
        {
            CurIndirectRec[index].pCurBinding = 
                ib_searchIterativeBindingEntry(NwkInfo.NwkAddr,SrcEndpoint,NULL);
            if (CurIndirectRec[index].pCurBinding)
                /*There is binding, format in indirect format*/
                Status = SUCCESS;
            else
                Status = APS_NO_BOUND_DEVICE;
        }
#endif
        /* Format the message */
        if (Status == APS_NO_BOUND_DEVICE)
        {
            /* Frm to coor.*/
            par_pToCorFrm = (P_APS_FRM_TOCOOR)mem_getMem(SIZE_OF_FULL_APS_HDR+NWK_NPDU_HEADER_SIZE+asduLength);
            if (par_pToCorFrm)
            {
                FORMAT_APS_FRM(par_pToCorFrm,APS_FRM_DATA,APS_DELIVERY_INDIRECT,1
                                ,ProfileID,ClusterId,TxOptions,FALSE);
                par_pToCorFrm->SrcEndPoint       = SrcEndpoint;
                nLen = SIZE_OF_FULL_APS_HDR-1;
                mem_cpy((BYTE*)par_pToCorFrm + nLen,asdu, asduLength);
                nLen        += asduLength;    
                
                par_pFrm    =   par_pToCorFrm;
                Status      = SUCCESS;
            }
            else
                Status = TRANSACTION_OVERFLOW;
                 
        }   
        else if (Status == SUCCESS)
        {
            /*There is some bound device, format in direct msg*/
            /*Format the frame*/
            par_pFrm =  (P_APS_FRM)mem_getMem(SIZE_OF_FULL_APS_HDR+NWK_NPDU_HEADER_SIZE+asduLength);
            if (par_pFrm)
            {
                FORMAT_APS_FRM(par_pFrm,APS_FRM_DATA,APS_DELIVERY_NORMAL,1,ProfileID,ClusterId,TxOptions,FALSE);

                par_pFrm->DestEndpoint      = DstEndpoint;
                par_pFrm->SrcEndPoint       = SrcEndpoint;

                nLen = SIZE_OF_FULL_APS_HDR;
                mem_cpy((BYTE*)par_pFrm + nLen,asdu, asduLength);
                nLen += asduLength;   
                
                Status      = SUCCESS;
            }
        }
        else
           /*There is a current indirect record*/
            Status = TRANSACTION_OVERFLOW;

        /* Store the indirect record for latter process */
        if (Status == SUCCESS)
        {
            if (par_pFrm->DeliveryMode == APS_DELIVERY_NORMAL)
            {
                CurIndirectRec[index].pData     = par_pFrm;
                CurIndirectRec[index].nLen      = nLen;
                CurIndirectRec[index].Status    = APS_INDIRECT_WAIT;    
                /*Send the request to process the indirect transmission*/
                aps_sendApsMsg(APS_INDIRECT_PROCESS_REQ, index);
            }
            else if (par_pFrm->DeliveryMode == APS_DELIVERY_INDIRECT)
            {
                Status = aps_sendData(pDestAddr, DestAddrMode, SrcEndpoint,DstEndpoint,
                                      par_pFrm,nLen,RadiusCounter,DiscoverRoute,FALSE,TxOptions,FALSE);
                if (Status != SUCCESS)
                    /*Release the memory*/
                    mem_freeMem((BYTE*)par_pFrm-NWK_NPDU_HEADER_SIZE) ;
            }
        }
    }   
    else /*Direct address mode*/
    {
        /*Format the frame*/
        par_pFrm =  (P_APS_FRM)mem_getMem(SIZE_OF_FULL_APS_HDR+NWK_NPDU_HEADER_SIZE+asduLength);
        if (par_pFrm)
        {
            FORMAT_APS_FRM(par_pFrm,APS_FRM_DATA,APS_DELIVERY_NORMAL,1,ProfileID,ClusterId,TxOptions,FALSE);            

            par_pFrm->DestEndpoint      = DstEndpoint;
            par_pFrm->SrcEndPoint       = SrcEndpoint;

            nLen = SIZE_OF_FULL_APS_HDR;
            mem_cpy((BYTE*)par_pFrm + nLen,asdu, asduLength);
            nLen += asduLength;           
        }
        else
            Status = TRANSACTION_OVERFLOW;  

        /* Address mode */
        if (Status == SUCCESS)
        {
            Status = aps_sendData(pDestAddr, DestAddrMode, SrcEndpoint,DstEndpoint,
                                  par_pFrm,nLen,RadiusCounter,DiscoverRoute,FALSE,TxOptions,FALSE);
            if (Status != SUCCESS)
                /*Release the memory*/
                mem_freeMem((BYTE*)par_pFrm-NWK_NPDU_HEADER_SIZE) ;
        }
    }    

    return Status;	
} // End of functions

/******************************************************************************/
//@FunctionName:   	nldeDataIndication					  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nldeDataIndication(UINT8  SrcAddrMode, ADDRESS*	pSrcAddress, BYTE NsduLength, BYTE *Nsdu, BYTE LinkQuality)
{
	/*********** Variable declaration ***********/
    P_APS_FRM           pFrm;
    P_APS_FRM           pACK;
    ACK_RECORD*         pRec;
    UINT8               index;
    UINT8               Status;
	/*********** Function body		 ***********/
    pFrm = (P_APS_FRM)Nsdu;
    switch(pFrm->FrmType)
    {
    case APS_FRM_ACK: 
        if (pFrm->DeliveryMode ==APS_DELIVERY_NORMAL)
        {
            pACK = pFrm;
            index = dataqueue_findRec(&ApsDataHandler,pACK->ApsCounter);
            if (index != APS_INVALID_HANDLE)
            {
                pRec = ApsDataHandler.RecList[index].pData;
                /*Check the ack*/
                if (pACK->SrcEndPoint == pRec->DestEp)
                {
                    aps_endTrans(pACK,pRec,index);
                }
            }
        }
    	break;
    case APS_FRM_CMD: 
    	break;
    case APS_FRM_DATA:
        if ((pFrm->DeliveryMode == APS_DELIVERY_NORMAL)
            || ((pFrm->DeliveryMode == APS_DELIVERY_INDIRECT)
                && (pFrm->IndirectAddMode == 0)))
        {            
            if (app_finEp(pFrm->DestEndpoint))
            {
                apsdDataIndication( pFrm, NsduLength,
                                    (SrcAddrMode==AM_SHORT_16)?APS_ADDR_SHORT:APS_ADDR_EXT,
                                    pSrcAddress,LinkQuality,FALSE);
                Status = SUCCESS;
            }
        }
        else if (pFrm->DeliveryMode == APS_DELIVERY_INDIRECT)
        {
            /*Check there is space for the current node*/
            for (index = 0; index<MAX_APS_INDIREC_QUEUE_SIZE;index++)
                if (CurIndirectRec[index].Status == APS_INDIRECT_NONE)
                    break;

            if (index == MAX_APS_INDIREC_QUEUE_SIZE)
            {
                Status = TRANSACTION_OVERFLOW;      /*Support only number of indirect request at a time*/
            }
            else
            {
                CurIndirectRec[index].pCurBinding   = 
                    ib_searchIterativeBindingEntry(pSrcAddress->Short,Nsdu[4]/*Endpoint*/,
                                                    NULL);
                if (CurIndirectRec[index].pCurBinding)
                    /*There is binding, format in indirect format*/
                    Status = SUCCESS;
                else
                {
                    Status = APS_NO_BOUND_DEVICE; /*omit*/
                }
            }

            /* Store the indirect record for latter process */
            if (Status == SUCCESS)
            {
                CurIndirectRec[index].pData   = mem_getMem(NWK_NPDU_HEADER_SIZE+NsduLength);
                if (CurIndirectRec[index].pData)
                {
                    CurIndirectRec[index].nLen      = NsduLength;
                    CurIndirectRec[index].pData     = CurIndirectRec[index].pData+NWK_NPDU_HEADER_SIZE;
                    CurIndirectRec[index].Status    = APS_INDIRECT_WAIT;    
                    mem_cpy(CurIndirectRec[index].pData, Nsdu, NsduLength);
                    /*Converted it to from coor. format: shift ClusterId and ProfileId*/
                    CurIndirectRec[index].pData[4] = CurIndirectRec[index].pData[3];
                    CurIndirectRec[index].pData[3] = CurIndirectRec[index].pData[2];
                    CurIndirectRec[index].pData[2] = CurIndirectRec[index].pData[1];
                    /*Send the request to process the indirect transmission*/
                    aps_sendApsMsg(APS_INDIRECT_PROCESS_REQ, index);
#ifdef ZIGBEE_STUB
                    aps_processIndirectRec(&CurIndirectRec[index]);
#endif
                }
            }
        }
        else if (pFrm->DeliveryMode == APS_DELIVERY_BROADCAST)
        {
            /*TBD: different frame format */
            Status = APS_NO_BOUND_DEVICE;
        }

        if (pFrm->AckRequest && Status == SUCCESS)
        {
            /* Send the ACK*/
            pACK = (P_APS_FRM)mem_getMem(sizeof(P_APS_FRM));
            if (pACK)
            {
                mem_cpy(pACK,pFrm,SIZE_OF_FULL_APS_HDR);
                pACK->FrmType = APS_FRM_ACK;
                nldeDataRequest(pSrcAddress->Short,(pFrm->DeliveryMode == APS_DELIVERY_NORMAL)?SIZE_OF_FULL_APS_HDR:(SIZE_OF_FULL_APS_HDR-1),
                                pACK,APS_INVALID_HANDLE, DEFAULT_RADIUS,FALSE,FALSE);
                mem_freeMem(pACK);
            }
        }

        break;
    default:
        break;
    }
} // End of functions
/******************************************************************************/
//@FunctionName:   	nldeDataConfirm						  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nldeDataConfirm(BYTE NsduHandle, NWK_STATUS Status)
{
	/*********** Variable declaration ***********/
    ACK_RECORD*             pRec;
    P_APS_FRM               par_pFrm;
	/*********** Function body		 ***********/
    /*Check the handler*/
    if (NsduHandle == APS_INVALID_HANDLE) /*This is un-stored transmission*/
    {
        /*Omit it*/
    }
    else if(NsduHandle < APS_INVALID_HANDLE)
    {
        pRec = ApsDataHandler.RecList[NsduHandle].pData;
        par_pFrm  = pRec->pData;
        if ((par_pFrm->DeliveryMode == APS_DELIVERY_NORMAL) ||
            (par_pFrm->DeliveryMode == APS_DELIVERY_INDIRECT))
        {

            if (par_pFrm->AckRequest) /*Hold to resend if need*/
            {
                if (Status == SUCCESS)
                    ApsDataHandler.RecList[NsduHandle].Status    = APS_DATA_WAIT_ACK;
                else
                    ApsDataHandler.RecList[NsduHandle].Status    = APS_DATA_WAIT_ACK;
            }
            else /*Release the memory*/
                aps_endTrans(par_pFrm,pRec,NsduHandle);
        }
        else if (par_pFrm->DeliveryMode == APS_DELIVERY_BROADCAST)
        {
            mem_freeMem((BYTE*)par_pFrm-NWK_NPDU_HEADER_SIZE) ; /*Data convention*/
        }
        else /*A-normal case*/
        {
#ifdef ZIGBEE_DEBUG
            err(ERR_UNRECOVERABLE,0,0,0);
#endif
        }
    }  
} // End of functions


/******************************************************************************/
//@FunctionName:  nwk_processApsEvent 										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nwk_processApsEvent(NWK_MSG*   pMsg, void* arg)
{
	/*********** Variable declaration ***********/
    UINT8                   Index;
    DATA_QUEUE*             pQueue;
    ACK_RECORD*             pRec;
    P_APS_FRM               pFrm;
	/*********** Function body		 ***********/
    if (pMsg->MsgType == APS_ACK_TIMER)
    {
        /*check the handler queue*/
        pQueue      = &ApsDataHandler;
        Index       = pQueue->FirstIndex;
        while (Index != pQueue->Size && pQueue->RecList[Index].pData) /* is the valid packet*/
        {
            pRec = pQueue->RecList[Index].pData;
            if (pQueue->RecList[Index].Status == APS_DATA_WAIT_ACK)
            {
                if (pQueue->RecList[Index].Reserved++ == apscMaxFrameRetries)
                {
                    /*If this is indirect transmission, inform to do next*/
                    pFrm = pRec->pData;
                    aps_endTrans(pFrm,pRec,Index);
                }
                else
                {
                    aps_resend(Index); /*resend*/

                    pQueue->RecList[Index].Status = APS_DATA_WAIT_ACK;
                    pQueue->RecList[Index].Reserved++;  /*Increase the count*/                    
                }
            }
            Index = pQueue->RecList[Index].NextIndex;
        }
    }
    if (pMsg->MsgType == APS_INDIRECT_PROCESS_REQ)
    {
        /*Process the next queued*/
        aps_processIndirectRec();
    }
} // End of functions


/*------------------------------------------------------------------------
                        APSME-BIND/UNBIND Service
-------------------------------------------------------------------------*/
/******************************************************************************/
//@FunctionName:  apsmeBindRequest  					  	                  */
//@Description:	  This function will add a binding in the binding table       */
//                - IEEE addr is mapped to
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS apsmeBindRequest(  WORD  SrcAddr,UINT8 SrcEndPoint, UINT8 ClusterId,
                            WORD  DesAddr,UINT8 DesEndPoint)
{
	/*********** Variable declaration ***********/
    BINDING_ENTRY*      pEntry;
    UINT8               index;
	/*********** Function body		 ***********/
    if (ZIGBEE_DEVICE_TYPE != ZIGBEE_PAN_COOR)
        return APS_ILLEGAL_DEVICE;

    /*Search if existed an such binding*/
    pEntry = ib_searchBindingEntry(SrcAddr,SrcEndPoint,DesAddr,DesEndPoint);
    if (pEntry)
    {
        /*Existed*/
        if (!ib_addCluster(pEntry,ClusterId)) /*Full*/
            return ZDP_TABLE_FULL;
        return ZDP_SUCCESS;
    }
    else
    {
        if(ib_addBindingEntry(SrcAddr,SrcEndPoint,DesAddr,DesEndPoint,1,&ClusterId))
            return ZDP_SUCCESS;
        else
            return ZDP_TABLE_FULL;

    }
} // End of functions

/******************************************************************************/
//@FunctionName:   apsmeUnbindRequest									  	  */
//@Description:	  This function will remove a binding in the binding table    */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS apsmeUnbindRequest(WORD  SrcAddr,UINT8 SrcEndPoint, UINT8 ClusterId,
                            WORD  DesAddr,UINT8 DesEndPoint)
{
	/*********** Variable declaration ***********/
    BINDING_ENTRY*      pEntry;
    UINT8               index;
	/*********** Function body		 ***********/
    if (ZIGBEE_DEVICE_TYPE != ZIGBEE_PAN_COOR)
        return APS_ILLEGAL_DEVICE;

    /*Search if existed an such binding*/
    pEntry = ib_searchBindingEntry(SrcAddr,SrcEndPoint,DesAddr,DesEndPoint);
    if (pEntry)  
    {
        /*Existed*/
        if (ib_removeCluster(pEntry,ClusterId)) /*Full*/            
        {
            if(pEntry->numClusterIds == 0) /*remove entry*/
                ib_removeBindingEntry(pEntry);

            return ZDP_SUCCESS;
        }
    }
    return APS_INVALID_BINDING;
} // End of functions
