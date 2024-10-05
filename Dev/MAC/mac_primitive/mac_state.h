/*******************************************************************************/
//Module: MAC/mac_primitive                                                    */   
//File Name: mac_state.h                                                       */
//Description: This files will provide function to manage MAC state			   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//01/30/2007        pnthai          Create                                     */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef MAC_STATE_H
#define MAC_STATE_H

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "types.h"
#include "mac_packet.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/


//----------------------------------------------------------------------------------------------------------
//Definitions of macro to access MAC attributes.

// Temporary buffer for some PIB attributes (the PIB is updated when the next beacon is received or 
// transmitted). These are the category 4 attributes from mlmeSetRequest().
#define MPIB_UPD_BATT_LIFE_EXT_BM    0x01 
#define MPIB_UPD_BEACON_ORDER_BM     0x02 
#define MPIB_UPD_RX_ON_WHEN_IDLE_BM  0x04 
#define MPIB_UPD_SUPERFRAME_ORDER_BM 0x08 

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/

//----------------------------------------------------------------------//
// Definition of MAC PIB
//----------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------------
// MAC PIB and  PIB Security Attributes (As defined in Tables 71 and 72 p. 135 of IEEE Std 802.15.4-2003)
//---------------------------------------------------------------------------------------------------------------
typedef struct {
    UINT8             macAckWaitDuration;                       // The max. number of symbols to wait for an ack frame to arrive ffg a transmitted data frame
    BOOL              macAssociationPermit;                     // Indication of whether a coordinator is currently allowing association  -- TRUE means association is permitted
    BOOL              macAutoRequest;                           // Indication of whether a device automatically sends a data request command if its address is listed in beacon frame
    BOOL              macBattLifeExt;                           // Indication of whether a battery life extension, by reduction of coordinator receiver operation time during the CAP, is enabled TRUE=enabled
    UINT8             macBattLifeExtPeriods;                    // The number of backoff periods during which the receiver is enabled following a beacon in a battery life extendion mode
    UINT8             macBeaconPayloadLength;                   // The length in octets, of the beacon payload
    BYTE              *macBeaconPayload;                        // The contents of a beacon payload
    UINT8             macBeaconOrder;                           // Specification of how often the coordinator transmits a beacon, BO=15--> no beacon
    UINT32            macBeaconTxTime;                          // The time that the device transmitted its last beacon frame, in symbol periods
    UINT8             macBSN;                                   // The sequence number added to the transmitted beacon frame
    QWORD             macCoordExtendedAddress;                  // The 64 bit address of the coordinator with which the device is associated
    UINT16            macCoordShortAddress;                     // The 16 bit short address assigned to the coordinator with which the device is associated
    UINT8             macDSN;                                   // The sequence number added to the transmitted data or MAC command frame
    BOOL              macGTSPermit;                             // TRUE if the PAN coordinator is to accept GTS requests. FALSE otherwise.
    UINT8             macMaxCSMABackoffs;                       // The maximum number of backoffs the CSMA-CA algorithm will attempt before declaring a channel access failure
    UINT8             macMinBE;                                 // The minimum value of the backoff exponent in the CSMA-CA algorithm
    UINT16            macPANId;                                 // The 16 bit identifier of the PAN on which the device is operating, PANID=0xffff --> not associated
    BOOL              macPromiscuousMode;                       // This indicates whether the MAC sublayer is in a promiscuous (receive all) mode. = ALL --> receive everything from PHY
    BOOL              macRxOnWhenIdle;                          // This indicates whether the MAC sublayer is to enable its receiver during idle periods
    UINT16            macShortAddress;                          // The 16 bit address that the device uses to communicate in the PAN
    UINT8             macSuperframeOrder;                       // This specifies the length of the active portion of the superframe, including the beacon frame. SO = 15 --> superframe will not be active
    UINT16            macTransactionPersistenceTime;            // The max time (in superframe periods) that a trasaction is stored by a coordinator and indicatedin its beacon
//     ACL_DESCRIPTOR    *macACLEntryDescriptorSet;                 // A set of ACL entries, each containing address information,security suite and security
//     UINT8             macACLEntryDescriptorSetSize;             // The number of entries in the ACL descriptor
//     BOOL              macDefaultSecurity;                       // Indication of whether the device is able to transmit secure frames to or accept secure frames from devices that are not explicitly listed in the ACL
//     UINT8             macDefaultSecurityMaterialLength;         // The number of octets contained in ACLSecurityMaterial
//     QWORD             macDefaultSecurityMaterial;               // The specific security material to be used to protect frames between the MAC sublayer and devices not in the ACL
//     UINT8             macDefaultSecuritySuite;                  // The unique identifier of the security suite to be used to protect communicationsbetween the MAC and devicesnot in the ACL
//     UINT8             macSecurityMode;                           // The identifier of the security mode in
} MAC_PIB;

