/* 
 * File:   I2C_H
 * Author: Gunnar Turnquist
 *
 * Created on May 31, 2019, 10:27 AM
 */

#ifndef I2C_H
#define I2C_H

#include <xc.h>

void I2C_init(double frequency);
void I2C_wait_for_idle(void);
void I2C_stop();
void I2C_start();
void I2C_restart();
void I2C_ack(void);
void I2C_nack(void);
void I2C_write(unsigned char address, char wait_ack);
void I2C_read(unsigned char *value, char ack_nack);

#endif