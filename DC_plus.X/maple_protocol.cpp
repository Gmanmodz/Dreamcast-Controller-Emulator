/* 
 * File:   MAPLE_PROTOCOL_CPP
 * Author: Gunnar Turnquist
 *
 * Created on May 21, 2019, 10:27 AM
 */

#include <cstdlib>
#include <xc.h>
#include <sys/attribs.h>
#include <proc/p32mz1024eff064.h>
#include "maple_protocol.h"
#include "controller_poll.h"

#include "MCP4706.h"

void maple_delay(){
    // ~250ns delay
    for(char i = 0; i <= 2; i++) {
        Nop();
    }
}

void maple_sync() {
    //transmitting sync sequence
    dat1 = 0;
    for(char c = 0; c < 4; c++) {
        dat2 = 1;
        maple_delay();
        dat2 = 0;
        maple_delay();
    }
    dat2 = 1;
    maple_delay();
    dat1 = 1;
    maple_delay();
    dat2 = 0;
    maple_delay();
}

void maple_terminate() {
    //this is used to terminate communication
    dat1 = 1;
    dat2 = 1;
    maple_delay();
    dat2 = 0;
    maple_delay();
    dat1 = 0;
    maple_delay();
    dat1 = 1;
    maple_delay();
    dat1 = 0;
    maple_delay();
    dat1 = 1;
    maple_delay();
    dat2 = 1;
}

void maple_send_byte(char data) {
    
    dat1 = 1;
    
    for(char c = 7; c >= 0; c = c - 2) {
        //phase 1
        dat2 = (data >> c) & 0b1;   //bit 1 is set
        maple_delay();              //data remains stable for some time
        dat1 = 0;                   //data is clocked in
        maple_delay();
        dat2 = 1;                   //dat2 returns high if it is not already

        //phase 2
        dat1 = (data >> (c - 1)) & 0b1;  //bit 2 is set
        maple_delay();
        dat2 = 0;
        maple_delay();
        dat1 = 1;
    } 
}

void maple_send_frame(char word[], char size) {
    
    //disable interrupt
    IEC3bits.CNBIE = 0;
    
    dat1_TRIS = 0;
    dat2_TRIS = 0;
    
    maple_sync();   //send the sync sequence first
    
    for(char c = 0; c < size; c++) {
        maple_send_byte(word[c]);   //send the bytes
    }
    
    maple_terminate();  //terminate communication
    
    dat1_TRIS = 1;
    dat2_TRIS = 1;

    //enable interrupt
    IFS3bits.CNBIF = 0; //clear interrupt flag
    IEC3bits.CNBIE = 1; //enable interrupt
}

void maple_init(){
    
    __builtin_disable_interrupts();
    
    //enable open-drain
    //dat1_ODC = 1;
    //dat2_ODC = 1;
    
    //enable change notification interrupt
    CNCONBbits.ON = 1;
    CNNEBbits.CNNEB3 = 1;
    CNNEBbits.CNNEB2 = 1;
    CNCONBbits.EDGEDETECT = 1;
    
    IPC29bits.CNBIP = 7;    //interrupt priority
    IFS3bits.CNBIF = 0; //clear interrupt flag
    IEC3bits.CNBIE = 1; //enable interrupt
     
    __builtin_enable_interrupts();
    
}

char receive_buf[20] = {};     //frame buffer
char receive_bits[1025] = {};    //receiving bits buffer

int receive_bit_count = 0;
int receive_byte_count = 0;

char sync_count = 0;
char terminate_count = 0;

void __ISR_AT_VECTOR(_CHANGE_NOTICE_B_VECTOR, IPL7AUTO) CNB_Handler(void) {
    
    if(dat1_flag) {
        CNFBCLR = 0x0008;
        sync_count = 0;
    }
    if(dat2_flag) {
        CNFBCLR = 0x0004;
        sync_count++;
    }
    
    while(sync_count > 0) {
        if(dat1_flag) {
            CNFBCLR = 0x0008;
            sync_count = 0;
        }
        if(dat2_flag) {
            CNFBCLR = 0x0004;
            sync_count++;
        }

        if(sync_count == 4) {
            //sync is true, pack the bits
            maple_receive_frame();
        }
    
    }
    
    IFS3CLR = 0x00800000;   //clear interrupt flag
}

