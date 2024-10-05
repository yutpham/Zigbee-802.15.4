/*******************************************************************************/
//Module: 	                                                                   */   
//File Name: app_framework                                                     */
//Description: 	This file implements the application framework for zigbee app. */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//06/25/2007                                                  				   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "zconf.h"
#include "apf_format.h"
#include "app_framework.h"
#include "queue.h"
#include "schedul.h"
#include "aps.h"
#include "zdp.h"
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
//@FunctionName:  app_checkEP 										  	      */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
ENDPOINT_DESTCRIPTOR*  app_finEp(UINT8 EpId)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    for (UINT8 i = 0;i< ZIGBEE_NUM_EP; i++)
        if ( EndpointDescriptors[i] &&
            (EndpointDescriptors[i]->SimpleDescriptor.EndPoint == EpId))
            return EndpointDescriptors[i];
    return NULL;
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will add endpoint to the ep list                */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL app_addEP(ENDPOINT_DESTCRIPTOR*  pEp)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    for (UINT8 i = 0;i< ZIGBEE_NUM_EP; i++)
    {
        if (EndpointDescriptors[i] == NULL)
        {
            EndpointDescriptors[i] = pEp;
            return TRUE;
        }
    }
    return FALSE;    
} // End of functions


/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	 this function will send msg to endpoint                      */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void apf_sendAppMsg(UINT8 EP, UINT8  MsgType, UINT16 Data)
{
	/*********** Variable declaration ***********/
    ZAPP_MSG     Msg;
	/*********** Function body		 ***********/
    /*Send msg to the queue*/
    Msg.EndPointId  = EP;
    Msg.MsgType     = MsgType;
    Msg.Data        = Data;
    if (EP == EP_ID_ZDO)
        schedul_sendMsg(TASK_APER_NWK, &Msg);
    else
        schedul_sendMsg(TASK_APER_APP, &Msg);
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	 this function will send msg to endpoint                      */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void apf_sendMsg(ZAPP_MSG*     pMsg)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    /*Send msg to the queue*/
    if (pMsg->EndPointId == EP_ID_ZDO)
        schedul_sendMsg(TASK_APER_NWK, pMsg);
    else
        schedul_sendMsg(TASK_APER_APP, pMsg);
} // End of functions

/******************************************************************************/
//@FunctionName:   apf_dispatchEvent							              */
//@Description:	Manages Event handling in application framework               */
//	                                                                          */    
//@Param   par_pMsg -  ZAPP_MSG*                                              */
//	                                                                          */   
/******************************************************************************/
void apf_dispatchEvent(ZAPP_MSG*     par_pMsg)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/

    /* Incoming data is broadcasting to endpoint */
    if (par_pMsg->EndPointId == EP_ID_BROADCAST)
    {
        /* Send message to application msg queue */
        for (int i=1;i<ZIGBEE_NUM_EP;i++)
        { 
            if (EndpointDescriptors[i])
            {
                par_pMsg = (ZAPP_MSG*)par_pMsg;
                par_pMsg->EndPointId = EndpointDescriptors[i]->SimpleDescriptor.EndPoint;
                schedul_sendMsg(TASK_APER_APP,par_pMsg);		
            }
        }
    }
    /* Search for endpoint */
    else if (par_pMsg->EndPointId == EP_ID_ZDO)
        zdo_processEvent(par_pMsg);
    else
        /* Send message to application msg queue */
        schedul_sendMsg(TASK_APER_APP,par_pMsg);

} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	 This function will send a apf frame                          */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS apf_sendFrm(   AF_FRM* pFrm, UINT8 DestAddrMode,ADDRESS* pDestAddr, BYTE DstEndpoint, 
                        BYTE ClusterId, BYTE SrcEndpoint,BYTE TxOptions)
{
	/*********** Variable declaration ***********/
    ENDPOINT_DESTCRIPTOR*   pEp;    
	/*********** Function body		 ***********/
    pEp = app_finEp(SrcEndpoint);
    pFrm->pData[0] = BUILD_UINT8(pFrm->FrmType,pFrm->TransCount);
    return apsdeDataRequest(DestAddrMode,pDestAddr, DstEndpoint, pEp->SimpleDescriptor.AppProfId, 
                            ClusterId, SrcEndpoint,pFrm->nCurrOffset,
                            pFrm->pData,TxOptions, FALSE, DEFAULT_RADIUS);
} // End of functions


