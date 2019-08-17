/* 
 * File:   SPI_H
 * Author: Gunnar Turnquist
 *
 * Created on June 7, 2019, 10:27 AM
 */

#ifndef SPI_H
#define SPI_H

#include <xc.h>

void SPI_init();
char SPI_send(char data);
void sendCommand(char data);

#endif