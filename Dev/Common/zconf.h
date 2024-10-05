/*******************************************************************************/
//Module: Common                                                               */   
//File Name: zconf.h                                                           */
//Description: Configuration for Zigbee stack include following part           */
/*             - Configuration of the Abstract OS 
                    + For scheduler: Change file schedul.c
                    + Timer:         Change static_timer.c
                    + Change also os_cfg_r.h also for configuration of timer
                    and number of memory partition                             
               - Configuration of the MAC layer: change below
               - Configuration of the capacity of nodes, change below
*/
//  -------------------------------------------------------------              */
//Revision History:                                                            */
//Date						 Programmer		Details                            */
//11/15/2006                 pnthai                                            */
//  -------------------------------------------------------------              */
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef Z_CONFIG
#define Z_CONFIG


/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "types.h"
/************************************************************************/
/*					Type Definition										*/
/************************************************************************/


/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/*-------------------------------------------------------------*/
/*                  General constants                          */
/*-------------------------------------------------------------*/
#define         VALID_CHANNELS   0x07FFF800

/* Device type */
/* Assign device option:
2: End point
1: Coordinator
0: PAN Coordinator    
*/
#define     ZIGBEE_PAN_COOR      0
#define     ZIGBEE_ROUTER        1
#define     ZIGBEE_ENDPOINT      2

#ifdef  TYPE_COOR
    #define     ZIGBEE_DEVICE_TYPE   ZIGBEE_PAN_COOR     
#elif   TYPE_ROUTER
    #define     ZIGBEE_DEVICE_TYPE   ZIGBEE_ROUTER    
#else
    #define     ZIGBEE_DEVICE_TYPE   ZIGBEE_ENDPOINT   
#endif

#define         ZIGBEE_DEVICE_FFD    (ZIGBEE_DEVICE_TYPE==ZIGBEE_ENDPOINT)? 0:1

/*-------------------------------------------------------------*/
/*                  Endpoint configuration                     */
/*-------------------------------------------------------------*/
/* Endpoint ID */
#define EP_ID_ZDO            0x00       /* ZDO */
#define EP_ID_BROADCAST      0xFF       /* ZDO */
#define ZIGBEE_NUM_EP        2          /* Number of endpoint */
/*-------------------------------------------------------------*/
/*                  Configuration for node capacity            */
/*-------------------------------------------------------------*/

/* capacity */
#define CAP_ALT_PANCOORD          0x01
#define CAP_DEVICETYPE_FFD        0x02
#define CAP_POWER_AC              0x04
#define CAP_RCVR_ON_IDLE          0x08
#define CAP_SECURITY_CAPABLE      0x00      /* Sercurity not implemented */
#define CAP_ALLOC_ADDR            0x80

#if ZIGBEE_DEVICE_TYPE == ZIGBEE_ROUTER
#define ENDDEVICE_CAP             0x01
#endif

#if ZIGBEE_DEVICE_TYPE == ZIGBEE_PAN_COOR
#define ENDDEVICE_CAP             0x01
#endif

#if ZIGBEE_DEVICE_TYPE == ZIGBEE_ENDPOINT
#define ENDDEVICE_CAP             0x00
#endif

#define ROUTER_CAP                0x01

/* Frequency band 
0x01    868 - 868.6 MHz
0x04    902 - 928 MHz
0x08    2400 - 2483.5 MHz
*/
#define     FREQ_BAND                   0x08    
/* Channel and radio */
#define     ZIGBEE_MAX_CHANNEL          26
#define     ZIGBEE_MIN_CHANNEL          11
#define     DEFAULT_CHANNEL             0x13        //0x00080000


/*-------------------------------------------------------------*/
/*                  Configuration for ZDO                      */
/*-------------------------------------------------------------*/

/* Device and service discovery
*/
#define     ZDO_NWK_ADDR_REQ        1
#define     ZDO_NWK_ADDR_RSP        1

#define     ZDO_IEEE_ADDR_REQ       0
#define     ZDO_IEEE_ADDR_RSP       0

#define     ZDO_NODE_DESC_REQ       0
#define     ZDO_NODE_DESC_RSP       0

