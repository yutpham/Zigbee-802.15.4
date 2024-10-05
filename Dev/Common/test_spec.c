//-----------------------------------------------------------------------
//  void main(void)
//
//  DESCRIPTION:
//      Startup sequence and main loop.
//-----------------------------------------------------------------------
void main(void) 
{    
    /* Test*/       
    UINT16      newFreq,oldFreq,iocfg0, iocfg1 ,panId;
    UINT8       spiStatus,n;	    
    /*Initialize*/
    MAC_PORT_INIT();
    SPI_INIT(); 
    FIFOP_INT_INIT();                                 /*FIFOP interrupt initialization*/
    DB_PERIPHERAL_PORT_INIT();                        /* Initialize peripheral ports */           

    /* Initialize the serial port*/
	INIT_UART1(UART_BAUDRATE_2K4,UART_OPT_8_BITS_PER_CHAR);   
    ENABLE_UART1();
    
    /*Send test data to UART1*/
    n = 'a';
    while(!TRUE)
    {
        while (!(UCSR1A & BM(UDRE1)));
            CLEAR_UART1_TX_INT();
        UDR1 = n++;
    }

    /* Initialize the C2420*/
    SET_RESET_ACTIVE(); 
    SET_VREG_ACTIVE();        
    wait(600);

    /*Turn on Osci*/
    SET_RESET_INACTIVE();       
    FASTSPI_STROBE(CC2420_SXOSCON);  
    /* Wait for oscillator become stable */
    spiStatus = 0;
     do {
         DISABLE_GLOBAL_INT();
         FASTSPI_UPD_STATUS(spiStatus);
         ENABLE_GLOBAL_INT();
     } while (!(spiStatus & CC2420_XOSC16M_STABLE_BM));

    DISABLE_GLOBAL_INT();

    /*Set PAN ID*/
    panId = 0xFFFF;
    FASTSPI_WRITE_RAM_LE(&panId, CC2420RAM_PANID, 2, n);

     // Flush the RX FIFO twice 
    FASTSPI_STROBE(CC2420_SFLUSHRX);
    FASTSPI_STROBE(CC2420_SFLUSHRX);                

    // Set the BCN_ACCEPT bit 
    FASTSPI_GETREG(CC2420_IOCFG0, iocfg0);
    iocfg0 |= 0x0800;
    FASTSPI_SETREG(CC2420_IOCFG0, iocfg0);

    ENABLE_GLOBAL_INT();

    /* Set channel: channel 19 */
    // Derive frequency programming from the given channel number
    newFreq = (UINT16) (19 - 11);             // Subtract the base channel:  
    newFreq = newFreq + (newFreq << 2);       // Multiply with 5, which is the channel spacing
    newFreq = newFreq + 357 + 0x4000;         // 357 is 2405-2048, 0x4000 is LOCK_THR = 1 
    // update the frequency register         
    FASTSPI_SETREG(CC2420_FSCTRL, newFreq);

    // Change MDM register         
    iocfg0 = 0x00C0; /*Set TX_MODE of the MDMCTRL1 to 3: please check the value*/
    FASTSPI_SETREG(CC2420_MDMCTRL1, iocfg0);

    /*Start TX*/
    FASTSPI_STROBE(CC2420_STXON);  

    /*Wait */
     while (TRUE)
 	    wait(5000);
#endif     
} // main