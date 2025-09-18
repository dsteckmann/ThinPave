


/**********************************************************************************************************************/
//
// Title:       Explorer II
// Company:     Instrotek
//
// Document No.:
// Issue:
// Description:  
//
// Filename:   
// Author:      
//
// Processor:     
// Clock:	    
//
//
// History: date, version, personal initials with comments about any changes
//
/**********************************************************************************************************************/

/*********************************************  INCLUDE FILES  ***************************************************/

#include <project.h>
#include "Globals.h"
#include "DataStructs.h"
#include "Utilities.h"
#include "StoreFunctions.h"
#include "conversions.h"
#include "Alfat.h"
#include "prompts.h"
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "Batteries.h"
#include "UARTS.h"
#include <math.h>
#include "Tests.h"


/************************************* EXTERNAL VARIABLE AND BUFFER DECLARATIONS  *************************************/
extern void PulseCntStrt( float timeSec );
extern void resetPulseTimers ( void );
extern void stop_ONE_SHOT_Early ( void );
extern uint32 getGMPulseCounts_2 ( void );
extern uint32 getGMPulseCounts_1 ( void );
extern uint8 checkCountReceived ( void );
extern void getGMPulseCounts ( uint32 count_array[] );

/************************************************  LOCAL DEFINITIONS  *************************************************/
#define DENSITY_PASS_PERCENT    .01

#define PRESCALE_7_5  1                   // prescale is 7.5s
#define PRESCALE_15   2                   // prescale is 7.5s
#define PRESCALE_30   (PRESCALE_15 * 2)
#define PRESCALE_60   (PRESCALE_15 * 4)
#define PRESCALE_120  (PRESCALE_15 * 8)
#define PRESCALE_240  (PRESCALE_15 * 16)


#define SMART_MC_CHI_COUNTS 32
#define STD_COUNT_DELAY 450

/*****************************************  VARIABLE AND BUFFER DECLARATIONS  *****************************************/
 int32_t  stat_dense_avg[6];
 int32_t  drift_dense_avg[6]; 
 uint32_t stat_dense[20][6];
  uint32_t drift_dense2[20];
 uint32_t drift_dense1[20];
 fp32_t   stat_dense_ratio[6];
 fp32_t   stat_dense_ratio[6];
 
 uint8_t  stat_pass_fail; 
 float    drift_dense_per[6];
 uint32_t drift_dense[5][6];

/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 
 void storeDriftResultsToUSB ( Bool display_error )
 {
  char name_string[30];
  char temp_str[60];
  FILE_PARAMETERS fp;  
  
  AlfatStart();   
  
  isrTIMER_1_Disable();
  
  // If the drive can be mounted, data will be stored.
  
  if ( initialize_USB( display_error ) )
  {
    //"Writing Drift Result"
    writing_drift_results_text();
    CyDelay ( 1500 );
    // make the file name string
    nullString(name_string, sizeof(name_string) );
    strcat( name_string, "DRIFT_RESULTS" );
    
    // open a file
    USB_open_file  ( name_string, &fp );  
    for ( uint8 k = 0; k < 6; k++ )
    {
      nullString(temp_str, sizeof(temp_str));
      sprintf( temp_str,"TUBE %u %% Drift = %.2f %c\r\n", k+1, (double)drift_dense_per[k], (drift_dense_per[k] <=0.5) ? 'P' : 'F');
      AlfatWriteStr(&fp,temp_str);
           
      nullString(temp_str, sizeof(temp_str));
      sprintf( temp_str,"TUBE %u Avg = %lu\r\n",k+1,(uint32)drift_dense_avg[k]);
      AlfatWriteStr(&fp,temp_str);
      
      for ( uint8 i = 0; i < 5; i++ )
      {
        nullString(temp_str, sizeof(temp_str));
        sprintf ( temp_str,"#%u TUBE%u %lu\r\n",i+1,k+1,(uint32) drift_dense[0][k]);
        AlfatWriteStr(&fp,temp_str);
      }  
  
    }
    // close file     
     AlfatFlushData(fp.fileHandle);
     AlfatCloseFile(fp.fileHandle);  
  }   
  
  AlfatStop();   
  isrTIMER_1_Enable();
 
 }
 
/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 
 void storeStdCountsToUSB ( Bool display_error )
 {
  char date_string[30];
  char name_string[30];
  char temp_str[60];
  
  uint8_t i, j, number_of_counts, index;
  uint16_t DSC1, DSC2;
  date_time_t TSC;
  FILE_PARAMETERS fp;  
  
  // Get the number of STD counts stored in memory
  index = NV_RAM_MEMBER_RD(stand_test.std_index); // index is always 1-30
  index -= 1;  //match index to array element number
  number_of_counts = NV_RAM_MEMBER_RD(stand_test.std_counts);   
 
  if(number_of_counts != 0)
  {        
    
    AlfatStart();   
    
     isrTIMER_1_Disable();
    
    // If the drive can be mounted, data will be stored.
    
    if ( initialize_USB( display_error ) )
    {
      CLEAR_DISP;
      
      LCD_PrintAtPosition( "Writing STD Counts", LINE2);
      
      // make the file name string
      nullString(name_string, sizeof(name_string) );
      strcat( name_string, "STD_COUNTS" );
      
      // open a file
      USB_open_file  ( name_string , &fp);  
    
      i = index;
      j = 0;
      
      nullString(temp_str, sizeof(temp_str));
      sprintf( temp_str, "Density1\tDensity2\tDate\t\r\n" );
      AlfatWriteStr(&fp,temp_str);
     do   //read all valid counts from memory
     {
      DSC1 = NV_RAM_MEMBER_RD(stand_test.dense_count1_x[i] );  //each count is 2 bytes long
      DSC2 = NV_RAM_MEMBER_RD(stand_test.dense_count2_x[i] );  //each count is 2 bytes long
      TSC = NV_RAM_MEMBER_RD(stand_test.date_count_x[i]  );   //each date is 4 bytes long
     
      getTimeDateStr ( TSC, date_string );     
      nullString(temp_str, sizeof(temp_str));
      sprintf( temp_str, "%u\t%u\t%s\r\n", DSC1, DSC2, date_string );
      AlfatWriteStr(&fp,temp_str);
      
      i = (i!=0) ? (i-1) : 29;
      j++;
    
     } while((i!=index) && (j<number_of_counts));  //index is array element with most recent data      
   
    
    // close file     
     AlfatFlushData(fp.fileHandle);
     AlfatCloseFile(fp.fileHandle);    
    }   
   AlfatStop();   
   
    isrTIMER_1_Enable();
 
  }
 }
 
 



/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 
 void storeStatTestResultsToUSB ( Bool display_error )
 {
  char date_string[30];
  char name_string[30];
  char temp_str[30];
  FILE_PARAMETERS fp;  
  uint8_t n;
  
  AlfatStart();   
  
  // If the drive can be mounted, stat test data will be stored.
  if ( initialize_USB( display_error ) )
  {
     CLEAR_DISP;
    
     LCD_PrintAtPosition( "Writing Stat Results", LINE2);
    // make the file name string
    nullString(name_string, sizeof(name_string) );
    strcat( name_string, "StatResults_" );
    
    // get the current time, and turn into a string
    read_RTC( &date_time_g );
    getTimeDateStr ( date_time_g, date_string );
    
    // replace the ":" with a "_"
    for ( n = 0; n < strlen(date_string); n++ )
    {
      if ( ( date_string[n] == ':' ) || (date_string[n] == '/') )
      {
        date_string[n] = '_';
      }
    }
    
    // add date to string
    strcat( name_string, date_string );
    // open a file
    USB_open_file  ( name_string, &fp );  
  
    // make header
    // get the current time, and turn into a string
    read_RTC( &date_time_g );
    getTimeDateStr ( date_time_g, date_string );
    sprintf( temp_str, "%s\t\r\n", date_string );
    AlfatWriteStr(&fp,temp_str);
 
 
    for ( uint8 k = 0; k < 6; k++ )
    {

      nullString(temp_str, sizeof(temp_str));
      sprintf( temp_str, "Tube %u Counts\t\r\n",k+1 );
      AlfatWriteStr(&fp,temp_str);
      for ( n = 0; n < 20; n++ )
      {
        
        LCD_position(LINE3);
        _LCD_PRINTF ( "TUBE:%2u  ",k+1);
        _LCD_PRINTF ( "CNT:%2u ",n+1);
        nullString(temp_str, sizeof(temp_str));
        sprintf( temp_str, "%ld\t\r\n", (uint32)stat_dense[n][k] );
        AlfatWriteStr(&fp,temp_str);
      }  

      nullString(temp_str, sizeof(temp_str));
      sprintf( temp_str, "TUBE %u Average\t %lu\t\r\n", k+1, (unsigned long)stat_dense_avg[k] );
      AlfatWriteStr(&fp,temp_str);
      
      nullString(temp_str, sizeof(temp_str));
      sprintf( temp_str, "TUBE %u Ratio\t %.2f\t\r\n", k+1,stat_dense_ratio[k] );
      AlfatWriteStr(&fp,temp_str);

    }  
  
       AlfatFlushData(fp.fileHandle);
       AlfatCloseFile(fp.fileHandle);   
  }   

  AlfatStop();   
 
  ADC_DelSig_Start();
  UART_GPS_Start();

  isrTIMER_1_Enable();
 }

 
