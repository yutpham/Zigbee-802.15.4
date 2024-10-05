//*********************************************************************************************
//Module: MAC/mac_interfaces
//File Name: mac_types.h
//Description: This file contains MAC Initialization and Definition of Constants and Attributes
//             This file also includes structures of MAC primitives
//Revision History:
//Date			Programmer		Details
//*********************************************************************************************

#ifndef MAC_TYPE_H
#define	MAC_TYPE_H


// Standard GCC include files for AVR
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>

//Common types
#include "types.h"

// Hardware abstraction library for the ATMEGA128(L)
#include "hal.h"
#include "hal_cc2420.h"

//Mac config
#include "zconf.h"


/*******************************************************************************************************
**************************               GENERAL DEFINITIONS                 **************************
*******************************************************************************************************/

/*Invalid MAC Short address*/
#define INVALID_EXT_ADDR        0x0000
#define INVALID_SHORT_ADDR      0xFFFF
#define RESERVED_SHORT_ADDR     0xFFFE
#define VALID_SHORT_ADDR        0xFFFD

#define NON_BEACON_NWK          15


//----------------------------------------------------------------------------------------------------------
// PHY / MAC constants
#define aMaxPHYPacketSize       127
#define aTurnaroundTime         12 // symbol periods

//---------------------------------------------------------------------------------------------------------------
// MAC sublayer constants  (As defined in Table 70 p. 134 of IEEE Std 802.15.4-2003)
//---------------------------------------------------------------------------------------------------------------
#define aBaseSlotDuration         60                                        // The number of symbols forming a superframe slot when the superframe order is equal to 0
#define aNumSuperframeSlots       16                                        // The number of slots contained in any superframe
#define aBaseSuperframeDuration   (aBaseSlotDuration * aNumSuperframeSlots) // The number of symbols forming a superframe when the superframe  order is equal to 0
#define aMaxBE                    5                                         // The max. value of the backoff exponent in the CSMA_CA algorithm
#define aMaxBeaconOverhead        75                                        // The max. number of octets added by the MAC sublayer to the payload of its beacon frame
#define aMaxBeaconPayloadLength   (aMaxPHYPacketSize - aMaxBeaconOverhead)  // The max. size, in octets, of a beacon payload
#define aGTSDescPersistenceTime   4                                         // The number of superframes in which a GTS descriptor exists in the beacon frame of a PAN coordinator
#define aMaxFrameOverhead         25                                        // The max. number of octets added by the MAC sublayer to its payload without security
#define aMaxFrameResponseTime     1220                                      // The max. number of CAP symbols in beacon enabled/non-enabled PAN to wait for a frame intended to a data request frame
#define aMaxFrameRetries          3                                         // The max. number of retries allowed after a transmission failure
#define aMaxLostBeacons           4                                         // The number of consecutive lost beacons that will cause the MAC sublayer of a receiving device to declare a loss in synchronization
#define aMaxMACFrameSize          (aMaxPHYPacketSize - aMaxFrameOverhead)   // The max. number of octets that can be transmitted in the MAC frame payload field
#define aMaxSIFSFrameSize         18                                        // The max. size of an MPDU, in octets that can be followed by a short interframe spacing (SIFS) period
#define aMinCAPLength             440                                       // The min. number of symbols forming the CAP
#define aMinLIFSPeriod            40                                        // The min. number of symbols forming a long interframe spacing (LIFS) period
#define aMinSIFSPeriod            12                                        // The min. number of symbols forming a SIFS period
#define aResponseWaitTime         (32 * aBaseSuperframeDuration)            // The max. number of symbols a device shall wait for a respond command to be available ff. a request command
#define aUnitBackoffPeriod        20                                        // The number of symbols forming the basic time period used by the CSMA_CA algorithm

