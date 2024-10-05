/*******************************************************************************/
//Module:  Common                                                              */   
//File Name: zconf.c                                                           */
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


/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "types.h"
#include "zconf.h"
#include "app_framework.h"
/************************************************************************/
/*					Type Definition										*/
/************************************************************************/

/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/
/* Capacity definition */
#if ZIGBEE_DEVICE_FFD == 1
    #define CAP_DEVICETYPE  (CAP_ALT_PANCOORD | CAP_DEVICETYPE_FFD | CAP_POWER_AC     \
                             | CAP_RCVR_ON_IDLE | CAP_SECURITY_CAPABLE                   \
                             | CAP_ALLOC_ADDR )
#else
    #define CAP_DEVICETYPE        CAP_RCVR_ON_IDLE
#endif

#define MAX_BUFFER_SIZE		      80		  

/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
ENDPOINT_DESTCRIPTOR*        EndpointDescriptors[ZIGBEE_NUM_EP];

/* ZDO do not change it*/
void    zdo_init(void*   pParam,void*  arg);
ENDPOINT_DESTCRIPTOR        ZDO_Descriptor =
{   {EP_ID_ZDO,0,0,0,0,0,0,0,0},           /* ZDO simple descriptors */
     NULL,
     zdo_init
};

/* Node descriptors
*/
NODE_DESCRIPTOR             NodeDescriptor =
{
    ZIGBEE_DEVICE_TYPE,      /*Logical Type*/
    0,			             /*Reserved*/
    0,	        	         /*APS flags*/
    FREQ_BAND,               /*Frequency Band*/
    CAP_DEVICETYPE,
    {0,0},                    /* Manufacturer Code */ 
    MAX_BUFFER_SIZE,          /* Maximum Buffer Size */
    {0, 0}                    /* Maximum Transfer Size*/
};    

/* Node power descriptor
*/
NODE_POWER_DESCRIPTOR         NodePowerDescriptor =
{
#if ZIGBEE_DEVICE_FFD == 1
  0x00,                     /* Power mode: Always ON*/    
  0x00,                     /* main power sources*/
  0x00,                     /* current power source: main power source */
  0x0C                      /* Power Level */
#else   /* End Device*/
  0x01,                     /* Power mode: Periodically ON*/
  0x02,                     /* available power sources: battery*/
  0x02,                     /* current power source: Battery */
  0x08                      /* Power Level: 66% */
#endif
};


/* device address*/
QWORD           aExtendedAddress = 0x1325;
/************************************************************************/
/*					Function            								*/
/************************************************************************/



/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
