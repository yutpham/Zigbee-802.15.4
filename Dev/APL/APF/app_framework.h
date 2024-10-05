/*******************************************************************************/
//Module: 	                                                                   */   
//File Name: app_framework                                                     */
//Description: 	This file implements the application framework type definitions
// 				for zigbee app. 											   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//06/25/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef APP_FRAMEWORK_H
#define APP_FRAMEWORK_H


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
    ZDO_DEVICE_STARTED  = 0x50,
    APF_DATA_INDICATION,
    APF_DATA_CONFIRM,

    APF_KEYS,           /*Keys msg*/
    APF_POLL_BOARD,           /*Keys msg*/

    MSG_APF             = 0x5F
} APF_MSG_TYPE;

/* Definition of data for each message type
    + APF, NWK, ZDO has conformance in msg format. First byte is msg_type
*/
typedef struct 
{
    UINT8                   MsgType;
    UINT8                   EndPointId;
    UINT16                  Data;
}ZAPP_MSG;

/* Definition of descriptors
*/
/* Node descriptors*/
typedef struct
{
    UINT8           LogicalType :3;
    UINT8           Reserved    :5;
    UINT8           APSFlags    :3;
    UINT8           FrequencyBand:5;
    UINT8           CapabilityFlags;
    BYTE            ManufacturerCode[2];
    BYTE            MaxBufferSize;
    BYTE            MaxTransferSize[2];
} NODE_DESCRIPTOR;

/* Node power descriptor
*/
typedef struct
{
    UINT8 PowerMode                 :4;
    UINT8 AvailablePowerSources     :4;
    UINT8 CurrentPowerSource        :4;
    UINT8 CurrentPowerSourceLevel   :4;
} NODE_POWER_DESCRIPTOR;

/* Simple Description Format Structure */
typedef struct
{
    BYTE          EndPoint;
    UINT16        AppProfId;
    UINT16        AppDeviceId;
    UINT8         AppDevVer :4;
    UINT8         AppFlags  :4;
    BYTE          AppNumInClusters;
    BYTE          *pAppInClusterList;
    BYTE          AppNumOutClusters;
    BYTE          *pAppOutClusterList;
} SIMPLE_DESCRIPTOR;

/* Descriptors of endpoint
*/
typedef struct  
{
    SIMPLE_DESCRIPTOR   SimpleDescriptor;
    P_FNC               pHandler;
    P_FNC               pInit;
}ENDPOINT_DESTCRIPTOR;


/* Endpoint list
*/
extern  ENDPOINT_DESTCRIPTOR*               EndpointDescriptors[];

/* Node descriptor
*/
extern ENDPOINT_DESTCRIPTOR                 ZDO_Descriptor;
extern NODE_DESCRIPTOR                      NodeDescriptor;
extern NODE_POWER_DESCRIPTOR                NodePowerDescriptor;
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
//@Description:	 this function will send msg to endpoint                      */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void apf_sendAppMsg(UINT8 EP, UINT8  MsgType, UINT16 Data);

/******************************************************************************/
//@FunctionName:   	apf_dispatchEvent    				  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    apf_dispatchEvent(ZAPP_MSG*     par_pMsg);

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	 This function will check the endpoint                         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL    app_checkEP(UINT8);

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will add endpoint to the ep list                */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL app_addEP(ENDPOINT_DESTCRIPTOR*  pEp);

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will build a single msg and send it using 
//              services in APS                                               */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS    apf_sendSingleMsg(BYTE* pData, UINT8 nDataLen,UINT8 TransSeq , 
                              AF_ADDR* pDstAddr, BYTE DstEndpoint, BYTE ClusterId, BYTE SrcEndpoint,
                              BYTE TxOptions);

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will send a single KVP using services in APS    */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS     apf_sendSingleKVP(UINT8 TransSeq, UINT8 CmdId, UINT8 AttrDataType, WORD AttrId, 
                              UINT8 ErrCode, BYTE* pData, UINT8 nDataLen,
                              AF_ADDR* pDstAddr, BYTE ClusterId, 
                              BYTE SrcEndpoint, BYTE TxOptions);


/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	 This function will send a apf frame                          */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS	apf_sendFrm(AF_FRM* pFrm, UINT8 DestAddrMode, ADDRESS* pDestAddr, BYTE DstEndpoint, 
                        BYTE ClusterId, BYTE SrcEndpoint, BYTE TxOptions);




#endif //! APP_FRAMEWORK
