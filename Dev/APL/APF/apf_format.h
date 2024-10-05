/*******************************************************************************/
//Module: 	                                                                   */   
//File Name: apf_format                                                        */
//Description: 	This file will implement the format of kvp/msg                 */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//07/27/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "types.h"
#include "aps.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/
#ifndef APF_FORMAT_H
#define APF_FORMAT_H

/* Definition of constant length
*/
#define AF_FRM_HR_LEN       0x01  // TRANS_COUNT(4bits)+FRAMETYPE(4bits)
#define AF_MSG_HR_LEN       0x02  // SEQ_NUM(8bits)+DATA_LENGTH(8bits)
#define AF_KVP_REQ_HR_LEN   0x04  // SEQ_NUM(8) + CMD_TYPE(4) + ATTRIB_TYPE(4) + ATTRIBID(16)
#define AF_KVP_RES_HR_LEN   0x05  // SEQ_NUM(8) + CMD_TYPE(4) + ATTRIB_TYPE(4) + ATTRIBID(16) + ERROR_CODE(8)   

#define         SIZE_OF_MSG_TRANS_HR            2
#define         SIZE_OF_KV_HR                   4
#define         SIZE_OF_KV_HR_RSP               5
#define         RESERVED_BEFORE_RES             0x111
/*********************************************************************
 * MACROS
 */

// Takes a byte out of a uint32.  
//     var - uint32
//     ByteNum - byte to take out (0 - 3)
#define BREAK_UINT32( var, ByteNum ) \
        (BYTE)((UINT32)((var >>(ByteNum * 8)) & 0x00FF))

#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) \
     ((UINT32)((UINT32)(Byte0 & 0x00FF) \
      + ((UINT32)(Byte1 & 0x00FF) << 8) \
      + ((UINT32)(Byte2 & 0x00FF) << 16) \
      + ((UINT32)(Byte3 & 0x00FF) << 24)))

#define BUILD_UINT16(loByte, hiByte) \
        ((UINT16)((loByte & 0x00FF) + ((hiByte & 0x00FF) << 8)))

#define HI_UINT16(a) ((a >> 8) & 0xFF)
#define LO_UINT16(a) (a & 0xFF)

#define BUILD_UINT8(hiByte, loByte) \
        ((UINT8)((loByte & 0x0F) + ((hiByte & 0x0F) << 4)))

#define HI_UINT8(a) ((a >> 4) & 0x0F)
#define LO_UINT8(a) (a & 0x0F)


/************************************************************************/
/*					Type Definition										*/
/************************************************************************/
typedef struct
{
    UINT8       AddrMode;
    ADDRESS     Addr;
    UINT8       EP;
} AF_ADDR;

/* AF Format
*/
   
/* Frame Types */
typedef enum
{
    AF_FRM_KVP = 0x01,
    AF_FRM_MSG = 0x02,
}AF_FRM_TYPE;


/*Command type*/
typedef enum
{
    AF_CMD_SET          = 0x01,
    AF_CMD_EVENT        = 0x02,
    AF_CMD_GET_ACK      = 0x04,
    AF_CMD_SET_ACK      = 0x05,
    AF_CMD_EVENT_ACK    = 0x06,
    AF_CMD_GET_RSP      = 0x08,
    AF_CMD_SET_RSP      = 0x09,
    AF_CMD_EVENT_RSP    = 0x0A
}AF_CMD_TYPE;
  
/*Attribute Data Types*/
typedef enum
{
    AF_ATT_NO_DATA     = 0x00,
    AF_ATT_UINT8       = 0x01,
    AF_ATT_INT8        = 0x02,
    AF_ATT_UINT16      = 0x03,
    AF_ATT_INT16       = 0x04,
    AF_ATT_SEMIPREC    = 0x0B,
    AF_ATT_ABS_TIME    = 0x0C,
    AF_ATT_REL_TIME    = 0x0D, /* Relative Time (32 bits)*/
    AF_ATT_CHAR_STR    = 0x0E,
    AF_ATT_OCTET_STR   = 0x0F
}AF_ATT_DATA_TYPE;
/* Error code*/
typedef enum
{
    AF_ERR_SUCCESS             =0x00,
    AF_ERR_INVALID_ENDPOINT    =0x01,
    AF_ERR_UNSUPPORTED_ATTRIB  =0x03,
    AF_ERR_INVALIDE_CMDTYPE    =0x04,
    AF_ERR_INVALID_DATALENGTH  =0x05,
    AF_ERR_INVALID_DATA        =0x06
}AF_ERR_CODE;
    
