/*********************************************************************
    Filename:       OnBoard.c
    Revised:        $Date: 2005/06/25 01:03:31 $
    Revision:       $Revision: 1.47 $
   
    Description:    This file contains the UI and control for the 
                    peripherals on the EVAL development board
    Notes:          This file targets the Chipcon CC2420DB
   
    Copyright (c) 2004 by Figure 8 Wireless, Inc., All Rights Reserved.
    Permission to use, reproduce, copy, prepare derivative works,
    modify, distribute, perform, display or sell this software and/or
    its documentation for any purpose is prohibited without the express
    written consent of Figure 8 Wireless, Inc.
*********************************************************************/
/*             This file is specific for CNL-Zigbee board           */
/*********************************************************************
 * INCLUDES
 */
#include "types.h"
#include "hal.h"
#include "mac_types.h"
#include "board.h"
#include "app_framework.h"
#include "schedul.h"
/*********************************************************************
 * MACROS
 */

#define TurnOnLED1  SET_GLED()
#define TurnOnLED2  SET_OLED()
#define TurnOnLED3  SET_RLED()
#define TurnOnLED4  SET_YLED()

#define TurnOffLED1 CLR_GLED()
#define TurnOffLED2 CLR_OLED()
#define TurnOffLED3 CLR_RLED()
#define TurnOffLED4 CLR_YLED()

//Interrupt Function definitions
#define INTERRUPT_KEYBD() SIGNAL(SIG_INTERRUPT6)

/*********************************************************************
 * CONSTANTS
 */

// MAC/2420 Power State
#define MAC_PWR_OFF  0
#define MAC_PWR_WAIT 1
#define MAC_PWR_ON   2


/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
// Registered keys task ID, initialized to NOT USED.
static BYTE KeysEP = 0xFF;

// LED state at last set/clr/blink update
static BYTE ledState;

static BYTE lastKey;
/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void ChkReset( void );
static void KBInit( void );

/*********************************************************************
 * @fn      InitBoard()
 * @brief   Initialize the CC2420DB Board Peripherals
 * @param   level: COLD,WARM,READY
 * @return  None
 */
void InitBoard()
{
    // Set up CC2420 peripherals
    DB_PERIPHERAL_PORT_INIT();
    // Set up unused I/O pins
    CFG_UNUSED();

    // Set up Orange LED dimmer (PWM0)
    PWM0_INIT( TIMER0_CLK_DIV256*3 );

    // Turn all LEDs off 
    LedOnOff( LED_ALL, LED_OFF );

    // Check for Brown-Out reset
    ChkReset();
    // Set up "keyboard" interrupts
    KBInit( );

    createKeysPollingTimer();
} 

/*********************************************************************
 * @fn      ChkReset()
 * @brief   Check reset flags - if reset was caused by a brown-out
 *          (low power), assume batteries are not reliable, hang in
 *          a loop and sequence through the LEDs.
 * @param   None
 * @return  None
 */
void ChkReset( void )
{
    BYTE led;
    BYTE rib;

    // Isolate reset indicator bits
    rib = MCUCSR & RSBTS;
    // Clear 'em all out
    MCUCSR &= ~RSBTS;

    // If Brown-Out reset, hang and blink 
    if ( rib == RSFBO ) {
        led = LED4;
        while ( 1 ) {
            LedOnOff( led, LED_ON );
            wait( 62500 );
            wait( 62500 );
            LedOnOff( led, LED_OFF );
            wait( 37500 );
            wait( 37500 );
            if ( !(led >>= 1) )
                led = LED4;
        }
    }

    // Clear all reset bits
    MCUCSR &= ~RSBTS;
} 

/*********************************************************************
 *                      LED CONTROL FUNCTIONS
 *
 * This development board provides 4 LEDs for display of user info.
 * These functions perform the hardware specific functions.
 */

 /*********************************************************************
 * Dimmer Support
 */
