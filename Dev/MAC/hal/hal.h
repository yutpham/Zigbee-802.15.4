/*******************************************************************************/
//Module:  MAC/hal                                                             */   
//File Name: hal.h                                                             */
//Description: Type definitions for common hardware abstraction   			   */
//  -------------------------------------------------------------              */
//Revision History:                                                            */
//Date						 Programmer		Details                            */
//  -------------------------------------------------------------              */
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/******************************************************************************************************
							RE-USED FROM C2420 STACK
*******************************************************************************************************/


#ifndef HAL_H
#define HAL_H


/*******************************************************************************************************
 *******************************************************************************************************
 **************************                   Basic stuff                     **************************
 *******************************************************************************************************
 *******************************************************************************************************/

#define BM(n) (1 << (n))
#define BF(x,b,s) (((x) & (b)) >> (s))
#define MIN(n,m) (((n) < (m)) ? (n) : (m))
#define MAX(n,m) (((n) < (m)) ? (m) : (n))
#define ABS(n) ((n < 0) ? -(n) : (n))

/*******************************************************************************************************
 *******************************************************************************************************
 **************************                   AVR I/O PORTS                   **************************
 *******************************************************************************************************
 *******************************************************************************************************/


//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
// Port A:
#define SWITCH0         0 // PA.0 - Output: SW0
#define SWITCH1         1 // PA.1 - Output: SW1
#define SWITCH2         2 // PA.2 - Output: SW2
#define SWITCH3         3 // PA.3 - Output: SW3
#define SWITCH4         4 // PA.4 - Output: SW4
#define SWITCH5         5 // PA.5 - Output: SW5
#define SWITCH6         6 // PA.6 - Output: SW6
#define SWITCH7         7 // PA.7 - Output: SW7
//-------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------
// Port B
#define CSN             0 // PB.0 - Output: SPI Chip Select (CS_N)
#define SCK             1 // PB.1 - Output: SPI Serial Clock (SCLK)
#define MOSI            2 // PB.2 - Output: SPI Master out - slave in (MOSI)
#define MISO            3 // PB.3 - Input:  SPI Master in - slave out (MISO)
#define OLED            4 // PB.4 - Output: Orange LED
#define VREG_EN         5 // PB.5 - Output: VREG_EN to CC2420
#define RESET_N         6 // PB.6 - Output: RESET_N to CC2420
#define GLED            7 // PB.7 - Output: Green LED
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
// Port D
#define FIFOP           0 // PD.0 - Input:  FIFOP from CC2420
#define FIFO            1 // PD.1 - Input:  FIFO from CC2420
#define UART1_RXD       2 // PD.2 - Input:  UART1 RXD
#define UART1_TXD       3 // PD.3 - Output: UART1 TXD
#define SFD             4 // PD.4 - Input:  SFD from CC2420
#define UART1_RTS       5 // PD.5 - Output: UART HW handshaking: RTS
#define CCA             6 // PD.6 - Input:  CCA from CC2420
#define UART1_CTS       7 // PD.7 - Input:  UART HW handshaking: CTS
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
// Port E
#define JOYSTICK_UP     0 // PE.0 - Input : Joystick up button
#define JOYSTICK_RIGHT  1 // PE.1 - Input : Joystick right
#define JOYSTICK_CENTER 2 // PE.2 - Input : Joystick center
#define RLED            3 // PE.3 - Output: Red LED
#define YLED            4 // PE.4 - Output: Yellow LED
#define BUTTON          5 // PE.5 - Input : Push button S2
#define JOYSTICK_INT    6 // PE.6 - Input : Joystick interrupt
//-------------------------------------------------------------------------------------------------------
// Port F
#define JOYSTICK_LEFT   1 // PF.1 - Input: Joystick left
#define JOYSTICK_DOWN   2 // PF.2 - Input: Joystick down

//-----------------------------------------------------------
// Clears LED
#define CLR_GLED()                  do { PORTB &= ~BM(GLED); } while (0)
#define CLR_OLED()                  do { PORTB &= ~BM(OLED); } while (0)
#define CLR_RLED()                  do { PORTE &= ~BM(RLED); } while (0)
#define CLR_YLED()                  do { PORTE &= ~BM(YLED); } while (0)
                                    
// Set LED                          
#define SET_GLED()                  do { PORTB |= BM(GLED); } while (0)
#define SET_OLED()                  do { PORTB |= BM(OLED); } while (0)
#define SET_RLED()                  do { PORTE |= BM(RLED); } while (0)
#define SET_YLED()                  do { PORTE |= BM(YLED); } while (0)
                                    
