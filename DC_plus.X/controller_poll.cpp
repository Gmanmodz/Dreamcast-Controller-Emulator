/* 
 * File:   CONTROLLER_POLL_H
 * Author: Gunnar Turnquist
 *
 * Created on May 27, 2019, 12:17 PM
 */

#include <cstdlib>
#include <xc.h>
#include "controller_poll.h"
#include "ADC.h"

char device_info[117] {
0b00011100,
0b00100000,
0b00000000,
0b00000101,
0b00000001,
0b00000000,
0b00000000,
0b00000000,
0b11111110,
0b00000110,
0b00001111,
0b00000000,
0b00000000,
0b00000000,
0b00000000,
0b00000000,

0b00000000,
0b00000000,
0b00000000,
0b00000000,
0b01110010,
0b01000100,
0b00000000,
0b11111111,
0b01100011,
0b01101101,
0b01100001,
0b01100101,
0b00100000,
0b01110100,
0b01110011,
0b01100001,

0b01110100,
0b01101110,
0b01101111,
0b01000011,
0b01101100,
0b01101100,
0b01101111,
0b01110010,
0b00100000,
0b00100000,
0b01110010,
0b01100101,
0b00100000,
0b00100000,
0b00100000,
0b00100000,

0b00100000,
0b00100000,
0b00100000,
0b00100000,
0b01100100,
0b01101111,
0b01110010,
0b01010000,
0b01100100,
0b01100101,
0b01100011,
0b01110101,
0b00100000,
0b01111001,
0b01000010,
0b00100000,

0b01010101,
0b00100000,
0b01110010,
0b01101111,
0b01110010,
0b01100101,
0b01100100,
0b01101110,
0b01100011,
0b01101001,
0b01001100,
0b00100000,
0b01100101,
0b01110011,
0b01101110,
0b01100101,

0b01101111,
0b01110010,
0b01000110,
0b00100000,
0b01000101,
0b01010011,
0b00100000,
0b01101101,
0b01000101,
0b00100000,
0b01000001,
0b01000111,
0b01010010,
0b01000101,
0b01010100,
0b01001110,

0b01010011,
0b01001001,
0b01010010,
0b01010000,
0b01001100,
0b00101100,
0b01010011,
0b01000101,
0b00100000,
0b00101110,
0b01000100,
0b01010100,
0b00100000,
0b00100000,
0b00100000,
0b00100000,

0b00000001,
0b11110100,
0b00000001,
0b10101110,

0b00011001    
};

char get_condition_response[17] = {
0b00000011,
0b00100000,
0b00000000,
0b00001000,

0b00000001,
0b00000000,
0b00000000,
0b00000000,

0b00000000,
0b00000000,
0b11111111,
0b11111111,

0b10000000,
0b10000000,
0b10000000,
0b10000000,

0b00101010        
};

char get_condition_response_buf2[17];   //back controller buffer data
char *buf[2] = {get_condition_response, get_condition_response_buf2};   //array of pointers to buffers
char buf_index = 0;

char debounce_tmr[8] = {};
char button_byte = 0xFF;
char button_byte_temp[8] = {};
char button_byte_prev[8] = {};

char debounce_tmr2[8] = {};
char button_byte2 = 0xFF;
char button_byte2_temp[8] = {};
char button_byte2_prev[8] = {};

char debounce_r = 0;
char debounce_l = 0;
char rtrigger = 0;
char rtrigger_temp = 0;
char rtrigger_prev = 0;
char ltrigger = 0;
char ltrigger_temp = 0;
char ltrigger_prev = 0;

int Map(int x, int inMin, int inMax, int outMin, int outMax) {
    // Map a single value onto a different range
    return (((x - inMin) * (outMax - outMin)) / (inMax - inMin)) + outMin;
}

int SX_MIN = 84;
int SX_MAX = 164;
int SY_MIN = 84;
int SY_MAX = 164;

char SX_reverse = 0;
char SY_reverse = 1;

char dead_zone = 15;

char LUT_SX[256];
char LUT_SY[256];