void maple_receive_frame() {
    
    IEC3bits.CNBIE = 0;
    
    //receive the transmission
    while(receive_bit_count < 40) {
        //phase 1
        while(!dat1_flag) {}
        receive_bits[receive_bit_count] = dat2_PORT;
        CNFBCLR = 0x0008;
        
        receive_bit_count++;

        //phase 2
        while(!dat2_flag) {}       
        receive_bits[receive_bit_count] = dat1_PORT;     
        CNFBCLR = 0x0004;

        receive_bit_count++;           
    }
    
    receive_bit_count = 0;
        
    //clear buffer
    for(char i = 0; i < 10; i++) {
        receive_buf[i] = 0;
    }    

    //pack the bits
    receive_byte_count = 0;
    char bit_count = 0;
        
    for(char i = 39; i >= 0; i--) {

        receive_buf[receive_byte_count] = (receive_buf[receive_byte_count] | (receive_bits[i] << bit_count));
        bit_count++;
            
        if(bit_count > 7) {
            receive_byte_count++;
            bit_count = 0;
        }

    }     
    
    //first bit is wrong for some reason, need to investigate. clearing it for now
    receive_buf[4] = receive_buf[4] & 0b01111111;

    //delay so it responds in ~60us
    for(int i = 0; i < 1500; i++) {
        Nop();
    }    
    
    //checksum check
    //if(receive_buf[0] == receive_buf[1] ^ receive_buf[2] ^ receive_buf[3] ^ receive_buf[4]) {
        //check the commands and respond
        switch(receive_buf[1]) {
            case 1:     //request device info
                maple_send_frame(device_info, 117);
                break;
            case 2:     //request extended device info
                break;
            case 3:     //reset device
                break;
            case 4:     //shutdown device
                break;
            case 9:     //get condition
                maple_send_frame(buf[buf_index], 17);
                break;
            case 10:    //get memory info
                break;
            case 11:    //block read
                break;
            case 12:    //block write
                break;
            case 14:    //set condition
                break;

            default:
                maple_send_frame(buf[buf_index], 17);

        }        
    //}   
    
        
    check_dac_combo();


    //CNFBCLR = 0x0008;   
    CNFBCLR = 0x0004;
    IFS3CLR = 0x00800000;   //clear interrupt flag
    IEC3bits.CNBIE = 1; //enable interrupt 
}

char dat1_buf = 0;
char dat2_buf = 0;
char bit_c = 7;
char word_count = 0;


/*
void maple_receive_frame2() {
    
    IEC3bits.CNBIE = 0;
    
    //CNFGCLR = 0xC0;
    
    for(char i = 0; i < 10; i++) {
        receive_buf[i] = 0;
    } 
    
    //receive the transmission
    word_count = 0;
        
    while(word_count <= receive_buf[0]) {
        while(receive_bit_count < 20) {

            //phase 1
            while(!dat1_flag) {}
            dat2_buf = dat2_PORT;
            receive_buf[receive_byte_count] = receive_buf[receive_byte_count] | (dat2_buf << bit_c);
            CNFGCLR = 0x0040;
            bit_c--;

            //phase 2
            while(!dat2_flag) {}       
            dat1_buf = dat1_PORT;
            receive_buf[receive_byte_count] = receive_buf[receive_byte_count] | (dat1_buf << bit_c);        
            CNFGCLR = 0x0080; 
            receive_bit_count++;    
            bit_c--;

            if(bit_c <= 0) {
                bit_c = 7;
                receive_byte_count++;
            }

        }
        receive_bit_count = 0;
        bit_c = 7;                
        word_count++;
        receive_buf[0] = receive_buf[0] & 0b01111111;
                
    }
    
    receive_bit_count = 0;
    bit_c = 7;   
    receive_byte_count = 0;

    //delay so it responds in ~60us
    for(int i = 0; i < 1500; i++) {
        Nop();
    }    
    
    //checksum check
    //if(receive_buf[0] == receive_buf[1] ^ receive_buf[2] ^ receive_buf[3] ^ receive_buf[4]) {
        //check the commands and respond
        switch(receive_buf[3]) {
            case 0b1:     //request device info
                maple_send_frame(device_info, 117);
                break;
            case 0b1001:     //get condition
                maple_send_frame(get_condition_response, 17);
                break;

            default:
                maple_send_frame(get_condition_response, 17);

        }        
    //}   
    
    //CNFGCLR = 0xC0;
    CNFGCLR = 0x0080;    
    IFS3CLR = 0x10000000;   //clear interrupt flag
    IEC3bits.CNGIE = 1; //enable interrupt 
}
*/