// Toggle LED                       
#define TOGGLE_GLED()               do { PORTB ^= BM(GLED); } while (0)
#define TOGGLE_OLED()               do { PORTB ^= BM(OLED); } while (0)
#define TOGGLE_RLED()               do { PORTE ^= BM(RLED); } while (0)
#define TOGGLE_YLED()               do { PORTE ^= BM(YLED); } while (0)
                         
// Get LED               
#define GET_GLED()                  (PORTB & BM(GLED))
#define GET_OLED()                  (PORTB & BM(OLED))
#define GET_RLED()                  (PORTE & BM(RLED))
#define GET_YLED()                  (PORTE & BM(YLED))

// Sets all leds simultaneously to display a 4-bit binary code
#define GLED_BM                     0x01
#define OLED_BM                     0x02
#define RLED_BM                     0x04
#define YLED_BM                     0x08

#define SET_LED_MASK(n) \
    do { \
        if (n & GLED_BM) SET_GLED(); else CLR_GLED(); \
        if (n & OLED_BM) SET_OLED(); else CLR_OLED(); \
        if (n & RLED_BM) SET_RLED(); else CLR_RLED(); \
        if (n & YLED_BM) SET_YLED(); else CLR_YLED(); \
    } while (0)

//-------------------------------------------------------------------------------------------------------
// Port setup macros

// CC2420 interface config
#define MAC_PORT_INIT() \
    do { \
        SFIOR |= BM(PUD); \
        DDRB  |= BM(RESET_N) | BM(VREG_EN) | BM(MOSI) | BM(SCK) | BM(CSN); \
        PORTB |= BM(CSN); \
    } while (0)
    
// CC2420DB peripheral interface config
#define DB_PERIPHERAL_PORT_INIT() \
    do { \
        DDRB  |= BM(OLED) | BM(GLED); \
       	DDRD  |= BM(UART1_CTS); \
        PORTD |= BM(UART1_RTS) | BM(UART1_CTS);\
        DDRE  |= BM(RLED) | BM(YLED); \
        PORTE &= ~(BM(RLED) | BM(YLED)); \
    } while (0)
    
// Enables SPI, selects "master", clock rate FCK / 2, and SPI mode 0
#define SPI_INIT() \
    do { \
        SPCR = BM(SPE) | BM(MSTR); \
        SPSR = BM(SPI2X); \
    } while (0)
/*******************************************************************************************************
 *******************************************************************************************************
 **************************            AVR<->CC2420 SPI INTERFACE             **************************
 *******************************************************************************************************
 *******************************************************************************************************/

//-------------------------------------------------------------------------------------------------------
// Initialization

// csyoon 060104 disable while setting baud rate
// set baud rate 8MHZ 38400 0x000c
// 8 bits, no parity rx en, tx en, clock rate normal
#define SERIAL_PORT_INIT() \
    do { \
		UCSR1B	= 0x00; \
		UBRR1L	= 0x0c; \
		UBRR1H	= 0x00; \
        UCSR1C  = 0x06; \
		UCSR1B  = 0x18; \
		UCSR1A  = 0x00; \
    } while (0)
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
// FAST SPI: Low level functions
//      x = value (BYTE or WORD)
//      p = pointer to the byte array to operate on
//      c = the byte count
//

// Enables/disables the SPI interface
#define SPI_ENABLE()                do { PORTB &= ~BM(CSN); } while (0)

#define SPI_DISABLE()               do { PORTB |= BM(CSN); } while (0)

/* Stubs for debug
*/
#ifdef ZIGBEE_STUB 
    #define FASTSPI_WAIT()
#else
    #define FASTSPI_WAIT() \
        do { \
            while (!(SPSR & BM(SPIF))); \
        } while (0) 
#endif
    
#define FASTSPI_TX(x) \
    do { \
        SPDR = x; \
        FASTSPI_WAIT(); \
    } while (0)

#define FASTSPI_RX(x) \
    do { \
        SPDR = 0; \
        FASTSPI_WAIT(); \
        x = SPDR; \
    } while (0)

#define FASTSPI_RX_GARBAGE() \
    do { \
        SPDR = 0; \
        FASTSPI_WAIT(); \
    } while (0)

