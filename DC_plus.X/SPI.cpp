/* 
 * File:    SPI.cpp
 * Author: Gunnar Turnquist
 *
 * Created on June 7, 2019, 10:27 AM
 */

#include <xc.h>

//SPI1
void SPI_init()
{
    SPI1CONbits.ON = 0; // Turn off SPI1 before configuring
    SPI1CONbits.MSTEN = 1; // Enable Master mode
    
    SPI1CONbits.MSSEN = 0;  //Slave Select Disabled
    
    SPI1CONbits.CKP = 1; // Clock signal is active low, idle state is high
    SPI1CONbits.CKE = 0; // Data is shifted out/in on transition from idle (high) state to active (low) state
    SPI1CONbits.SMP = 1; // Input data is sampled at the end of the clock signal
    SPI1CONbits.MODE16 = 0; // Do not use 16-bit mode
    SPI1CONbits.MODE32 = 0; // Do not use 32-bit mode (combines with the above line to activate 8-bit mode)
    //SCK FREQ = FPB / (2 * (SPI1BRG + 1)
    SPI1BRG = 0x3E7; // Set Baud Rate Generator to 999. SCK = 100K Hz. 
    SPI1CONbits.ENHBUF = 0; // Disables Enhanced Buffer mode 
    SPI1CONbits.ON = 1; // Configuration is done, turn on SPI1 peripheral
}

char SPI_send(char data)
{
    SPI1BUF = data; // Send **data** to SPI1BUF.
    while (SPI1STATbits.SPIRBE); // Which the receive buffer is empty, loop
    return (char)SPI1BUF; // Return whatever is in SPI1BUF
}

void sendCommand(char data) {
    SPI1BUF = data; // Send **data** to SPI1BUF.
    while (SPI1STATbits.SPIRBE); // Which the receive buffer is empty, loop
}