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

#include <project.h>
#include "stdbool.h"
#include "PacketHandler.h"
#include <stdio.h>
#include "stdlib.h"
#include "Globals.h"
#include "Utilities.h"
#include "DataStructs.h"

#define NUM_CMDS 3
const uint8 cmd_arry[NUM_CMDS] = { CMD_REMOTE_START,CMD_REMOTE_RECALL,CMD_SET_CC };

// Firmware to get the packet
#define STATE_WAITING          0
#define STATE_CHECKING         1
#define STATE_WAIT_FOR_ID      2
#define STATE_WAIT_FOR_LEN_LSB 3
#define STATE_WAIT_FOR_LEN_MSB 4
#define STATE_COLLECT_PAYLOAD  5
#define STATE_GET_CS_A         6
#define STATE_GET_CS_B         7


uint8 STATE = 0;
uint8 ID = 0;
uint16 length = 0;
uint8 packet_buffer[32];
uint16 packet_ptr = 0;
uint8 cs_a = 0, cs_b = 0;
uint8 calc_cs_a = 0, calc_cs_b = 0;

//Used for packet parsing
bool PacketStarted = false;   // Set to true if a packet is being parsed
bool PacketComplete = false;  // Set to true when the whole packet is received
bool PacketProcessed = true;  // Goes true once we process the packet command or data

/*******************************************************************************
 *  DESCRIPTION: Receives data packets from the BLE module
 ******************************************************************************/
#define BUFSIZE 512
#define BUFSIZE1 511   // should hold 42 doubles 42 * 8 = 336
char recBuf[BUFSIZE]; //, recBufFree = 1;
int recIndx = 0, recState = 0;

CY_ISR ( BlueToothtRx_ISR ) // receiving data from the BLE Module
{
  uint16 ch;
  do 
  {
      //TODO put timeout in, reset recState=recIndex=0,recBufFree=1
      ch = BlueToothUart_GetByte(); //MSB has error
      if((ch & 0xff00) != 0) // error has occured
      {
          break; //TODO HANDLE THE ERROR
      }
      ch &= 0xFF;
      if(recIndx > BUFSIZE1) { recState = 0; }                    // too big for our buffer
  
    // send to command interpreter 
    checkDataStream ( ch );
    
  } while(BlueToothUart_GetRxBufferSize());  //returns 0 on empty
}

// THe command IDs are stored in a const array. Check the array for a match.
bool checkForValidID ( uint8 ID )
{
	for ( uint8 i = 0; i < NUM_CMDS; i++ )
	{
		if ( ID == cmd_arry[i] )
			return 1;
	
	}	
	return 0;
		
}

// Reset the packet flags and states
 void resetPacketCheck ( void )
 {
	STATE = STATE_WAITING; // go back to the wating state
	PacketStarted = false;
	PacketComplete = false;
	PacketProcessed = true;
 }
	
// Command packet to send data and commands between the gauge and iPhone (App)

