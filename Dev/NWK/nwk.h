//*******************************************************************************************************
//Module: NWK Layer
//File Name: nwk.h
//Description: This file contains NWK Initialization and status Definition of Constants and Attributes
//Revision History:
//Date			Programmer		Details
//01/18/2007    VQ				Defined NWK constants, attributes, etc 
//*******************************************************************************************************


#ifndef NWK_H
#define NWK_H

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "types.h"
#include "nwk_common.h"
#include "mac_primitives.h"

/************************************************************************/
/*					Constant declaration								*/
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


/*-------------------------------------------------------------------------*/
/*                             NLDE-SAP Primitives                         */
/*-------------------------------------------------------------------------*/

/******************************************************************************/
//@FunctionName: TBD									  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS  nldeDataRequest(UINT16 DstAddr, BYTE NsduLength, BYTE *Nsdu, 
                           BYTE NsduHandle, BYTE Radius, BYTE DiscoverRoute, BOOL SecurityEnable); 

/******************************************************************************/
//@FunctionName: TBD									  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nldeDataConfirm(BYTE NsduHandle, NWK_STATUS Status); 
 
/******************************************************************************/
//@FunctionName: TBD									  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nldeDataIndication(UINT8  SrcAddrMode, ADDRESS*	SrcAddress, 
                        BYTE NsduLength, BYTE *Nsdu, BYTE LinkQuality);



/*-------------------------------------------------------------------------*/
/*                             NMLE-SAP Primitives                         */
/*-------------------------------------------------------------------------*/

//----------------------------------
// NMLE-Set/Get Primitives
//----------------------------------

/******************************************************************************/
//@FunctionName: TBD									  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
NWK_STATUS nlmeGetRequest(BYTE NIBAttribute,void *pNIBAttribValue,UINT8* NIBAttribLength);

void nlmeGetConfirm(NWK_STATUS Status, BYTE NIBAttribute,
					 UINT16	NIBAttributeLength, void *NIBAttributeValue);

NWK_STATUS nlmeSetRequest(  BYTE NIBAttribute, UINT8 NIBAttributeLength, 
                            void *NIBAttributeValue);

void nlmeSetConfirm(BYTE Status, BYTE NIBAttribute);

//-----------------------------
// NMLE-RESET PRIMITIVE 
//-----------------------------
void nlmeResetRequest();
void nlmeResetConfirm(NWK_STATUS status);


//-----------------------------
// NMLE-SYNC PRIMITIVE
//-----------------------------
Z_STATUS    nlmeSyncRequest(BOOL Track);
void        nlmeSyncIndication();
void        nlmeSyncConfirm(BYTE Status);


//----------------------------
// NMLE-NETWORK DISCOVERY PRIMITIVE
//----------------------------
void nlmeNetworkDiscoveryRequest(DWORD ScanChannels, UINT8 ScanDuration);

void nlmeNetworkDiscoveryConfirm(BYTE NetworkCount, NWK_DESC* NetworkDescriptor, 
                                 NWK_STATUS Status);

//----------------------------
// NMLE-NETWORK PERMIT JOIN PRIMITIVE
//----------------------------
void nlmePermitJoiningRequest(UINT8 PermitDuration);
void nlmePermitJoiningConfirm(NWK_STATUS Status);

//----------------------------
// NMLE-NETWORK-JOIN PRIMITIVE
//----------------------------
void nlmeJoinRequest(UINT16	PanID,BOOL	JoinAsRouter,BOOL	RejoinNetwork,
                     DWORD	ScanChannels,UINT8	ScanDuration,UINT8	PowerSource,
                     BOOL	RxOnWhenIdle, UINT8	MACSecurity);

void nlmeJoinConfirm(UINT16 PanID,	NWK_STATUS Status);

void nlmeJoinIndication(    WORD    ShortAddr,
                            QWORD*  pExtAddr,
                            BYTE    CapInfor,
                            BOOL    Sercurity);

//----------------------------
//	NMLE-NETWORK-LEAVE  PRIMITIVE
//----------------------------
void nlmeLeaveRequest (ADDRESS* NWK_DeviceAddress, BOOL RemoveChildren, BOOL MACSecurityEnable);

void nlmeLeaveIndication();

void nlmeLeaveConfirm(ADDRESS* NWK_DeviceAddress, NWK_STATUS Status);


//------------------------------------
//	NMLE-NETWORK FORMATION  PRIMITIVE
//------------------------------------
void nlmeNetworkFormationRequest(DWORD ScanChannels, UINT8 ScanDuration, UINT8 BeaconOrder, 
                                 UINT8 SuperframeOrder, UINT16 PandId, BOOL BatteryLifeExtension);

void nlmeNetworkFormationConfirm(NWK_STATUS status);

//------------------------------------
//	NMLE-NETWORK START-ROUTER  PRIMITIVE
//------------------------------------
void nlmeStartRouterRequest(UINT8 BeaconOrder, UINT8 SuperframeOrder, BOOL BatLifeExt);
void nlmeStartRouterConfirm(NWK_STATUS  Status);


//------------------------------------
//	NMLE-NETWORK DIRECT JOIN  PRIMITIVE
//------------------------------------

void nlmeJoinDirectRequest(ADDRESS*  pDeviceAddr, CAPACITY_INFO CapInfor);
void nlmeJoinDirectConfirm(ADDRESS*  pDeviceAddress, NWK_STATUS Status);

#endif //!NWK_H




