/*******************************************************************************/
//Module: MAC                                                                  */   
//File Name: mac_beacon.h                                                      */
//Description: This component will define the MAC layer behavior for beacon 
//             related function. For RFD, that includes:
//               + Process the received beacon and update information related
//                 to beacon
//               + For the beacon network, periodically handle the beacon and 
//                 inform the upper layer: beacon loss...
//               + Defines functions for beacon-request primitive
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date              Programmer      Details                                    */
//02/27/2007        pnthai          Created                                    */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef MAC_BEACON_H
#define MAC_BEACON_H


/************************************************************************/
/*                  Include files                                       */
/************************************************************************/
#include "types.h"
#include "mac_types.h"
#include "mac_packet.h"
/************************************************************************/
/*                  Constant declaration                                */
/************************************************************************/

/************************************************************************/
/*                  Type Definition                                     */
/************************************************************************/
typedef struct
{
    BOOL    bTrackBeaconReq;   //Request to track the beacon
    BOOL    bTrackBeacon;      //Beacon will be tracked or not         
    BOOL    bBeaconLost;
#if     ZIGBEE_DEVICE_FFD == TRUE  
    P_MPDU_TX   pBeacon; 
    BYTE        Beacon[23+1+2+1+32+14]; /*TBD:GST = 1*/
    UINT8       nPendingAddrLen;
    BOOL        bTransmitBeacon;
    UINT32      nBeaconOffset;     
#endif  // ! ZIGBEE_DEVICE_FFD == TRUE  

    UINT8   nCurrNumOfLoss;
    UINT8   nBeaconDuration;

    UINT8   nFinalCapSlot;
    BOOL    bBatteryLifeExt;
} BEACON_INFO;


// timer to manage slot and beacon timer
typedef struct
{
    INT32 volatile          bosCounter;
    UINT32                  captureTime;
    UINT32                  captureBosCounter;
    UINT16                  captureTcnt;
    UINT32                  bosCounterAdjustTime;
} SLOT_TIME_INFO;

typedef struct  
{
    UINT8                   NumofShortAddr  :3;
    UINT8                   Reserved1       :1;
    UINT8                   NumofExtAddr    :3;
    UINT8                   Reserved2       :1;
}PENDING_ADDR_SPEC;
/************************************************************************/
/*                  Variable declaration                                */
/************************************************************************/
extern SLOT_TIME_INFO   SlotInfo;
extern BEACON_INFO      BeaconInfo;
/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/


/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/******************************************************************************/
//@FunctionName:  beacon_updateNewBeacon                                      */
//@Description: This function will updates MAC with new received beacon       */
//              parameters in the beacon will be updated to PIB and Mac states*/
//                                                                            */    
//@Param                                                                      */
//                                                                            */   
/******************************************************************************/
void    beacon_updateNewBeacon(P_MPDU_RX  par_pRxMpdu);

/*****************************************************************************/
//@FunctionName:  beacon_calBeaconMargin                                     */
//@Description: Because of the crystal inaccuracy, a margin must be added    */
//              before the beacon that will be received. The result is that: */
//          - RX must be turned on a short while before the beacon is        */
//            expected to be received.
//          - TX must stop before this margin is reached                     */
//                                                                           */    
//@Param                                                                     */
//Reused from C2420                                                          */   
/*****************************************************************************/
UINT8   beacon_calBeaconMargin(void);


#endif //!MAC_BEACON_H
