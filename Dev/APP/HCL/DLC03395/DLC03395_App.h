#ifndef DLC03395_APP_H
#define DLC03395_APP_H

/*********************************************************************
    Filename:       DLC03395_App.h
    Revised:        $Date: 2004/12/30 17:08:44 $
    Revision:       $Revision: 1.2 $
    
    Description:    
    
				  Dimmer Load Control - Application

          ZigBee Document 03395r0ZB
            
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
#define DLC03395_APP_ENDPOINT_MANDATORY   2
#define DLC03395_APP_ENDPOINT_OPTIONAL    22

#define DLC03395_STATE_CHANGED_EVT            0x0001
#define DLC03395_CHANGE_DIMMERDEMO_STATE_EVT  0x0002
#define DLC03395_SEND_STATUS_EVT              0x0004


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
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the DLC03395 Application
 */
extern void DLC03395_Init(void* arg1, void* arg2);

/*
 * Task Initialization for the stored information
 */
extern void DLC03395_InitSavedInfo( void );

/*
 * Task Event Processor for the DLC03395 Application
 */
void DLC03395_ProcessEvent( void* arg1, void* arg2 );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* DLC03395_APP_H */
