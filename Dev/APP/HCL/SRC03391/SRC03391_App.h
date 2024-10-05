#ifndef SRC03391_APP_H
#define SRC03391_APP_H

/*********************************************************************
    Filename:       SRC03391_App.h
    Revised:        $Date: 2004/12/30 16:28:14 $
    Revision:       $Revision: 1.2 $
    
    Description:    
    
				  Switch Remote Control - Application

          ZigBee Document 03391r0ZB
            
    Notes:  

    Copyright (c) 2003 by Figure 8 Wireless, Inc., All Rights Reserved.
    Permission to use, reproduce, copy, prepare derivative works,
    modify, distribute, perform, display or sell this software and/or
    its documentation for any purpose is prohibited without the express
    written consent of Figure 8 Wireless, Inc.
*********************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
 
/*********************************************************************
 * CONSTANTS
 */
#define SRC03391_APP_ENDPOINT_MANDATORY     5
#define SRC03391_APP_ENDPOINT_OPTIONAL      25

#define SRC03391_SEND_STATE               0x0001

#define DRC03392_APP_ENDPOINT_MANDATORY   1
#define DRC03392_APP_ENDPOINT_OPTIONAL    21

#define DRC03392_SEND_STATE_EVT           0x0001
#define DRC03392_SEND_LEVEL_EVT           0x0002
#define DRC03392_SEND_PRESET_EVT          0x0004
#define DRC03392_READ_LEVEL_EVT           0x0008

/**/
// AppDevVer values
#define APPDEVVER_1               0x00
// AppFlags - bit map
#define APPFLAG_NONE              0x00  
#define APPFLAG_COMPLEXDESC       0x01  // Complex Descriptor 
                                  // Available
// Command Types
#define CMDTYPE_SET          0x01
#define CMDTYPE_EVENT        0x02
#define CMDTYPE_GET_ACK      0x04    // GET with ACK
#define CMDTYPE_SET_ACK      0x05    // SET with ACK
#define CMDTYPE_EVENT_ACK    0x06    // EVENT with ACK
#define CMDTYPE_GET_RESP     0x08    // GET Response
#define CMDTYPE_SET_RESP     0x09    // SET Response
#define CMDTYPE_EVENT_RESP   0x0A    // EVENT Response

// Error Codes
#define ERRORCODE_SUCCESS             0x00
#define ERRORCODE_INVALID_ENDPOINT    0x01
#define ERRORCODE_UNSUPPORTED_ATTRIB  0x03
#define ERRORCODE_INVALIDE_CMDTYPE    0x04
#define ERRORCODE_INVALID_DATALENGTH  0x05
#define ERRORCODE_INVALID_DATA        0x06

// Attribute Data Types
#define DATATYPE_NO_DATA     0x00
#define DATATYPE_UINT8       0x01    // unsigned integer 8 bit
#define DATATYPE_INT8        0x02    // signed integer 8 bit
#define DATATYPE_UINT16      0x03    // unsigned integer 16 bit
#define DATATYPE_INT16       0x04    // signed integer 16 bit
#define DATATYPE_SEMIPREC    0x0B    // Semi-Precision (16 bit)
#define DATATYPE_ABS_TIME    0x0C    // Absolute Time (32 bits)
#define DATATYPE_REL_TIME    0x0D    // Relative Time (32 bits)
#define DATATYPE_CHAR_STR    0x0E    // Character String 
#define DATATYPE_OCTET_STR   0x0F    // Octet String

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the SRC03391 Application
 */
extern void SRC03391_Init( void* arg1, void* arg2 );

/*
 * Task Event Processor for the SRC03391 Application
 */
extern void SRC03391_ProcessEvent( void* arg1, void* arg2 );

/*
 * Changed the On Off state then generates a send event.
 */
extern void SRC03391_ChangeState( byte State );

/*
 * Toggle the On Off state then generates a send event.
 */
extern void SRC03391_ToggleState( void );

/*
 * Helper Function
 */

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SRC03391_APP_H */