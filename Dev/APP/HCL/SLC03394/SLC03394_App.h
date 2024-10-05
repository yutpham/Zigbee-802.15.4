#ifndef SLC03394_APP_H
#define SLC03394_APP_H

/*********************************************************************
    Filename:       SLC03394_App.h
    Revised:        $Date: 2004/12/30 16:39:52 $
    Revision:       $Revision: 1.2 $
    
    Description:    
    
				  Switching Load Control - Application

          ZigBee Document 03394r0ZB
            
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
#define SLC03394_APP_ENDPOINT_MANDATORY   6
#define SLC03394_APP_ENDPOINT_OPTIONAL    26

#define SLC03394_STATE_CHANGED_EVT        0x0001
#define SLC03394_SEND_STATUS_EVT          0x0002


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
 * Task Initialization for the SLC03394 Application
 */
extern void SLC03394_Init( void* arg1, void* arg2 );

/*
 * Task Event Processor for the SLC03394 Application
 */
extern void SLC03394_ProcessEvent( void* arg1, void* arg2 );

/*
 * Status_SLC funtion for SLC03394 Application
 */
extern void SLC03394_Set_Status(uint16 AttribId, uint16 Data);

/*
 * Helper function for GET
 */
extern void SLC03394_SetSavedProgramSLC( uint16 AttribId, byte State );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SLC03394_APP_H */