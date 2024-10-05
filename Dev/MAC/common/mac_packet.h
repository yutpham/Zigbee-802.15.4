/*******************************************************************************/
//Module:  MAC/common                                                          */   
//File Name: mac_packet.h                                                      */
//Description: This component define the format of packet used internally by
//             MAC and functions to format the packet.						   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//01/15/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef MAC_PACKET_H
#define MAC_PACKET_H

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "mac_types.h"
#include "mac_state.h"

/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/
//define constant for patching the PHY Packet
#define MAC_FRAME_CTRL_LEN         2
#define MAC_SEQ_LEN                1
#define MAC_FCS_LEN                2
#define MAC_HR_LEN                 3

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/


//-----------------------------------------------------------------------//
//--------------------------BITE MASK USING IN MAC-----------------------//
// TX options 
#define TX_OPT_SECURITY_ENABLE 0x08
#define TX_OPT_INDIRECT        0x04
#define TX_OPT_GTS             0x02
#define TX_OPT_ACK_REQ         0x01
#define TX_OPT_NONE            0x00

//-------------------------------------------------------------------------------------------------------
// Transmission mode
#define TX_MODE_NO_CSMACA           0x00 /* Transmitted using NOT CSMA-CA */
#define TX_MODE_CSMACA              0x01 /* Transmitted using CSMA-CA */
#define TX_MODE_FORCE_UNSLOTTED     0x02 /* Force unslotted transmission in a beacon network */
#define TX_MODE_MAC_INTERNAL        0x04 /* The packet is transmitted by the MAC layer */
#define TX_MODE_SCAN_RELATED        0x08 /* This packet is related to channel scanning */
//-------------------------------------------------------------------------------------------------------
 


//-------------------------------------------------------------------------------------------------------
// MAC command payload lengths, including the command frame identifier

#define CMD_ASSOCIATION_REQUEST_PAYLOAD_LENGTH          2
#define CMD_ASSOCIATION_RESPONSE_PAYLOAD_LENGTH         4
#define CMD_DISASSOCIATION_NOTIFICATION_PAYLOAD_LENGTH  2
#define CMD_DATA_REQUEST_PAYLOAD_LENGTH                 1
#define CMD_PAN_ID_CONFLICT_NOTIFICATION_PAYLOAD_LENGTH 1
#define CMD_ORPHAN_NOTIFICATION_PAYLOAD_LENGTH          1
#define CMD_BEACON_REQUEST_PAYLOAD_LENGTH               1
#define CMD_COORDINATOR_REALIGNMENT_PAYLOAD_LENGTH      8
#define CMD_GTS_REQUEST_PAYLOAD_LENGTH                  2


//-------------------------------------------------------------------------------------------------------
// MAC command packet types
typedef enum {
    CMD_ASSOCIATION_REQUEST = 0x1,
    CMD_ASSOCIATION_RESPONSE,
    CMD_DISASSOCIATION_NOTIFICATION,
    CMD_DATA_REQUEST,
    CMD_PAN_ID_CONFLICT_NOTIFICATION,
    CMD_ORPHAN_NOTIFICATION,
    CMD_BEACON_REQUEST,
    CMD_COORDINATOR_REALIGNMENT,
    CMD_GTS_REQUEST
} MAC_COMMAND_TYPE;
//-------------------------------------------------------------------------------------------------------

//-----------------------------------------------------//
//Type of MAC packet using in the stack
//-----------------------------------------------------//
typedef enum 
{
    MAC_FRM_BEACON = 0,     //Beacon frame
    MAC_FRM_DATA ,          //Data frame
    MAC_FRM_ACK,            //ACK frame
    MAC_FRM_CMD             //command
}MAC_FRM_TYPE;

//-----------------------------------------------------//
//Status of packet in the pool
//-----------------------------------------------------//
typedef struct 
{
    UINT8      PktUsed:1;
    UINT8      Reserved:7;
}PktStatus;

//----------------------------------------------------------------//
//Define MPDU for MAC
//----------------------------------------------------------------//
typedef struct  
{
    UINT8      LinkQuality;
    UINT8      Reserved:7;
    UINT8      CRCCheck:1;
}MAC_FCS;


