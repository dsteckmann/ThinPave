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
#include "project.h"
#include "Globals.h"
#include "Utilities.h"
#include "DataStructs.h"
#include "Batteries.h"
#include "PacketHandler.h"

extern uint32 getSerialNumber ( void );

/******************************************************************************
 *  Name:           BlueToothUartPutCharDelay
 *  PARAMETERS:     character to send
 *  DESCRIPTION:    Sends character to BlueToothUart then delays 9 ms
 *****************************************************************************/ 
void BlueToothUartPutCharDelay(char c) {
    BlueToothUart_PutChar ( c );  
    CyDelay ( 1 );   
}
/******************************************************************************
 *  Name:           BlueToothUartPutPktHeaderDelay
 *  PARAMETERS:     2nd byte of the ID, and the len of the data to follow
 *  DESCRIPTION:    Sends header to BlueToothUart with intercharacter delay of 9 ms
 *****************************************************************************/ 
void BlueToothUartPutPktHeaderDelay(char beginFlag2, uint16 len) {
    uint8 lenMSB = (uint8)((len>>8) & 0x00FF);
    uint8 lenLSB = (uint8)(len & 0x00FF);
    BlueToothUartPutCharDelay ( BEGIN_FLAG_0 );  
    BlueToothUartPutCharDelay ( BEGIN_FLAG_1 );
    BlueToothUartPutCharDelay ( beginFlag2 );
    BlueToothUartPutCharDelay (lenLSB);
    BlueToothUartPutCharDelay (lenMSB);
}


/******************************************************************************
 *  Name:           BlueToothUartPutBuffer
 *  PARAMETERS:     uint8* pointer to Buffer to send and the len of the data to send
 *  DESCRIPTION:    Sends the buffers contets to BlueToothUart
 *****************************************************************************/ 
void BlueToothUartPutBuffer(uint8* buf, uint16 len) {
    int i;
    uint8 d;

    for( i = 0; i < len ; i++ ) 
    {
      d = buf[i];
      BlueToothUart_PutChar(d);
      CyDelay ( 1  );
      if (((i % 185) == 0 ) && (i != 0 ))
      {
        CyDelay ( 500 ); // This is needed to break long data stream into multiple blocks
      }
    }
 
}

/******************************************************************************
 *  Name:           SendBLEData
 *  PARAMETERS:     pointer to StationData to send
 *  DESCRIPTION:    sends Station data to the BLE module
 *****************************************************************************/ 
void SendBLEData ( station_data_t * ble_data, bool isRecall )
{  
  uint8  cs_a = 0, cs_b = 0  ;
  uint16 len = sizeof(station_data_t);  
  uint8  lsb_len,msb_len;  
  bool recallB = isRecall;
  uint8 array[128]; // addd all of the date to the array before transmitting the data
  uint16 index = 0;  
  uint32 sn = getSerialNumber();
  
  // update the BLE station date with the latest voltages
  ble_data->battery_voltage[0] = readBatteryVoltage(NICAD) ;
  ble_data->battery_voltage[1] = readBatteryVoltage(ALK) ;
  ble_data->ser_number = sn;
  ble_data->version = VERSION;
  
  #define extra_bytes 0    // Add serial number
  
  lsb_len = (uint8)((len +extra_bytes) & 0x00FF);
  msb_len = (uint8)(((len+extra_bytes)>>8) & 0x00FF);
  
  cs_a += CMD_FLAG_READ; // add the ID to the Cs.
  cs_b += cs_a;
  
  cs_a += lsb_len; // add the lsb_len byte to the Cs.
  cs_b += cs_a;
  
  cs_a += msb_len; // add the msb_len byte to the Cs.
  cs_b += cs_a;
  
  array[index++] = ( BEGIN_FLAG_0 );  
  array[index++] = ( BEGIN_FLAG_1 );
  array[index++] = ( CMD_FLAG_READ );
  array[index++] = ( lsb_len );
  array[index++] = ( msb_len ); 
  
 // station_data_t new_station;
 // memcpy (&new_station,(uint8*) ble_data, len );
  // add the payload to the cs.
  uint8* const_ptr = (uint8*) ble_data;
  for ( uint16 i = 0; i < len; i++ )
  {
   cs_a += const_ptr[i];
   cs_b += cs_a;
   array [index++] = const_ptr[i];
  }

  array [index++] = cs_a;
  array [index++] = cs_b;
  
  BlueToothUartPutBuffer (array, index );

  // Don't send the recall flag yet.
}

