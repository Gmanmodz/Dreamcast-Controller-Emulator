/* 
 * File:   LM49450_H
 * Author: Gunnar Turnquist
 *
 * Created on May 31, 2019, 10:27 AM
 */

#ifndef LM49450_H
#define LM49450_H

#include <xc.h>

void LM49450_write(unsigned char reg, char data);
void LM49450_init();
void check_vol_buttons();

#endif