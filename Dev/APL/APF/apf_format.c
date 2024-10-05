/*******************************************************************************/
//Module: 	                                                                   */   
//File Name: apf_format                                                        */
//Description: 	This file will implement the format of kvp/msg                 */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//07/27/2007                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "apf_format.h"
#include "app_framework.h"
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
//@FunctionName:   										  	                  */
//@Description:	This function will allocate memory and format the incom frm   */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
#define     FORMAT_INCOM_MSG(pFrm , pApsFrm, nDataLength,SrcAddressMode,pSrcAddr,LinkQuality )  \
                            pFrm->SrcAddr.AddrMode  = SrcAddressMode;   \
                            pFrm->SrcAddr.Addr      = *pSrcAddr;        \
                            pFrm->LinkQuality       = LinkQuality;      \
                            pFrm->Security          = SecurityUse;      \
                            pFrm->Broadcast         = (pApsFrm->DeliveryMode == APS_DELIVERY_BROADCAST); \
                            pFrm->ClusterId         = pApsFrm->ClusterId;      \
                            pFrm->CmdLen            = nDataLength;              

AF_IMCOM_FRM*  apf_buildIncomFrm(P_APS_FRM par_pFrm, UINT8 nDataLength, UINT8 SrcAddressMode, ADDRESS*  pSrcAddr,
                                 UINT8 LinkQuality, BYTE SecurityUse)
{
	/*********** Variable declaration ***********/
    AF_IMCOM_FRM*       pFrm;
    P_APS_FRM_FROMCOOR  pFromCoorFrm;
    P_APS_FRM_TOCOOR    pToCoorFrm;
	/*********** Function body		 ***********/
    pFrm = mem_getMem(sizeof(AF_IMCOM_FRM)+nDataLength);
    if (pFrm)
    {
        if (par_pFrm->DeliveryMode == APS_DELIVERY_NORMAL)
        {
            nDataLength = nDataLength - SIZE_OF_FULL_APS_HDR;
            FORMAT_INCOM_MSG(pFrm,par_pFrm,nDataLength,SrcAddressMode,pSrcAddr,LinkQuality);
            pFrm->DstEp = par_pFrm->SrcEndPoint;    
            mem_cpy(pFrm->Cmd,&par_pFrm->Payload,nDataLength);
        }
        if (par_pFrm->DeliveryMode == APS_DELIVERY_INDIRECT)
        {
            nDataLength = nDataLength - SIZE_OF_FULL_APS_HDR+1;
            if (par_pFrm->IndirectAddMode == 1)
            {
                pToCoorFrm = par_pFrm;
                pFrm->DstEp = pToCoorFrm->SrcEndPoint;    
                FORMAT_INCOM_MSG(pFrm,pToCoorFrm,nDataLength,SrcAddressMode,pSrcAddr,LinkQuality);
                mem_cpy(pFrm->Cmd,&pToCoorFrm->Payload,nDataLength);
            }
            else
            {
                pFromCoorFrm = par_pFrm;
                FORMAT_INCOM_MSG(pFrm,pFromCoorFrm,nDataLength,SrcAddressMode,pSrcAddr,LinkQuality);
                mem_cpy(pFrm->Cmd,&pFromCoorFrm->Payload,nDataLength);
            }
        }
        return pFrm;
    }
    return NULL;
} // End of functions


/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	  EP will call this function to release the frame after using */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void  apf_releaseInComFrm(AF_IMCOM_FRM*  pFrm)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    mem_freeMem(pFrm);
} // End of functions