/******************************************************************************
 *
 *  Name: measurePulses ( )
 *
 *  PARAMETERS: time is secs
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: 
 *
 *****************************************************************************/ 

void measurePulses ( uint8_t line, uint8_t time1, uint32_t * density_2_count, uint32_t * density_1_count )  
// acquires sys 1 and 2 density count during tests 
{
  uint16_t  i = 0;
  uint8_t  LCD_line;  //line3+11   
  uint32_t density_1_temp, density_2_temp,div_by; 
  uint8_t batt_flag;
  
  // Save the state of the auto turn off flag
  BOOL temp_auto_turn_off = Spec_flags.auto_turn_off;
  
  LCD_line = line + 11;
  
  enum buttons button = DFLT;
 
  Controls.reset_count = FALSE;                            // clear skip over code bit
  
  if ( Flags.bat_volt == 1 )
  {
    batt_flag = 1;
  }
  else
  {
    batt_flag = 0;
  }  
  
  Flags.bat_volt = 1;   // Don't do background battery monitoring
 
  delay_ms(10);
   wait_for_key_release();
  // check for gauge temperature on the Xplorer2. 
  if (Features.dummy_mode == FALSE) 
  {
    check_temp(0);                                     // check internal temperature of gauge
  }
  
  Spec_flags.auto_turn_off = FALSE;                  // disable auto_turn_off during count
  
  // Set the prescale value
  switch ( time1 )
  {
     case 5:
     case 120: div_by = PRESCALE_120;
              break;
     case 30: div_by = PRESCALE_30;
              break;
     case 60: div_by = PRESCALE_60;
              break;
     case 240:div_by = PRESCALE_240;
              break;
    
     default: div_by = PRESCALE_60;
             break;
  } 
  
    LCD_position(LINE1);
   
    if(Flags.stand_flag)
    {
      std_text();                                     //TEXT// display "Standard Count"      
    }    
    else if(Flags.stat_flag)
    {
       stat_text();                                    //TEXT// display "Stat Test"     
    }    
    else if(Flags.drift_flag)
    {
       drift_text();                                   //TEXT// display "Drift Test"      
    }    
   
      
    if( (Features.auto_store_on) && (Flags.in_spec_cal == FALSE ) )
    {
     LCD_position(LINE1);
     current_project_text( project_info.current_project );  
     LCD_position(LINE2);
     display_station_name(project_info.current_station_name);         //TEXT// display "Station: %s",current_station               
    }
    else   // Display Time 
    {
     LCD_position(LINE1);
     read_RTC( &date_time_g ); 
     printTimeDate ( date_time_g );
    }
   
    display_time(time1,line );
    
    //TEXT// display "Time: %3u sec. " on LCD_LINE   
     if ( Offsets.den_offset_pos )
    { 
        display_offset( Offsets );                               //TEXT// display "Offset:"  on LINE4      
    }
   
  if ( !Features.language_f )      // if in spanish mode
  {
    LCD_line +=2;       // LINE3+8
  }    

  Controls.update_time = FALSE;
  LCD_timer = 0;  
 
  resetPulseTimers ( );
  PulseCntStrt( time1 );
  
  i = 0;
  while ( checkCountDone() == FALSE ) 
  {       
    uint32 timer;
    CyDelay ( 250 );
    i++;      

      if( !Spec_flags.self_test )
      {
        if ( i==40 )                  //turn off backlight after 10 seconds if it's on
      	{
          KEY_B_LIGHT_DISABLE ();
  	    };
  	
        button = getLastKey();
        if ( (!Flags.stat_flag && (button == ENTER) ) || (button == ESC) )          
        {
          if(button == ENTER)               // ENTER was pressed, reset count
          {
            Controls.reset_count = TRUE;    // set skip over code bit         
          }    
          break;
        }
        if ( checkCountDone() == FALSE ) 
        {
          // The one shot counts at 1000HZ.
          timer = ONE_SHOT_TIMER_ReadCounter (); 
          timer /= PULSETIMERCLK;
          LCD_position (LCD_line);
          _LCD_PRINTF ( "%3u", time1 - (uint8_t)timer);
        }  
       }
   }                               

  // loop while waiting for counts
  if ( checkCountDone() == TRUE )  //count completed 
  {
    uint8 k = 0;
    while ( checkCountReceived() == FALSE )
    {
      CyDelay ( 100 );
      k++; 
      if ( k > 15 )
        break;
    }
    if ( checkCountReceived() == TRUE )
    {
     CyDelay ( 100 );
    }
  }
  
  
  if ((checkCountDone() == TRUE ) || ( checkCountReceived() == TRUE ))  //count completed 
  {
    density_1_temp  =  getGMPulseCounts_1 (); 
    density_2_temp  =  getGMPulseCounts_2 (); 

    
    *density_1_count  = (uint32_t)(density_1_temp/div_by);
    *density_2_count  = (uint16_t)(density_2_temp/div_by);   
    
    if ( !Spec_flags.self_test )
    {
     NV_MEMBER_STORE(D2_CNT_AVG, *density_2_count);
     NV_MEMBER_STORE(D1_CNT_AVG, *density_1_count);
    }   
  }
  else
  {
    // Counts not received error
    error_in_counting_text();
    CyDelay ( 2000 );
  }
  
  if ( button != ENTER )               // either test ended or, esc was pressed to exit
  {           
    stop_ONE_SHOT_Early();
    
  }  
 
  if ( batt_flag == 1 )
  {
    Flags.bat_volt = 1;
  }
  else
  {
    Flags.bat_volt = 0;
  }
  
  Spec_flags.auto_turn_off = temp_auto_turn_off;
  shutdown_timer = 0;    
}


/******************************************************************************
 *
 *  Name: measurePulsesForDensity ( )
 *
 *  PARAMETERS: time is secs
 *
 *  DESCRIPTION: acquires sys 1 and 2 density count during tests 
 *               
 *            
 *  RETURNS: 
 *
 *****************************************************************************/ 

