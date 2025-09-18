/*******************************************************************
 *
 *    FILE: gps.c
 *
 *    DESCRIPTION:
 *
 *    AUTHOR: 
 *
 *    HISTORY: 10/1/2021 Added Support for SAM UBLOX module
 *                       Requires the NMEA strings that are not needed to be turned off
 *                       Adds support for teh differential gps fix '2'
 *
 *******************************************************************/

/** include files **/
#include "Globals.h"
#include "LCD_drivers.h"
#include "Keypad_functions.h"
#include "prompts.h"

#define rxBufSize 256

char gps_parse_error = 0;

static char rxBuf[rxBufSize];
static volatile int16 rxIndx = 0;
char GGA_data = 0;

GPSDATA gdata;

char gps_Buff[rxBufSize];       // receive string storage buffer

void checkGPSFix();
void parseGPSString();
void disableGPS ( void ) ;
void initGPS ( void );
extern void pulseBuzzer ( void );

/*******************************************************************************
* Function Name: CY_ISR(GPS_RX_ISR)
********************************************************************************
* Summary: captures and parses nmea strings from gps, fills in GPS_DATA_t gps data
* Parameters:  none
* Return: 		none
*******************************************************************************/
CY_ISR(GPS_RX_ISR) 
{

	char ch;	
 
  ch = UART_GPS_ReadRxStatus();
  
	while( (ch = UART_GPS_GetChar()) != 0)
	{
		rxBuf[rxIndx] = ch;
    // NMEA string is terminated by "\r\n"
    if ( rxBuf[rxIndx] == '\n' || rxIndx >= rxBufSize - 1)
		{
 
      if( rxBuf[0] == '$' && rxBuf[rxIndx-1] == '\r' ) // there is a valid line of GPS data.
      {
       if ( (memcmp ( "$GPGGA", &rxBuf[0] ,6 )  == 0 ) ||  (memcmp ( "$GNGGA", &rxBuf[0] ,6 ) == 0) ) // SAM module has new GGA data name
       {
        // get GGA data
        rxBuf[rxIndx-1] = '\0'; // null terminate GPS string
        memcpy ( gps_Buff, rxBuf , rxIndx );
        GGA_data = 1;
        checkGPSFix();
        parseGPSString();
       }
		  }
      rxIndx = 0;
	  }
    else
    {
     rxIndx++;
     if ( rxIndx ==  rxBufSize )
     {
      rxIndx = 0;
     }
    }
  }  
}



/*
 *  FUNCTION: clearGPSData
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION: Clear all the GPS reading data

 *  RETURNS:
 *
 */

void clearGPSData ( )
{
  gdata.latitude =  0; 
  gdata.longitude = 0;
  gdata.sats = 0;
  gdata.altitude = 0;
  gdata.fix = ' ';
  
}  



/*
 *  FUNCTION: checkGPSFix
 *
 *  PARAMETERS: gps_Buff contains a null terminated GGA string
 *
 *  DESCRIPTION: parse the GGA received string (NEMA 0183 complaint)
 *               and check fix
 *  RETURNS:
 *
 */

void checkGPSFix()
{
   uint16   i,j;
   uint8 comma_pos[13];

   i = 0;
   j = 1;

   // find the position in gps_Buff of the first 7 comma's
   for ( i = 0; i < rxIndx; i++ )
   {
      if ( gps_Buff[i] == ',' )
      {
        comma_pos[j++] = i;
      }  
    
    if ( j == 7 )
    break;
    
    if ( gps_Buff[i] == 'M' ) 
    {
      break;
    }  
    
  } 

 if (  j == 7 ) 
 { // TYPE OF FIX is located between comma's 6 and 7
    gps_fix =  gps_Buff[comma_pos[6]+1];
    if  ( gps_fix != '1' && gps_fix != '0'  && gps_fix != '2' ) // differential signal is '2'
    {
      gps_fix = ' ';
    }
    else
    {
     gps_parse_error = 0;
    }
 }
 else
 {
 
  gps_parse_error++;
  if ( gps_parse_error > 10 )
  {
   gps_fix = ' ';
   // reset GPS
   disableGPS();
   CyDelay (500); 

   initGPS();
  }
 }

}

/*
 *  FUNCTION: parseGPSString
 *
 *  PARAMETERS: gps_Buff contains a null terminated GGA string
 *
 *  DESCRIPTION: parse the GGA received string (NEMA 0183 complaint)
 *               data is comma delimited
 *  RETURNS:
 *
 */