/*
 * Dim (set level) of LED2 
 *   lvl:  light level, 0-100 %
 */
void Dimmer( BYTE lvl )
{
    BYTE tdc;

    // Inverted percentage
    tdc = 100 - lvl;
    // Percentage of full-scale
    lvl = ( (UINT16)tdc * 255 ) / 100;
    // Set timer0 PWM duty cycle
    PWM0_SET_DUTY_CYCLE( lvl );
}

/*********************************************************************
 * @fn      LedOnOff
 *
 * @brief   Turns specified LED on or off
 * @param   mask - LED bit mask
 * @param   on - LED_ON,LED_OFF,LED_BLINK
 * @return  none
 */
void LedOnOff( BYTE mask, BYTE on )
{
  if ( mask & LED1 ) {
    if ( on == LED_ON )
      TurnOnLED1;
    else
      TurnOffLED1;
  }
  if ( mask & LED2 ) {
    if ( on == LED_ON )
      Dimmer( DIM_ON_LEVEL );
    else
      Dimmer( 0 );
  }
  if ( mask & LED3 ) {
    if ( on == LED_ON )
      TurnOnLED3;
    else
      TurnOffLED3;
  }
  if ( mask & LED4 ) {
    if ( on == LED_ON )
      TurnOnLED4;
    else
      TurnOffLED4;
  }

  // Remember current state
  if ( on )
    ledState |= mask;
  else
    ledState &= ~mask;
}
/*********************************************************************
 * Keypad Support
 */
 
// "Keyboard" initialization
void KBInit( void )
{
    // Joystick ins are input
    PORTE |= ( PEJI | KEYS_INT );
    DDRE &= ~( PEJI | KEYS_INT );
    PORTF |= ( PFJI );
    DDRF &= ~( PFJI );

    DISABLE_JOYSTICK_INT();
}


// Set endpoint to receive keys
void RegisterEP(UINT8   EP)
{
    KeysEP = EP;
}

// Send "Key Pressed" message to application
BYTE OnBoard_SendKeys( BYTE shift, BYTE keys )
{
    WORD    Data;
    BYTE    *pBuf;

    pBuf    = &Data;
    *pBuf++ = shift;
    *pBuf = keys;

    apf_sendAppMsg(KeysEP,APF_KEYS,Data);
}

// "Keyboard" Interrupt Service Routine
INTERRUPT_KEYBD()
{
//     BYTE keys;
//     
//     // Disable interrupts
//     //DISABLE_JOYSTICK_INT();
//     CLEAR_JOYSTICK_INT();   
//     
//     /*Read key*/
//     keys = KEYS;
// 
//     if ( !(keys & EVAL_SW_MASK) ) 
//         return;
// 
//     /* send a key  */
//     OnBoard_SendKeys( ((keys & EVAL_SW6) ? true : false), keys );
// 
//     // Wait a little while to avoid key bounce
//     wait(10);
// 
//     //ENABLE_JOYSTICK_INT();  // Enable interrupts
}

//Poll the key
void pollBoard()
{
	/*********** Variable declaration ***********/
    BYTE keys;
	/*********** Function body		 ***********/    
    /*Read key*/
    keys = KEYS;

    if(keys == lastKey)
        return;
    else if (keys == 0x00) /*No button*/
    {
        lastKey = 0x00;
        return;
    }

    lastKey = keys;

    /* send a key  */
    OnBoard_SendKeys( ((keys & EVAL_SW6) ? true : false), keys );
} // End of functions

//Create timer to polll keys
void createKeysPollingTimer()
{
	/*********** Variable declaration ***********/  
    ZAPP_MSG        Msg;
	/*********** Function body		 ***********/
    Msg.EndPointId  = KeysEP;
    Msg.MsgType     = APF_POLL_BOARD;
    mtimer_creatTmr(&Msg,(UINT32)200,TRUE,TASK_APER_APP); /*100ms second*/
} // End of functions
