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
/* ========================================
 * Copyright InstroTek Inc., 2014
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF InstroTek Inc.
 * ========================================*/
#include "Project.h"
#include "Globals.h"
#include "elite.h"


extern uint16 const countTime ;

volatile uint32 pulseCounts[2];
volatile BOOL cntDone = FALSE;

#define rxCountBufSize 128
static char rxCountBuf[rxCountBufSize];
static volatile int16 rxCountIndx = 0;
char count_Buff[rxCountBufSize];       // receive string storage buffer
 
static  uint32 gm_count[6];
static  uint8  counts_received;
void parseCountString();

// After end of one shot pulse, read the remainder in the counters
// Set the global countdown flag equal to TRUE
CY_ISR(ISR_ONESHOT)
{   
  cntDone = TRUE;
  ONE_SHOT_TIMER_Stop();
}


/*******************************************************************************
* Function Name: CY_ISR(COUNT_RX_ISR)
********************************************************************************
* Summary: captures and parses count strings from the baseboard
*          $1000,1001,1003,2004,30005,6,\r\n  is an example of the data received
*          Max length should be about 44 characters.
*
* Parameters:  none
* Return: 		none
*******************************************************************************/
CY_ISR ( COUNT_RX_ISR ) 
{

	char ch;	
 
  ch = UART_COUNT_ReadRxStatus();
  
	while( (ch = UART_COUNT_GetChar()) != 0)
	{
		rxCountBuf[rxCountIndx] = ch;
    // Count string is terminated by "\r\n"
    if ( rxCountBuf[rxCountIndx] == '\n' || rxCountIndx >= rxCountBufSize - 1)
		{
      if( rxCountBuf[0] == '$' && rxCountBuf[rxCountIndx-1] == '\r' ) // there is a valid line of count data.
      {
        // get the 6 counts data
        memcpy ( count_Buff, rxCountBuf+1 , rxCountIndx-2 );
        parseCountString();
		  }
      rxCountIndx = 0;
	 }
   else
   {
     rxCountIndx++;
     if ( rxCountIndx ==  rxCountBufSize )
     {
      rxCountIndx = 0;
     }
    }
  }
}


/*
 *  FUNCTION: parseCountString
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION: parse the received count string to get the 6 counts.
 *
 *  RETURNS:
 *
 */

void parseCountString()
{
 uint16   i,j;
 uint8 comma_pos[13];
 //uint8 cs;

 for ( i = 0; i < 6; i++ )
 {
  gm_count[0] = 0;
 }

  j = 0;

  // find the position in count_Buff of the 6 commas
  for ( i = 0; i < rxCountBufSize; i++ )
  {
    if ( count_Buff[i] == ',' )
    {
      comma_pos[j++] = i+1;
      count_Buff[i] = '\0'; //    Put null into string
    }  
  // there are 6 commas.  
  if ( j == 7 )
     break;
 } 

  // update the count values
  if ( j == 7 )
  {
    gm_count[0] = atoi(&count_Buff[ 0 ]);
    gm_count[1] = atoi( &count_Buff[ comma_pos[0] ] );
    gm_count[2] = atoi( &count_Buff[ comma_pos[1] ] );
    gm_count[3] = atoi( &count_Buff[ comma_pos[2] ] );
    gm_count[4] = atoi( &count_Buff[ comma_pos[3] ] );
    gm_count[5] = atoi( &count_Buff[ comma_pos[4] ] );  
    //cs = atoi( &count_Buff[ comma_pos[4] ] ); 
    counts_received = 1;
  }


}

