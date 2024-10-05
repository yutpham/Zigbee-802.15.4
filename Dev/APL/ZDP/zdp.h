/*******************************************************************************/
//Module: 	                                                                   */   
//File Name: zdp                                                               */
//Description: This file will implement ZDP functionality of Zigbee stack      */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//07/31/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef ZDP_H
#define ZDP_H

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "types.h"
#include "apf_format.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/

/********************************************************************* */
/* Message/Cluster IDS
 */
#define NWK_addr_req            0x00
#define IEEE_addr_req           0x01
#define Node_Desc_req           0x02
#define Power_Desc_req          0x03
#define Simple_Desc_req         0x04
#define Active_EP_req           0x05
#define Match_Desc_req          0x06
#define Complex_Desc_req        0x10
#define User_Desc_req           0x11
#define Discovery_Register_req  0x12
#define End_Device_annce        0x13
#define User_Desc_set           0x14
#define End_Device_Bind_req     0x20
#define Bind_req                0x21
#define Unbind_req              0x22
#define Mgmt_NWK_Disc_req       0x30
#define Mgmt_Lqi_req            0x31
#define Mgmt_Rtg_req            0x32
#define Mgmt_Bind_req           0x33
#define Mgmt_Leave_req          0x34
#define Mgmt_Direct_Join_req    0x35

#define NWK_addr_rsp            (NWK_addr_req | 0x80)
#define IEEE_addr_rsp           (IEEE_addr_req | 0x80)
#define Node_Desc_rsp           (Node_Desc_req | 0x80)
#define Power_Desc_rsp          (Power_Desc_req | 0x80)
#define Simple_Desc_rsp         (Simple_Desc_req | 0x80)
#define Active_EP_rsp           (Active_EP_req | 0x80)
#define Match_Desc_rsp          (Match_Desc_req | 0x80)
#define Complex_Desc_rsp        (Complex_Desc_req | 0x80)
#define User_Desc_rsp           (User_Desc_req | 0x80)
#define Discovery_Register_rsp  (Discovery_Register_req | 0x80)
#define User_Desc_conf          (User_Desc_set | 0x80)
#define End_Device_Bind_rsp     (End_Device_Bind_req | 0x80)
#define Bind_rsp                (Bind_req | 0x80)
#define Unbind_rsp              (Unbind_req | 0x80)
#define Mgmt_NWK_Disc_rsp       (Mgmt_NWK_Disc_req | 0x80)
#define Mgmt_Lqi_rsp            (Mgmt_Lqi_req | 0x80)
#define Mgmt_Rtg_rsp            (Mgmt_Rtg_req | 0x80)
#define Mgmt_Bind_rsp           (Mgmt_Bind_req | 0x80)
#define Mgmt_Leave_rsp          (Mgmt_Leave_req | 0x80)
#define Mgmt_Direct_Join_rsp    (Mgmt_Direct_Join_req | 0x80)

/* Address request type
*/
#define ZDP_ADDR_REQ_SINGLE     0
#define ZDP_ADDR_REQ_EXTENDED   1

#define RESPONSE_BIT            0x08


/*A part of Z_STATUS*/
typedef enum
{
    ZDP_SUCCESS           = 0x00,  // Operation completed successfully
    ZDP_INVALID_REQTYPE   = 0x80,  // The supplied request type was invalid
    ZDP_DEVICE_NOT_FOUND  = 0x81,  // Reserved
    ZDP_INVALID_EP        = 0x82,  // Invalid endpoint value
    ZDP_NOT_ACTIVE        = 0x83,  // Endpoint not described by a simple descriptor
    ZDP_NOT_SUPPORTED     = 0x84,  // Optional feature not supported
    ZDP_TIMEOUT           = 0x85,  // Operation has timed out
    ZDP_NO_MATCH          = 0x86,  // No match for end device bind
    ZDP_TABLE_FULL        = 0x87,  // Bind request failed, no resources
    ZDP_NO_ENTRY          = 0x88  // Unbind request failed, no entry
}ZDP_STATUS;
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
//@Description:	This function will send msg to request NwkAddr corresponding
//              to a Ext address.
//                                                                           */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS zdp_NwkAddrReq( QWORD *pIEEEAddress, BYTE ReqType, 
                         UINT8 StartIndex, BOOL SecurityUse);

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will send msg to request IEEE corresponding
//              to a NwkAddr.
//                                                                           */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS zdp_IEEEAddrReq(UINT16 NwkAddr, BYTE ReqType, 
                         UINT8 StartIndex);


/******************************************************************************/
//@FunctionName: zdp_EndDeviceAnnce						  	                  */
//@Description:	This function will announce the the allocated nwk addr to nwk */                
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS zdp_EndDeviceAnnce( UINT16 NwkAddr, BYTE* pIEEEAddr, 
                            BYTE SecurityEnable ) ;

/******************************************************************************/
//@FunctionName:  zdp_EndDeviceBindReq					  	                  */
//@Description:	 This function will generate a end_device_bind request        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS zdp_EndDeviceBindReq( AF_ADDR* pDstAddr, 
                                 UINT16  LocalCoordinator,
                                 BYTE    endPoint,
                                 UINT16  ProfileID,
                                 BYTE NumInClusters, BYTE *InClusterList,
                                 BYTE NumOutClusters, BYTE *OutClusterList,
                                 BYTE SecurityEnable );

/******************************************************************************/
//@FunctionName: zdp_EDBindRes						  	                      */
//@Description:	 This function will generate ED_bin_ to the source            */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS zdp_EDBindRes(AF_ADDR* pDstAddr, UINT8 Trans, UINT8    Status);

/******************************************************************************/
//@FunctionName: zdp_BindUnbindReq						  	                  */
//@Description:	This function will generate a bin/unbind request and send
//              to the coor.                                                  */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS zdp_BindUnbindReq( BOOL BindOrUnbind, AF_ADDR* pDstAddr, 
                              BYTE *pSourceAddr, BYTE SrcEndPoint,
                              BYTE ClusterID, 
                              BYTE *pDestinationAddr, BYTE DstEndPoint,
                              BYTE SecurityEnable );
/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void zdp_sendEDBindReq( BYTE endPoint );

/******************************************************************************/
//@FunctionName: zdp_BindUnBindRes						  	                  */
//@Description:	 This function will generate bin_res/unbind_res to the source */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS zdp_BindUnBindRes(AF_ADDR* pDstAddr, UINT8 Trans, UINT8    Status, BOOL    Bind);

#endif //!ZDP_H
