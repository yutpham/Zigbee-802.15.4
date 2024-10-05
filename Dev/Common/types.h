//*******************************************************************************/
//Module: Common                                                               */   
//File Name: types.h                                                           */
//Description: Defines standard data types                                     */
//  -------------------------------------------------------------              */
//Revision History:                                                            */
//Date						 Programmer		Details                            */
//11/15/2006                                                                   */
//  -------------------------------------------------------------              */
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

#ifndef TYPE_H
#define TYPE_H

/*******************************************************************************************************
*******************************************************************************************************
**************************               GENERAL DEFINITIONS                 **************************
*******************************************************************************************************
*******************************************************************************************************/
//-------------------------------------------------------------------------------------------------------
// Data types
//-------------------------------------------------------------------------------------------------------

// Boolean
typedef enum
{
    false,
    true
} BOOL;


#ifndef NULL
#define NULL (0)
#endif

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE 
#define FALSE 0
#endif


/*** Data Types ***/
typedef unsigned char   byte;
typedef unsigned char   uint8;
typedef char            int8;
typedef unsigned short  uint16;
typedef short           int16;
typedef unsigned long   uint32;
typedef long            int32;

// Data
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef unsigned long long  QWORD;

// Unsigned numbers
typedef unsigned char   UINT8;
typedef unsigned short  UINT16;
typedef unsigned long   UINT32;
typedef unsigned long   UINT64;

// Signed numbers
typedef signed char     INT8;
typedef signed short    INT16;
typedef signed long     INT32;
typedef signed long     INT64;

//Address types
typedef union
{
    WORD            Short;
    QWORD           Extended;
} ADDRESS;

 typedef byte    LongAddr[8];

typedef struct
{
    ADDRESS addr;
    byte    addrMode;
}AddrType;

typedef unsigned char      Z_STATUS;
//---------------------------------------------
//ADDRESS

//address length
#define MAC_PAN_ID_LEN             2
#define ADDR_LEN_SHORT_16          2
#define ADDR_LEN_EXTENDED_64       8

// Address modes:
#define AM_NONE                 0
#define AM_SHORT_16             2
#define AM_EXTENDED_64          3


/*	 Broadcasting Address      */
#define BROADCAST_ADDR              0xffff

// Function pointer
typedef void (*P_FNC)(void *ptmr, void *parg);


/* Error level */
typedef enum
{
    ERR_NO = 0,
    ERR_DEBUG,
    ERR_UNRECOVERABLE
} ERROR;

#endif  // !TYPE_H