#define FASTSPI_TX_WORD_LE(x) \
    do { \
        FASTSPI_TX(x); \
        FASTSPI_TX((x) >> 8); \
    } while (0)
    
#define FASTSPI_TX_WORD(x) \
    do { \
        FASTSPI_TX(((WORD)(x)) >> 8); \
        FASTSPI_TX((BYTE)(x)); \
    } while (0)
    
#define FASTSPI_TX_MANY(p,c) \
    do { \
        for (UINT8 spiCnt = 0; spiCnt < (c); spiCnt++) { \
            FASTSPI_TX(((BYTE*)(p))[spiCnt]); \
        } \
    } while (0)
        
#define FASTSPI_RX_WORD_LE(x) \
    do { \
        SPDR = 0; \
        FASTSPI_WAIT(); \
        x = SPDR; \
        SPDR = 0; \
        FASTSPI_WAIT(); \
        x |= SPDR << 8; \
    } while (0)

#define FASTSPI_RX_WORD(x) \
    do { \
        SPDR = 0; \
        FASTSPI_WAIT(); \
        x = SPDR << 8; \
        SPDR = 0; \
        FASTSPI_WAIT(); \
        x |= SPDR; \
    } while (0)
    
#define FASTSPI_RX_MANY(p,c) \
    do { \
        for (UINT8 spiCnt = 0; spiCnt < (c); spiCnt++) { \
            FASTSPI_RX((p)[spiCnt]); \
        } \
    } while (0)
        
// Register address:
#define FASTSPI_TX_ADDR(a) \
    do { \
        SPDR = a; \
        FASTSPI_WAIT(); \
    } while (0)

// Register address:
#define FASTSPI_RX_ADDR(a) \
    do { \
        SPDR = (a) | 0x40; \
        FASTSPI_WAIT(); \
    } while (0)
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
//  FAST SPI: Register access
//      s = command strobe
//      a = register address
//      v = register value

#define FASTSPI_STROBE(s) \
    do { \
        SPI_ENABLE(); \
        FASTSPI_TX_ADDR(s); \
        SPI_DISABLE(); \
    } while (0)

#define FASTSPI_SETREG(a,v) \
    do { \
        SPI_ENABLE(); \
        FASTSPI_TX_ADDR(a); \
        FASTSPI_TX((BYTE) ((v) >> 8)); \
        FASTSPI_TX((BYTE) (v)); \
        SPI_DISABLE(); \
    } while (0)

#define FASTSPI_GETREG(a,v) \
    do { \
        SPI_ENABLE(); \
        FASTSPI_RX_ADDR(a); \
        FASTSPI_RX_WORD(v); \
        SPI_DISABLE(); \
    } while (0)

// Updates the SPI status byte
#define FASTSPI_UPD_STATUS(s) \
    do { \
        SPI_ENABLE(); \
        FASTSPI_TX_ADDR(CC2420_SNOP); \
        s = SPDR; \
        SPI_DISABLE(); \
    } while (0)
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
//  FAST SPI: FIFO access
//      p = pointer to the byte array to be read/written
//      c = the number of bytes to read/write
//      b = single data byte

#define FASTSPI_WRITE_FIFO(p,c) \
    do { \
        SPI_ENABLE(); \
        FASTSPI_TX_ADDR(CC2420_TXFIFO); \
        for (UINT8 spiCnt = 0; spiCnt < (c); spiCnt++) { \
            FASTSPI_TX(((BYTE*)(p))[spiCnt]); \
        } \
        SPI_DISABLE(); \
    } while (0)

#define FASTSPI_READ_FIFO(p,c) \
    do { \
        SPI_ENABLE(); \
        FASTSPI_RX_ADDR(CC2420_RXFIFO); \
        for (UINT8 spiCnt = 0; spiCnt < (c); spiCnt++) { \
            while (!FIFO_IS_ACTIVE); \
            FASTSPI_RX(((BYTE*)(p))[spiCnt]); \
        } \
        SPI_DISABLE(); \
    } while (0)

#define FASTSPI_READ_FIFO_BYTE(b) \
    do { \
        SPI_ENABLE(); \
        FASTSPI_RX_ADDR(CC2420_RXFIFO); \
        FASTSPI_RX(b); \
        SPI_DISABLE(); \
    } while (0)