//---------------------------------------------------------------//
// Definition of state of transaction of MAC
//---------------------------------------------------------------//
typedef enum 
{
    MAC_ASSOC_REQUEST= 0,
    MAC_ASSOC_REQUEST_SENT,
    MAC_ASSOC_DATA_REQUEST,
    MAC_ASSOC_DATA_REQUEST_SENT,
    MAC_ASSOC_RESPONSE,
    MAC_ASSOC_END
}MAC_ASSO_STATE;


/* Definition of scan status
*/
typedef enum
{
    SCAN_INITIALIZATION = 0,        /* Initialize scan procedure */

    /* General */
    SCAN_INIT_NEW_CHANNEL,
    SCAN_SCANNING,
    SCAN_FINISHED_CHANNEL,        

    /* Orphan scan */
    SCAN_ORPHAN_REALIGNED,

    SCAN_FINISHED                   /* Initialize scan procedure */
}MAC_SCAN_STATE;

/* Definition of Rx Enable State    
*/
typedef enum
{
    RX_ENABLE  = 0,
    RX_NORMAL
}MAC_RXENABLE_STATE;


/* Definition of Polling status    
*/
typedef enum
{
    POLL_INIT = 0,
    POLL_SENT_REQUEST,
    POLL_FINISHED
}MAC_POLL_STATE;

typedef enum 
{
    MAC_INITALIZATION = 0,
    MAC_TRANSAC_NO ,
    MAC_TRANSAC_ASSOSCIATION,           //Association transaction
    MAC_TRANSAC_DISASSOSCIATION,        //Association transaction
    MAC_TRANSAC_SCAN,                   //Scan transaction
    MAC_TRANSAC_POLL,                    //Poll transaction
    MAC_TRANSAC_RXENABLE                //RX Enable transaction
    //...
}MAC_TRANSACTION;

typedef enum
{
    MAC_DEVICE_INIT = 0,
    MAC_DEVICE_ASSSOCIATED,
    MAC_DEVICE_STARTED_NWK
} MAC_DEVICE_STATE;

//---------------------------------------------------------------//
// Definition of mac state.
//---------------------------------------------------------------//
typedef struct  
{
    UINT8                nTransacType;
    volatile UINT8       nTransacState;
    UINT8                DeviceState;
}MAC_STATE ;


// Data holding structure + a mask to tell which ones to update
typedef struct {
    BOOL        macBattLifeExt;
    UINT8       macBeaconOrder;
    BOOL        macRxOnWhenIdle;
    UINT8       macSuperframeOrder;
    BYTE        updateMask;
} MAC_PIB_TEMP_BUFFER;

//---------------------------------------------------------------//
// Definition of mac Info.
//---------------------------------------------------------------//
typedef struct {
    volatile MAC_STATE      sState;
    MAC_PIB_TEMP_BUFFER     pibTempBuffer;
    BYTE                    DeviceType;
} MAC_INFO;



/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
//variable holds MAC PIB
extern MAC_PIB         mpib;

//Variable holds MAC info.
extern MAC_INFO        macInfo;
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


/******************************************************************************/
//@FunctionName: mac_init  										  	          */
//@Description:	Call this function to first time initialization of MAC        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mac_init();


/******************************************************************************/
//@FunctionName: mac_requestState  										  	  */
//@Description:	This function will control state transition of MAC            */
//              Currently, only one transaction is allowed at time            */
//              TBD: check this                                               */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL    mac_requestState(UINT8 nTransacType, UINT8 nTransacState);


/******************************************************************************/
//@FunctionName:   mac_validateMacState										  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
//BOOL mac_validateMacState(MAC_COMMAND_TYPE parCmd, MAC_FRM_TYPE par_Frm);

/******************************************************************************/
//@FunctionName: mac_updateDelayedPib  										  */
//@Description:	This function will update the delayed Pib. It will be called 
//              at starting of a new super-frame                               */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mac_updateDelayedPib();

/******************************************************************************/
//@FunctionName:   mac_resetState										  	  */
//@Description:	This function will clean up MAC due to a initialization or
//              a failure of transaction. Other concerned component will be reset*/
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL    mac_resetState(void);



#endif  //!MAC_STATE_H

