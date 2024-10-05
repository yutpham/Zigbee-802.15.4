/*******************************************************************************/
//Module: NWK                                                                  */   
//File Name: nwk_msg.h                                                         */
//Description: This file will define the msg_type and msg_format for the event
//             of network layer    
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//05/28/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef NWK_MSG_H
#define NWK_MSG_H

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "types.h"

/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/


/************************************************************************/
/*					Type Definition										*/
/************************************************************************/
/* General message for all application
    - Zigbee maintains the conformance of message type between APF, APS, ZDO and NWK
        + NWW message type starts from 0x01-0x1F
        + ZDO message type starts from 0x20-0x3F
        + APS message type starts from 0x40-0x4F
        + APF message type starts from 0x50-0x5F
        + From 0x60 is application definition types
*/
typedef enum
{
    /* Confirmation */
    MSG_MAC_CONFIRM_ASSOCIATION = 0x01,
    MSG_MAC_CONFIRM_SCAN,
    MSG_MAC_CONFIRM_START,    
    MSG_MAC_CONFIRM_DATA,

    MSG_MAC_CONFIRM_DISASSOCIATION,
    MSG_MAC_CONFIRM_RESET,
    MSG_MAC_CONFIRM_POLL,   
    MSG_MAC_CONFIRM_SYNC,   

    /* Indication */
    MSG_MAC_INDI_DATA,
    MSG_MAC_INDI_ORPHAN,
    MSG_MAC_INDI_COMM_STATUS,
    
    MSG_MAC_INDI_ASSO,
    MSG_MAC_INDI_DISASSOC,
    MSG_MAC_INDI_BEACON,
    MSG_MAC_INDI_LOSS,

    /* Internal msg */
    MSG_PERMIT_JOIN_TIMEOUT,

    /* Broadcasting*/
    MSG_BT_JITER_TIMEOUT,
    MSG_BT_1s_TIMEOUT,

    MSG_NWK   = 0x1F
} NWK_MSG_TYPE;


/* All the data sent from MAC to NWK are defined in this section
*/
/* Msg for specific cases
*/
/* Association/Indication */
typedef struct  
{
    QWORD   devAddr;
    BYTE    CapacityInfor;
    BOOL    Sercurity;
    UINT8   aclEntry;
}MSG_ASSO_INDI_DATA;


/* Disassociation/Indication */
typedef struct  
{
    QWORD   devAddr;
    BYTE    Reason;
    BOOL    Sercurity;
    UINT8   AclEntry;
}MSG_DISASSO_INDI_DATA;

/* All the data sent from NWK to Upper are defined in this section
*/
typedef struct  
{
    WORD    ShortAddr;
    QWORD   ExtAddr;
    BYTE    CapInfor;
    BOOL    Sercurity;
}MSG_JOIN_INDI;


/* Comm Status */
typedef struct  
{
    WORD    panId;
    BYTE    srcAddrMode;
    ADDRESS SrcAddr;
    BYTE    status;
}MSG_COMM_STATUS_DATA;

/*Do not change this order, APF, NWK, ZDO has conformance in msg format
    - First byte is msg_type
*/
typedef struct  
{
    NWK_MSG_TYPE         MsgType;       
    UINT8                Status;
    WORD                 Data;
}NWK_MSG;

/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/

/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/

#endif //!NWK_MSG_H

