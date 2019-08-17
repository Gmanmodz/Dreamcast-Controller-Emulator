/* 
 * File:   main.cpp
 * Author: Gunnar Turnquist
 *
 * Created on May 21, 2019, 10:27 AM
 */

#include <cstdlib>
#include <xc.h>
#include <sys/attribs.h>
#include <proc/p32mz1024eff064.h>
#include "PIC_CONFIG.h"
#include "maple_protocol.h"
#include "ADC.h"
#include "controller_poll.h"
#include "I2C.h"
#include "LM49450.h"
#include "MCP4706.h"
#include "SPI.h"
#include "SSD1331.h"

#define CS1 LATEbits.LATE7
#define CS2 LATGbits.LATG9

using namespace std;

void PPS_lock(){
    /* Unlock sequence to enable writing to IOLOCK nit */
    SYSKEY = 0x00000000;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    
    CFGCONbits.IOLOCK=1;
    /* Lock */
    SYSKEY = 0x33333333;
}

void PPS_unlock(){
    /* Unlock sequence to enable writing to IOLOCK nit */
    SYSKEY = 0x00000000;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    
    CFGCONbits.IOLOCK=0; // clears the lock bit to unlock it
}

int main(int argc, char** argv) {
    
    //pre-fetch module
    PRECONbits.PREFEN = 3;
    PRECONbits.PFMWS = 2; 

    //set ports to input/output
    TRISB = 0xFFFF; 
    TRISC = 0xFFFF;       
    TRISD = 0xFFFF;
    TRISE = 0xFF;
    TRISG = 0xFFFF;
    TRISF = 0xFF;
    
    //set ports to digital/analog IO
    ANSELB = 0;
    ANSELE = 0;      
    ANSELG = 0;
    
    ANSELBbits.ANSB9 = 1;   //SX
    ANSELBbits.ANSB10 = 1;  //SY
    
    //set weak pull-ups
    CNPUCbits.CNPUC15 = 1;  //DU
    CNPUBbits.CNPUB15 = 1;  //DD
    CNPUCbits.CNPUC12 = 1;  //DL
    CNPUBbits.CNPUB14 = 1;  //DR
    CNPUEbits.CNPUE3 = 1;   //A
    CNPUEbits.CNPUE2 = 1;   //B
    CNPUFbits.CNPUF1 = 1;   //X
    CNPUEbits.CNPUE1 = 1;   //Y
    CNPUEbits.CNPUE4 = 1;   //S
    CNPUEbits.CNPUE0 = 1;   //R
    CNPUFbits.CNPUF4 = 1;   //L
    CNPUDbits.CNPUD2 = 1;   //plus
    CNPUDbits.CNPUD3 = 1;   //minus
    
    //disable comparator
    CVRCONbits.ON = 0;
    CVRCONbits.CVROE = 0;

    init_adc(); //initialize ADC module   
    
    //SDA/SCL on pins 43 and 44 -- no PPS needed
    I2C_init(100000);   //initialize I2C1 with 100KHz SCL
    
    LM49450_init();     //initialize LM49450
    MCP4706_init();     //initialize MCP4706 DAC
    
    //set initial volume
    LM49450_write(0x08, 15); //speaker volume
    LM49450_write(0x07, 15); //headphone volume 
    
    //set initial dac Vout
    MCP4706_write(0b00000000, 255);   //Vout = (val / 256) * 3.3V
    
    /*
    //initialize SPI1
    PPS_unlock();
    //SDO1 / MOSI on RD11
    RPD11Rbits.RPD11R  = 0b0101;
    //SDI / MISO on RC14
    SDI1Rbits.SDI1R = 0b0111;
    PPS_lock();
    
    SPI_init();
    
    //initialize SPI OLED
    TRISGbits.TRISG9 = 0;   //set CCS2 to output
    CS2 = 0; 
    SSD1331_init();
    CS2 = 1;
    */
    
    //initialize FRAM
    //TRISEbits.TRISE7 = 0;
    //CS1 = 1;
    
    //initialize Stick LUT
    LUT_init();
     
    //set up maple bus
    maple_init();
    
    while(1) {
        
        controller_poll();
        
        check_vol_buttons();
        
    }
    
    return 0;
}