/******************************************************************************
 *  Name:           SendBLEDataCC
 *  DESCRIPTION:    Sends EepromData Constants to the BLE module
 *****************************************************************************/ 
void SendBLEDataCC() {
  uint8 cs_a = 0,cs_b = 0  ;
  uint16 len = sizeof(constants_t); // including extra bytes for new Serial Number
  uint8  lsb_len,msb_len;
  uint8 array[400]; // addd all of the date to the array before transmitting the data
  uint16 index = 0;

  #define extra_bytes 0
 
  lsb_len = (uint8)((len +extra_bytes) & 0x00FF);
  msb_len = (uint8)(((len+extra_bytes)>>8) & 0x00FF);
  
  cs_a += CMD_FLAG_CC; // add the ID to the Cs.
  cs_b += cs_a;
  
  cs_a += lsb_len; // add the lsb_len byte to the Cs.
  cs_b += cs_a;
  
  cs_a += msb_len; // add the msb_len byte to the Cs.
  cs_b += cs_a;

    
  array[index++] = ( BEGIN_FLAG_0 );  
  array[index++] = ( BEGIN_FLAG_1 );
  array[index++] = ( CMD_FLAG_CC );
  array[index++] = ( lsb_len );
  array[index++] = ( msb_len ); 
  
  // add the payload to the cs.
  uint8* const_ptr = (uint8*)(&eepromData.Constants);
  for ( uint16 i = 0; i < len; i++ )
  {
   cs_a += const_ptr[i];
   cs_b += cs_a;
   array [index++] = const_ptr[i];
  }

  array [index++] = cs_a;
  array [index++] = cs_b;
  
  BlueToothUartPutBuffer (array, index );

}  

/******************************************************************************
 *  DESCRIPTION:    Sends Acknowlegement or calibration receipt to the PC.
 *****************************************************************************/ 
void SendBleCcAck() {
    uint32 sn = getSerialNumber();
    BlueToothUartPutPktHeaderDelay(ACK_FLAG_CC, 4);
    BlueToothUartPutBuffer((uint8*)(&eepromData.Constants), 4);
    BlueToothUartPutBuffer((uint8*)(&sn), 4);
}
/******************************************************************************
 *  Name:           RequestBleVersion
 *  DESCRIPTION:    Requests the BLE modules firmware version
 *****************************************************************************/ 
void RequestBleVersion() { 
    uint32 sn = getSerialNumber();
    BlueToothUartPutPktHeaderDelay(CMD_FLAG_VER, 4); // protocol nees 
    BlueToothUartPutBuffer((uint8*)(&sn), 4);
}

/******************************************************************************
 *  Name:           SendBleNewSn
 *  DESCRIPTION:    Sends The Gauge Serial Number to the BLE module
 *****************************************************************************/ 
void SendBleSn() 
{ 
  uint8  lsb_len,msb_len;  
  uint8 cs_a = 0,cs_b = 0  ;
  uint8 array[32]; // addd all of the date to the array before transmitting the data
  uint16 index = 0;
  
  uint32 sn = getSerialNumber();

  lsb_len = (uint8)((4) & 0x00FF);  // sn is uint32, 4 bytes
  msb_len = (uint8)(((4)>>8) & 0x00FF);

  cs_a += CMD_FLAG_SN; // add the ID to the Cs.
  cs_b += cs_a;
  cs_a += lsb_len; // add the lsb_len byte to the Cs.
  cs_b += cs_a;
  cs_a += msb_len; // add the msb_len byte to the Cs.
  cs_b += cs_a;

  array[index++] = ( BEGIN_FLAG_0 );  
  array[index++] = ( BEGIN_FLAG_1 );
  array[index++] = ( CMD_FLAG_SN );
  array[index++] = ( lsb_len );
  array[index++] = ( msb_len ); 
  
  uint8* sn_ptr = (uint8*)(&sn);
  for ( uint8 i = 0; i < 4; i++ )
  {
   cs_a += sn_ptr[i];
   cs_b += cs_a;
   array [index++] = sn_ptr[i];
  }
  array [index++] = cs_a;
  array [index++] = cs_b;
  BlueToothUartPutBuffer (array, index );

}
  
/******************************************************************************
 *  Name:           SendBleReset
 *  DESCRIPTION:    Sends The Gauge Serial Number to the BLE module
 *****************************************************************************/ 
void SendBleReset() {
    uint32 i = 8371;
    BlueToothUartPutPktHeaderDelay(CMD_FLAG_RESET, 4);
    BlueToothUartPutBuffer((uint8*)(&i), 4);
}



/* [] END OF FILE */