/*----------------------------------------------------------------------------
                            PARSE OF INCOMMING AF FRAME
----------------------------------------------------------------------------*/

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
AF_FRM_TYPE apf_getTypeOfFrm(AF_IMCOM_FRM* pFrm)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    return HI_UINT8(pFrm->Cmd[0]);
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT8 apf_getNumOfTrans(AF_IMCOM_FRM* pFrm)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    return LO_UINT8(pFrm->Cmd[0]);
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	 This function will allocate memory and copy the requested 
//               transaction into it.                                         */
//              Notice that memory is shared between MSG transactions         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL apf_getMSGTrans(AF_IMCOM_FRM* pFrm, AF_MSG* pMsg, UINT8 nCount)
{
	/*********** Variable declaration ***********/
    UINT8           nIndex;
    UINT8           nOffset;
    AF_FRM_TYPE     FrmType;
    /*********** Function body		 ***********/
    FrmType = HI_UINT8(pFrm->Cmd[0]);
    if (nCount > LO_UINT8(pFrm->Cmd[0]))
		return FALSE;

    /*Build MSG*/
    if (FrmType == AF_FRM_MSG)
    {
        nOffset = 1;
        for ( nIndex = 0; nIndex < nCount-1; nIndex++ )
        {
            pMsg = &pFrm->Cmd[nOffset];
            nOffset += 1+1;                 /*Sequence number+data length*/
            nOffset += pMsg->DataLen;       /*Data length*/
        }
        /*Reach the destination*/
        pMsg->TransSeqNumber    = pFrm->Cmd[nOffset++];
        pMsg->DataLen           = pFrm->Cmd[nOffset++];
        pMsg->pData             = &pFrm->Cmd[nOffset++];
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
UINT8 apf_getKVPLength(AF_KVP* pCurKVP)
{
	/*********** Variable declaration ***********/
    UINT8       nLength;
	/*********** Function body		 ***********/
    /*Header length*/
    nLength = 0;
    if (pCurKVP->CmdType < RESERVED_BEFORE_RES)       /*Not response*/
        nLength += SIZE_OF_KV_HR;
    else
        nLength += SIZE_OF_KV_HR_RSP;    

    /* Attribute data */    
    switch(pCurKVP->AttrDataType)
    {
    case AF_ATT_NO_DATA:
        nLength += 0;
        break;
    case AF_ATT_UINT8:
    case AF_ATT_INT8:
        nLength += sizeof(UINT8);
        break;
    case AF_ATT_UINT16:
    case AF_ATT_INT16:
    case AF_ATT_SEMIPREC:
        nLength += sizeof(INT16);
        break;
    case AF_ATT_ABS_TIME:
    case AF_ATT_REL_TIME:
        nLength += sizeof(UINT32);
        break;
    case AF_ATT_OCTET_STR:
    case AF_ATT_CHAR_STR:
        nLength += pCurKVP->pData[0];
        break;
    default:
        break;
    }    
    return nLength;
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will extract a transaction from the KVP         */
//              Notice that memory is shared between KVP transactions         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL apf_getKVPTrans(AF_IMCOM_FRM* pFrm, AF_KVP* pKVP, UINT8 nCount)
{
	/*********** Variable declaration ***********/
    UINT8           nIndex;
    UINT8           nOffset;
    AF_FRM_TYPE     FrmType;
    AF_KVP*         pCurKVP;
    /*********** Function body		 ***********/
    FrmType = HI_UINT8(pFrm->Cmd[0]);
    if (nCount > LO_UINT8(pFrm->Cmd[0]))
		return FALSE;
    
    /*Build MSG*/
    if (FrmType == AF_FRM_KVP)
    {
        nOffset = 1;
        for ( nIndex = 0; nIndex < nCount-1; nIndex++ )
        {
            /*Check length of the KV*/
            pCurKVP = &pFrm->Cmd[nOffset];
            nOffset += apf_getKVPLength(pCurKVP);
        }

        /*Reach the destination*/
        pCurKVP = &pFrm->Cmd[nOffset];
        pKVP->AttrDataType  = pCurKVP->AttrDataType;
        pKVP->AttrId        = pCurKVP->AttrId;
        pKVP->CmdType       = pCurKVP->CmdType;
        pKVP->TransSeqNumber    = pCurKVP->TransSeqNumber;
        if (pCurKVP->CmdType < RESERVED_BEFORE_RES)
        {
            pKVP->DataLen = apf_getKVPLength(pCurKVP) - SIZE_OF_KV_HR;
            pKVP->pData   = &pFrm->Cmd[nOffset+SIZE_OF_KV_HR];
        }
        else
        {
            pKVP->DataLen = apf_getKVPLength(pCurKVP) - SIZE_OF_KV_HR;
            pKVP->ErrCode = pFrm->Cmd[nOffset+SIZE_OF_KV_HR];
            pKVP->pData   = &pFrm->Cmd[nOffset+SIZE_OF_KV_HR_RSP];
        }

        return TRUE;
    }
    return FALSE;
} // End of functions


/*----------------------------------------------------------------------------
                            BUILD OF AF FRAME BEFORE SEND
----------------------------------------------------------------------------*/
/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	 This function will init a frm before format it with data     */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
AF_FRM*    apf_initFrm(AF_FRM_TYPE    par_FrmType)
{
	/*********** Variable declaration ***********/
    AF_FRM*  pFrm;
	/*********** Function body		 ***********/
    pFrm = mem_getMem(sizeof(AF_FRM));
    if (pFrm)
    {
        pFrm->nCurrOffset   = 1;  /*reserver for header*/
        pFrm->TransCount    = 0;
        pFrm->nFrmSize      = 0;
        pFrm->FrmType       = par_FrmType;
        pFrm->pData         = NULL;
    }
    return pFrm;
} // End of functions


/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will add new MSG to the FRM after formatting frm*/
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL   apf_addMsg(AF_FRM*  par_pFrm, AF_MSG*   par_pMsg)
{
	/*********** Variable declaration ***********/
    BYTE*       pBuf;
	/*********** Function body		 ***********/
    if (par_pFrm ==NULL)
        return FALSE;

    pBuf = par_pFrm->pData;
    par_pFrm->pData = mem_getMem(par_pFrm->nCurrOffset+SIZE_OF_MSG_TRANS_HR+par_pMsg->DataLen);
    if (par_pFrm->pData == NULL)
    {
        par_pFrm->pData = pBuf;
        return FALSE;   
    }
    else
    {
        par_pFrm->TransCount++;
        mem_cpy(par_pFrm->pData,pBuf,par_pFrm->nCurrOffset);
        par_pFrm->pData[par_pFrm->nCurrOffset++] = par_pMsg->TransSeqNumber;
        par_pFrm->pData[par_pFrm->nCurrOffset++] = par_pMsg->DataLen;
        mem_cpy(&par_pFrm->pData[par_pFrm->nCurrOffset],par_pMsg->pData, par_pMsg->DataLen);
        par_pFrm->nCurrOffset += par_pMsg->DataLen;            
        if (pBuf)
            mem_freeMem(pBuf);

        return TRUE;
    }
    return FALSE;
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will release an allocated frm                   */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void apf_releaseFrm(AF_FRM*  par_pFrm)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    mem_freeMem(par_pFrm->pData);
    mem_freeMem(par_pFrm);
} // End of functions
