/* 
 * File:   MCP4706_H
 * Author: Gunnar Turnquist
 *
 * Created on June 5, 2019, 10:27 AM
 */

#ifndef MCP4706_H
#define MCP4706_H

#include <xc.h>

void MCP4706_write(unsigned char reg, char data);
void MCP4706_init();
void check_dac_combo();

#endif