void LUT_init() {   
    int pos;
    
    for (pos = 0; pos < 256; pos++) {
        
        if(pos < SX_MIN) {
            LUT_SX[pos] = 0;
        }
        else if(pos > SX_MAX) {
            LUT_SX[pos] = 255;
        }
        else if(pos < (127 + dead_zone) && pos > (127 - dead_zone)) {
            LUT_SX[pos] = 127;
        }
        else {
            LUT_SX[pos] = Map(pos, SX_MIN, SX_MAX, 0, 255);
        }
        
        if(pos < SY_MIN) {
            LUT_SY[pos] = 0;
        }
        else if(pos > SY_MAX) {
            LUT_SY[pos] = 255;
        }
        else if( (pos < (127 + dead_zone)) && (pos > (127 - dead_zone)) ) {
            LUT_SY[pos] = 127;
        }        
        else {
            LUT_SY[pos] = Map(pos, SY_MIN, SY_MAX, 0, 255);
        }        
        
    }
}

void controller_poll() {
    
    //store the port states
    button_byte_temp[7] = DR;
    button_byte_temp[6] = DL;
    button_byte_temp[5] = DD;
    button_byte_temp[4] = DU;
    button_byte_temp[3] = S;
    button_byte_temp[2] = A;
    button_byte_temp[1] = B;
    
    button_byte2_temp[1] = Y;
    button_byte2_temp[2] = X;
    
    rtrigger_temp = R;
    ltrigger_temp = L;
    
    //debounce the ports
    //byte 1
    for(char index = 1; index < 8; index++) {
        if(!button_byte_temp[index]){
            debounce_tmr[index]++;
        }

        if(button_byte_temp[index] ^ button_byte_prev[index]) {
            debounce_tmr[index] = 0;
            button_byte |= 1 << index;  //clear data
        }

        if(debounce_tmr[index] > 3) {
            button_byte &= ~(1 << index);   //set data
        }

        button_byte_prev[index] = button_byte_temp[index];  //store previous state
    }
    
    //byte 2
    for(char index = 1; index < 3; index++) {
        if(!button_byte2_temp[index]){
            debounce_tmr2[index]++;
        }

        if(button_byte2_temp[index] ^ button_byte2_prev[index]) {
            debounce_tmr2[index] = 0;
            button_byte2 |= 1 << index;  //clear data
        }

        if(debounce_tmr2[index] > 3) {
            button_byte2 &= ~(1 << index);   //set data
        }

        button_byte2_prev[index] = button_byte2_temp[index];  //store previous state
    }    
    
    //debounce triggers
    if(!rtrigger_temp){
        debounce_r++;
    }
    if(rtrigger_temp ^ rtrigger_prev) {
        debounce_r = 0;
        rtrigger = 0;  //clear data
    }
    if(debounce_r > 3) {
        rtrigger = 0xFF;   //set data
    }
    rtrigger_prev = rtrigger_temp;  //store previous state        
    
    if(!ltrigger_temp){
        debounce_l++;
    }
    if(ltrigger_temp ^ ltrigger_prev) {
        debounce_l = 0;
        ltrigger = 0;  //clear data
    }
    if(debounce_l > 3) {
        ltrigger = 0xFF;   //set data
    }
    ltrigger_prev = ltrigger_temp;  //store previous state       
        
    //read analog SX SY
    int * ADC_buf = read_adc();
    
    int SX = ADC_buf[1];
    int SY = ADC_buf[0];
    
    //convert to 8 bit value
    SX = SX >> 4;
    SY = SY >> 4;
    
    //direction reverse
    if(SX_reverse) {
        SX = 255 - SX;
    }    
    if(SY_reverse) {
        SY = 255 - SY;
    }     
    
    //LUT
    SX = LUT_SX[SX];
    SY = LUT_SY[SY];
    
    //use the back buffer when manipulating the front buffer
    buf_index = 1;
    
    //set the debounced buttons
    get_condition_response[8] = rtrigger;
    get_condition_response[9] = ltrigger;
    get_condition_response[10] = button_byte2;
    get_condition_response[11] = button_byte;
    get_condition_response[14] = SY;
    get_condition_response[15] = SX;
    
    //checksum
    get_condition_response[16] = get_condition_response[0] ^ get_condition_response[1] ^ get_condition_response[2] ^ get_condition_response[3] ^
            get_condition_response[4] ^ get_condition_response[5] ^ get_condition_response[6] ^ get_condition_response[7] ^
            get_condition_response[8] ^ get_condition_response[9] ^ get_condition_response[10] ^ get_condition_response[11] ^
            get_condition_response[12] ^ get_condition_response[13] ^ get_condition_response[14] ^ get_condition_response[15];   
    
    //switch back to the front buffer
    buf_index = 0;
    
    //copy buffers
    for(char i = 0; i < 17; i++) {
        get_condition_response_buf2[i] = get_condition_response[i];
    }
    
}