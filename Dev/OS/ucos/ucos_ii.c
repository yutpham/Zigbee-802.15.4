/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                          (c) Copyright 1992-2006, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File    : uCOS_II.C
* By      : Jean J. Labrosse
* Version : V2.83
*********************************************************************************************************
*/

#define  OS_GLOBALS                           /* Declare GLOBAL variables                              */
#include <ucos_ii.h>


#define  OS_MASTER_FILE                       /* Prevent the following files from including includes.h */
#include <os_mem.c>
#include <os_tmr.c>
