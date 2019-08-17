/* 
 * File:   MCP4706.cpp
 * Author: Gunnar Turnquist
 *
 * Created on June 5, 2019, 10:27 AM
 */

#include <xc.h>
#include "I2C.h"
#include "controller_poll.h"

#define MCP4706_ADDR    0b1100001

void MCP4706_write(unsigned char reg, char data) {
    I2C_start();
    I2C_write(MCP4706_ADDR << 1, 0);            
    I2C_write(reg, 0);
    I2C_write(data, 0);   
    I2C_stop();
}

void MCP4706_init() {
    
    //GENERAL CALL WAKE UP COMMAND
    I2C_start();
    I2C_write(0x00, 0);
    I2C_write(0x09, 0);   
    I2C_stop();   
    
/*
VREF1:VREF0 Resistor Ladder Voltage Reference (VRL) Selection bits
0x =VDD (Unbuffered)
10 =VREF pin (Unbuffered)
11 =VREF pin (Buffered)
   
PD1:PD0 Power-Down Selection bits
00 = Not Powered Down (Normal operation)
01 = Powered Down ? VOUT is loaded with 1 k? resistor to ground.
10 = Powered Down ? VOUT is loaded with 100 k? resistor to ground.
11 = Powered Down ? VOUT is loaded with 500 k? resistor to ground.
 
G Gain Selection bit
0 = 1x (gain of 1)
1 = 2x (gain of 2). Not applicable when VDD is used as VRL   
*/
    
    //Write Volatile Configuration Bits   
    I2C_start();
    I2C_write(MCP4706_ADDR << 1, 0);            
    I2C_write(0b10000000, 0);   //VREF is VDD, Not powered down, gain of 1
    I2C_stop();

}

char up_edge = 0;
char down_edge = 0;
char left_edge = 0;
char idle_edge = 0;

char set_dac = 0;
char dac_val = 0;

void check_dac_combo() {
    
    //3v3-idle, 0v up, .6v down, 1.57v menu
    
    if(!DL && !S) {
        idle_edge = 0;
        dac_val = 122;
        if(left_edge == 0) {
            left_edge = 1;
            set_dac = 1;
        }
    }
    else if(!DU && !S) {
        idle_edge = 0;
        dac_val = 0;
        if(up_edge == 0) {
            up_edge = 1;
            set_dac = 1;
        }        
    }
    else if(!DD && !S) {
        idle_edge = 0;
        dac_val = 46;
        if(down_edge == 0) {
            down_edge = 1;
            set_dac = 1;
        }        
    }
    else {
        left_edge = 0;
        up_edge = 0;
        down_edge = 0;
        idle_edge = 0;
        
        dac_val = 255;
        
        if(idle_edge == 0) {
            idle_edge = 1;
            set_dac = 1;
        }
    }
    
    if(set_dac) {
        MCP4706_write(0b00000000, dac_val);
        set_dac = 0;
    }
            
}