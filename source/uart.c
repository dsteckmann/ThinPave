
/**********************************************************************
* 2009 Microchip Technology Inc.
*
* FileName:        	uart.c
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       	PIC24F
* Compiler:        	MPLAB C30 v3.11 or higher
*
* 
*
* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author        	Date      	Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* 
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**********************************************************************/

#include "project.h"
#include "Globals.h"
#include "UARTS.h"

#define MAX_RX_TIME_ERROR   0x1122
RX_BUFFER  s_Alfat_RX;
RX_BUFFER  s_Computer_RX;



#define KEY_NONE   0xAA
static volatile uint8_t plus_data[4];

// The Depth for Plus Gauge is updated in the ISR.
uint8 getPlusDepthReading ( void )
{
  CyGlobalIntDisable;
  return plus_data[1] ;
  CyGlobalIntEnable;
}

uint8 getPlusRemoteKey ( void )
{
  if ( plus_data[2] == KEY_NONE )
    return 0;
  else
    return plus_data[2] ;
}

/******************************************************************************
 *
 *  NAME:  clearComputerRXBuffer ( void )
 *
 *  PARAMETERS: None
 *
 *  DESCRIPTION: Clear out the buffer by making all values NULL
 *               Set the indicies to 0 
 *
 *  RETURNS:None
 *
 *****************************************************************************/ 

void clearComputerRXBuffer ( void )
{
  uint16_t fn;
  
  delay_ms(1);
   
   // wait for RX FIFO buffer to be empty
   while( (UART2_RX_STS_FIFO_NOTEMPTY & UART2_ReadRxStatus() ) )
   {
    UART2_ReadRxData(); // Reads data from RX data register, and clears FIFO flag
    delay_ms(10); // wait to see if more data arrives
   }  
    
  // This isn't necessary,but is used for testing
  // It puts all 0xFF into buffer. Could be nulls or not done at all
  
  for (fn=0; fn < BUFF_SIZE; fn++)
  { 
   s_Computer_RX.rx_buffer[fn] = 0xFF;
    
  }  
  
  // buffer pointers are reset.
 s_Computer_RX.buffer_end = s_Computer_RX.buffer_start = 0;
  
}

 /******************************************************************************
 * Name: getRXBuffer 
 *  
 *
 *  PARAMETERS:None
 *
 *  DESCRIPTION: returns a char from the receive buffer or buffer empty
 *
 *  RETURNS: data byte or buffer empty
 *
 *****************************************************************************/ 
   
uint16_t getcRXBuffer (  RX_BUFFER * buffer )
{
  uint8_t data;
  
  data =  buffer->rx_buffer[buffer->buffer_start];
  
  buffer->buffer_start++;   // point to next data entery
 
  buffer->buffer_start &= (BUFF_SIZE - 1);   // overflow starts rewriting buffer
     
  return (uint16_t) data;
  
}
 

 /******************************************************************************
 *
 *  Name:  getRXNumEntries (  )
 *
 *
 *  PARAMETERS: None
 *
 *  DESCRIPTION: Calculates the number of rx bytes in the buffer
 *
 *  RETURNS: number of bytes 0 to BUFF_SIZE
 *
 *****************************************************************************/ 
   
uint16_t getRXNumEntries (  RX_BUFFER * buffer )
{
  uint16_t num_entries;
  
  if ( buffer->buffer_end > buffer->buffer_start )
  { 
    num_entries = buffer->buffer_end - buffer->buffer_start;

  }  
  else if( buffer->buffer_end < buffer->buffer_start )
  { 
    num_entries = ( BUFF_SIZE - buffer->buffer_start) + buffer->buffer_end ;
  }
  else
  {
    num_entries = 0;
  }
  return num_entries;
}

 /******************************************************************************
 * Name: waitc_RX ( )
 *  
 *
 *  PARAMETERS: rx buffer pointer, max time to wait for character
 *
 *  DESCRIPTION: waits for a rx char
 *
 *  RETURNS: rx char or time-out error
 *
 *****************************************************************************/

uint16_t waitc_RX ( RX_BUFFER * buffer , int32_t max_msec_delay )
{
  int32_t wait_time = max_msec_delay * 20;
  uint16_t num;
  uint16_t data;
  
  do
  {
   // get the number of chars in rx buffer. rx buffer is filled by ISR
   num = getRXNumEntries ( buffer );
   
   // if buffer has data, fetch it and return the char
   if ( num > 0 )
   {
     data = getcRXBuffer ( buffer );
     return data;
   }
  
   // decrement the max time delay counter
   delay_us ( 50 );
   wait_time--;
   
  } while ( wait_time > 0 );    // loop until max time or char received
  
  return MAX_RX_TIME_ERROR;
     
    
}


 /******************************************************************************
 * Name: waits_RX ( )
 *  
 *
 *  PARAMETERS: rx buffer pointer, max time to wait for character, address of string
 *
 *  DESCRIPTION: waits for a rx NUll terminated string, and adds chars to passed address
 *
 *  RETURNS: 1 or time-out error
 *
 *****************************************************************************/

uint16_t waits_RX ( RX_BUFFER * buffer , int32_t max_msec_delay, char * string )
{
  int32_t wait_time = max_msec_delay * 20;
  uint16_t num;
  uint8_t data = 0;
  
  do
  {
   // get the number of chars in rx buffer. rx buffer is filled by ISR
   num = getRXNumEntries ( buffer );
   
   // if buffer has data, fetch it and return the char
   if ( num > 0 )
   {
     data = getcRXBuffer ( buffer );    // get the char
     *string = data;                    // add the char to the string
     string++;                          // point to next empty string position
     if ( '\0' == data )
     {
       return 1;                        // finished when null received
     }  
   }
  
   // decrement the max time delay counter
   delay_us ( 50 );
   wait_time--;
   
  } while ( wait_time > 0 );    // loop until max time or char received
  
  return MAX_RX_TIME_ERROR;
     
 }

 

//*************** Interrupt Service routine for UART2 Transmission *************************************
/******************************************************************************
 *
 *  NAME: _U2RXInterrupt ( void )
 *
 *  PARAMETERS: None
 *
 *  DESCRIPTION: Receive data and put it in a buffer
 *
 *  RETURNS:None
 *
 *****************************************************************************/ 
CY_ISR ( U2RXInterrupt )
{
  char ch;

  while( (ch = UART2_GetChar()) != 0)
  {
    if (ch == XON )
    {
      ok_2_print = TRUE;
    }      
    else if (ch == XOFF )
    {
      ok_2_print = FALSE;
    } 
    else
    {
      s_Computer_RX.rx_buffer[s_Computer_RX.buffer_end++] = ch;
      if ( s_Computer_RX.buffer_end >= BUFF_SIZE )
      {
       s_Computer_RX.buffer_end = 0;
      }
    }  
  }
}

/******************************************************************************
 *
 *  initUARTS()
 *
 *  PARAMETERS:None
 *
 *  DESCRIPTION: Initializes UART2, the UART for PC comms.
 * 
 *               UART2 = 9600 BAUD   N,8,1
 *               Makes an external interrupt.
 *               
 *
 *  RETURNS:    None
 *
 *****************************************************************************/ 
void initUARTS ( void )
{	
   
  UART2_Start();
  isrUART2_StartEx ( U2RXInterrupt ); // assign the ISR, and enable it
  isrUART2_Disable();  // Keep the RX receive ISR running
  UART2_Stop();
  
} 