void measurePulsesForDensity ( uint32_t * density_2_count, uint32_t * density_1_count )  
{
  uint16_t  i = 0;
  uint8_t  LCD_line;  //line3+11   
  uint32_t density_1_temp, density_2_temp,div_by; 
  enum buttons button = DFLT;
  uint8_t batt_flag;
  uint32 timer;
  uint16_t time1 = NV_RAM_MEMBER_RD( COUNT_TIME ); // Get the count time
  char buff[21];
  // Save the state of the auto turn off flag
  BOOL temp_auto_turn_off = Spec_flags.auto_turn_off;
  uint8 mode;
  mode = NV_RAM_MEMBER_RD (measure_mode );
  
  wait_for_key_release();
  
  LCD_line = LINE4 + 11;
  
  Controls.reset_count = FALSE;                            // clear skip over code bit
  delay_ms(10);
  check_temp(0);                                     // check internal temperature of gauge
  
  Spec_flags.auto_turn_off = FALSE;                  // disable auto_turn_off during count
  
  
  if ( Flags.bat_volt == 1 )
  {
    batt_flag = 1;
  }
  else
  {
    batt_flag = 0;
  }  
  
  Flags.bat_volt = 1;   // Don't do background battery monitoring
  
  // Set the prescale value
  switch ( time1 )
  {
     case 5:
     case 120: div_by = PRESCALE_120;
              break;
     case 30: div_by = PRESCALE_30;
              break;
     case 60: div_by = PRESCALE_60;
              break;
     case 240:div_by = PRESCALE_240;
              break;
     default: div_by = PRESCALE_60;
             break;
  } 
  
  
      // get current layer thickness
  float  thickness = (double)NV_RAM_MEMBER_RD ( MAT_THICK ) ;   // constant is in mm * 10. (254 = 25.4mm)

  
  CLEAR_DISP;
  LCD_position(LINE1);
  
  // Display Mode and THickness

  if ( mode != PROFILE_DENSITY_MODE )
  {  // display thickness
    count_text(7);  //TEXT// display "THICKNESS"
    if ( Features.SI_units == FALSE )
    {
     snprintf( buff,20," %.2f in.",thickness / 25.4);
     LCD_print ( buff ); 
    }
    else 
    {
     snprintf( buff,20," %.2f mm",thickness);
     LCD_print ( buff ); 
    }
  }
  
  // display mode
  if ( mode == METAL_DENSITY_MODE )
  {
  LCD_position (LINE3);
  count_text(9);  //TEXT// display "MODE:THICKNESS"
  }
  else if ( mode == NORMAL_DENSITY_MODE ) 
  {
  LCD_position (LINE3);
  count_text(8);  //TEXT// display "MODE:NORMAL"
  }
  else if ( mode == PROFILE_DENSITY_MODE ) 
  {
    LCD_position (LINE3);
    count_text(10);  //TEXT// display "MODE:NORMAL"
  }
  
  CyDelay ( 1500 );
  CLEAR_DISP;
  
  if( Features.auto_store_on ) 
  {
   LCD_position(LINE1);
   current_project_text( project_info.current_project );  
   LCD_position(LINE2);
   display_station_name(project_info.current_station_name);         //TEXT// display "Station: %s",current_station               
  }
  else   // Display Time 
  {
   LCD_position(LINE1);
   read_RTC( &date_time_g ); 
   printTimeDate ( date_time_g );
  }
  
  display_time ( time1, LINE4 );
  
  //TEXT// display "Time: %3u sec. " on LCD_LINE   

  if ( Offsets.den_offset_pos )
  { 
    display_offset( Offsets );                               //TEXT// display "Offset:"  on LINE4      
  }
  
  Controls.update_time = FALSE;
  LCD_timer = 0;  
 
  // Reset the counting hardware
  resetPulseTimers ( );
  PulseCntStrt( time1 );
  
  i = 0;
  // Counting loop
  while ( checkCountDone() == FALSE ) 
  {       
   CyDelay ( 250 );
    i++;      
    if ( i==40 )                  //turn off backlight after 10 seconds if it's on
  	{
      KEY_B_LIGHT_DISABLE ();
    };

    button = getLastKey();
    if ( ((button == ENTER) ) || (button == ESC) )          
    {
      if(button == ENTER)               // ENTER was pressed, reset count
      {
        Controls.reset_count = TRUE;    // set skip over code bit         
      }    
      break;
    }
    
    if ( checkCountDone() == FALSE ) 
    {
      // The one shot counts at 1000HZ.
      timer = ONE_SHOT_TIMER_ReadCounter (); 
      timer /= PULSETIMERCLK;
      LCD_position (LCD_line);
      _LCD_PRINTF ( "%3u", time1 - (uint8_t)timer);
    }
  }                               

  if ( button != ESC) 
  {
    // loop while waiting for counts
    if ( checkCountDone() == TRUE )  //count completed 
    {
      uint8 k = 0;
      while ( checkCountReceived() == FALSE )
      {
        CyDelay ( 100 );
        k++; 
        if ( k > 15 )
          break;
      }
      if ( checkCountReceived() == TRUE )
      {
       CyDelay ( 100 );
      }
    }
    
    if ( ( checkCountDone() == TRUE ) && ( checkCountReceived() == TRUE )) //count completed 
    {
      density_1_temp  =  getGMPulseCounts_1 (); 
      density_2_temp  =  getGMPulseCounts_2 (); 

      *density_1_count  = (uint32_t)(density_1_temp/div_by);
      *density_2_count  = (uint16_t)(density_2_temp/div_by);   
    }
    else
    {
      // Counts not received from error
      error_in_counting_text();
      CyDelay ( 2000 );
    }
  }
  if ( button != ENTER )               // either test ended or, esc was pressed to exit
  {           
    stop_ONE_SHOT_Early();
  }  
 
  if ( batt_flag == 1 )
  {
    Flags.bat_volt = 1;
  }
  else
  {
    Flags.bat_volt = 0;
  }
  
  Spec_flags.auto_turn_off = temp_auto_turn_off;
  shutdown_timer = 0;    
}

  
void measurePulses6Tubes (  )  
{
  uint16_t  i = 0;
  uint8_t  LCD_line;  //line3+11   
  uint32_t div_by; 
  enum buttons button = DFLT;
  uint8_t batt_flag;
  uint32 timer;
  uint16_t time1 = NV_RAM_MEMBER_RD( COUNT_TIME ); // Get the count time
  
  // Save the state of the auto turn off flag
  BOOL temp_auto_turn_off = Spec_flags.auto_turn_off;
  
  wait_for_key_release();
  
  LCD_line = LINE4 + 11;
  
  Controls.reset_count = FALSE;                            // clear skip over code bit
  delay_ms(10);
                                    // check internal temperature of gauge
  
  Spec_flags.auto_turn_off = FALSE;                  // disable auto_turn_off during count
  
  
  if ( Flags.bat_volt == 1 )
  {
    batt_flag = 1;
  }
  else
  {
    batt_flag = 0;
  }  
  
  Flags.bat_volt = 1;   // Don't do background battery monitoring
  
  // Set the prescale value
  switch ( time1 )
  {
     case 5:
     case 120: div_by = PRESCALE_120;
              break;
     case 30: div_by = PRESCALE_30;
              break;
     case 60: div_by = PRESCALE_60;
              break;
     case 240:div_by = PRESCALE_240;
              break;
    
     default: div_by = PRESCALE_60;
             break;
  } 
  
  CLEAR_DISP;
  LCD_position(LINE1);
  read_RTC( &date_time_g ); 
  printTimeDate ( date_time_g );
  
  display_time ( time1, LINE4 );

  Controls.update_time = FALSE;
  LCD_timer = 0;  
 
  // Reset the counting hardware
  resetPulseTimers ( );
  PulseCntStrt( time1 );
  
  i = 0;
  // Counting loop
  while ( checkCountDone() == FALSE )
  {       
   CyDelay ( 250 );
    i++;      
    if ( i==40 )                  //turn off backlight after 10 seconds if it's on
  	{
      KEY_B_LIGHT_DISABLE ();
    };

    button = getLastKey();
    if ( ((button == ENTER) ) || (button == ESC) )          
    {
      if(button == ENTER)               // ENTER was pressed, reset count
      {
        Controls.reset_count = TRUE;    // set skip over code bit         
      }    
      break;
    }
    
    if ( checkCountDone() == FALSE ) 
    {
      // The one shot counts at 1000HZ.
      timer = ONE_SHOT_TIMER_ReadCounter (); 
      timer /= PULSETIMERCLK;
      LCD_position (LCD_line);
      _LCD_PRINTF ( "%3u", time1 - (uint8_t)timer);
    }
  }                               

  if ( checkCountDone() == TRUE )  //count completed 
  {
    uint8 k = 0;
    while ( checkCountReceived() == FALSE )
    {
      CyDelay ( 100 );
      k++; 
      if ( k > 15 )
        break;
    }
    if ( checkCountReceived() == TRUE )
    {
     CyDelay ( 100 );
    }
  }
  
  if ( Features.sound_on ) 
  { 
    output_high(BUZZER); 
    CyDelay ( 1000 );
    output_low(BUZZER); 
  } 
  
  if ( button == ESC )               // either test ended or, esc was pressed to exit
  {           
    stop_ONE_SHOT_Early();
  }
  else
  {
    displayCounts6 ( div_by );
  }  

  if ( batt_flag == 1 )
  {
    Flags.bat_volt = 1;
  }
  else
  {
    Flags.bat_volt = 0;
  }  
  
  
  
  Spec_flags.auto_turn_off = temp_auto_turn_off;
  shutdown_timer = 0;    
}

/******************************************************************************
 *
 *  Name: clearPulseTimer
 *
 *  PARAMETERS: Reset, then start the pulse timer, pulse counters
 *                
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: 
 *
 *****************************************************************************/

void clearAndStartPulseTimer ( void )
{

  Global_ID();   
  
  LCD_timer = 0;  

  TIMER_1_WriteCounter ( 0 );
  isrTIMER_1_ClearPending();
  TIMER_1_Start();

  Global_IE();                // turn on all enabled interrupts 

  Controls.update_time = FALSE;
}

/******************************************************************************
 *
 *  Name: measurePulses ( )
 *
 *  PARAMETERS: time is secs
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: 
 *
 *****************************************************************************/ 