#define     CLOCK_SPEED_MHZ                 8  // MHz //Clock frequency
#define     MAC_SYMBOL_DURATION             16 // us //MAC primitives duration
#define     MAC_TIMER_OVERFLOW_VALUE        (CLOCK_SPEED_MHZ * MAC_SYMBOL_DURATION * aUnitBackoffPeriod - 1)
#define     MAC_TIMER_BACKOFF_SLOT_OFFSET   ((MAC_TIMER_OVERFLOW_VALUE + 1) / 4)
//---------------------------------------------------------------------------------------------------------------
// MAC ACL and security information bases (As defined in Table 75 p. 168 of IEEE Std 802.15.4-2003)
//---------------------------------------------------------------------------------------------------------------
#define MAC_SECURITY_NONE               0x00
#define MAC_SECURITY_AES_CTR            0x01
#define MAC_SECURITY_AES_CCM128         0x02
#define MAC_SECURITY_AES_CCM64          0x03
#define MAC_SECURITY_AES_CCM32          0x04
#define MAC_SECURITY_AES_CBC_MAC128     0x05
#define MAC_SECURITY_AES_CBC_MAC64      0x06
#define MAC_SECURITY_AES_CBC_MAC32      0x07
//---------------------------------------------------------------------------------------------------------------
//MAC Security Modes (As defined in Table 72 p. 138 of IEEE Std 802.15.4-2003)
//---------------------------------------------------------------------------------------------------------------
#define MAC_UNSECURED_MODE              0x00
#define MAC_ACL_MODE                    0x01
#define MAC_SECURED_MODE                0x02
//---------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------
// Definition of MAC enum that will be exported to upper layer
//-----------------------------------------------------------------------
typedef enum
{
    SUCCESS                 = 0,
    UNDEFINED               = 0x01,
    BEACON_LOSS             = 0xE0,
    CHANNEL_ACCESS_FAILURE  = 0xE1,
    DENIED                  = 0xE2,
    DISABLE_TRX_FAILURE     = 0xE3,
    FAILED_SECURITY_CHECK   = 0xE4,
    FRAME_TOO_LONG          = 0xE5,
    INVALID_GTS             = 0xE6,
    INVALID_HANDLE          = 0xE7,
    INVALID_PARAMETER       = 0xE8,
    NO_ACK                  = 0xE9,
    NO_BEACON               = 0xEA,
    NO_DATA                 = 0xEB,
    NO_SHORT_ADDRESS        = 0xEC,
    OUT_OF_CAP              = 0xED,
    PAN_ID_CONFLICT         = 0xEE,
    REALIGNMENT             = 0xEF,
    TRANSACTION_EXPIRED     = 0xF0,
    TRANSACTION_OVERFLOW    = 0xF1,
    TX_ACTIVE               = 0xF2,
    UNAVAILABLE_KEY         = 0xF3,
    UNSUPPORTED_ATTRIBUTE   = 0xF4,
    RX_DEFERRED             = 0xF5
}MAC_STATUS;

//-----------------------------------------------------------------------
// MAC PAN information base
//------------------------------------------------------------------------
// These constants shall be used with mlmeSetRequest and mlmeGetRequest.
typedef enum {
    MAC_ACK_WAIT_DURATION = 0x40,
    MAC_ASSOCIATION_PERMIT,
    MAC_AUTO_REQUEST,
    MAC_BATT_LIFE_EXT,
    MAC_BATT_LIFE_EXT_PERIODS,
    MAC_BEACON_PAYLOAD,
    MAC_BEACON_PAYLOAD_LENGTH,
    MAC_BEACON_ORDER,
    MAC_BEACON_TX_TIME,
    MAC_BSN,
    MAC_COORD_EXTENDED_ADDRESS,
    MAC_COORD_SHORT_ADDRESS,
    MAC_DSN,
    MAC_GTS_PERMIT,
    MAC_MAX_CSMA_BACKOFFS,
    MAC_MIN_BE,
    MAC_PAN_ID,
    MAC_PROMISCUOUS_MODE,
    MAC_RX_ON_WHEN_IDLE,
    MAC_SHORT_ADDRESS,
    MAC_SUPERFRAME_ORDER,
    MAC_TRANSACTION_PERSISTENCE_TIME,
    MAC_ACL_ENTRY_DESCRIPTOR_SET = 0x70,
    MAC_ACL_ENTRY_DESCRIPTOR_SETSIZE,
    MAC_DEFAULT_SECURITY,
    MAC_DEFAULT_SECURITY_MATERIAL_LENGTH,
    MAC_DEFAULT_SECURITY_MATERIAL,
    MAC_DEFAULT_SECURITY_SUITE,
    MAC_SECURITY_MODE
} MAC_PIB_ATTR_ID;