void parseGPSString()
{
   uint16   i,j;
   uint8 comma_pos[13];
   char g_string[10];
   float long_minutes, long_decimal_minutes, long_degrees;
  float lat_minutes, lat_decimal_minutes, lat_degrees;
   i = 0;
   j = 1;

   // find the position in gps_Buff of the first 10 comma's
   for ( i = 0; i < rxBufSize; i++ )
   {
      if ( gps_Buff[i] == ',' )
      {
        comma_pos[j++] = i;
      }  
    
    if ( j == 11 )
    break;
    
    if ( gps_Buff[i] == 'M' ) 
    {
      break;
    }  
    
  } 

  if ( j <= 11 )
  {
   // latitude is located between comma's 2 and 3
   // get the decimal minutes
   g_string[6] = '\0'; 
   g_string[5] = '\0';  
   g_string[4] = '\0';  
   
   i = 0;
   for ( j = comma_pos[2]+5; j<comma_pos[3] ; j++ )
   {
    g_string[i++] =  gps_Buff[j];
   } 
    lat_decimal_minutes = atof ( g_string );
  
   // get the minutes
   g_string[2] = '\0';
   g_string[1] =  gps_Buff[comma_pos[2]+4];
   g_string[0] =  gps_Buff[comma_pos[2]+3];
   lat_minutes = atof ( g_string );
  
   lat_minutes += lat_decimal_minutes;
   lat_minutes /= 60;
  
   // get the degrees
   g_string[2] = '\0';
   g_string[1] =  gps_Buff[comma_pos[2]+ 2];
   g_string[0] =  gps_Buff[comma_pos[2]+ 1];
   lat_degrees = atof ( g_string );
  
   lat_degrees += lat_minutes;
  
   // latitude hemisphere is located between comma's 3 and 4
   // get the hemispere (N or S)
   if ( gps_Buff[comma_pos[4]-1] == 'S' )
   {
    lat_degrees *= -1.0;
   } 

   gdata.latitude = lat_degrees;
  
   // longitude is located between comma's 4 and 5
   // get the decimal minutes
   g_string[6] = '\0'; 
   g_string[5] = '\0';
   i = 0;
   j = comma_pos[5]-comma_pos[4];
   
   if ( j < 13 )
   {
     for ( j = comma_pos[4]+6; j<comma_pos[5] ; j++ )
     {
      g_string[i++] =  gps_Buff[j];
     } 
   } 
  long_decimal_minutes = atof( g_string );
 
   // get the minutes
   g_string[2] = '\0';
   g_string[1] =  gps_Buff[comma_pos[4]+5];
   g_string[0] =  gps_Buff[comma_pos[4]+4];
   long_minutes = atof(g_string);
   long_minutes += long_decimal_minutes;
   long_minutes /= 60;
  
   // get the degrees
   g_string[3] = '\0';
   g_string[2] =  gps_Buff[comma_pos[4]+3];
   g_string[1] =  gps_Buff[comma_pos[4]+2];
   g_string[0] =  gps_Buff[comma_pos[4]+1];
   long_degrees = atof ( g_string );
   long_degrees += long_minutes;
  
   // longitude hemisphere is located between comma's 5 and 6
   // get the hemispere (E or W)
   if ( gps_Buff[comma_pos[6]-1] == 'W' )
   {
     long_degrees *= -1.0 ;
   }

    gdata.longitude = long_degrees;
  
   // TYPE OF FIX is located between comma's 6 and 7
   gdata.fix =  gps_Buff[comma_pos[7]-1];
   gdata.fix = gdata.fix - 0x30;            // convert to a byte value from ascii

   // get number of received satellites  ( 7 and 8 0
   g_string[2] = '\0';
   g_string[1] = gps_Buff[comma_pos[8] - 1];
   g_string[0] = gps_Buff[comma_pos[8] - 2];
   gdata.sats = atoi(g_string);

   // altitude is located between comma's 9 and 10
   j = 0;
   for ( i = comma_pos[9]+1; i < comma_pos[10]; i++ )
   {
      g_string[j++] = gps_Buff[i];               // copy altitude ( -9999.9 to 99999.9 meters)
      g_string[j] = '\0';                        // null at end of string
   }

   gdata.altitude =  atoi(g_string);           // covert the altitude,decimal will be trunc.
 }

}
  