void measurePulsesStandardCount ( uint8_t line, uint32_t * density_count2, uint32_t * density_count1 )  // acquires density counts during tests 
{
  uint8_t  LCD_line, sec=0;  //line3+11   
  uint8_t batt_flag;
  uint32 timer, i;

  // Save the state of the auto turn off flag
  BOOL temp_auto_turn_off = Spec_flags.auto_turn_off;
  enum buttons button = DFLT;

  // Where to display time
  LCD_line = line + 11;
  
  // don't check battery state
  if ( Flags.bat_volt == 1 )
  {
    batt_flag = 1;
  }
  else
  {
    batt_flag = 0;
  }  
  Flags.bat_volt = 1;   // Don't do background battery monitoring
  
  
  delay_ms(10);
 
  Spec_flags.auto_turn_off = FALSE;                  // disable auto_turn_off during count
  
  // When key is released, stop running the keyscanner
  wait_for_key_release();  
    
  std_count_text();

 //TEXT// display "Time: %3u sec. " on LCD_LINE   
  
  display_time(240,line );
  
  // get the start of the test
  timer = msTimer;   
  LCD_position (LCD_line);
  _LCD_PRINTF ( "%3u ", (uint8)(240.0));
 
  resetPulseTimers ( );
  PulseCntStrt( 240 ) ;
  
  i = msTimer + 1000 ;// add one second to time
  sec = 0;
  while ( checkCountDone() == FALSE )
  {       
     CyDelay ( 50 );
     
     if ( timer + 10000 >  msTimer )
     {
       KEY_B_LIGHT_DISABLE();
		 }; 
    
     // Keyscan is disabled, so check keys here
     button = getLastKey();
     if ( button == ESC )          
     {
       break;
     }
    
     // update every 1 second
     if ( i < msTimer ) 
     {
       LCD_position (LCD_line);
       _LCD_PRINTF ( "%3u", 240 - (uint8) (++sec));
       i = msTimer + 1000; // add one second to time
      } // end of LCD update and keyscan
    
  } 
  
  if ( checkCountDone() == TRUE )  //count completed 
  {
    uint8 k = 0;
    while ( checkCountReceived() == FALSE )
    {
      CyDelay ( 100 );
      k++; 
      if ( k > 15 )
        break;
    }
    if ( checkCountReceived() == TRUE )
    {
     CyDelay ( 100 );
    }
  }
  
  if ( ( checkCountDone() == TRUE ) && ( checkCountReceived() == TRUE ))
  {
    // read in the counts
    uint32 density_1_temp  =  getGMPulseCounts_1 (); 
    uint32 density_2_temp  =  getGMPulseCounts_2 (); 
    *density_count1  = density_1_temp/32;            
    *density_count2  = density_2_temp/32;            
  }  
  else
  {
    // Counts not received from error
    error_in_counting_text();
    CyDelay ( 2000 );
  }
    
 if ( batt_flag == 1 )
 {
   Flags.bat_volt = 1;
 }
 else
 {
   Flags.bat_volt = 0;
 }
  
 Spec_flags.auto_turn_off = temp_auto_turn_off;
 shutdown_timer = 0;    
}


#define STAND_TEST 0
/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: 
 *
 *****************************************************************************/ 

/////TEST FUNCTIONS/////////////////////////////////////////////////////////////
void standard_count_test(void)  // leads user through standard count procedure (STD button initiates)
{
  int8_t tests = 0, tests_tot = 0, index;
  uint32_t density1_cnt,density1_count[4];
  uint32_t density2_cnt,density2_count[4];
  int32_t date_int;
  float percent_diff_dense1, percent_diff_dense2, avg_std_dense2, avg_std_dense1, d1_stand_cal, d2_stand_cal;
  char dense1_pass_fail[10] , dense2_pass_fail[10];
  enum buttons button;
  uint16_t d1_stand;
  uint16_t d2_stand;
  float temp;

  static uint8 dummy_tests = 0;

  sprintf( dense2_pass_fail,"P" );
  sprintf( dense1_pass_fail,"P" );
 
  Flags.stand_flag = TRUE;                           // set "Stand" flag for display format during measuring
  Global_IE();

  Controls.shut_dwn = FALSE;                         // disable shut down feature when NO is pressed 

  d1_stand = NV_RAM_MEMBER_RD(DEN1_STAND);             //read the last stored density1 stand from eeprom 

  d2_stand = NV_RAM_MEMBER_RD(DEN2_STAND);           //read the last stored density 2 stand from eeprom   

  CLEAR_DISP;
  LCD_position(LINE1);
  
  if ( d1_stand !=1 )
  {
    _LCD_PRINTF ( "DS 1 = %u",d1_stand);  
  }  
  else 
  {
    _LCD_PRINT ( "DS 1 = 0");
  }  
  
  LCD_position(LINE2);
  if ( d2_stand !=1 )
  {
    _LCD_PRINTF ( "DS 2 = %u", d2_stand );  
  }  
  else 
  {
    _LCD_PRINT ( "DS 2 = 0");
  }  
  
  new_std_count();         // display "New STD Count?" on LINE3
  Press_YES_or_NO(LINE4);  // display "Press YES or NO"  
  
  while(1)
  {
    button = getKey ( TIME_DELAY_MAX );
    
    if((button == NO) || (button == ESC))                                // NO or ESC was pressed, exit to ready screen
    {
      break;
    }  
    else if(button == YES)
    {
      
      std_count_press_start();  // display "  Press START for\n  Standard Count" 
      while(1)
      {
        button = getKey ( TIME_DELAY_MAX );
        if(button == ESC || button == ENTER)
        {
          break;
        }  
      }
      
       if (( get_depth( ) != SAFE_POSITION ) && ( Features.auto_depth == TRUE ))             // test for SAFE position
       {
        not_in_safe_text();                 // TEXT: "NOT IN SAFE POSITION
        delay_ms(1000);
        break;
       }
          
      }
      
      button = getLastKey();
      if(button == ESC)                                 // ESC was pressed, exit to ready screen
      {
        break;
      }  
      else if  (button == ENTER) 
      {
        if  (  ( NO_ERROR == alfat_error ) )
        {
          AlfatStart();   
          read_RTC ( &date_time_g ); //get time and date, date_unformatted now contains coded date value
          AlfatStop();   
        }  
        
        Controls.shut_dwn = TRUE;                         // enable shut down feature when NO is pressed
        if(Controls.LCD_light)                            // ENTER was pressed, continue with test
		    {
          KEY_B_LIGHT_DISABLE()  ;               // turn off keyboard backlight    
		    };


        CLEAR_DISP;
        measurePulsesStandardCount(LINE4, &density2_cnt, &density1_cnt);  // 4 minute test, 240 sec
       
       if ( button == ESC )          
       {
        break;
       }
    
 
        if ( Features.dummy_mode == TRUE )
        { 
         density2_cnt = 2500 + (++dummy_tests * 5 ) ;
         density1_cnt = 6810 + (++dummy_tests * 5 ) ;
        }

        button = getLastKey();
        
        if(button == ESC)                                 // ESC was pressed, exit to main screen
        {
          break;
        }  // break out of while loop
     
         //beep at end of test, use delay to store station data in secret
          if ( Features.sound_on )
          {
            { 
             uint8_t i;
             for( i = 0; i<5; i++ )
             {
              output_high(BUZZER);  
              delay_ms(500);
              output_low(BUZZER);  
              delay_ms(500);
             }
            }
          } 

        //COUNT COMPLETED WITHOUT INTERRUPTION//    
        Controls.shut_dwn = FALSE;                        // disable shut down feature when NO is pressed
        if(Features.avg_std_mode == TRUE)                 // this mode will compare the new standards to a rolling average of the last four valid
        {                                                 // measurements and display the % error.
          density1_count[0] = NV_RAM_MEMBER_RD(stand_test.dense_count1_1);
          density1_count[1] = NV_RAM_MEMBER_RD(stand_test.dense_count1_2);
          density1_count[2] = NV_RAM_MEMBER_RD(stand_test.dense_count1_3);
          
          density2_count[0] = NV_RAM_MEMBER_RD(stand_test.dense_count2_1);
          density2_count[1] = NV_RAM_MEMBER_RD(stand_test.dense_count2_2);
          density2_count[2] = NV_RAM_MEMBER_RD(stand_test.dense_count2_3);
          
          tests = NV_RAM_MEMBER_RD(stand_test.std_number);             //get number of tests completed
          
          if (tests == 2 )
          {
           // average the last two "stored" readings
            avg_std_dense1 = ((float)density1_count[0] + (float)d1_stand)/2.0;
            avg_std_dense2 = ((float)density2_count[0] + (float)d2_stand)/2.0;
          }
          else if (tests == 3 )
          {
           // average the last three "stored" readings
            avg_std_dense1 = ((float)density1_count[0] + (float)density1_count[1] + (float)d1_stand)/3.0;
            avg_std_dense1 = ((float)density1_count[0] + (float)density1_count[1] + (float)d1_stand)/3.0;
          }
          else if ( tests == 4 )
          {
            // average the last four "stored" readings
            avg_std_dense1 = ((float)density1_count[0] + (float)density1_count[1] + (float)density1_count[2] + (float)d1_stand)/4.0;
            avg_std_dense2 = ((float)density2_count[0] + (float)density2_count[1] + (float)density2_count[2] + (float)d2_stand)/4.0;            
          }
          else // test = 1 or 0
          {
            avg_std_dense1 = (float)d1_stand;
            avg_std_dense2 = (float)d2_stand;
          }
      
        }
        else // Not in averaging mode
        {
          d1_stand_cal   =  (float)NV_RAM_MEMBER_RD(Constants.DENS_1_CAL_STD); //read calibrated standard counts from memory
          d2_stand_cal   =  (float)NV_RAM_MEMBER_RD(Constants.DENS_2_CAL_STD); //read calibrated standard counts from memory
               
         // gets days since last calibration
         date_int = decode_date( date_time_g ) - decode_date ( NV_RAM_MEMBER_RD (Constants.CAL_DATE) );  
  
          temp = (-0.693/(30.0 * 365.25)) * (float)date_int;         
          temp = pow ( 2.71828183, temp );
          avg_std_dense1 = d1_stand_cal * temp;
          avg_std_dense2 = d2_stand_cal * temp;
          
        }        
        
        percent_diff_dense1 = ((float)density1_cnt - avg_std_dense1) / avg_std_dense1;       //calculate % difference in density cnt from rolling avg.
        if(fabsf(percent_diff_dense1) > DENSITY_PASS_PERCENT)
        {
          sprintf ( dense1_pass_fail, "F" );
        }  
        
        percent_diff_dense2 = ((float)density2_cnt - avg_std_dense2) / avg_std_dense2;       //calculate % difference in density cnt from rolling avg.
        if(fabsf(percent_diff_dense2) > DENSITY_PASS_PERCENT)
        {
          sprintf ( dense2_pass_fail, "F" );
        }    
        CLEAR_DISP;

        LCD_position(LINE1);        
        sprintf ( lcdstr, "DS 1=%ld %.1f%c %s",(uint32)density1_cnt,(double)(percent_diff_dense1*100.0),0x25,dense1_pass_fail);   
        LCD_print ( lcdstr );
         
        LCD_position(LINE2);
        sprintf ( lcdstr, "DS 2=%ld %.1f%c %s",(uint32)density2_cnt,(double)(percent_diff_dense2*100.0),0x25,dense2_pass_fail);   
        LCD_print ( lcdstr );
        
        use_new_std_count();     // display "Use New STD Count?"
        Press_YES_or_NO(LINE4);  // display "Press YES or NO"         
        while(1)
        {
         button = getKey ( TIME_DELAY_MAX );
         
          if((button == NO) || (button == YES) || (button == ESC))
          {
            break;
          }  
        }           
  
        if((button == NO) || (button == ESC))
        {
          break;
        }  
        else if(button == YES)
        {
          index = NV_RAM_MEMBER_RD(stand_test.std_index); //read std index position
          index %= 30;          
          
          //save dense sys 1 std to buffer
          NV_MEMBER_STORE(stand_test.dense_count1_x[index],  (uint16_t)density1_cnt) ;
                    
          // save dense sys 2 std to buffer
          NV_MEMBER_STORE(stand_test.dense_count2_x[index],  density2_cnt) ;
          
          //save std date to buffer
          NV_MEMBER_STORE(stand_test.date_count_x[index],  date_time_g ) ;
                  
          NV_MEMBER_STORE(stand_test.std_index,++index);
          
          tests_tot = NV_RAM_MEMBER_RD(stand_test.std_counts);
          
          if(++tests_tot <= 30)    // write number of tests up to 30
          {
             NV_MEMBER_STORE(stand_test.std_counts,tests_tot);
          }  
          
          // store the last 4 minute count as the standard count
          NV_MEMBER_STORE(DEN1_STAND, (uint16_t)density1_cnt);
          NV_MEMBER_STORE(DEN2_STAND, (uint16_t)density2_cnt);
          
          if(Features.avg_std_mode)
          {
            NV_MEMBER_STORE(stand_test.dense_count1_3, density1_count[1] );       //shift values and write to eeprom
            NV_MEMBER_STORE(stand_test.dense_count1_2, density1_count[0]);       //shift values and write to eeprom
            NV_MEMBER_STORE(stand_test.dense_count1_1, d1_stand);             
            
            NV_MEMBER_STORE(stand_test.dense_count2_3, density2_count[1] );       //shift values and write to eeprom
            NV_MEMBER_STORE(stand_test.dense_count2_2, density2_count[0]);       //shift values and write to eeprom
            NV_MEMBER_STORE(stand_test.dense_count2_1, d2_stand);             
            
            
            if(++tests < 5)
            {
             NV_MEMBER_STORE(stand_test.std_number,tests);         // store number of tests in eeprom until 4th test   
            }  
          }
          break;
        }
      }        
    }   
  Flags.stand_flag = FALSE;
  Controls.shut_dwn = TRUE; 
}