/* Instead of relying on the characteristics for every item sent over BLE, the RX and TX
 characteristics used to send the GGA and RTCM3 data will be used.
 This removes the data parsing from the BLE module and puts it into the gauge and App. The BLE is just a serial
 port. This allows us to update the gauge and App to expand what is transmitted without changing the BLE code.


 The packet structure will be as follows
 
 Byte
 0		  0xAA  			 Preamble sync char 1
 1      0x55  			 Preamble sync char 2
 2      0x33   			 ID for Measurement Data Packet
 3      0x5D.        LSB payload byte length
 4      0x00         MSB payload byte length
 5		  Payload byte 0
 ...
 4+N    Payload byte N
 5+N    CS_A			Checksum byte A
 6+N    CS_B            Checksum byte B
                        Checksum is calculated over a Byte 2 to all of payload.
						If buffer[m] contains all the m bytes from ID to end of payload
						CS_A = 0, CS_B = 0;
						for ( i = 0; i< m; i++ )
						{
							CS_A = CS_A + buffer[m];
							CS_B = CS_B + CS_A;
						}

 This routine waits for the begin flags. If they are received, it is assumed that the incoming data is a packet.
 The Packet Started flag is set when the begin flags are received.
 The Packet Complete Flg is set when the whole packet is received, and the checksum is good.
 Depending on the ID of the pack, the packet will processed by specific functions to get the data.

 parameters passed: c is the byte received.


*/
 void checkForPacket ( uint8 c )
 {
   
   switch ( STATE )
   {
	  // This is the intial state. No BEGIN FLAG has been received.
    case STATE_WAITING:
			if ( c != BEGIN_FLAG_0 )
			{
				PacketStarted = false;
         packet_ptr = 0;
			}
			else
			{
				STATE = STATE_CHECKING;	// BEGIN_FLAG_0 was received. If a valid packet, BEGIN_FLAG_1 will be next
			}	
			break;
		
	  case STATE_CHECKING: // Look for the second byte of the synch frame
			if ( c != BEGIN_FLAG_1 )       // if BEGIN_FLAG_1 is not received, then the data is not part of a packet
			{
				STATE = STATE_WAITING; // go back to the wating state
				PacketStarted = false;
			}
			else
			{	
				if ( PacketProcessed == true ) // This may need to be forced true if there is a hiccup somewhere. The calling function must set it true
				{	                             // if the app can process the data quick enough, this check may not be needed.
				 PacketStarted = true;         // The app can check to see if a potential packet is arriving.
				}
				STATE = STATE_WAIT_FOR_ID;
				calc_cs_a = calc_cs_b = 0;  // set the bytes uses to calc the checksums to 0
			}
			break;	
			
			 
		case STATE_WAIT_FOR_ID:

				ID = c;
				if ( checkForValidID ( ID ) == false ) // Check if the ID is valid. This will check a list of ID's
				{
					STATE = STATE_WAITING; // if not a valid ID, then go back to waiting for next packet.
					ID = 0;
				}	// This shouldn't be false, since it appears the correct start of packet was received.
				else
				{	
					STATE = STATE_WAIT_FOR_LEN_LSB;
					calc_cs_a = calc_cs_a + c;
					calc_cs_b = calc_cs_b + calc_cs_a;
				}	
				
				break;
		
    case STATE_WAIT_FOR_LEN_LSB:       // get the LSB byte length of the data packet.
			length = c;                     
			STATE = STATE_WAIT_FOR_LEN_MSB; 
			calc_cs_a = calc_cs_a + c;
			calc_cs_b = calc_cs_b + calc_cs_a;
			break;
			
		case STATE_WAIT_FOR_LEN_MSB:      // get MSB byte len
			 calc_cs_a = calc_cs_a + c;
			 calc_cs_b = calc_cs_b + calc_cs_a;
		   length = length + (c<<8);
      
       if ( length == 0 )       // no bytes in payload, so only the two check sum bytes are needed.
			 {
				STATE = STATE_GET_CS_A;
			 }
       else
       {
			  STATE = STATE_COLLECT_PAYLOAD; // Go collect the payload.
       } 
			 packet_ptr = 0;
			 break;
			 
		case STATE_COLLECT_PAYLOAD:
       packet_buffer[packet_ptr++] = c;   // put the byte into array
			 calc_cs_a = calc_cs_a + c;
			 calc_cs_b = calc_cs_b + calc_cs_a;
  		 if ( packet_ptr >= length )       // received all bytes, so only the two check sum bytes are needed.
			 {
				STATE = STATE_GET_CS_A;
			 }	
      
      if (packet_ptr > 450 )  // If the payload goes over 450 bytes, something bad happened, so reset.
      {                       // None of the packets should be over 400 bytes, so I picked 450 to reset
       STATE = STATE_WAITING;
			 PacketStarted = false;   
       PacketProcessed = true;
      }  
			break;
			 
		case STATE_GET_CS_A:  
			 cs_a = c;
			 STATE = STATE_GET_CS_B;
		   break;
			
		case STATE_GET_CS_B:  
			 cs_b = c;
			 
			 // all the packet has been received. CHeck that the CS is good.
			 if (( cs_a ==  calc_cs_a) && ( cs_b ==  calc_cs_b) )
			 {
				// CS is good. This means the packet can be processed. 
				PacketComplete = true;
				PacketProcessed = false;	// With Complete flag true and Processed flag false, the app can process this packet
			 }
       PacketStarted = false;    // reset the Started flag to begin packet reception again.
			 STATE = STATE_WAITING;    
			 break;	
			
		default:
			 STATE = STATE_WAITING;
			 PacketStarted = false;   
       PacketProcessed = true;
			 break;
		
	}// end switch
		
 }   


//************  process the data payload ************************************//	 
// Currently, the station_data and the Gauge constants are sent after areading
// So, along with the GGA data, there are only three packets.
// The serial number packet is also sent when the serial number is changed, but
// THis will be implented after the other packets are implemented.

void processPacketStream ( void )
{
   
  // process the packet based on the packet ID.
  switch ( ID )
  {
    case CMD_REMOTE_START:
      g_remote_start_flag = TRUE;
      break;
    
    case CMD_REMOTE_RECALL:
      g_remote_recall_flag = TRUE;
      break;
    
    case CMD_SET_CC:
    {
    	uint16 size = sizeof(constants_t);
      constants_t const_data;
     // copy the byte array into the station data struct.
	    memcpy ( &const_data, packet_buffer, length );
      CyDelay ( 1 );
    }
    break;
    default:
      PacketProcessed = true;
      break;
  
  }
 PacketComplete =  false;
 PacketProcessed = true;

}


// move rx data from internal buffer to one we can use
// This is done in the interrupt if the packet is valid.
void checkDataStream ( uint8 ch )
{
  checkForPacket(ch);
  if ( PacketComplete ==  true && PacketProcessed == false)
  {
    processPacketStream ();
  }
  
}

/* [] END OF FILE */
