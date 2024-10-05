/*******************************************************************************/
//Module: APL/ZDO 	                                                           */
//File Name: zdo.h                                                             */
//Description: This file implement the behaviors of the ZDO of Coordinator,    */
//             Router or End Device in zigbee network                          */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//06/26/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef ZDO_H
#define ZDO_H

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "nwk.h"
#include "nwk_msg.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/
typedef enum  
{
    DEV_INIT = 0x01,        /* Start device from beginning, in the initialization mode*/
    DEV_STARTED             /* Device started, in the normal mode*/
}DEV_STATE;

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
    MSG_ZDO_START = 0x20,
    MSG_ZDO_CONFIRM_RESET,

    MSG_ZDO_CONFIRM_FORMATION,
    MSG_ZDO_CONFIRM_DIS,
    MSG_ZDO_CONFIRM_JOIN,
    MSG_ZDO_CONFIRM_START_ROUTER,
    MSG_ZDO_CONFIRM_DATA,

    MSG_ZDO_INDI_JOIN,
    MSG_ZDO_INDI_SYNC,

    MSG_ZDO_ED_ANNCE,               /*Announce end device*/
    MSG_ZDO_BINDING_TIMEOUT,        /*Process binding timeout*/
    

    /* End of ZDO event */
    MSG_ZDO = 0x3F
} ZDO_MSG_TYPE;

/* Discovery confirm msg
*/
typedef struct  
{
    NWK_STATUS      Status;
    UINT8           nNumOfNwk;
    void*           NwkDesc;
}DISCOR_CONFIRM;

/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
/* Configuration Attribute Definitions
*/
/*----Config_NWK_Mode_and_Params----*/
extern UINT8           confDeviceType;
extern DWORD           confChannelList;
extern UINT8           confScanDuration;
extern WORD            confPandId;
extern UINT8           confProtocolVers;
extern WORD            confStackProfile;
extern UINT8           confBeaconOrder;
extern UINT8           confSuperFrmOrder;
extern BOOL            confBattLifeExt;
extern UINT8           confScanAttempts;
/*--Config_NWK_Time_btwn_Scans--*/
extern UINT8           confTimeBetweenScans;
/*--Config_Max_Bind--*/
extern UINT8           confMaxBind;
/*--Config_EndDev_Bind_Timeout--*/
extern UINT8           confBindingTimeout;
/*Config_Permit_Join_Duration*/
extern UINT8           confPermitJoinDuration;
/*Config_NWK_Leave_removeChildren*/
extern BOOL            confLeaveRemoveChildrens;
/*Config_NWK_BroadcastDeliveryTime*/
extern UINT8           confBroacastDelivTime;
/*Config_NWK_TransactionPersistenceTime*/
extern UINT8           confTransPersistenceTime;
/*Config_NWK_indirectPollRate*/
extern UINT8           confIndirectPollRate;
/*Config_Max_Assoc*/
extern UINT8           confMaxAssoc;
/*Config_NWK_Join_Direct_Addrs*/
extern void*           confJoinDirectAddrs;

/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/


/************************************************************************/
/*                      Function                                        */
/************************************************************************/



#endif //!ZDO_H
