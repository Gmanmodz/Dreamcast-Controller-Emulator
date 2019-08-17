/* 
 * File:   SSD1331.cpp
 * Author: Gunnar Turnquist
 *
 * Created on June 10, 2019, 10:27 AM
 */

#include <xc.h>
#include "SSD1331.h"
#include "SPI.h"

void SSD1331_init() {
    
 // Initialization Sequence
    sendCommand(SSD1331_CMD_DISPLAYOFF);  	// 0xAE
    sendCommand(SSD1331_CMD_SETREMAP); 	// 0xA0
    
    sendCommand(0x72);				// RGB Color
    
    /*
#if defined SSD1331_COLORORDER_RGB
    sendCommand(0x72);				// RGB Color
#else
    sendCommand(0x76);				// BGR Color
#endif
     */
    
    sendCommand(SSD1331_CMD_STARTLINE); 	// 0xA1
    sendCommand(0x0);
    sendCommand(SSD1331_CMD_DISPLAYOFFSET); 	// 0xA2
    sendCommand(0x0);
    sendCommand(SSD1331_CMD_NORMALDISPLAY);  	// 0xA4
    sendCommand(SSD1331_CMD_SETMULTIPLEX); 	// 0xA8
    sendCommand(0x3F);  			// 0x3F 1/64 duty
    sendCommand(SSD1331_CMD_SETMASTER);  	// 0xAD
    sendCommand(0x8E);
    sendCommand(SSD1331_CMD_POWERMODE);  	// 0xB0
    sendCommand(0x0B);
    sendCommand(SSD1331_CMD_PRECHARGE);  	// 0xB1
    sendCommand(0x31);
    sendCommand(SSD1331_CMD_CLOCKDIV);  	// 0xB3
    sendCommand(0xF0);  // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
    sendCommand(SSD1331_CMD_PRECHARGEA);  	// 0x8A
    sendCommand(0x64);
    sendCommand(SSD1331_CMD_PRECHARGEB);  	// 0x8B
    sendCommand(0x78);
    sendCommand(SSD1331_CMD_PRECHARGEC);  	// 0x8C
    sendCommand(0x64);
    sendCommand(SSD1331_CMD_PRECHARGELEVEL);  	// 0xBB
    sendCommand(0x3A);
    sendCommand(SSD1331_CMD_VCOMH);  		// 0xBE
    sendCommand(0x3E);
    sendCommand(SSD1331_CMD_MASTERCURRENT);  	// 0x87
    sendCommand(0x06);
    sendCommand(SSD1331_CMD_CONTRASTA);  	// 0x81
    sendCommand(0x91);
    sendCommand(SSD1331_CMD_CONTRASTB);  	// 0x82
    sendCommand(0x50);
    sendCommand(SSD1331_CMD_CONTRASTC);  	// 0x83
    sendCommand(0x7D);
    sendCommand(SSD1331_CMD_DISPLAYON);	//--turn on oled panel

}