/*******************************************************************************
* Function Name: 
********************************************************************************
* Summary:
*     
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void initOneShotTimer ( void )
{
	one_shot_timer_clock_Start(); // turn on the clock
	ONE_SHOT_TIMER_Start();       // Configure the timer
  isrOneShot_ClearPending();
  isrOneShot_StartEx(ISR_ONESHOT);
  ONE_SHOT_TIMER_Stop();

}


/*******************************************************************************
* Function Name: 
********************************************************************************
* Summary:
*     
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void setOneShotTimeMs ( uint32 one_shot_time_ms )
{
  	uint32 scale = (PULSETIMERCLK/1000);
    
    ONE_SHOT_TIMER_WritePeriod ( one_shot_time_ms * scale + 1);
}


/*******************************************************************************
* Function Name: 
********************************************************************************
* Summary:
*     
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void triggerOneShotPulse ( uint32 one_shot_time_ms )
{
	
  ONE_SHOT_TIMER_Stop();
  ONE_SHOT_TIMER_ClearFIFO();
  isrOneShot_ClearPending();
  setOneShotTimeMs ( one_shot_time_ms );

  ONE_SHOT_RESET_Write(1);			// Put ONE_SHOT in reset
  ONE_SHOT_RESET_Write(0);			// Take ONE_SHOT out of reset
  ONE_SHOT_TIMER_Start();  
}


/*******************************************************************************
* Function Name: 
********************************************************************************
* Summary:
*     
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void stop_ONE_SHOT_Early ( void )
{
  ONE_SHOT_TIMER_Stop();
  ONE_SHOT_TIMER_ClearFIFO();
  ONE_SHOT_TIMER_WriteCounter(0);
  ONE_SHOT_RESET_Write(1);			// Put ONE_SHOT in reset
  isrOneShot_ClearPending();
}



/*******************************************************************************
* Function Name: void moist CntStrt(uint8 timeSec)
********************************************************************************
* Summary: 		sets the timer period to match the given time, 
				resets and starts the timer as a one shot.
* Parameters:   int8 timeSec time to run test in seconds, warmup time in ms
* Return: 		none
*******************************************************************************/
void initPulseCntStrt( void )
{
  initOneShotTimer();
  ONE_SHOT_TIMER_Start();
  UART_COUNT_Start();
  UART_COUNT_ClearRxBuffer();
  isrCount_RX_StartEx( COUNT_RX_ISR );
  
}


/*******************************************************************************
* Function Name: PulseCntStrt( uint16 timeSec )
********************************************************************************
* Summary: 		sets the timer period to match the given time, 
*     				resets and starts the timer as a one shot.
* Parameters:  timeSec time to run test in seconds
*
* Return: 		none
*******************************************************************************/
void PulseCntStrt( float timeSec )
{
	uint32 ms = (uint32)(timeSec * 1000.0); 
  uint8 i;
 	cntDone = FALSE;
  
  // clear counts to zero
  for ( i = 0; i < 6; i++ )
  {
    gm_count[i] = 0;
  }  
  
  // reset the rx count receive to 0 bytes.
  UART_COUNT_ClearRxBuffer();
  rxCountIndx = 0;
  counts_received = 0;
  
  // send out the one shot pulse
  triggerOneShotPulse ( ms );
  CyDelay ( 10 );
    
}


uint8 checkCountDone ( void )
{

  return cntDone;
}

uint8 checkCountReceived ( void )
{
   return counts_received;
}

void resetPulseTimers ( void )
{
  Global_ID();   
  TIMER_1_WriteCounter ( 0 );
  isrTIMER_1_ClearPending();
  TIMER_1_Start();
  isrTIMER_1_Enable();
  Global_IE();                // turn on all enabled interrupts 
}

uint32 getGMPulseCounts_1 ( void )
{

  pulseCounts[PROBE_GM1_COUNT] = 0;
  pulseCounts[PROBE_GM1_COUNT] += gm_count[0];
  pulseCounts[PROBE_GM1_COUNT] += gm_count[1];
  pulseCounts[PROBE_GM1_COUNT] += gm_count[2];  
  
  return pulseCounts[PROBE_GM1_COUNT];
}

uint32 getGMPulseCounts_2 ( void )
{
  pulseCounts[PROBE_GM2_COUNT] = 0;
  pulseCounts[PROBE_GM2_COUNT] += gm_count[3];
  pulseCounts[PROBE_GM2_COUNT] += gm_count[4];
  pulseCounts[PROBE_GM2_COUNT] += gm_count[5];  
  
  return pulseCounts[PROBE_GM2_COUNT];
  
}

void getGMPulseCounts ( uint32 count_array[] )
{
  uint8 i;
  for ( i = 0; i < 6; i++ )
  {
    count_array[i] = gm_count[i];
  }  
}

/* [] END OF FILE */

/* [] END OF FILE */