/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Take 20, 60 sec readings.
 *               AVG = (SUM of 20)/20
 *               Theoretical_SD = sqrt(AVG)
 *               SD = SQRT ( SUM[reading(i) - avg]^2  / 19 )
 *               Ratio = SD/Theo
 *               
 *            
 *               Ideal Ratio = 1. With Prescale, ideal ratio = Sqrt(ps)/ps
 * 
 * 
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

uint8_t stat_test(void)  // leads user through static test procedure
{  
   int8_t   display_set = 0,  pass_fail[6] = {0,0,0,0,0,0 }, i;
  int32_t  dc_sub[6]  = {0,0,0,0,0,0}, delta[6] = {0,0,0,0,0,0};
  int32_t  dc_sum[6] = {0,0,0,0,0,0};  
  float    dc_std[6] = {0,0,0,0,0,0}, dc_theo[6] = {0,0,0,0,0,0};
  enum buttons button;
  uint32 gm[6];
  uint32_t density2_cnt = 0;
  uint32_t density1_cnt = 0;
  uint32_t density_cnt[20][6] ;
  float test_ratio_upper, test_ratio_lower;

  // Save the state of hte auto turn off flag
  BOOL temp_auto_turn_off = Spec_flags.auto_turn_off;

  Spec_flags.auto_turn_off = FALSE;                  // disable auto_turn_off during count

   test_ratio_upper = RATIO_UPPER; 
   test_ratio_lower = RATIO_LOWER;
  
  while(1)
  {
    for ( uint8 k = 0; k < 6; k++ )
    { 
     dc_sub[k] = 0;
     delta[k]  = 0;
     dc_sum[k] = 0;  
    }  
  
    press_start(0);  // display "  Press START for\n  20m. STAT TEST" on LINE2 and LINE3      
    while(1)
    {
      button = getKey(TIME_DELAY_MAX);
      if((button == ENTER) || (button == ESC) || (button == MENU))
      {
        break;
        }  
    }    
    if(button != ENTER)                        // ESC was pressed, exit to menu
    {
      break;  
    }  
   
    if(Controls.LCD_light)
  	{
      KEY_B_LIGHT_DISABLE()    ;                        // turn off keyboard backlight    
  	};

    reading();  // display "Reading # " on LINE2
    for(i=0; i<20; i++)                             // loop for taking (20) 1 minute counts
    {      
      LCD_position(LINE2+10);
      _LCD_PRINTF ( "%d",(i+1));


      if ( Features.dummy_mode == TRUE )
      {
        measurePulses (LINE3, 1, &density2_cnt, &density1_cnt );
        density1_cnt = 1000;
        density2_cnt = 1000;
      }
            
      else      
      {
       #if 1
        measurePulses (LINE3, 60, &density2_cnt, &density1_cnt  );
        // get the 6 gm tube counts;
        getGMPulseCounts ( &gm[0]);
        for ( uint8 k = 0; k < 6; k++ )
        {
         density_cnt[i][k] = gm[k]/(PRESCALE_60);
        }
        #else
         measurePulses (LINE3, 15, &density2_cnt, &density1_cnt  );
        // get the 6 gm tube counts;
        getGMPulseCounts ( &gm[0]);
        for ( uint8 k = 0; k < 6; k++ )
        {
         density_cnt[i][k] = gm[k]/(PRESCALE_15);
        }
        #endif
      
      }  
            
      if( getLastKey() == ESC)                             // ESC was pressed, exit to menu
      {
         break;
      }
      for ( uint8 k = 0; k < 6; k++ )
      {
        stat_dense[i][k] =  density_cnt[i][k];
        dc_sum[k] += (int32_t)density_cnt[i][k];  
      }  
    }
    
    if(Controls.LCD_light)    
    {
      LCD_timeout = 70;  //7 seconds    
    }
    
    if ( getLastKey() == ESC )                             // ESC was pressed, exit to menu
    { 
      break;     
    }  
    for ( uint8 k = 0; k < 6; k++ )
    {
      stat_dense_avg[k] = dc_sum[k] / 20;
      dc_theo[k] = sqrt((float)stat_dense_avg[k] );
    }  
        
    for(i=0; i<20; i++)
    {        
      for ( uint8 k = 0; k < 6; k++ )
      {
        delta[k] = (int32_t)stat_dense[i][k] - stat_dense_avg[k];  //+-
        dc_sub[k] += (delta[k] * delta[k]); //+
      }  
    }
    for ( uint8 k = 0; k < 6; k++ )
    {
      dc_std[k] = sqrt((float)dc_sub[k] / 19.0); //+
      stat_dense_ratio[k] = dc_std[k] / (dc_theo[k] ); //+
      NV_MEMBER_STORE(D_CNT_STD[k], (uint16_t)stat_dense_avg[k]);  //store values for L% calc in drift test
    }  
   
    for ( uint8 k = 0; k < 6; k++ )
    {
      if(stat_dense_ratio[k] < test_ratio_lower || stat_dense_ratio[k] > test_ratio_upper )
      {
        pass_fail[k] = 'F';        
      } 
      else
      {
        pass_fail[k] = 'P';        
      }
    }

    
    while(1)                                   //remains in display mode until button is pressed
    {
      CLEAR_DISP;
   
     if ( display_set == 0)
     {
        //Display ratio and pass fail
        LCD_position(LINE1);
        checkFloatLimits ( & stat_dense_ratio[0] );
        
        LCD_position(LINE1);
        sprintf ( lcdstr,"R1:%4.2f %c",stat_dense_ratio[0],  pass_fail[0] );
        LCD_print ( lcdstr );
        checkFloatLimits ( & stat_dense_ratio[1] );
        LCD_position(LINE1+10);
        sprintf ( lcdstr,"R2:%4.2f %c",stat_dense_ratio[1],  pass_fail[1] );
        LCD_print ( lcdstr );
        
        LCD_position(LINE2);
        checkFloatLimits ( & stat_dense_ratio[2] );
        sprintf ( lcdstr,"R3:%4.2f %c",stat_dense_ratio[2],  pass_fail[2] );
        LCD_print ( lcdstr );
        LCD_position(LINE2+10);
        checkFloatLimits ( & stat_dense_ratio[3] );
        sprintf ( lcdstr,"R4:%4.2f %c",stat_dense_ratio[3],  pass_fail[3] );
        LCD_print ( lcdstr );
        
        LCD_position(LINE3);
        checkFloatLimits ( & stat_dense_ratio[4] );
        sprintf ( lcdstr,"R5:%4.2f %c",stat_dense_ratio[4],  pass_fail[4] );
        LCD_print ( lcdstr );
        LCD_position(LINE3+10);
        checkFloatLimits ( & stat_dense_ratio[5] );        
        sprintf ( lcdstr,"R6:%4.2f %c",stat_dense_ratio[5],  pass_fail[5] );
        LCD_print ( lcdstr );
     }
     else if ( display_set == 1 )
     {  
        LCD_position(LINE1);
        _LCD_PRINTF ( "1A:%5lu", (uint32)stat_dense_avg[0]);
        LCD_position(LINE1+10);
        _LCD_PRINTF ( "2A:%5lu", (uint32)stat_dense_avg[1]);
        LCD_position(LINE2);
        _LCD_PRINTF ( "3A:%5lu", (uint32)stat_dense_avg[2]);
        LCD_position(LINE2+10);
        _LCD_PRINTF ( "4A:%5lu", (uint32)stat_dense_avg[3]);
        LCD_position(LINE3);
        _LCD_PRINTF ( "5A:%5lu", (uint32)stat_dense_avg[4]);
        LCD_position(LINE3+10);
        _LCD_PRINTF ( "6A:%5lu", (uint32)stat_dense_avg[5]);
    }    
    else if ( display_set >= 2 && display_set < 22 )
    {    
        LCD_position(LINE1);
        sprintf ( lcdstr,"#%u D1:%lu",(uint8)(display_set-1), (uint32)stat_dense[display_set-2][0]);
        LCD_print ( lcdstr );
        LCD_position(LINE1+11);
        sprintf ( lcdstr," D2:%lu", (uint32)stat_dense[display_set-2][1] );
        LCD_print ( lcdstr );
          
        LCD_position(LINE2);
        sprintf ( lcdstr,"#%u D3:%lu",(uint8)(display_set-1), (uint32)stat_dense[display_set-2][2] );
        LCD_print ( lcdstr );
        LCD_position(LINE2+11);
        sprintf ( lcdstr," D4:%lu", (uint32)stat_dense[display_set-2][3] );
        LCD_print ( lcdstr );
        
           
        LCD_position(LINE3);
        sprintf ( lcdstr,"#%u D5:%lu",(uint8)(display_set-1), (uint32)stat_dense[display_set-2][4] );
        LCD_print ( lcdstr );
        LCD_position(LINE3+11);
        sprintf ( lcdstr," D6:%lu", (uint32)stat_dense[display_set-2][5] );
        LCD_print ( lcdstr );
        
    }  
 
    while( 1 )
    {
      button = getKey ( TIME_DELAY_MAX  );
      if((button == ESC) || (button == UP) || (button == DOWN) || (button == STORE)  )
      {
        break;
      }  
      
   }
   
   if ( button == STORE )
   {
     storeStatTestResultsToUSB ( TRUE );
   } 
   else if(button == DOWN)                         
   {
    display_set += 1;
    if ( display_set > 21 )
      display_set = 0;
   }  
   else if(button == UP)                     
   {
    if ( display_set > 0 )
      display_set -= 1;
    else
      display_set = 21;
   }  
   else
   {
    break;
   }
  } // end while(1)    
   
 }
  // re-enable auto turn off if it was on
  Spec_flags.auto_turn_off = temp_auto_turn_off;
  shutdown_timer = 0;
  return(0);
}