#define FASTSPI_READ_FIFO_NO_WAIT(p,c) \
    do { \
        SPI_ENABLE(); \
        FASTSPI_RX_ADDR(CC2420_RXFIFO); \
        for (UINT8 spiCnt = 0; spiCnt < (c); spiCnt++) { \
            FASTSPI_RX(((BYTE*)(p))[spiCnt]); \
        } \
        SPI_DISABLE(); \
    } while (0)

#define FASTSPI_READ_FIFO_GARBAGE(c) \
    do { \
        SPI_ENABLE(); \
        FASTSPI_RX_ADDR(CC2420_RXFIFO); \
        for (UINT8 spiCnt = 0; spiCnt < (c); spiCnt++) { \
            FASTSPI_RX_GARBAGE(); \
        } \
        SPI_DISABLE(); \
    } while (0)
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
//  FAST SPI: CC2420 RAM access (big or little-endian order)
//      p = pointer to the variable to be written
//      a = the CC2420 RAM address
//      c = the number of bytes to write
//      n = counter variable which is used in for/while loops (UINT8)
//
//  Example of usage:
//      UINT8 n;
//      UINT16 shortAddress = 0xBEEF;
//      FASTSPI_WRITE_RAM_LE(&shortAddress, CC2420RAM_SHORTADDR, 2);

#define FASTSPI_WRITE_RAM_LE(p,a,c,n) \
    do { \
        SPI_ENABLE(); \
        FASTSPI_TX(0x80 | (a & 0x7F)); \
        FASTSPI_TX((a >> 1) & 0xC0); \
        for (n = 0; n < (c); n++) { \
            FASTSPI_TX(((BYTE*)(p))[n]); \
        } \
        SPI_DISABLE(); \
    } while (0)

#define FASTSPI_READ_RAM_LE(p,a,c,n) \
    do { \
        SPI_ENABLE(); \
        FASTSPI_TX(0x80 | (a & 0x7F)); \
        FASTSPI_TX(((a >> 1) & 0xC0) | 0x20); \
        for (n = 0; n < (c); n++) { \
            FASTSPI_RX(((BYTE*)(p))[n]); \
        } \
        SPI_DISABLE(); \
    } while (0)
    
#define FASTSPI_WRITE_RAM(p,a,c,n) \
    do { \
        SPI_ENABLE(); \
        FASTSPI_TX(0x80 | (a & 0x7F)); \
        FASTSPI_TX((a >> 1) & 0xC0); \
        n = c; \
        do { \
            FASTSPI_TX(((BYTE*)(p))[--n]); \
        } while (n); \
        SPI_DISABLE(); \
    } while (0)

#define FASTSPI_READ_RAM(p,a,c,n) \
    do { \
        SPI_ENABLE(); \
        FASTSPI_TX(0x80 | (a & 0x7F)); \
        FASTSPI_TX(((a >> 1) & 0xC0) | 0x20); \
        n = c; \
        do { \
            FASTSPI_RX(((BYTE*)(p))[--n]); \
        } while (n); \
        SPI_DISABLE(); \
    } while (0)
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
// Other useful SPI macros
#define FASTSPI_RESET_CC2420() \
    do { \
        FASTSPI_SETREG(CC2420_MAIN, 0x0000); \
        FASTSPI_SETREG(CC2420_MAIN, 0xF800); \
    } while (0)
//-------------------------------------------------------------------------------------------------------


/*******************************************************************************************************
 *******************************************************************************************************
 **************************                 CC2420 PIN ACCESS                 **************************
 *******************************************************************************************************
 *******************************************************************************************************/


//-------------------------------------------------------------------------------------------------------
// CC2420 pin access

// Pin status
#define RESET_IS_1()                (PINB & BM(RESET_N))
#define VREG_IS_1()                 (PINB & BM(VREG_EN))
#define FIFOP_IS_1()                (PIND & BM(FIFOP))
#define SFD_IS_1()                  (PIND & BM(SFD))
#define FIFO_IS_1()                 (PIND & BM(FIFO))
#define CCA_IS_1()                  (PIND & BM(CCA)) 

#define FIFO_IS_ACTIVE()            FIFO_IS_1()
#define RESET_IS_ACTIVE()           !RESET_IS_1()
#define VREG_IS_ACTIVE()            VREG_IS_1()
#define FIFOP_IS_ACTIVE()           FIFOP_IS_1()
#define SFD_IS_ACTIVE()             SFD_IS_1()
#define CCA_IS_ACTIVE()             CCA_IS_1()