//---------------------------------------------------------------------------------------------------------------
// Elements of PAN Descriptor p.76 Table 41
//---------------------------------------------------------------------------------------------------------------
typedef struct  
{
    WORD        BeaconOrder     :4;
    WORD        SuperFrmOrder   :4;
    WORD        FinalCap        :4;
    WORD        BatLifeExt      :1;
    WORD        Res1            :1;
    WORD        PanCoord        :1;
    WORD        AssoPermit      :1;
}SUPER_FRAME;

typedef struct {
    UINT8   CoordAddrMode;
    UINT16  CoordPANId;
    ADDRESS CoordAddress;
    UINT8   LogicalChannel;
    WORD    SuperframeSpec;
    BOOL    GTSPermit;
    UINT8   LinkQuality;
    UINT32  TimeStamp;
    BOOL    SecurityUse;
    UINT8   ACLEntry;
    BOOL    SecurityFailure;
} PAN_DESCRIPTOR;

/* Structure of beacon payload
*/
typedef struct  
{
    UINT8   ProtocolId;
    UINT8   StackProfile        :4;
    UINT8   nwkProtocolVersion  :4;
    UINT8   Reserved            :2;
    UINT8   RouterCapacity      :1;
    UINT8   DeviceDepth         :4;
    UINT8   EndDeviceCapacity   :1;
    QWORD   nwkExtendedPANID;
    UINT8   TxOffset[3];
}BEACON_PAYLOAD;

//Notification packet to NWK layer
typedef struct{
    UINT8           BSN;
    PAN_DESCRIPTOR  PanDescriptor;
    BYTE            PendAddrSpec;
    ADDRESS         AddrList[7];
    UINT8           sduLength;
    BEACON_PAYLOAD  Payload;
}MLME_BEACON_NOTIFY_INDICATION;

/* Data indication: the same with header of MPDU*/
typedef struct  
{
    UINT8      FrameType:3;
    UINT8      SecurityEnable:1;
    UINT8      FramePending:1;
    UINT8      ACKRequest:1;
    UINT8      IntraPAN:1;
    UINT8      Reserverd1:1;

    UINT8      Reserverd2:2;
    UINT8      DestAddMode:2;
    UINT8      Reserverd3:2;
    UINT8      SourceAddMode:2;
}FRAME_CONTROL;

typedef struct {
    BYTE    srcAddrMode;
    WORD    srcPanId;
    ADDRESS srcAddr;
    BYTE    dstAddrMode;
    WORD    dstPanId;
    ADDRESS dstAddr;
    UINT8   mpduLinkQuality;
    BOOL    securityUse;
    UINT8   aclEntry;
    INT8    msduLength;
    BYTE*   pMsdu;
} MCPS_DATA_INDICATION;


// The results returned by mlmeScanRequest 
typedef struct {
    BYTE    scanType;
    DWORD   unscannedChannels;
    UINT8   resultListSize;
    union{
        UINT8 pEnergyDetectList[16];
        PAN_DESCRIPTOR pPANDescriptorList[MAC_MAX_PAN_DESCRIPTOR]; 
    };
} MAC_SCAN_RESULT;




#endif	// !MAC_TYPE