/******************************************************************************
 *
 *  Name:
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 
void drift_test(void)  // leads user through drift test procedure
{
  #define GM_TUBES 6
  #define READINGS 5
  int8_t display_set = 0, i;
  
  uint32 dc_sum[6] = {0,0,0,0,0,0};
  uint32 dc_statavg = 0;
  
  int32_t delta_1 = 0, delta = 0;
  char  pass = 'P', fail = 'F'; 
  enum buttons button;
  uint32_t density2_cnt = 0;
  uint32_t density1_cnt = 0;
  uint32 gm[6];
  uint32_t density_cnt[READINGS][GM_TUBES] ;
  
  // Save the state of the auto turn off flag
  BOOL temp_auto_turn_off = Spec_flags.auto_turn_off;
  Spec_flags.auto_turn_off = FALSE;                  // disable auto_turn_off during count
#if 0
  press_for_Stat();// Press #1 for SYS 1 and SYS2 test. Press 2 for all tubes
  while(1)
  {
    button = getKey(TIME_DELAY_MAX);
      
    if(button == ONE || button == ESC || button == TWO)
    {
      break;
    }  
        
  }    
 
  if ( button == ESC )
    goto  state_test_end;
  #endif
  while(1)
  {    
 
    press_start(1);  // display "  Press START for\n  20m. DRIFT TEST" on LINE2 and LINE3      
    while(1)
    {
      button = getKey(TIME_DELAY_MAX);
      
      if(button == ENTER || button == ESC || button == MENU)
      {
        break;
      }  
        
    }    
    if(button != ENTER)                                 // ESC was pressed, exit to menu
    { 
      break;
    }  
  
    
    Flags.drift_flag = TRUE;
    if(Controls.LCD_light)
  	{
     KEY_B_LIGHT_DISABLE(); // turn off keyboard backlight    
  	};

    // Get the 5, 4 minute readings
    reading();  // display "Reading # " on LINE2
    for(i=0; i < READINGS; i++)
    {      
      LCD_position (LINE2+10);
      _LCD_PRINTF ( "%d ",(i+1) );
    
      measurePulses( LINE3, 240, &density2_cnt, &density1_cnt );                           
      // get the six seperate GM tubes for this reading
      getGMPulseCounts ( &gm[0]);
      for ( uint8 gmt = 0; gmt < GM_TUBES; gmt++ )
      {
        density_cnt[i][gmt] = gm[gmt]/PRESCALE_240;
      }
      
      button = getLastKey();
      
      if(button == ESC)
      {
        Flags.drift_flag = FALSE;
        break;
      }  
      for ( uint8 gmt = 0; gmt < GM_TUBES; gmt++ )
      {
        drift_dense[i][gmt] = density_cnt[i][gmt];
        dc_sum[gmt] += density_cnt[i][gmt];

      }
      delay_ms(10);
    }
    
    Flags.drift_flag = FALSE;
    
    if(Controls.LCD_light)
    {
      LCD_timeout = 70;                                   // 7 seconds      
    }  
         
    if(button == ESC)                                 // ESC was pressed, exit to menu
    {
      break;
    }      
    for ( uint8 k = 0; k < GM_TUBES; k++ )
    {
      drift_dense_avg[k] = dc_sum[k] / READINGS;
      dc_statavg = NV_RAM_MEMBER_RD ( D_CNT_STD[k] );            // retreive stat values from EEPROM
      delta   =  dc_statavg - drift_dense_avg[k];
      delta_1 = (dc_statavg + drift_dense_avg[k]) / 2;        // new code
      if ( delta_1 != 0 )
      { 
        drift_dense_per[k] = fabsf ((float)(100.0 * ( (float)delta / (float)delta_1 )));         // new code
      }
      else
      { 
        drift_dense_per[k] = 9999;
      }    
     
     // routine for diagnostic self test...store counts,%drift for review, determine pass/fail
  	 //********************************* copies the below statement from if(diag)
  	 NV_MEMBER_STORE( dr_dense_avg[k], drift_dense_avg[k] );
    }
       
//DISPLAY DATA//DISPLAY DATA//DISPLAY DATA//DISPLAY DATA//DISPLAY DATA//DISPLAY DATA//   
    while(1)
    {  
      CLEAR_DISP;
      switch(display_set)
      {        
        case 0:///////////////////////////////////////////////////////////////
          LCD_position (LINE1);
          stat_drift_count_text(0); //TEXT// display "D1% Drift = "
          
          sprintf( lcdstr,"%.2f %c", (double)drift_dense_per[0], (drift_dense_per[0] <=0.5) ? pass : fail);
          LCD_print (lcdstr);
          
          LCD_position (LINE2);
          stat_drift_count_text(1); //TEXT// display "Dense.1 Avg = "
          _LCD_PRINTF ( "%lu",(uint32)drift_dense_avg[0]);   
              
          LCD_position (LINE3);
          stat_drift_count_text(2); //TEXT// display "D2%% Drift = "
          sprintf ( lcdstr,"%.2f %c",(double)drift_dense_per[1], (drift_dense_per[1] <= .5) ? pass : fail);
          LCD_print ( lcdstr );
          
          LCD_position (LINE4);
          stat_drift_count_text(3); //TEXT// display "Dense.2 Avg = "
          _LCD_PRINTF ( "%lu",(uint32)drift_dense_avg[1]);
        break; 
          
        case 1: ///////////////////////////////////////////////////////////////
          LCD_position (LINE1);
          stat_drift_count_text(6); //TEXT// display "D3% Drift = "
          
          sprintf( lcdstr,"%.2f %c", (double)drift_dense_per[2], (drift_dense_per[2] <=0.5) ? pass : fail);
          LCD_print (lcdstr);
          
          LCD_position (LINE2);
          stat_drift_count_text(7); //TEXT// display "Dense.3 Avg = "
          _LCD_PRINTF ( "%lu",(uint32)drift_dense_avg[2]);   
              
          LCD_position (LINE3);
          stat_drift_count_text(8); //TEXT// display "D4%% Drift = "
          sprintf ( lcdstr,"%.2f %c",(double)drift_dense_per[3], (drift_dense_per[3] <= .5) ? pass : fail);
          LCD_print ( lcdstr );
          
          LCD_position (LINE4);
          stat_drift_count_text(9); //TEXT// display "Dense.4 Avg = "
          _LCD_PRINTF ( "%lu",(uint32)drift_dense_avg[3]);
        break;           

        case 2:///////////////////////////////////////////////////////////////
          LCD_position (LINE1);
          stat_drift_count_text(10); //TEXT// display "D5% Drift = "
          
          sprintf( lcdstr,"%.2f %c", (double)drift_dense_per[4], (drift_dense_per[4] <=0.5) ? pass : fail);
          LCD_print (lcdstr);
          
          LCD_position (LINE2);
          stat_drift_count_text(11); //TEXT// display "Dense.5 Avg = "
          _LCD_PRINTF ( "%lu",(uint32)drift_dense_avg[4]);   
              
          LCD_position (LINE3);
          stat_drift_count_text(12); //TEXT// display "D6%% Drift = "
          sprintf ( lcdstr,"%.2f %c",(double)drift_dense_per[5], (drift_dense_per[5] <= .5) ? pass : fail);
          LCD_print ( lcdstr );
          
          LCD_position (LINE4);
          stat_drift_count_text(13); //TEXT// display "Dense.6 Avg = "
          _LCD_PRINTF ( "%lu",(uint32)drift_dense_avg[5]);
        break;           
        
               ////////////////////////////////////////////////////////////////////////////////
        case 3:///////////////////////////////////////////////////////////////
          LCD_position (LINE1);
          sprintf ( lcdstr,"#1  D1 %lu D2 %lu",(uint32) drift_dense[0][0],(uint32)drift_dense[0][1]);
          LCD_print ( lcdstr );
         
          LCD_position (LINE2);
          sprintf ( lcdstr,"#2  D1 %lu D2 %lu",(uint32) drift_dense[1][0],(uint32)drift_dense[1][1]);
          LCD_print ( lcdstr );
         
          LCD_position (LINE3);
          sprintf ( lcdstr,"#3  D1 %lu D2 %lu", (uint32)drift_dense[2][0],(uint32)drift_dense[2][1]);
          LCD_print ( lcdstr );
         
          LCD_position (LINE4);
          sprintf ( lcdstr,"#4  D1 %lu D2 %lu",(uint32) drift_dense[3][0],(uint32)drift_dense[3][1]);
          LCD_print ( lcdstr );
        break;
              ////////////////////////////////////////////////////////////////////////////////            
        case 4:///////////////////////////////////////////////////////////////
          LCD_position (LINE1);
          sprintf ( lcdstr,"#5  D1 %lu D2 %lu",(uint32) drift_dense[4][0], (uint32)drift_dense[4][1]);
          LCD_print ( lcdstr );
        break;

                       ////////////////////////////////////////////////////////////////////////////////
        case 5:///////////////////////////////////////////////////////////////
          LCD_position (LINE1);
          sprintf ( lcdstr,"#1  D3 %lu D4 %lu",(uint32) drift_dense[0][2],(uint32)drift_dense[0][3]);
          LCD_print ( lcdstr );
         
          LCD_position (LINE2);
          sprintf ( lcdstr,"#2  D3 %lu D4 %lu",(uint32) drift_dense[1][2],(uint32)drift_dense[1][3]);
          LCD_print ( lcdstr );
         
          LCD_position (LINE3);
          sprintf ( lcdstr,"#3  D3 %lu D4 %lu", (uint32)drift_dense[2][2],(uint32)drift_dense[2][3]);
          LCD_print ( lcdstr );
         
          LCD_position (LINE4);
          sprintf ( lcdstr,"#4  D3 %lu D4 %lu",(uint32) drift_dense[3][2],(uint32)drift_dense[3][3]);
          LCD_print ( lcdstr );
        break;
              ////////////////////////////////////////////////////////////////////////////////            
        case 6:///////////////////////////////////////////////////////////////
          LCD_position (LINE1);
          sprintf ( lcdstr,"#5  D3 %lu D4 %lu",(uint32) drift_dense[4][2], (uint32)drift_dense[4][3]);
          LCD_print ( lcdstr );
        break;  
               ////////////////////////////////////////////////////////////////////////////////
        case 7:///////////////////////////////////////////////////////////////
          LCD_position (LINE1);
          sprintf ( lcdstr,"#1  D5 %lu D6 %lu",(uint32) drift_dense[0][4],(uint32)drift_dense[0][5]);
          LCD_print ( lcdstr );
         
          LCD_position (LINE2);
          sprintf ( lcdstr,"#2  D5 %lu D6 %lu",(uint32) drift_dense[1][4],(uint32)drift_dense[1][5]);
          LCD_print ( lcdstr );
         
          LCD_position (LINE3);
          sprintf ( lcdstr,"#3  D5 %lu D6 %lu", (uint32)drift_dense[2][4],(uint32)drift_dense[2][5]);
          LCD_print ( lcdstr );
         
          LCD_position (LINE4);
          sprintf ( lcdstr,"#4  D5 %lu D6 %lu",(uint32) drift_dense[3][4],(uint32)drift_dense[3][5]);
          LCD_print ( lcdstr );
        break;
              ////////////////////////////////////////////////////////////////////////////////            
        case 8:///////////////////////////////////////////////////////////////
          LCD_position (LINE1);
          sprintf ( lcdstr,"#5  D5 %lu D6 %lu",(uint32) drift_dense[4][4], (uint32)drift_dense[4][5]);
          LCD_print ( lcdstr );
        break;          
          
      } 

      while(1)
      {
        button = getKey(TIME_DELAY_MAX);
          
        if((button == ESC) || (button == UP) || (button == DOWN) || (button == STORE) )
        {
          break;
        }  
      }        
      
      if ( button == STORE )
      {
       storeDriftResultsToUSB ( TRUE );
      }
      else if ( button == DOWN )   // down button
      {
        display_set += 1;   
        if ( display_set > 8 )
          display_set = 0;
      }  
      else if ( button == UP )                              // up button
      {
        if ( display_set > 0 )
          display_set -= 1; 
        else
          display_set = 8; 
      }  
      else  //button == ESC exit function
      {
        break;
      }  
    }
    break;
 } 
  
  
 // re-enable auto turn off if it was on
 Spec_flags.auto_turn_off = temp_auto_turn_off;
 shutdown_timer = 0;
 
 Flags.drift_flag = FALSE; 
 Controls.shut_dwn = TRUE;
}



/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void extended_drift_test ( void )
{
  uint16_t loop_cnt, c ;
  enum buttons button;
  
  //array that enter_name stores the name
  char test_name[] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
  char number_ptr[] = {0,0,0,0,0};

  char date_time_str[35];
  
  uint32_t density1_cnt = 0;
  uint32_t density2_cnt = 0;
  char temp_str[100] = "\0";
  float temp_calc, voltage;
  // Save the state of the auto turn off flag
  BOOL temp_auto_turn_off = Spec_flags.auto_turn_off;
  uint32 gm[6];
  uint32 density_cnt[6];
  FILE_PARAMETERS fp; 
  float div_by;
 
  Spec_flags.auto_turn_off = FALSE;                  // disable auto_turn_off during count
 
  Features.auto_store_on = FALSE;
  
  CLEAR_DISP;
  LCD_PrintAtPosition("EXTENDED COUNT TEST",LINE1);  
  LCD_PrintAtPosition("COUNTS:            ",LINE2);  

  Enter_to_Accept(LINE3);        //TEXT// display "Enter to Accept"
     
  ESC_to_Exit(LINE4);          //TEXT// display "ESC to Exit"    
        
  sprintf(number_ptr,"%d",0);
        
  loop_cnt = (uint16_t)enterNumber( number_ptr,LINE2 + 12, 4 );  // prompt user to enter number of counts
  button = getLastKey();      
  if(button == ESC)
  {
    Spec_flags.auto_turn_off = temp_auto_turn_off;
    shutdown_timer = 0;    
    return;          
  }  

  for(;;)
  {
    CLEAR_DISP;
    LCD_PrintAtPosition("Enter Test Name",LINE1);  
    YES_to_Accept(LINE3);              //TEXT// display "YES to Accept"
    ESC_to_Exit(LINE4);                //TEXT// display "ESC to Exit" 
    enter_name ( test_name,LINE2) ;  
    
    button = getLastKey();
            
    if(button == ESC)
    {
      Spec_flags.auto_turn_off = temp_auto_turn_off;
      shutdown_timer = 0;    
      return;          
    }  
    else if ( button == YES )        // YES was pressed to accept entered project name
    {  
      if ( test_name[0] != '\0' )   // make sure something was entered for the project name
      {
       break;       
      }  
    }
  }      

 CLEAR_DISP;
 LCD_PrintAtPosition ("Drift Test Name",LINE1);
 LCD_PrintAtPosition(test_name,LINE2);
 delay_ms(1000);
 
 CLEAR_DISP;

 AlfatStart();   
 read_RTC( &date_time_g );  //get date


 USB_text(0); // display "  Insert External\n Drive in USB Port\n     Press ENTER" on LINE1, LINE2 and LINE4 
 while(1)
 {
   button = getKey ( TIME_DELAY_MAX );
    
   if((button == ENTER) || (button == ESC))
   {
     break;
   }  
      
 }      
 if(button == ESC)
 {
   Spec_flags.auto_turn_off = temp_auto_turn_off;
   shutdown_timer = 0;    
   return;
 }  
 
 CLEAR_DISP;
 LCD_PrintAtPosition("CREATING NEW FILE",LINE1);
 
 // Create a file on USB by this name
 if(!check_for_USB())
 {       
   USB_text(2);  // display "   No USB Device\n    Detected." on LINE2 and LINE3
   delay_ms(2000);    
   return;
 }   
 else if(!initialize_USB(TRUE))  //try to initialize USB.  if it cant initialize, escape
 {
   Spec_flags.auto_turn_off = temp_auto_turn_off;
   shutdown_timer = 0;    
   return; 
 }

 USB_open_file  ( test_name, &fp );  

 sprintf(temp_str,"TUBE1,TUBE2,TUBE3,TUBE4,TUBE5,TUBE6,NICD,ALKALINE,TEMPERATURE,TIME\r\n");
 AlfatWriteStr(&fp,temp_str);  //write string to USB
 AlfatFlushData(fp.fileHandle);
  
 cnt_time =  NV_RAM_MEMBER_RD( COUNT_TIME );

 // Set the prescale value
 switch ( cnt_time )
 {
     case 5:
     case 120: div_by = PRESCALE_120;
              break;
     case 30: div_by = PRESCALE_30;
              break;
     case 60: div_by = PRESCALE_60;
              break;
     case 240:div_by = PRESCALE_240;
              break;
    
     default: div_by = PRESCALE_60;
             break;
  }

 for ( c=0; c<loop_cnt; c++ )
 {
   float nimh_volts,alk_volts;
   
   // Display the count number
   CLEAR_DISP;
   LCD_PrintAtPosition("EXTENDED DRIFT TEST",LINE1);
   LCD_PrintAtPositionCentered("ESC to EXIT",LINE4+10);
   sprintf(temp_str,"COUNT %d OF %d  ",c+1,loop_cnt);
   LCD_PrintAtPosition(temp_str,LINE2);
   
    
   // Take the count
  measurePulses( LINE3,cnt_time, &density2_cnt, &density1_cnt ); 
  
  getGMPulseCounts ( &gm[0]);
  for ( uint8 gmt = 0; gmt < GM_TUBES; gmt++ )
  {
     density_cnt[gmt] = gm[gmt]/div_by;
  }  
   // leave function early if esc pressed
   button = getLastKey();
   if ( button == ESC )
   {
     Spec_flags.auto_turn_off = temp_auto_turn_off;
     shutdown_timer = 0;    
     return;
   }  
   
   // Get the time at the end of the count
   

   read_RTC( &date_time_g );  //get date
   getTimeDateStr ( date_time_g, date_time_str );
 
   Global_ID();

   nimh_volts = readADCVolts(NICAD_ADC_CHAN) ;   
   nimh_volts /= .177;
    
   alk_volts = readADCVolts(ALKALINE_ADC_CHAN) ;
   alk_volts /= .177;

   voltage = readADCVolts( TEMPERATURE_ADC_CHAN );
    // °C = (Vout - 500mV) / 10 mv/°C
    temp_calc = (voltage - .5) * 100;

   Global_IE();
   delay_ms( 200 );

   sprintf(temp_str,"%ld,%ld,%ld,%ld,%ld,%ld,%.2f,%.2f,%.0f,%s\r\n",(uint32)density_cnt[0],(uint32)density_cnt[1],(uint32)density_cnt[2],(uint32)density_cnt[3],(uint32)density_cnt[4],(uint32)density_cnt[5],nimh_volts,alk_volts,temp_calc,date_time_str);
   AlfatWriteStr(&fp,temp_str);    //write string to USB
   AlfatFlushData(fp.fileHandle);


 }// end for loop
 AlfatCloseFile( fp.fileHandle ); 
Spec_flags.auto_turn_off = temp_auto_turn_off;
shutdown_timer = 0;    
}


 
/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 
 void storeProfileDensitiesToUSB ( Bool display_error,float* density )
 {
  char date_string[30];
  char name_string[30];
  char temp_str[60];
  float thickness;
  FILE_PARAMETERS fp;  
 
  AlfatStart();   
    
  isrTIMER_1_Disable();
   
   CLEAR_DISP;
   LCD_PrintAtPositionCentered( "Writing Profile", LINE1+10);
   LCD_PrintAtPositionCentered( "Densities", LINE2+10); 
   CyDelay ( 1000 );
  // If the drive can be mounted, data will be stored.
    
  if ( initialize_USB( display_error ) )
  {

   // make the file name string
   nullString(name_string, sizeof(name_string) );
   strcat( name_string, "PROFILE_DENS_" );
    
   // get the current time, and turn into a string
   read_RTC( &date_time_g );
   getTimeDateStr ( date_time_g, date_string );
    
   // replace the ":" with a "_"
   for ( uint8 n = 0; n < strlen(date_string); n++ )
   {
      if ( ( date_string[n] == ':' ) || (date_string[n] == '/') )
      {
        date_string[n] = '_';
      }
   }
    
   // add date to string
   strcat( name_string, date_string );
  
   // open a file
   USB_open_file  ( name_string , &fp);  

   nullString(temp_str, sizeof(temp_str));
   sprintf( temp_str, "DEPTH,Density\r\n" );
   AlfatWriteStr(&fp,temp_str);
  
   for ( uint8 n = 0; n < 16; n ++ )   //read all valid counts from memory
   {
     nullString(temp_str, sizeof(temp_str));
     if ( Features.SI_units == FALSE )
     {
       thickness = ( 1 + n ) * .25;
       sprintf( temp_str,"%4.2f in.,%5.1f PCF\r\n", thickness, density[n] * ( KG_PCF_CONV ) );
     } 
     else
     { 
       thickness = ( 1 + n ) * 6.35;
       sprintf( temp_str,"%5.2fmm,%.0f KG/M3\r\n", thickness, density[n] );
     }  
     LCD_PrintAtPosition ( temp_str, LINE3 );
     AlfatWriteStr(&fp,temp_str);
     CyDelay ( 100 );
   } 

    // close file     
    AlfatFlushData(fp.fileHandle);
    AlfatCloseFile(fp.fileHandle);    
  }   
  
  AlfatStop();   
  isrTIMER_1_Enable();

 }
 
 