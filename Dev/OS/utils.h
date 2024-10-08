/*******************************************************************************/
//Module:  OS                                                                  */   
//File Name: utils.h                                                           */
//Description:  Implements type definitions for utils.c                        */
//  -------------------------------------------------------------              */
//Revision History:                                                            */
//Date						 Programmer		Details                            */
//11/15/2006                 pnthai                                            */
//  -------------------------------------------------------------              */
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef UTILS_H
#define UTILS_H


/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "zconf.h"
#include "types.h"
/************************************************************************/
/*					Type Definition										*/
/************************************************************************/


/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/


/************************************************************************/
/*					Function            								*/
/************************************************************************/



/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/


/************************************************************************/
/*                      Function                                        */
/************************************************************************/



/************************************************************************/
/*                      RANDOM GENERATOR                                */
/************************************************************************/
/******************************************************************************/
//@FunctionName: random_init             				  	                  */
//@Description:	This function will be call at starting to initialize          */
//              Random generator                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void random_init(); 	


/******************************************************************************/
//@FunctionName:  ramdom_getRandomByte 										  */
//@Description:	This function will be call to get a random BYTE               */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BYTE ramdom_getRandomByte(void);

/******************************************************************************/
//@FunctionName: wait  										  	              */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void wait(UINT16 timeout);

/************************************************************************/
/*                      MEMORY MANAGEMEM                                */
/************************************************************************/

/******************************************************************************/
//@FunctionName:  mem_init 										  	          */
//@Description:	Initialize memory management component                        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mem_init();

/******************************************************************************/
//@FunctionName: mem_getMem  										  	      */
//@Description:	 This function will return the free memory block 			  */	
//               Currently, function support: 32BYTE-Block					  */
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void* mem_getMem(UINT8 par_nSize);

/******************************************************************************/
//@FunctionName:  mem_freeMem 										  	      */
//@Description:	 This function will the free memory block					  */	
//               Currently, function support: 32BYTE-Block					  */				
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mem_freeMem(void* par_pBuf);


/******************************************************************************/
//@FunctionName: mem_cpy  										  	          */
//@Description:	 This function is called to copy two memory block             */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void  mem_cpy (BYTE *pdest, BYTE *psrc, UINT16 size);

#endif //!UTILS_H
