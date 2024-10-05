/*******************************************************************************/
//Module: OS	                                                               */   
//File Name: nv.c                                                              */
//Description: This file implements EEPROM functions    					   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//07/22/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "types.h"
#include "nv.h"
#include "mac_types.h"

/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/
#define IntStorage        unsigned char IntsByte
#define restoreInt()      SREG = IntsByte
#define storeDisableInt() IntsByte = SREG; DISABLE_GLOBAL_INT()
#define storeEnableInt()  IntsByte = SREG; ENABLE_GLOBAL_INT()

#define _BV(n)          (1<<n)
#define NVMWE           _BV(EEMWE)  // Master Write Enable
#define NVDWE           _BV(EEWE)   // Data Write Enable
#define NVDRE           _BV(EERE)   // Data Read Enable

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/


/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/

/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/

/******************************************************************************/
//@FunctionName: nv_read     							  	                  */
//@Description:	This function will read an memory from  EEPROM                */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nv_read( UINT16 Addr, void *pData, UINT16  nLen )
{
	/*********** Variable declaration ***********/
    byte *p = pData;
	/*********** Function body		 ***********/
    IntStorage;

    while ( nLen > 0 )
    {
        while ( EECR & NVDWE ); // Wait 

        storeDisableInt();       

        EEAR = Addr++;          
        EECR |= NVDRE;          
        *p++ = EEDR;            

        restoreInt();            
        nLen--;
    }
} // End of functions

/******************************************************************************/
//@FunctionName:  nv_write 										  	          */
//@Description:	 This function will write an memory into EEPROM               */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void nv_write( UINT16 Addr, void *pData, UINT16  nLen )
{
	/*********** Variable declaration ***********/
    byte *p = pData;
	/*********** Function body		 ***********/
    IntStorage;

    // while there's more to save
    while ( nLen > 0 )
    {
        while ( EECR & NVDWE ); 

        EEAR = Addr++;          
        EEDR = *p++;            

        storeDisableInt();       

        EECR |= NVMWE;          
        EECR |= NVDWE;          

        restoreInt();            
        nLen--;
    }
} // End of functions