// CC2420 reset_n pin
#define SET_RESET_ACTIVE()          do { PORTB &= ~BM(RESET_N); } while (0)
#define SET_RESET_INACTIVE()        do { PORTB |= BM(RESET_N); } while (0)

// CC2420 voltage regulator enable pin
#define SET_VREG_ACTIVE()           do { PORTB |= BM(VREG_EN); } while (0)
#define SET_VREG_INACTIVE()         do { PORTB &= ~BM(VREG_EN); } while (0)
//-------------------------------------------------------------------------------------------------------



/*******************************************************************************************************
 *******************************************************************************************************
 **************************                   SERIAL PORTS                    **************************
 *******************************************************************************************************
 *******************************************************************************************************/


//-------------------------------------------------------------------------------------------------------
// UART hardware flow control
#define CLR_RTS()                   do { PORTD &= ~BM(UART1_RTS); } while (0)
#define SET_RTS()                   do { PORTD |= BM(UART1_RTS); } while (0)

#define SET_CTS()                   do { PORTD |= BM(UART1_CTS); } while (0)
#define CLR_CTS()                   do { PORTD &= ~BM(UART1_CTS); } while (0)
//-------------------------------------------------------------------------------------------------------




/*******************************************************************************************************
 *******************************************************************************************************
 **************************               EXTERNAL INTERRUPTS                 **************************
 *******************************************************************************************************
 *******************************************************************************************************/


//-------------------------------------------------------------------------------------------------------
// Rising edge trigger for external interrupt 0 (FIFOP)
#define FIFOP_INT_INIT()            do { EICRA |= 0x03; CLEAR_FIFOP_INT(); } while (0)

// FIFOP on external interrupt 0
#define ENABLE_FIFOP_INT()          do { EIMSK |= 0x01; } while (0)
#define DISABLE_FIFOP_INT()         do { EIMSK &= ~0x01; } while (0)
#define CLEAR_FIFOP_INT()           do { EIFR = 0x01; } while (0)
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
// SFD interrupt on timer 1 capture pin
#define ENABLE_SFD_CAPTURE_INT()    do { TIMSK |= BM(TICIE1); } while (0)
#define DISABLE_SFD_CAPTURE_INT()   do { TIMSK &= ~BM(TICIE1); } while (0)
#define CLEAR_SFD_CAPTURE_INT()     do { TIFR = BM(ICF1); } while (0)
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
// Falling edge trigger for external interrupt 6 (joystick)
#define JOYSTICK_INT_INIT()         do { EICRB |= 0x20; } while (0)

// Joystick on external interrupt 6
#define ENABLE_JOYSTICK_INT()       do { EIMSK |= 0x40; } while (0)
#define DISABLE_JOYSTICK_INT()      do { EIMSK &= ~0x40; } while (0)
#define CLEAR_JOYSTICK_INT()        do { EIFR = 0x40; } while (0)

/*******************************************************************************************************
 *******************************************************************************************************
 **************************                    INTERRUPTS                     **************************
 *******************************************************************************************************
 *******************************************************************************************************/


//-------------------------------------------------------------------------------------------------------
// General
#define ENABLE_GLOBAL_INT()         do { asm ("sei\n\t" ::); } while (0)
#define DISABLE_GLOBAL_INT()        do { asm ("cli\n\t" ::); } while (0)
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
// UART1 interrupts
#define ENABLE_UART1_INT()          do { UCSR1B |= (BM(UDRIE1) | BM(RXCIE1)); } while (0)
#define DISABLE_UART1_INT()         do { UCSR1B &= ~(BM(UDRIE1) | BM(RXCIE1)); } while (0) 

#define ENABLE_UART1_TX_INT()       do { UCSR1B |= BM(UDRIE1); } while (0)
#define DISABLE_UART1_TX_INT()      do { UCSR1B &= ~BM(UDRIE1); } while (0) 
#define IS_UART1_TX_INT_ENABLED()   (UCSR1B & BM(UDRIE1)) 
#define CLEAR_UART1_TX_INT()        do { UCSR1A &= ~BM(UDRE1); } while (0) /* TBD: Is this correct? */

#define ENABLE_UART1_RX_INT()       do { UCSR1B |= BM(RXCIE1); } while (0)
#define DISABLE_UART1_RX_INT()      do { UCSR1B &= ~BM(RXCIE1); } while (0) 
#define IS_UART1_RX_INT_ENABLED()   (UCSR1B & BM(RXCIE1)) 
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
// Timer/Counter 0 interrupts

