/*******************************************************************************/
//Module: 	APL                                                                */   
//File Name: aps.h                                                             */
//Description: 	Implements header files for aps.c and definition of 		   */
//				application primitives		   								   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//06/25/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef APS_H
#define APS_H

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "nwk.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/
#define         apscMaxAddrMapEntries           8
#define         apscMaxDescriptorSize           64
#define         apscMaxDiscoverySize            64
#define         apscMaxFrameOverhead            6
#define         apscMaxFrameRetries             3
#define         apscMaxAckWaitDureation         0.05*2*nwkcMaxDepth

#define         apscBindingTimeout              10  /*second*/
#define         SIZE_OF_FULL_APS_HDR        7
/************************************************************************/
/*					Type Definition										*/
/************************************************************************/

typedef enum
{
    APS_TX_SERCURITY    = 0x01,
    APS_TX_USE_NWKKEY   = 0x02,
    APS_TX_ACK_REQ      = 0x04,
}APS_TX_OPTION;

typedef enum
{
    APS_ADDR_INDIRECT = 0x00,
    APS_ADDR_SHORT,
    APS_ADDR_EXT
}APS_ADDR_MODE;

/* Definition of APS frame format
*/
typedef enum
{
    APS_FRM_DATA = 0x00,
    APS_FRM_CMD,
    APS_FRM_ACK
}APS_FRAME_TYPE;

typedef enum
{
    APS_DELIVERY_NORMAL = 0x00,
    APS_DELIVERY_INDIRECT, 
    APS_DELIVERY_BROADCAST
}APS_DELIVERY_MODE;

typedef struct  
{
    /* Frame control
    */    
    UINT8       FrmType         :2;
    UINT8       DeliveryMode    :2;
    UINT8       IndirectAddMode :1;
    UINT8       Sercurity       :1;
    UINT8       AckRequest      :1;
    UINT8       Reserved        :1;

    /* Addressing field
    */
    UINT8       DestEndpoint;
    UINT8       ClusterId;
    UINT16      ProfileId;
    UINT8       SrcEndPoint;

    UINT8       ApsCounter;

    /* Payload
    */
    BYTE        Payload;
}APS_FRM;

typedef     APS_FRM*    P_APS_FRM;

typedef struct  
{
    /* Frame control
    */    
    UINT8       FrmType         :2;
    UINT8       DeliveryMode    :2;
    UINT8       IndirectAddMode :1;
    UINT8       Sercurity       :1;
    UINT8       AckRequest      :1;
    UINT8       Reserved        :1;

    /* Addressing field
    */
    /*No dest ep*/
    UINT8       ClusterId;
    UINT16      ProfileId;
    UINT8       SrcEndPoint;

    UINT8       ApsCounter;

    /* Payload
    */
    BYTE        Payload;
}APS_FRM_TOCOOR;

typedef     APS_FRM_TOCOOR*    P_APS_FRM_TOCOOR;


typedef struct  
{
    /* Frame control
    */    
    UINT8       FrmType         :2;
    UINT8       DeliveryMode    :2;
    UINT8       IndirectAddMode :1;
    UINT8       Sercurity       :1;
    UINT8       AckRequest      :1;
    UINT8       Reserved        :1;

    /* Addressing field
    */
    UINT8       DestEndpoint;
    UINT8       ClusterId;
    UINT16      ProfileId;

    /*No source ep*/
    UINT8       ApsCounter;

    /* Payload
    */
    BYTE        Payload;
}APS_FRM_FROMCOOR;

typedef     APS_FRM_FROMCOOR*    P_APS_FRM_FROMCOOR;


typedef enum
{
    APS_SUCCESS = 0x00,
    APS_NO_SHORT_ADDRESS = 0xA0,        /*Not defined in the Zigbee Standard, 
                                         assigned it by implementation*/
    APS_NO_BOUND_DEVICE,
    APS_SECURITY_FAIL,
    APS_ILLEGAL_DEVICE,
    APS_ILLEGAL_REQUEST,
    APS_INVALID_BINDING,
    APS_NO_ACK   
}APS_STATUS;


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
    APS_ACK_TIMER               = 0x40,
    APS_INDIRECT_PROCESS_REQ,
    MSG_APS                     = 0x4F
} APS_MSG_TYPE;
/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/

/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/

/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/*------------------------------------------------------------------------
                        APSDE-DATA Service
-------------------------------------------------------------------------*/
/******************************************************************************/
//@FunctionName: This function will initialize the APS. Called by ZDO         */
//@Description:	aps_init                                                      */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void aps_init();



/******************************************************************************/
//@FunctionName:  apsdeDataRequest 										  	  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS apsdeDataRequest(  UINT8 DestAddrMode,ADDRESS* DestAddr, BYTE DstEndpoint, 
                            UINT16 ProfileID, BYTE ClusterId, BYTE SrcEndpoint,
                            BYTE asduLength, BYTE *asdu, BYTE TxOptions, 
                            BYTE DiscoverRoute, BYTE RadiusCounter );

/******************************************************************************/
//@FunctionName: apsdeDataConfirm   					  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void apsdeDataConfirm(UINT8 DestAddrMode,ADDRESS* DestAddr,
                      BYTE DstEndpoint, BYTE SrcEndpoint, 
                      APS_STATUS Status);

/******************************************************************************/
//@FunctionName:  apsdDataIndication 	    			  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void apsdDataIndication(P_APS_FRM par_pFrm, UINT8 nDataLength, UINT8 SrcAddressMode, ADDRESS*  SrcAddr,
                        UINT8 LinkQuality, BYTE SecurityUse );


/*------------------------------------------------------------------------
                        APSME-BIND/UNBIND Service
-------------------------------------------------------------------------*/
/******************************************************************************/
//@FunctionName:  apsmeBindRequest  					  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS apsmeBindRequest(  WORD  SrcAddr,UINT8 SrcEndPoint, UINT8 ClusterId,
                            WORD  DesAddr,UINT8 DesEndPoint);

/******************************************************************************/
//@FunctionName:  apsmeBindConfirm							                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void apsmeBindConfirm(APS_STATUS    Status,
                      WORD  SrcAddr,UINT8 SrcEndPoint, UINT8 ClusterId,
                      WORD  DesAddr,UINT8 DesEndPoint);


/******************************************************************************/
//@FunctionName:   apsmeUnbindRequest									  	  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS apsmeUnbindRequest(WORD  SrcAddr,UINT8 SrcEndPoint, UINT8 ClusterId,
                            WORD  DesAddr,UINT8 DesEndPoint);


/******************************************************************************/
//@FunctionName: apsmeUnbindConfirm  									      */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void apsmeUnbindConfirm(APS_STATUS    Status,
                      WORD  SrcAddr,UINT8 SrcEndPoint, UINT8 ClusterId,
                      WORD  DesAddr,UINT8 DesEndPoint);



#endif //!APS_H