#define     ZDO_END_DEVICE_ANN      1
/* Put definition here */


/* Security manager 
*/
/*Not implemented*/


/* Binding Manager
*/
#define     ZDO_ENDDEVICE_BIND_REQ      1
#define     ZDO_ENDDEVICE_BIND_RSP      1

#define     ZDO_ENDDEVICE_BIND_UNBIND_REQ 1
#define     ZDO_ENDDEVICE_BIND_RSP      1
/* Put definition here */


/* Node manager
*/
/* Put definition here */


/* Configuration Attribute
*/
#define         conf_CHANNEL_LIST                    0x00080000   
#define         conf_SCAN_DURATION                   6

#define         conf_PAN_ID                          0x2420  

#define         conf_PROTOCOL_VERS                   0x01
#define         conf_PROFILE_ID                      0x00

#define         conf_BEACON_ORDER                    0x0F
#define         conf_SUP_FRM_ORDER                   0x0F

#define         conf_BATT_EXT                        0x00
#define         conf_SCAN_ATTPEMPT                   0x03    
#define         conf_TIME_BWT_SCANS                  0x03
#define         conf_MAX_BINDING                     0x05
#define         conf_BINDING_TIMEOUT                 0x05
#define         conf_PERMIT_JOINT_DUR                0x05 
#define         conf_REMOVE_CHILDREN                 0x00   
#define         conf_TRANSACTION_PERSISTENCE_TIME    0x03 
#define         conf_INDIRECT_POLL_RATE              0x03
#define         conf_MAX_ASSOCIATION                 0x05


/*-------------------------------------------------------------*/
/*                  Configuration for ASP                      */
/*-------------------------------------------------------------*/
#define         MAX_BINDING_CLUSTER_IDS              0x03
#define         MAX_APS_DATA_QUEUE_SIZE              0x04
#define         MAX_APS_INDIREC_QUEUE_SIZE           0x04

/*-------------------------------------------------------------*/
/*                  Configuration for NWK                      */
/*-------------------------------------------------------------*/
#define         NWK_MAX_CHILD                       0x08
#define         NWK_MAX_ROUTER                      0x04

/* Config for network topology
*/
#define         USE_TREE_ROUTING                    TRUE
#define         USE_TREE_ADDR_ALLOC                 TRUE

/* Config for network capacity
*/
#define         DEFAULT_RADIUS                      0x07
#define         NWK_NEIGHBOR_TBLE_SIZE		        (NWK_MAX_CHILD+1) 	// values: nwkMaxChildren + 1 	
#define         MAX_NWK_DESC_SIZE                   0x01    
#define         MAX_NWK_DATA_QUEUE_SIZE             0x08
/*-------------------------------------------------------------*/
/*                  Configuration for system                   */
/*-------------------------------------------------------------*/

/* Number of memory block for 32-bytes and 128-bytes memory partition */
#define         ZIGBEE_MEM_BLOCK_16             16
#define         ZIGBEE_MEM_BLOCK_32             12
#define         ZIGBEE_MEM_BLOCK_64             7
#define         ZIGBEE_MEM_BLOCK_96             2

/*Msg timer*/
#define             MAX_MSG_TIMER       8

/*Restart try delay*/
#define         ZIGBEE_RETRY_DELAY              5 /*Second*/
#define         ZIGBEE_MAX_RETRIES              3

/*-------------------------------------------------------------*/
/*                  Configuration for MAC                      */
/*-------------------------------------------------------------*/
/* Device capacity */
#define     MAC_RFD_ACTIVE_SCAN_ENABLE      0

/* Max PAN descriptor of the MAC */
#define     MAC_MAX_PAN_DESCRIPTOR          3

/*number of packets in the TX pool */
#define     MAC_TX_POOL_SIZE                4  
/*number of packets in the RX pool */
#define     MAC_RX_POOL_SIZE                3  


/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
#ifdef ZIGBEE_DEBUG
extern UINT8                   ControlVariable;
extern BYTE*                   APP;
extern QWORD                   CoorAddr;
#endif


/* device address*/
extern QWORD           aExtendedAddress;
/************************************************************************/
/*					Function            								*/
/************************************************************************/



/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/


#endif //!UTILS_H
