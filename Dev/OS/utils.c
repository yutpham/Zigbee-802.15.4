/*******************************************************************************/
//Module: OS                                                                   */   
//File Name: utils.c                                                           */
//Description:  Implements utility functions                                   */
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
#include "mac_types.h"
#include "utils.h"
#include "ucos_ii.h"
#include "hal.h"
/************************************************************************/
/*					Type Definition										*/
/************************************************************************/

/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/
#define             RANDOM_CONST            0x1021
/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/

/* Memory partition of general used 
*/
BYTE                MemPool16[ZIGBEE_MEM_BLOCK_16][16];
BYTE                MemPool32[ZIGBEE_MEM_BLOCK_32][32];
BYTE                MemPool64[ZIGBEE_MEM_BLOCK_64][64];
BYTE                MemPool96[ZIGBEE_MEM_BLOCK_96][96];

/* Pointer to the 32-byte partition
*/
OS_MEM*             Mem[4];
/* Variable to hold random value
*/
WORD                random;
/************************************************************************/
/*					Function            								*/
/************************************************************************/



/************************************************************************/
/*	
				Variable declaration								*/
/************************************************************************/


/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/************************************************************************/
/*                      MEMORY MANAGEMEM                                */
/************************************************************************/

/******************************************************************************/
//@FunctionName:  mem_init   							  	                  */
//@Description:	Initialize memory management component                        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mem_init()
{
    UINT8           nErr;
    Mem[0]  = OSMemCreate(MemPool16, 
                           ZIGBEE_MEM_BLOCK_16,
                           16,
                           &nErr);        
    Mem[1]  = OSMemCreate(MemPool32, 
                           ZIGBEE_MEM_BLOCK_32,
                           32,
                           &nErr);        
    Mem[2] = OSMemCreate(MemPool64, 
                           ZIGBEE_MEM_BLOCK_64,
                           64,
                           &nErr);        
    Mem[3] = OSMemCreate(MemPool96, 
                          ZIGBEE_MEM_BLOCK_96,
                           96,
                           &nErr);        
}

/******************************************************************************/
//@FunctionName:   mem_getLeastMem										  	  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void*  mem_getLeastMem(UINT8 index) 
{
	/*********** Variable declaration ***********/
    UINT8           nErr;
    void*           pMem;
	/*********** Function body		 ***********/
    for (UINT8 i = index;i<4;i++)
    {
        pMem = OSMemGet(Mem[i], &nErr);
        if (pMem)
            return pMem;
    }    
    return NULL;
} // End of functions


/******************************************************************************/
//@FunctionName: mem_getMem  										  	      */
//@Description:	 This function will return the free memory block              */
//               Currently, function support: 32BYTE-Block					  */	
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void* mem_getMem(UINT8 par_nSize)
{
    if (par_nSize <= 16)
        return mem_getLeastMem(0);
    else if (par_nSize <= 32)
        return mem_getLeastMem(1);
    else if (par_nSize <= 64)
        return mem_getLeastMem(2);
    else if (par_nSize <= 96)
        return mem_getLeastMem(3);

    return NULL;
}

/******************************************************************************/
//@FunctionName:  mem_freeMem 										  	      */
//@Description:	 This function will the free memory block 					  */
//               Currently, function support: 32BYTE-Block			 	      */
//	                                                                          */   
/******************************************************************************/
void mem_freeMem(void* par_pBuf)
{
    UINT8           nErr;
    if (MemPool16 <= par_pBuf && par_pBuf < (BYTE*)MemPool16+16*ZIGBEE_MEM_BLOCK_16)
        nErr = OSMemPut(Mem[0], (void*)par_pBuf);
    else if (MemPool32 <= par_pBuf && par_pBuf < (BYTE*)MemPool32+32*ZIGBEE_MEM_BLOCK_32)
        nErr = OSMemPut(Mem[1], (void*)par_pBuf);
    else if (MemPool64 <= par_pBuf && par_pBuf < (BYTE*)MemPool64+64*ZIGBEE_MEM_BLOCK_64)
        nErr = OSMemPut(Mem[2], (void*)par_pBuf);
    else if(MemPool96 <= par_pBuf && par_pBuf < (BYTE*)MemPool96+96*ZIGBEE_MEM_BLOCK_96)
        nErr = OSMemPut(Mem[3], (void*)par_pBuf);
}

/******************************************************************************/
//@FunctionName: mem_cpy  										  	          */
//@Description:	 This function is called to copy two memory block             */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void  mem_cpy (BYTE *pdest, BYTE *psrc, UINT16 size)
{
    while (TRUE) 
    {
        if (size == 0)
            break;
        *pdest++ = *psrc++;
        size--;
    }
}

/******************************************************************************/
//@FunctionName: mem_set  										  	          */
//@Description:	 This function is called to set 0 memory block                */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void  mem_set (BYTE *pmem, UINT16 size)
{
    while (TRUE) 
    {
        if (size == 0)
            break;
        *pmem++ = 0;
        size--;
    }
}


/************************************************************************/
/*                      RANDOM GENERATOR                                */
/************************************************************************/
/******************************************************************************/
//@FunctionName: random_init  										  	      */
//@Description:	This function will be call at starting to initialize		  */
//              Random generator                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void random_init()
{
    
	UINT8 i;

    // Enable RXBPF, ADC and VGA
    FASTSPI_SETREG(CC2420_MANAND, 0xFFF1);
    
    // Output ADC_Q[0] to CCA pin
    FASTSPI_SETREG(CC2420_IOCFG1, 0x0001);
   
    random = 0;
    while (random == 0) {
        for (i = 0; i < 16; i++) {
#ifdef ZIGBEE_DEBUG
            random = 0x0101;
#else
            // Sample ADC_Q[0] 16 times and shift into random register
            random = (random << 1) | (!CCA_IS_1());
#endif
        }
    }    
    // Set CC2420 registers back to default
    FASTSPI_SETREG(CC2420_MANAND, 0xFFFF);
    FASTSPI_SETREG(CC2420_IOCFG1, 0x0000);
    
} 	


/******************************************************************************/
//@FunctionName: ramdom_getRandomByte  										  */
//@Description:	This function will be call to get a random BYTE				  */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BYTE ramdom_getRandomByte(void)
{
    UINT8 n = 8;

    do {
        if ((random >> 8) & 0x80) {
            random = (random << 1) ^ RANDOM_CONST;
        } else {
            random = (random << 1);
        }
    } while (--n);

    //return (UINT8)(127*random()/RAND_MAX);
    
    return random;
}

/******************************************************************************/
//@FunctionName: wait  										  	              */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void wait(UINT16 timeout) {
    // This sequence uses exactly 8 clock cycle for each round
    do {
        asm volatile ("nop\n\t" ::);
        asm volatile ("nop\n\t" ::);
        asm volatile ("nop\n\t" ::);
        asm volatile ("nop\n\t" ::);
    } while (--timeout);
} 

/******************************************************************************/
//@FunctionName:  cmpQword 										  	          */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL cmpQword(QWORD *pA, QWORD *pB) {
    return (*pA == *pB);
}
