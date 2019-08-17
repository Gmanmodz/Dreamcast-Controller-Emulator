/* 
 * File:   LM49450.cpp
 * Author: Gunnar Turnquist
 *
 * Created on May 31, 2019, 10:27 AM
 */

#include <xc.h>
#include "I2C.h"
#include "controller_poll.h"

#define LM49450_ADDR    0b1111101 
            
void LM49450_write(unsigned char reg, char data) {
    I2C_start();
    I2C_write(LM49450_ADDR << 1, 0);            
    I2C_write(reg, 0);
    I2C_write(data, 0);   
    I2C_stop();
}

void LM49450_init() {

    //mode control register
    //128 oversampling: B6:B5 = 0b01
    //spread spectrum
    //device enabled
    LM49450_write(0x00, 0b00101001); 
    //clock control register
    //division is 1 B5:B0 = 0b000001
    LM49450_write(0x01, 0b00000001); // /1
    //LM49450_write(0x02, 0x95);    // 75 (0x95, recommended value for 12.228MHz
    //charge pump clock register 
    //I2S mode control register
    //32bit word size: B6:B4 = 0b111
    //left-mode
    //LM49450_write(0x03, 0b01110001);     //for Wii 
    //I2S clock register
    LM49450_write(0x04, 0b00000000);   
    
    LM49450_write(0x02, 0x89);   //68 for 11.28MHz
    LM49450_write(0x03, 0b00000010);     //for DC. 16bit word size, Right-Justified    
}

char debounce_plus = 0;
char debounce_minus = 0;

char set_volume = 0;
char volume = 15;

char plus_edge = 0;
char minus_edge = 0;

void check_vol_buttons() {
    
    if(!plus) {
        debounce_plus++;
    }
    else {
        debounce_plus = 0;
        plus_edge = 0;
    }
    if(!minus) {
        debounce_minus++;
    }
    else {
        debounce_minus = 0;
        minus_edge = 0;
    }
    
    //we only want to increment volume on the rising edge. the plus_edge will reset once the button is released
    if(debounce_plus > 3) {
        //plus is debounced, set volume
        if(plus_edge == 0) {
            plus_edge = 1;
            volume++;
            set_volume = 1;      
        }
    }
    if(debounce_minus > 3) {
        if(minus_edge == 0) {
            minus_edge = 1;
            volume--;
            set_volume = 1;
        }
    }
    
    //volume stays in range
    if(volume > 32) {
        volume = 32;
    }
    if(volume < 0) {
        volume = 0;
    }
    
    if(set_volume) {
        LM49450_write(0x08, volume); //speaker volume
        LM49450_write(0x07, volume); //headphone volume 
        set_volume = 0;
    }
    
}