// Compare match interrupt
#define ENABLE_T0_COMPARE_INT()     do { TIMSK |= BM(OCIE0); } while (0)
#define DISABLE_T0_COMPARE_INT()    do { TIMSK &= ~BM(OCIE0); } while (0)
#define CLEAR_T0_COMPARE_INT()      do { TIFR = BM(OCF0); } while (0)
#define GET_T0_COMPARE_INT_FLAG()   (TIFR & BM(OCF0))

// Overflow interrupt
#define ENABLE_T0_OVERFLOW_INT()    do { TIMSK |= BM(TOIE1); } while (0)
#define DISABLE_T0_OVERFLOW_INT()   do { TIMSK &= ~BM(TOIE1); } while (0)
#define CLEAR_T0_OVERFLOW_INT()     do { TIFR = BM(TOV0)); } while (0)
#define GET_T0_OVERFLOW_INT_FLAG()  (TIFR & BM(TOV0))
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
// Timer/Counter 1 interrupts

// Compare match interrupt A
#define ENABLE_T1_COMPA_INT()       do { TIMSK |= BM(OCIE1A); } while (0)
#define DISABLE_T1_COMPA_INT()      do { TIMSK &= ~BM(OCIE1A); } while (0)
#define CLEAR_T1_COMPA_INT()        do { TIFR = BM(OCF1A); } while (0)

// Compare match interrupt C
#define ENABLE_T1_COMPC_INT()       do { ETIMSK |= BM(OCIE1C); } while (0)
#define DISABLE_T1_COMPC_INT()      do { ETIMSK &= ~BM(OCIE1C); } while (0)
#define CLEAR_T1_COMPC_INT()        do { ETIFR = BM(OCF1C); } while (0)

// Capture interrupt
#define ENABLE_T1_CAPTURE_INT()     do { TIMSK |= BM(TICIE1); } while (0)
#define DISABLE_T1_CAPTURE_INT()    do { TIMSK &= ~BM(TICIE1); } while (0)
#define CLEAR_T1_CAPTURE_INT()      do { TIFR = BM(ICF1); } while (0)
//-------------------------------------------------------------------------------------------------------




/*******************************************************************************************************
 *******************************************************************************************************
 **************************                         ADC                       **************************
 *******************************************************************************************************
 *******************************************************************************************************/


//-------------------------------------------------------------------------------------------------------
// ADC initialization
#define ADC_INIT() \
    do { \
        ADCSRA = BM(ADPS0) | BM(ADPS1) | BM(ADFR); \
        ADMUX = BM(REFS0); \
    } while (0)

// Selects which ADC channel to use. The channels (0-3) are defined in the development board definition
// files, e.g. hal_cc2420db.h, as ADC_INPUT_...
#define ADC_SET_CHANNEL(channel) do { ADMUX = (ADMUX & ~0x1F) | (channel); } while (0)

// Enables/disables the ADC
#define ADC_ENABLE() do { ADCSRA |= BM(ADEN); } while (0)
#define ADC_DISABLE() do { ADCSRA &= ~BM(ADEN); } while (0)
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
// ADC sampling

// Macro for taking a single sample in single-conversion mode (not required in continuous mode)
#define ADC_SAMPLE_SINGLE() \
    do { \
        ADCSRA |= BM(ADSC); \
        while (!(ADCSRA & 0x10)); \
    } while(0)

// Macros for obtaining the latest sample value
#define ADC_GET_SAMPLE_10(x) \
    do { \
        x =  ADCL; \
        x |= ADCH << 8; \
    } while (0)

#define ADC_GET_SAMPLE_8(x) \
    do { \
        x = ((UINT8) ADCL) >> 2; \
        x |= ((INT8) ADCH) << 6; \
    } while (0)
//-------------------------------------------------------------------------------------------------------




/*******************************************************************************************************
 *******************************************************************************************************
 **************************               PULSE WIDTH MODULATOR               **************************
 *******************************************************************************************************
 *******************************************************************************************************/


//-------------------------------------------------------------------------------------------------------
// Pulse width modulator (PWM) using timer 0 (8 bits)
//
// Example of usage:
//     // Initialize
//     PWM_INIT(TIMER0_CLK_DIV1);
//
//     // Increase level gradually (over approx 2.5 seconds)
//     for (UINT8 n; n < 255; n++) {
//         PWM0_SET_DUTY_CYCLE(n);
//         halWait(10000);
//     }

