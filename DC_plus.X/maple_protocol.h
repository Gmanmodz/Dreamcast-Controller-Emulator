/* 
 * File:   MAPLE_PROTOCOL_H
 * Author: Gunnar Turnquist
 *
 * Created on May 21, 2019, 10:27 AM
 */

#ifndef MAPLE_PROTOCOL_H
#define	MAPLE_PROTOCOL_H

#include <xc.h>

//dat1 - RB3
//dat2 - RB2

#define dat1    LATBbits.LATB3
#define dat2    LATBbits.LATB2

#define dat1_PORT   PORTBbits.RB3
#define dat2_PORT   PORTBbits.RB2

#define dat1_TRIS TRISBbits.TRISB3
#define dat2_TRIS TRISBbits.TRISB2

#define dat1_ODC    ODCBbits.ODCB3
#define dat2_ODC    ODCBbits.ODCB2

#define dat1_flag   CNFBbits.CNFB3
#define dat2_flag   CNFBbits.CNFB2

void maple_delay();
void maple_send_byte(char data);
void maple_send_frame(char word[], char size);
void maple_sync();
void maple_terminate();
void maple_init();
void __ISR_AT_VECTOR(_CHANGE_NOTICE_B_VECTOR, IPL7AUTO) CNB_Handler(void);
void maple_receive_frame();
void maple_receive_frame2();

extern char receive_buf[];

#endif	