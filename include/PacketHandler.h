/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#ifndef PACKETHANDLER_H
  
#define PACKETHANDLER_H

#include <project.h>
#include "stdbool.h"

#define BEGIN_FLAG_0     0xAA
#define BEGIN_FLAG_1     0x55 
#define CMD_REMOTE_START 0x53 // Start a measurement from the app
#define CMD_REMOTE_RECALL 0x52
#define CMD_FLAG_READ    0x33  // new reading station data
#define CMD_FLAG_CC      0x77  // calibration constants
#define ACK_FLAG_CC      0x78  // calibration constants
#define CMD_SET_CC       0x77  //  setting CC in eeprom
#define CMD_FLAG_SN      0x88  // serial number
#define CMD_FLAG_VER     0x89  // firmware version
#define CMD_SET_SN       0x90  // pc setting sn
#define CMD_FLAG_RESET   0x99  // software reset
#define DELIMITER_FLAG   '~'   // for receiving data from the ble module
#define END_FLAG         '\n'  // for receiving data from the ble module

  
extern bool PacketStarted;    // Set to true if a packet is being parsed
extern bool PacketComplete;  // Set to true when the whole packet is received
extern bool PacketProcessed;  // Goes true once we process the packet command or data

void checkDataStream ( uint8 ch );

#endif

/* [] END OF FILE */
