/* 
 * File:   CONTROLLER_POLL_H
 * Author: Gunnar Turnquist
 *
 * Created on May 27, 2019, 12:17 PM
 */

#ifndef CONTROLLER_POLL_H
#define	CONTROLLER_POLL_H

#include <xc.h>

#define DU  PORTCbits.RC15
#define DD  PORTBbits.RB15
#define DL  PORTCbits.RC12
#define DR  PORTBbits.RB14
#define A   PORTEbits.RE3
#define B   PORTEbits.RE2
#define X   PORTFbits.RF1
#define Y   PORTEbits.RE1
#define S   PORTEbits.RE4
#define R   PORTEbits.RE0
#define L   PORTFbits.RF4
#define plus    PORTDbits.RD2
#define minus   PORTDbits.RD3

extern char device_info[];
extern char get_condition_response[];
extern char button_byte;

extern char *buf[2];
extern char buf_index;

void controller_poll();
void LUT_init();

#endif	