// Initialization
#define PWM0_INIT(period) \
    do { \
        OCR0 = 0; \
        TCCR0 = BM(WGM00) | BM(COM01) | BM(COM00); \
        PWM0_SET_PERIOD(period); \
    } while(0)

// Sets the PWM period
#define PWM0_SET_PERIOD(period) do { TCCR0 = ((TCCR0 & ~0x07) | (period)); } while (0)

// Period definitions for use with the PWM0_INIT and PWM0_SET_PERIOD macros
#define TIMER0_CLK_STOP         0x00 /* Stop mode (the timer is not counting)    */
#define TIMER0_CLK_DIV1         0x01 /* Total period = Clock freq / 256          */
#define TIMER0_CLK_DIV8         0x02 /* Total period = Clock freq / (256 * 8)    */
#define TIMER0_CLK_DIV32        0x03 /* Total period = Clock freq / (256 * 32)   */
#define TIMER0_CLK_DIV64        0x04 /* Total period = Clock freq / (256 * 64)   */
#define TIMER0_CLK_DIV128       0x05 /* Total period = Clock freq / (256 * 128)  */
#define TIMER0_CLK_DIV256       0x06 /* Total period = Clock freq / (256 * 256)  */
#define TIMER0_CLK_DIV1024      0x07 /* Total period = Clock freq / (256 * 1024) */

// Sets the PWM duty cycle
#define PWM0_SET_DUTY_CYCLE(dutyCycle) do { OCR0 = (dutyCycle); } while (0)
//-------------------------------------------------------------------------------------------------------




/*******************************************************************************************************
 *******************************************************************************************************
 **************************                      TIMER 0                      **************************
 *******************************************************************************************************
 *******************************************************************************************************/


//-------------------------------------------------------------------------------------------------------
// Timer 0 (8 bits)

// Initialization
#define TIMER0_INIT(prescaler, timeout) \
    do { \
        TCNT0 = 0; \
        OCR0 = timeout - 1; \
        TCCR0 = prescaler; \
    } while(0)

// Prescaler
// The prescaler constants, called TIMER0_CLK_DIV???, are defined in the above PWM section...
#define TIMER0_SET_PRESCALER(prescaler) do { TCCR0 = prescaler; } while (0)

// Operations
#define TIMER0_RESET_VALUE()            do { TCNT0 = 0; } while (0)
#define TIMER0_SET_VALUE(value)         do { TCNT0 = value; } while (0)
#define TIMER0_SET_COMPARE_VALUE(value) do { OCR0 = value; } while (0)
//-------------------------------------------------------------------------------------------------------




/*******************************************************************************************************
 *******************************************************************************************************
 **************************                      TIMER 1                      **************************
 *******************************************************************************************************
 *******************************************************************************************************/


//-------------------------------------------------------------------------------------------------------
// Timer 1 (16 bits)

// Initialize timer 1 for interrupt at regular intervals, controlled by the compare A value, and the
// precaler value
#define TIMER1_INIT(options)            do { TCCR1B = (TCCR1B & 0x07) | options | BM(WGM12); } while (0)

// Options for TIMER1_INIT
#define TIMER1_CAPTURE_NOISE_CANCELLER  BM(ICNC1)
#define TIMER1_CAPTURE_ON_POSEDGE       BM(ICES1)
#define TIMER1_CAPTURE_ON_NEGEDGE       0

// Start/stop
#define TIMER1_START(prescaler)         do { TCCR1B = (TCCR1B & ~0x07) | prescaler; } while (0)
#define TIMER1_STOP()                   do { TCCR1B &= ~0x07; } while (0)

// Prescaler definitions to be used with TIMER1_START(...)
#define TIMER1_CLK_STOP                 0x00 /* Stop mode (the timer is not counting)    */
#define TIMER1_CLK_DIV1                 0x01 /* Total period = Clock freq / 256          */
#define TIMER1_CLK_DIV8                 0x02 /* Total period = Clock freq / (256 * 8)    */
#define TIMER1_CLK_DIV64                0x03 /* Total period = Clock freq / (256 * 32)   */
#define TIMER1_CLK_DIV256               0x04 /* Total period = Clock freq / (256 * 64)   */
#define TIMER1_CLK_DIV1024              0x05 /* Total period = Clock freq / (256 * 128)  */
#define TIMER1_CLK_T1_NEGEDGE           0x06 /* Total period = Clock freq / (256 * 256)  */
#define TIMER1_CLK_T1_POSEDGE           0x07 /* Total period = Clock freq / (256 * 1024) */