typedef struct  
{
    FRAME_CONTROL   FrmControl;
    UINT8           SeqNum;
    WORD            dstPanId;
    ADDRESS         dstAddr;
    WORD            srcPanId;
    ADDRESS         srcAddr;
    UINT8           nLength;
    BYTE            PayLoad[aMaxMACFrameSize];
    MAC_FCS         Fcs;
}MPDU;

typedef struct 
{
    MPDU                Mpdu;
}MPDU_RX;

typedef   MPDU_RX*  P_MPDU_RX;     


//----------------------------------------------------------------//
//Indirect packet info.
//----------------------------------------------------------------//
typedef struct  
{
    // For indirect packets
    INT16   nTimeToLive;         // This variable is signed because "time to live" becomes -1 when purged
    BOOL    bRequested;
    BOOL    bTransmissionStarted;// This flag indicates that the transmission has been started and that the packet cannot be removed
    UINT8   nPoolIndex;
}INDIRECT_PKT_INFO;

//----------------------------------------------------------------//
// Packet info.
//----------------------------------------------------------------//
typedef struct  
{
    UINT8   IndirectPktIndex;
    BYTE    txOptions;
    BYTE    msduHandle;
    BYTE    txMode;

    BOOL    bToCoord;
    UINT8   nDuration;      // Backoff periods including ACK and IFS
    UINT8   nRetryLeft;     // The number of retries left
}PKT_INFO;

//-----------------------------------------------------//
// CSMA-CA "state machine"
//-----------------------------------------------------//
typedef enum 
{
    CSMACA_STATE_UNDEFINED = 0,
    CSMACA_STATE_INIT,
    CSMACA_STATE_RANDOM_DELAY,
    CSMACA_STATE_TURNON_RX,
    CSMACA_STATE_PERFORM_CCA,
} CSMACA_STATE;

typedef enum 
{
    CSMACA_RESULT_NO_ERR = 0,
    CSMACA_RESULT_SHORT_TIME,
    CSMACA_RESULT_RESOURCE_SHORT,
    CSMACA_RESULT_SUCCESS,
    CSMACA_RESULT_FAILURE
}CSMACA_RESULT;

typedef struct  
{
    BOOL            bSlotted;
    CSMACA_STATE    eState;
    CSMACA_RESULT   eCurrentResult;

    UINT8           be;            // Back off exponent
    UINT8           nb;            // Number of backoffs    
    UINT8           cw;

    BOOL            bWaitBeaconForTx;
    UINT8           nRandomBackoff;

    PKT_INFO*       pPktInfo;
}CSMACA_INFO;

typedef enum 
{
    MAC_TX_INIT = 0,
    MAC_TX_PROCESSING,
    MAC_TX_TRANS,
    MAC_TX_TRANSMITTING,
    MAC_TX_WAIT_ACK,
    MAC_TX_ACKED,
    MAC_TX_NO_ACK,
    MAC_TX_IDLE
}MAC_TX_STATE;

//----------------------------------------------------------------//
// Packet TX structure
//----------------------------------------------------------------//
typedef struct {
    MPDU                Mpdu;       //MPDU
    PKT_INFO            PktInfo;    //Packet info.
    BOOL                bTransmitting;
} MPDU_TX;

typedef MPDU_TX*    P_MPDU_TX;

typedef struct  
{
    MAC_TX_STATE        eState;             // Current state of transmission
    BOOL                bFrmPending;        // This field holds the frm pending field of received ACK     
    BOOL                bUsedCsmaca;        // Use CSMA-CA or not
    CSMACA_INFO         sCsmaca;            // Info. to handle csma-ca procedure
    MPDU_TX*            pPacket;
}MAC_TX;
/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/


/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/******************************************************************************/
//@FunctionName: MAC_STATUS mac_formatMpdu(P_MPDU_TX par_pTxMpdu,             */
//@Description:	This function will format the MPDU                            */
//	                                                                          */
//@Param                                                                      */
//	                                                                          */
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
                          UINT8 nLen);


/******************************************************************************/
//@FunctionName:   mac_getMpduLength                 	  	                  */
//@Description:	This function will give the length of a valid MPDU. If the    */
//              MPDU is not valid, return value will be 0                     */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
inline UINT8      mac_getMpduLength(MPDU*    par_pMpdu);

#endif //!MAC_PACKET_H