/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will build a single msg and send it using 
//              services in APS                                               */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS    apf_sendSingleMsg(BYTE* pData, UINT8 nDataLen,UINT8 TransSeq , 
                              AF_ADDR* pDstAddr, BYTE DstEndpoint, BYTE ClusterId, BYTE SrcEndpoint,
                              BYTE TxOptions)
{
	/*********** Variable declaration ***********/
    BYTE*       pBuffer;
    BYTE        *pTemp;
    BYTE        nBufLen;
    Z_STATUS    Status;
    ENDPOINT_DESTCRIPTOR*   pEp;
	/*********** Function body		 ***********/
    /*Calculate length of single msg*/
    nBufLen = 1 + 1 + 1 + nDataLen;
    pBuffer = mem_getMem(nBufLen);
    if (pBuffer)
    {
        /*Header */
        pTemp = pBuffer;
        *pTemp++ = BUILD_UINT8(AF_FRM_MSG,1);
        *pTemp++ = TransSeq;
        *pTemp++ = nDataLen;
        mem_cpy(pTemp,pData,nDataLen);
        pEp = app_finEp(SrcEndpoint);
        if (pEp)
            Status = apsdeDataRequest(pDstAddr->AddrMode,&pDstAddr->Addr, DstEndpoint, pEp->SimpleDescriptor.AppProfId, 
                                      ClusterId, SrcEndpoint,nBufLen,pBuffer,TxOptions, FALSE, DEFAULT_RADIUS);
        else
            Status = ZDP_INVALID_EP;
        mem_freeMem(pBuffer);
        return Status;
    }
    else
        return TRANSACTION_OVERFLOW;
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will send a single KVP using services in APS    */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS        apf_sendSingleKVP(UINT8 TransSeq, UINT8 CmdId, UINT8 AttrDataType, WORD AttrId, 
                              UINT8 ErrCode, BYTE* pData, UINT8 nDataLen,
                              AF_ADDR* pDstAddr, BYTE ClusterId, 
                              BYTE SrcEndpoint, BYTE TxOptions)
{
	/*********** Variable declaration ***********/
    BYTE*       pBuffer;
    BYTE        *pTemp;
    BYTE        nBufLen;
    Z_STATUS    Status;
    ENDPOINT_DESTCRIPTOR*   pEp;
	/*********** Function body		 ***********/
    /*Calculate length of single KVP*/
    nBufLen = SIZE_OF_KV_HR_RSP + nDataLen;
    pBuffer = mem_getMem(nBufLen);
    if (pBuffer)
    {
        /*Header */
        pTemp = pBuffer;
        *pTemp++ = BUILD_UINT8(AF_FRM_KVP,1);
        *pTemp++ = TransSeq;
        *pTemp++ = BUILD_UINT8(CmdId,AttrDataType);
        *pTemp++ = LO_UINT16( AttrId);
        *pTemp++ = HI_UINT16( AttrId);
        /*Error code*/
        if (CmdId > RESERVED_BEFORE_RES)        
            *pTemp++ = ErrCode;
        mem_cpy(pTemp,pData,nDataLen);
        pEp = app_finEp(SrcEndpoint);
        if (pEp)
            Status = apsdeDataRequest(pDstAddr->AddrMode,&pDstAddr->Addr, pDstAddr->EP, pEp->SimpleDescriptor.AppProfId, 
                                      ClusterId, SrcEndpoint,nBufLen,pBuffer,TxOptions, FALSE, DEFAULT_RADIUS);
        else
            Status = ZDP_INVALID_EP;

        mem_freeMem(pBuffer);
        return Status;
    }
    else
        return TRANSACTION_OVERFLOW;
} // End of functions

/******************************************************************************/
//@FunctionName:   apsdeDataConfirm						  	                  */
//@Description:	  Implements Data Confirm primitive                           */
//	                                                                          */    
//@Param:	DestAddrMode - Destination Address Mode                           */
//			DestAddr - Destination Address									  */
//			DstEndpoint - Destination Endpoint   							  */
//			SrcEndpoint - Source Endpoint       							  */
//			Status - the result of the request								  */
//	                                                                          */   
/******************************************************************************/
void apsdeDataConfirm(UINT8 DestAddrMode,ADDRESS* DestAddr,
                      BYTE  DstEndpoint, BYTE SrcEndpoint, 
                      APS_STATUS Status)
{
	/*********** Variable declaration ***********/
    ZAPP_MSG        appMsg;
	/*********** Function body		 ***********/
    appMsg.EndPointId   =   SrcEndpoint;
    appMsg.MsgType      =   APF_DATA_CONFIRM;
    appMsg.Data         =   DstEndpoint;
    apf_dispatchEvent(&appMsg);        
} // End of functions

/******************************************************************************/
//@FunctionName:   apsdDataIndication					  	                  */
//@Description:	   Implements the Data Indication primitive                   */
//	                                                                          */    
//@Param:                                                                     */
//	                                                                          */   
/******************************************************************************/
void apsdDataIndication(P_APS_FRM par_pFrm, UINT8 nDataLength, UINT8 SrcAddressMode, ADDRESS*  SrcAddr,
                        UINT8 LinkQuality, BYTE SecurityUse )
{
	/*********** Variable declaration ***********/
    ZAPP_MSG        appMsg;
    AF_IMCOM_FRM*   pAfFrm;
	/*********** Function body		 ***********/
    pAfFrm = apf_buildIncomFrm(par_pFrm, nDataLength,SrcAddressMode, 
                                SrcAddr, LinkQuality, SecurityUse);
    if (pAfFrm)
    {
        appMsg.EndPointId   =   par_pFrm->DestEndpoint;
        appMsg.MsgType      =   APF_DATA_INDICATION;
        appMsg.Data         =   pAfFrm;
        apf_dispatchEvent(&appMsg);        
    }
    /*Omit if can not build*/
} // End of functions