// Compare value modification
#define TIMER1_SET_COMPA_VALUE(value)   do { OCR1A = value; } while (0)
#define TIMER1_SET_COMPB_VALUE(value)   do { OCR1B = value; } while (0)
#define TIMER1_SET_COMPC_VALUE(value)   do { OCR1C = value; } while (0)
//-------------------------------------------------------------------------------------------------------


// Waits for the next backoff slot boandary (global interrupts should be turned off!)
#define WAIT_FOR_BOUNDARY()             while (!(TIFR & BM(OCF1B)))


/*******************************************************************************************************
 *******************************************************************************************************
 **************************               SERIAL PORT (UART1)                 **************************
 *******************************************************************************************************
 *******************************************************************************************************/


//-------------------------------------------------------------------------------------------------------
//  INIT_UART1(baudRate,options)
//
//  DESCRIPTION:
//      A macro which does all the initialization necessary to communicate on UART 1. The UART is
//      configured according to options (defined below). Note that this macro does not call
//      ENABLE_UART1().
//
//  ARGUMENTS:
//      baudRate
//          One of the UART_BAUDRATE_... constants defined below
//      options
//          One or more of the UART_OPT constants defined below. The value 0 gives one stop bit, no
//          parity and 5 bits per char.
//-------------------------------------------------------------------------------------------------------
#define INIT_UART1(baudRate,options) \
    do { \
        UBRR1H = (baudRate) >> 8; \
        UBRR1L = (baudRate); \
        UCSR1C = (BYTE) options; \
        if (options > 0xFF) { \
            UCSR1B |= 0x04; \
        } else { \
            UCSR1B &= ~0x04; \
        } \
        UCSR1A |= BM(U2X1); \
    } while (0)

// Baud rate codes for use with the INIT_UART1 macro
#define UART_BAUDRATE_2K4           416
#define UART_BAUDRATE_4K8           207
#define UART_BAUDRATE_9K6           103
#define UART_BAUDRATE_14K4          68
#define UART_BAUDRATE_19K2          51
#define UART_BAUDRATE_28K8          34
#define UART_BAUDRATE_38K4          25
#define UART_BAUDRATE_57K6          16
#define UART_BAUDRATE_76K8          12
#define UART_BAUDRATE_115K2         8
#define UART_BAUDRATE_230K4         3
#define UART_BAUDRATE_250K          3
#define UART_BAUDRATE_500K          1   
#define UART_BAUDRATE_1M            0
    
// Options for use with the INIT_UART1 macro
#define UART_OPT_ONE_STOP_BIT       0
#define UART_OPT_TWO_STOP_BITS      0x08
#define UART_OPT_NO_PARITY          0
#define UART_OPT_EVEN_PARITY        0x20
#define UART_OPT_ODD_PARITY         0x30
#define UART_OPT_5_BITS_PER_CHAR    0
#define UART_OPT_6_BITS_PER_CHAR    0x02
#define UART_OPT_7_BITS_PER_CHAR    0x04
#define UART_OPT_8_BITS_PER_CHAR    0x06
#define UART_OPT_9_BITS_PER_CHAR    0x0406
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
// Enable/disable macros

// Enable/disable UART1
#define ENABLE_UART1()              (UCSR1B |= (BM(RXEN1) | BM(TXEN1))) 
#define DISABLE_UART1()             (UCSR1B &= ~(BM(RXEN1) | BM(TXEN1)))
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
// Macros which are helful when transmitting and receiving data over the serial interface.
//
// Example of usage:
//      UART1_SEND(pData[0]);
//      for (i = 1; i < len; i++) {
//          UART1_WAIT_AND_SEND(pData[i]);
//      }

#define UART1_WAIT()                do { while (!(UCSR1A & BM(UDRE1))); CLEAR_UART1_TX_INT(); } while (0)
#define UART1_SEND(x)               do { UDR1 = (x); } while (0)
#define UART1_WAIT_AND_SEND(x)      do { UART1_WAIT(); UART1_SEND(x); } while (0)
#define UART1_RECEIVE()             UDR1
#define UART1_WAIT_AND_RECEIVE(x)   do { UDR = 0; UART1_WAIT(); UART1_RX(x); } while (0)
//-------------------------------------------------------------------------------------------------------






#endif //! HAL_H