/*Latency request*/
typedef enum
{
    NO_LAT_REQ,
    FAST_BCN,
    SLOW_BEACON
} AF_NWK_LAT_TYPE;

/* Special AF Data Types
*/
 
/*  ZigBee Absolute Time format */
typedef UINT32 AF_ABS_TIME;         /*Second, from midnight 1st jan 2000*/

/*  ZigBee Relative Time format */
typedef UINT32 AF_REL_TIME;         /*Relative time, millisecond*/

/* ZigBee Semi-Precision format */
typedef struct
{
    UINT16 sign:1;
    UINT16 exponent:5;
    UINT16 mantissa:10;
} AF_SEMI_PRE;

typedef struct
{
    BYTE        TransSeqNumber;
    UINT8       CmdType     :4; 
    UINT8       AttrDataType:4;
    UINT16      AttrId;
    BYTE        ErrCode;            // Only used in responses
    BYTE        DataLen;            // Number of bytes in TransData
    BYTE*       pData;          
} AF_KVP;  

/*  MSG Command Format */
typedef struct
{
    BYTE   TransSeqNumber;
    BYTE   DataLen;               // Number of bytes in TransData
    BYTE*  pData;               
} AF_MSG;  

/*Format of sending AF frame*/
typedef struct  
{
    UINT8       nFrmSize;
    UINT8       nCurrOffset;
    UINT8       TransCount:4;
    UINT8       FrmType   :4;
    BYTE*       pData;
}AF_FRM;

/* format of incoming packet/msg */
typedef struct
{ 
    BYTE        DstEp;                  // Destination endpoint 
    AF_ADDR     SrcAddr;
    BOOL        Broadcast;
    BYTE        ClusterId;
    UINT8       LinkQuality;
    BYTE        Security;
    UINT8       ACLEntry;
    BYTE        CmdLen;
    BYTE        Cmd[1];
} AF_IMCOM_FRM;  
/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/

/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/


/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/*----------------------------------------------------------------------------
                            BUILD OF INCOMMING AF FRAME
----------------------------------------------------------------------------*/

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will allocate memory and format the incom frm   */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
AF_IMCOM_FRM*  apf_buildIncomFrm(P_APS_FRM par_pFrm, UINT8 nDataLength, UINT8 SrcAddressMode, 
                                 ADDRESS*  pSrcAddr,
                                 UINT8 LinkQuality, BYTE SecurityUse);

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	  EP will call this function to release the frame after using */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void  apf_releaseInComFrm(AF_IMCOM_FRM*  pFrm);


/*----------------------------------------------------------------------------
                            PARSE OF INCOMMING AF FRAME
----------------------------------------------------------------------------*/

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
AF_FRM_TYPE apf_getTypeOfFrm(AF_IMCOM_FRM* pFrm);

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT8 apf_getNumOfTrans(AF_IMCOM_FRM* pFrm);

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	 This function will allocate memory and copy the requested 
//               transaction into it.                                         */
//              Notice that memory is shared between MSG transactions         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL apf_getMSGTrans(AF_IMCOM_FRM* pFrm, AF_MSG* pMsg, UINT8 nCount);

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will extract a transaction from the KVP         */
//              Notice that memory is shared between KVP transactions         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL apf_getKVPTrans(AF_IMCOM_FRM* pFrm, AF_KVP* pKVP, UINT8 nCount);


/*----------------------------------------------------------------------------
                            BUILD OF AF FRAME BEFORE SEND
----------------------------------------------------------------------------*/
/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	 This function will init a frm before format it with data     */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
AF_FRM*    apf_initFrm(AF_FRM_TYPE    par_FrmType);


/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will add new MSG to the FRM after formatting frm*/
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL    apf_addMsg(AF_FRM*  par_pFrm, AF_MSG*   par_pMsg);

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will release an allocated frm                   */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void apf_releaseFrm(AF_FRM*  par_pFrm);


#endif //!APF_FORMAT_H
