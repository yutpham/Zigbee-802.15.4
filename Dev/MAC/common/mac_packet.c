/*******************************************************************************/
//Module:  MAC/common                                                          */   
//File Name: mac_packet.c                                                      */
//Description: This component define the format of packet used internally by
//             MAC and functions to format the packet.                         */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date              Programmer      Details                                    */
//02/21/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*                  Include files                                       */
/************************************************************************/
#include "mac_packet.h"
/************************************************************************/
/*                  Constant declaration                                */
/************************************************************************/

/************************************************************************/
/*                  Type Definition                                     */
/************************************************************************/


/************************************************************************/
/*                  Variable declaration                                */
/************************************************************************/

/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/


/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/******************************************************************************/
//@FunctionName: msupCalcPacketDuration    	     		  	                  */
//@Description: Calculates the number of backoff slots required to transmit   */
//a packet, including (turnaround time, acknowledgment and) inter-frame spacing.*/
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
inline UINT8 msupCalcPacketDuration(UINT8 length, BOOL ackRequest) {
    UINT8 totalByteCount, lastBosByteCount;
    
    // Calculate the number of bytes in the whole packet (not including the preamble, but including
    // SFD, length-byte and PSDU)
    totalByteCount = 1 + 1 + length;
    
    // Acknowledgment request
    if (ackRequest) {
        
        // The number of bytes in the last backoff slot
        lastBosByteCount = totalByteCount % (aUnitBackoffPeriod / 2);
        
        // Round up the total count to N * (aUnitBackoffPeriod / 2), and add an extra slot if there isn't enough turnaround time before the ack (12-32 symbols)
        if (lastBosByteCount) totalByteCount += (aUnitBackoffPeriod / 2) - lastBosByteCount;
        
        // Add the bytes in the acknowledgment frame
        totalByteCount += (aUnitBackoffPeriod / 2) + (1 + 1 + 5);
    }
    
    // Add interframe spacing
    if (length > aMaxSIFSFrameSize) {
        totalByteCount += (aMinLIFSPeriod / 2);
    } else {
        totalByteCount += (aMinSIFSPeriod / 2);
    }
    
    // Round up
    return ((BYTE) (totalByteCount + ((aUnitBackoffPeriod / 2) - 1))) / (aUnitBackoffPeriod / 2);
    
} // msupCalcPacketDuration



/******************************************************************************/
//@FunctionName: mac_formatMpduHr                                             */
//@Description: This function will format the MPDU                            */
//                                                                            */
//@Param                                                                      */
//                                                                            */
/******************************************************************************/
inline MAC_STATUS mac_formatMpdu(P_MPDU_TX par_pTxMpdu,
                          BYTE type,
                          BYTE srcAddrMode,
                          WORD srcPanId,
                          ADDRESS* pSrcAddr,
                          BYTE desAddrMode,
                          WORD destPanId,
                          ADDRESS* pDestAddr,
                          BYTE txOptions,
                          BYTE* pPayLoad,
                          UINT8 nLen)
{
    /*********** Variable declaration ***********/
    MPDU*       pMpdu;
    /*********** Function body       ***********/
    pMpdu = &par_pTxMpdu->Mpdu;

    mem_set(&pMpdu->FrmControl,2);

    pMpdu->FrmControl.SourceAddMode = srcAddrMode;
    pMpdu->srcPanId = srcPanId;
    pMpdu->srcAddr = *pSrcAddr;
    pMpdu->FrmControl.DestAddMode = desAddrMode;
    pMpdu->dstPanId = destPanId;
    pMpdu->dstAddr = *pDestAddr;

    //payload
    pMpdu->nLength = nLen;
    mem_cpy((BYTE*)pMpdu->PayLoad,(BYTE*)pPayLoad,nLen);

    //Control frame
    pMpdu->SeqNum = mpib.macDSN++;          //sequence number
    pMpdu->FrmControl.FrameType = type;     //frame type   
    if (txOptions & TX_OPT_ACK_REQ)
        pMpdu->FrmControl.ACKRequest = 1;
    if ((srcAddrMode && desAddrMode) && (srcPanId == destPanId))
        pMpdu->FrmControl.IntraPAN = 1;

    //get the packet length
    nLen = mac_getMpduLength(pMpdu);
    //calculate the packet duration
    par_pTxMpdu->PktInfo.nDuration   = msupCalcPacketDuration(nLen, txOptions & TX_OPT_ACK_REQ);
    par_pTxMpdu->PktInfo.txOptions   = txOptions;

    if (desAddrMode == AM_SHORT_16)
        par_pTxMpdu->PktInfo.bToCoord    = ((mpib.macPANId == destPanId) && 
                                            (pDestAddr->Short == mpib.macCoordShortAddress));
    if (desAddrMode == AM_EXTENDED_64)
        par_pTxMpdu->PktInfo.bToCoord    = ((mpib.macPANId == destPanId) && 
                                            (pDestAddr->Extended == mpib.macCoordExtendedAddress));
    
    return SUCCESS;
} // End of functions

/****************************************************************************/
//@FunctionName: mac_getMpduHrLength                                          */
//@Description: This function will give the length of a valid MPDU. If the    */
//              MPDU is not valid, return value will be 0                     */
//                                                                            */    
//@Param                                                                      */
//                                                                            */   
/******************************************************************************/
inline UINT8 mac_getMpduHrLength(MPDU* par_pMpdu)
{
    /*********** Variable declaration ***********/
    UINT8       nLength = 0;
    /*********** Function body       ***********/
    /*Frame control + Seq*/
    nLength+= sizeof(FRAME_CONTROL)+sizeof(UINT8);

    /*Src Address*/
    if(par_pMpdu->FrmControl.SourceAddMode > 1)
        nLength+= sizeof(WORD) + (par_pMpdu->FrmControl.SourceAddMode)*6-10;

    /*Des Address*/
    if(par_pMpdu->FrmControl.DestAddMode > 1)
        nLength+= sizeof(WORD) + (par_pMpdu->FrmControl.DestAddMode)*6-10;

    if (par_pMpdu->FrmControl.IntraPAN)
    {
        nLength-=sizeof(WORD); /*without source PanId*/
    }    
    return nLength;
} // End of functions

/****************************************************************************/
//@FunctionName:   mac_getMpduLength                                          */
//@Description: This function will give the length of a valid MPDU. If the    */
//              MPDU is not valid, return value will be 0                     */
//                                                                            */    
//@Param                                                                      */
//                                                                            */   
/******************************************************************************/
inline UINT8 mac_getMpduLength(MPDU* par_pMpdu)
{
    /*********** Variable declaration ***********/
    /*********** Function body       ***********/           
    return par_pMpdu->nLength+mac_getMpduHrLength(par_pMpdu);
} // End of functions