/*
 *  FUNCTION: turnoffStrings
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION: SAM-M8Q module has multiple NMEA strings that are automaticcaly output.
 *               This function uses the Ublox UBX commands to turn them off.
 *  RETURNS:
 *
 */
void turnoffNMEAStrings ( void )
{
 uint8 n = 0;
    Global_ID();
  for ( n=0; n< 2;n++)
  {
  UART_GPS_PutString ( "$PUBX,40,VTG,0,0,0,0,0,0*5E\r\n");
  CyDelay ( 50 );
  UART_GPS_PutString ( "$PUBX,40,RMC,0,0,0,0,0,0*47\r\n");
  CyDelay ( 50);
  UART_GPS_PutString ( "$PUBX,40,GSV,0,0,0,0,0,0*59\r\n");
  CyDelay ( 50);
  UART_GPS_PutString ( "$PUBX,40,GSA,0,0,0,0,0,0*4E\r\n");
  CyDelay ( 50 );
  UART_GPS_PutString ( "$PUBX,40,GLL,0,0,0,0,0,0*5C\r\n");
  CyDelay ( 50 );
  //UART_GPS_PutString ( "$PUBX,40,GGA,0,0,0,0,0,0*5A\r\n");
 } 
  Global_IE();
}

void initGPS ( void ) 
{
  UART_GPS_Start();
  isrGPS_RX_StartEx( GPS_RX_ISR);
  UART_GPS_Stop();
  rxIndx = 0;
  GGA_data = 0 ;
   
  GPS_EN_REG_Write ( 0 );
  UART_GPS_Start();
  turnoffNMEAStrings();
  
  clearGPSData ();
  
  

}


void disableGPS ( void ) 
{
  UART_GPS_Stop();        // stop the UART
  GPS_EN_REG_Write( 1 );  // power down the GPS 
  isrGPS_RX_Disable();    // diable the interrupt
}

  
/*
 *  FUNCTION: gps_test
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION: Display the GPS results on the screen
 *               
 *  RETURNS:
 *
 */
void gps_test ( void )
{
  //uint8 csum= 0;
  char buff[25];
  initGPS();
  /*
  snprintf( buff,25,"$PMTK251,38400");
  i = strlen ( buff );
  
  for( n=1;n < i; n++ )
  {
    csum ^= buff[n];
  }
  snprintf ( buff2, 10,"%X*\r\n",csum);
  strcat( buff,buff2 );
  
  UART_GPS_PutString ( buff );
  CyDelay ( 2000 );
  UART_GPS_PutString ( buff );
  CyDelay ( 2000 );
  */
  wait_for_key_release();
  global_special_key_flag = FALSE;
  CLEAR_DISP;
  ESC_to_Exit(LINE4);               //  "ESC to Exit"
  while ( global_special_key_flag == FALSE )
  {
     if (( gps_fix == '1' ) || ( gps_fix == '2') )
     {
      Global_ID();
      parseGPSString();
      Global_IE();
      snprintf( buff,20,"LAT: %9.6f",gdata.latitude);
      LCD_PrintAtPosition ( buff, LINE1 );
      
      snprintf( buff,20,"LNG: %9.6f", gdata.longitude);
      LCD_PrintAtPosition ( buff, LINE2 );      
      
      if  ( gps_fix == '1')
      {
        snprintf( buff,20,"ALT:%u SAT:%u F:S  ", gdata.altitude, gdata.sats); // display S if differential satellite not detected
      }
      else
      {
        snprintf( buff,20,"ALT:%u SAT:%u F:D  ", gdata.altitude, gdata.sats);// display D if differential satellite detected
      }
      
      LCD_PrintAtPosition ( buff, LINE3 ); 
      
     }
     else if ( gps_fix == '0' )
     {
      LCD_position(LINE1);
      _LCD_PRINT("No GPS FIX          ");
      LCD_position(LINE2);
      _LCD_PRINT("                    ");
      LCD_position(LINE3);
      _LCD_PRINT("                    ");
     }
     else
     {
      LCD_position(LINE1);
      _LCD_PRINT("No GPS String        ");
      LCD_position(LINE2);
      _LCD_PRINT("                    ");
      LCD_position(LINE3);
      _LCD_PRINT("                    ");
     }

     delay_ms ( 1000 );
    
  }

}

