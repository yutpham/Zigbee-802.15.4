#ifndef ONBOARD_H
#define ONBOARD_H

/*********************************************************************
    Filename:       OnBoard.h
    Revised:        $Date: 2005/06/25 01:05:24 $
    Revision:       $Revision: 1.49 $
   
    Description:    Defines stuff for EVALuation boards
    Notes:          This file targets the Chipcon CC2420DB
   
    Copyright (c) 2004 by Figure 8 Wireless, Inc., All Rights Reserved.
    Permission to use, reproduce, copy, prepare derivative works,
    modify, distribute, perform, display or sell this software and/or
    its documentation for any purpose is prohibited without the express
    written consent of Figure 8 Wireless, Inc.
*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
/*********************************************************************
 * GLOBAL VARIABLES
 */
 
/*********************************************************************
 * MACROS
 */

#ifndef __GNUC__
  #define _BV(n) (1<<n)
#endif

// Global Interrupt handling
#ifdef __GNUC__
  #define IntsStorage       unsigned char IntsByte
  #define EnableInts        asm ("sei\n\t");
  #define DisableInts       asm ("cli\n\t");
  #define RestoreInts       SREG = IntsByte;
  #define StoreDisableInts  IntsByte = SREG; DisableInts;
  #define StoreEnableInts   IntsByte = SREG; EnableInts;
#else
  #define IntsStorage       unsigned char IntsByte
  #define EnableInts        __enable_interrupt();
  #define DisableInts       __disable_interrupt();
  #define RestoreInts       __restore_interrupt(IntsByte);
  #define StoreDisableInts  IntsByte = __save_interrupt(); DisableInts;
  #define StoreEnableInts   IntsByte = __save_interrupt(); EnableInts;
#endif

// Make a bit mask
#define BM _BV  // Chipcon did not use gcc-libc _BV macro

// "Key" inputs (joystick and S2)
#define PEBI _BV(BUTTON)
#define PEJI (_BV(JOYSTICK_UP) | _BV(JOYSTICK_RIGHT) | _BV(JOYSTICK_CENTER))
#define PFJI (_BV(JOYSTICK_LEFT) | _BV(JOYSTICK_DOWN))

#define KEYS_INT _BV(JOYSTICK_INT)
#define KEYS ((PINE & PEJI) | (~PINE & PEBI) | ((PINF & PFJI) << 2))

#define JOYSTICK_WAKE_INIT() (EICRB &= ~0x30)

// Reset identification flags
#define RSFJT _BV(JTRF)  // Reset from JTAG
#define RSFWD _BV(WDRF)  // Reset from Watchdog
#define RSFBO _BV(BORF)  // Reset from Brown-Out
#define RSFEX _BV(EXTRF)  // Reset from External
#define RSFPO _BV(PORF)  // Reset from Power-On
#define RSBTS (RSFJT | RSFWD | RSFBO | RSFEX | RSFPO)

// Configure Unused I/O pins
#define CFG_UNUSED() (PORTE |= _BV(PE7))

// Power conservation
#define SLEEP_IDLE     0
#define SLEEP_PWRDOWN _BV(SM1)
#define SLEEP_MASK (_BV(SM0) | _BV(SM1) | _BV(SM2))

#define enter_sleep_mode(mode)   \
{                                \
  MCUCR = (MCUCR & ~SLEEP_MASK); \
  MCUCR |= (MCUCR | mode);       \
  MCUCR |= _BV(SE);              \
  asm ("sleep");                 \
  MCUCR &= ~_BV(SE);             \
}

// Sleep mode definitions (enabled with POWER_SAVING compile option)
#define SLEEP_LITE 1  // AVR idle, all wakeup sources
#define SLEEP_DEEP 2  // AVR powerdown, external int wakeup

#define OSAL_SET_CPU_INTO_SLEEP(m) OnBoard_Sleep(m);  // Called from OSAL_PwrMgr

/*********************************************************************
 * CONSTANTS
 */
// I/O Processing Task Event Codes
#define LED_BLINK_EVT          0x0001
#define KEYPRESS_POLL_EVT      0x0002 
#define KEYPRESS_DEBOUNCE_EVT  0x0004

// Eval board LEDs
#define LED_OFF    0x00
#define LED_ON     0x01
#define LED_BLINK  0x02
#define LED_FLASH  0x04
#define LED_TOGGLE 0x08

#define LED_NONE 0x00
#define LED1     0x01
#define LED2     0x02
#define LED3     0x04
#define LED4     0x08
#define LED_ALL (LED1 | LED2 | LED3 | LED4)

#define NBR_OF_LEDS     4
#define DIM_ON_LEVEL    33
#define LED_BLINK_TIME  1000
#define LED_DUTY_CYCLE  50
#define LED_FLASH_COUNT 5

// Eval board switches (keys)
#define EVAL_SW1 0x01  // Joystick up
#define EVAL_SW2 0x02  // Joystick right
#define EVAL_SW3 0x10  // Joystick down
#define EVAL_SW4 0x08  // Joystick left
#define EVAL_SW5 0x04  // Joystick center
#define EVAL_SW6 0x20  // Button S2

#define EVAL_SW_MASK 0x3F

// This Key definition is unique to this development system 
// It is used to bypass the Network layer NV restore
#define SW_BYPASS_NV EVAL_SW5

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
* FUNCTIONS
*/

/*
* Initialize the Peripherals
*/
extern void InitBoard();

/* Serial Port Control Functions */
/*
* Initialize serial ports
*   return: serial ports in use (bit mask)
*/

/*********************************************************************
* @fn      LedOnOff
*
* @brief   Turns specified LED on or off
* @param   mask - LED bit mask
* @param   on - LED_ON,LED_OFF,LED_BLINK
* @return  none
*/
extern void LedOnOff( BYTE mask, BYTE on );
/*
* Dim (set level) of User LEDs 
*   lvl:  light level, 0-100 %
*/
extern void Dimmer( BYTE lvl );

// Set endpoint to receive keys
extern void RegisterEP(UINT8   EP);
/*********************************************************************
*********************************************************************/

#endif // ONBOARD_H
