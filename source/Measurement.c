/**********************************************************************************************************************/
//
// Title:       Elite
// Company:     Instrotek
//
// Document No.:
// Issue:
// Description:
//
// Filename:
// Author:
//
//
//
// History: date, version, personal initials with comments about any changes
//
/**********************************************************************************************************************/
/*********************************************  INCLUDE FILES  ***************************************************/

#include "Globals.h"
#include "prompts.h"
#include "DataStructs.h"
#include "Utilities.h"
#include "StoreFunctions.h"
#include "ProjectData.h"
#include "conversions.h"
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "Batteries.h"
#include "Tests.h"
#include "SDcard.h"


/************************************* EXTERNAL VARIABLE AND BUFFER DECLARATIONS  *************************************/

extern void SendBLEData ( station_data_t * ble_data, bool isRecall );

/************************************************  LOCAL DEFINITIONS  *************************************************/
#define MAX_FLOAT_VALUE 10000.0
#define MIN_FLOAT_VALUE .001
// 15 minute wait time when 50ms delay is used
#define WAIT_TIME_BEFORE_BREAK  ( 11380 )
#define NO_DEMO_DEBUG  1

/*****************************************  VARIABLE AND BUFFER DECLARATIONS  *****************************************/

station_data_t station_d;

/******************************************************************************
 *
 *  Name: void displayResults (void)
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION: 
 *
 *
 *  RETURNS: 
 *
 *****************************************************************************/
void displayResults ( void )
{
  char buff[21], temp_str[21];
  char LCD_offset;
  float cr1, cr2, density, per_MA;
  enum buttons button;
  uint8 auto_scroll_advance, loop_cnt, projected_stored = 0;
  uint16_t wait_time = 0;  // Display data // loop in this routine for 15 minutes
  
  while(1) 
  {   
    //display data until button is pressed to exit
    CLEAR_DISP; // All density values are in KG/M3 at this point
    switch ( gp_disp ) 
    {
      case 1:  // SYS 1 count // SYS 2 count  //  DCR1  DCR2
        LCD_position (LINE1);
        LCD_offset = 13;
        count_text(0);  //TEXT// display "D1 Count:"
        sprintf(temp_str,"%lu",(unsigned long)station_d.density1_count);
        print_string_backward (temp_str,LINE1+LCD_offset );

        LCD_position (LINE2);
        count_text(1);  //TEXT// display "D2 Count";
        sprintf(temp_str,"%lu",(unsigned long)station_d.density2_count);
        print_string_backward(temp_str,LINE2 + LCD_offset);
        
        // display the two count ratios
        cr1 = (float)station_d.density1_count / (float)station_d.density1_stand;
        if ( cr1 > 10 || cr1 < -10  )
        {
          cr1 = 0.0;
        }
        
        cr2 = (float)station_d.density2_count / (float)station_d.density2_stand;
        if ( cr2 > 10 || cr2 < -10  )
        {
          cr2 = 0.0;
        }
        LCD_position (LINE3);
        sprintf(temp_str,"CR1: %4.2f CR2 %4.2f", cr1, cr2 );
        _LCD_PRINTF ( "%s",temp_str);  

        break;
        
    case 2: //  WET DENSITY    // %MAX    // %% VOIDS    
        
        density = station_d.density;
        LCD_position (LINE1);
        count_text(4);  //TEXT// display "    WD:"
        displayValueWithUnitsBW ( density, LINE1 + 16, temp_str );
        
        LCD_offset = 12;
        per_MA = (density / NV_RAM_MEMBER_RD(MARSHALL)) * 100;
        checkFloatLimits ( & per_MA );
        LCD_position (LINE2);
        sprintf(temp_str,"%.1f",(double)per_MA);
        count_text(5);  //TEXT// display "   %%MA:"
        print_string_backward(temp_str,LINE2+LCD_offset);
        
        LCD_position (LINE3);
        sprintf(temp_str,"%.1f",100.0 - (double)per_MA);
        count_text(6);  //TEXT// display ""Voids:"
        print_string_backward(temp_str,LINE3+LCD_offset);  
        break;
        
    case 3:
        snprintf( buff,20,"LAT: %9.6f",station_d.gps_read.latitude); //gdata.latitude);
        LCD_PrintAtPosition ( buff, LINE1 );
        snprintf( buff,20,"LNG: %9.6f", station_d.gps_read.longitude); //gdata.longitude);
        LCD_PrintAtPosition ( buff, LINE2 );
        snprintf( buff,20,"ALT:%4u SAT:%2u ",station_d.gps_read.altitude,station_d.gps_read.sats); // gdata.altitude, gdata.sats);
        LCD_PrintAtPosition ( buff, LINE3 );
        break;
        
    case 0:
        LCD_position(LINE1);
        printTimeDate ( date_time_g ) ;
        
        LCD_position (LINE2);
        count_text(7);  //TEXT// display "THICKNESS"
        
        // display thickness
        if ( Features.SI_units == FALSE )
        {
           snprintf( buff,20," %.2f in.",station_d.thickness / 25.4);
           LCD_print ( buff );
        }
        else 
        {
          snprintf( buff,20," %.2f mm",station_d.thickness);
          LCD_print ( buff );
        }

        // display mode
        if (  NV_RAM_MEMBER_RD (measure_mode ) == METAL_DENSITY_MODE )
        {
          LCD_position (LINE3);
          count_text(9);  //TEXT// display "MODE:THICKNESS"
        }
        else if ( NV_RAM_MEMBER_RD (measure_mode ) == NORMAL_DENSITY_MODE ) 
        {
          LCD_position (LINE3);
          count_text(8);  //TEXT// display "MODE:NORMAL"
        }
           
        break;
    } // end of "Switch"
    
  // this is LINE 4  
  LCD_position(LINE4);
  if(Features.language_f)
  {
   _LCD_PRINT("    PRESS UP/DOWN   ");
  }
  else
  {
    _LCD_PRINT ("Arriba/Abajo       ");
  }
  
  auto_scroll_advance = FALSE;
  if ( Features.auto_scroll )
  {
    loop_cnt = 0;
    while(1)
    {
     button = getKey(50);
      if ( ++wait_time > WAIT_TIME_BEFORE_BREAK )
      {
        button = ESC ;
      }
      if((button == ESC) || (button == UP) || (button == DOWN) || (++loop_cnt >= 100) || (button == STORE) || (button == ENTER) || (button == THICKNESS))
      {
        wait_time = 0;
        break;
      }
    }
    if(loop_cnt >= 100)
    {
      auto_scroll_advance = TRUE; // scroll to next screen
    }
    else
    {
      auto_scroll_advance = FALSE;
    }
  }
  else
  {
    while(1)
    {
      #if AUTO_TEST
        button = getKey(50);
        if(button != ESC)
        {
          button = ENTER;
        }
      #else
      button = getKey(50);
      #endif
      if ( ++wait_time > WAIT_TIME_BEFORE_BREAK )
      {
        button = ESC ;
      }
      if((button == ESC) || (button == UP) || (button == DOWN) || (button == STORE) || (button == ENTER) )
      {
        wait_time = 0;
        break;
      }
    }
  }
  
  // scroll down
  if((button == DOWN) || (auto_scroll_advance))    // down button
  {
    if (  Features.gps_on == FALSE )
    {
      gp_disp = (gp_disp + 1)%3;
    }
    else
    {
       gp_disp = (gp_disp + 1)%4;
    }
  }
  else if(button == UP)                      // up button
  {
    if (  Features.gps_on == FALSE )
    {
      gp_disp = ((gp_disp + 3)-1)%3;
    }
    else
    {
     gp_disp = ((gp_disp + 4)-1)%4;
    }
  }
  else if ( button == STORE )
  {
    if ( Spec_flags.recall_flag )
    {
      no_data_to_store_text( );
      delay_ms ( 1500 );
    }
    else
    {
      
      if  ( station_d.mode == NV_RAM_MEMBER_RD (measure_mode) )
      {
        // if auto store is off, store the result
        if (( Features.auto_store_on == 0 ) && ( projected_stored == 0 ))
        {
          storeStationData ( project_info.current_project, station_d  );
          projected_stored = 1; // Don't store the data twice.
        }
        else
        {
          no_data_to_store_text( );
          delay_ms ( 1500 );
        }
      }
      else
      {
        // warn the user that they must change the measurement mode or select a new project
        // "Project Storage "
        // "Measurement Mode"
        // "Does not Match "
        // "Current Mode Setting"
        error_in_measurement_mode_text();
        output_high(BUZZER); 
        CyDelay ( 3000 );
        output_low(BUZZER); 
      }
      
      
    }  
  }
  else if( (button==ESC) || (button==ENTER) )
  {
    break; // break out of the while loop that scrolls the data
  }
 }
  // store the starting dispay screen in memory
  NV_MEMBER_STORE(GP_DISPLAY,gp_disp);
  
}


/******************************************************************************
 *
 *  Name: void displayResults (void)
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION: 
 *
 *
 *  RETURNS: 
 *
 *****************************************************************************/
void displayProfileResults ( float * density )
{
  char buff[21];
  enum buttons button;
  uint8  loop_cnt = 0, n, lcd_position;
  uint16_t wait_time = 0;  // Display data // loop in this routine for 15 minutes
  float thickness;
  uint8 projected_stored = 0;
  
  while(1) 
  {   
    //display data until button is pressed to exit
    CLEAR_DISP; // All density values are in KG/M3 at this point
    if ( loop_cnt < 4 )
    {
      // .50" to 4.00"
      lcd_position = LINE1;
      for ( n = 0; n < 4; n ++ )
      {
        thickness = ( loop_cnt*4 + 2 + n ) * .25;
        if ( thickness < 4.01 )
        {
          if ( Features.SI_units == FALSE )
          {
            sprintf( buff,"%4.2f in. %5.1f PCF", thickness, density[loop_cnt*4+n] * ( KG_PCF_CONV ) );
            LCD_PrintAtPosition ( buff, lcd_position );
            lcd_position += 20;
          } 
          else
          { 
            thickness *= 25.4;
            sprintf( buff,"%5.2fmm ", thickness );
            LCD_PrintAtPosition ( buff, lcd_position );
            displayValueWithUnitsBW (  density[loop_cnt*4+n] , lcd_position + 18, buff );
            lcd_position += 20;
          }
        }  
      } 
    }
    else if ( loop_cnt == 4 )
    {
      // display other stuff
      // Counts, count ratio, time/date,GPS
        uint8 LCD_offset = 13;
        count_text(0);  //TEXT// display "D1 Count:"
        sprintf(buff,"%lu",(unsigned long)station_d.density1_count);
        print_string_backward (buff,LINE1+LCD_offset );

        LCD_position (LINE2);
        count_text(1);  //TEXT// display "D2 Count";
        sprintf(buff,"%lu",(unsigned long)station_d.density2_count);
        print_string_backward(buff,LINE2 + LCD_offset);
        
        // display the two count ratios
        float cr1 = (float)station_d.density1_count / (float)station_d.density1_stand;
        if ( cr1 > 10 || cr1 < -10  )
        {
          cr1 = 0.0;
        }
        
        float cr2 = (float)station_d.density2_count / (float)station_d.density2_stand;
        if ( cr2 > 10 || cr2 < -10  )
        {
          cr2 = 0.0;
        }
        LCD_position (LINE3);
        sprintf(buff,"CR1: %4.2f CR2 %4.2f", cr1, cr2 );
        _LCD_PRINTF ( "%s",buff); 
        LCD_position (LINE4);
        printTimeDate ( date_time_g ) ;
    }
    else if ( loop_cnt == 5 )
    {
      snprintf( buff,20,"LAT: %9.6f",station_d.gps_read.latitude); //gdata.latitude);
      LCD_PrintAtPosition ( buff, LINE1 );
      snprintf( buff,20,"LNG: %9.6f", station_d.gps_read.longitude); //gdata.longitude);
      LCD_PrintAtPosition ( buff, LINE2 );
      snprintf( buff,20,"ALT:%4u SAT:%2u ",station_d.gps_read.altitude,station_d.gps_read.sats); // gdata.altitude, gdata.sats);
      LCD_PrintAtPosition ( buff, LINE3 );
    }

    LCD_position(LINE1+19);
    LCD_NChar_PutChar(LCD_UP);
    LCD_position(LINE4+19);
    LCD_NChar_PutChar(LCD_DOWN);
    
    while(1)
    {
      button = getKey(50);
      if ( ++wait_time > WAIT_TIME_BEFORE_BREAK )
      {
        button = ESC ;
      }
      if((button == ESC) || (button == UP) || (button == DOWN) || (button == STORE) || ( button == ENTER ))
      {
        wait_time = 0;
        break;
      }
    }
       
    // scroll down
    if( button == DOWN )    // down button
    {
      if (  Features.gps_on == FALSE )
      { 
        loop_cnt = (loop_cnt + 1)%5;
      } 
      else
      {
        loop_cnt = (loop_cnt + 1)%6;
      }
      
    }
    else if(button == UP)                      // up button
    {
      if (  Features.gps_on == FALSE )
      {      
        loop_cnt = ((loop_cnt + 5)-1)%5;
      }
      else
      {
        loop_cnt = ((loop_cnt + 6)-1)%6;
      }
    }
    else if ( button == STORE )
    {
      if ( Spec_flags.recall_flag )
      {
        no_data_to_store_text( );
        delay_ms ( 1500 );
      }
      else
      {
        if ( station_d.mode == NV_RAM_MEMBER_RD (measure_mode))
        {
          // if auto store is off, store the result
          if (( Features.auto_store_on == 0 ) && ( projected_stored == 0 ))
          {
            storeStationData ( project_info.current_project, station_d  );
            projected_stored = 1; // Don't store the data twice.
          }
          else
          {
            no_data_to_store_text( );
            delay_ms ( 1500 );
          } 
        }  
        else
        {
          // warn the user that they must change the measurement mode or select a new project
          // "Project Storage "
          // "Measurement Mode"
          // "Does not Match "
          // "Current Mode Setting"
          error_in_measurement_mode_text();
          output_high(BUZZER); 
          CyDelay ( 3000 );
          output_low(BUZZER); 
        }  
      }
    }
    else if( (button==ESC) || (button==ENTER) )
    {
      break; // break out of the while loop that scrolls the data
    }

  }

}


/******************************************************************************
 *
 *  Name: placeGaugeinBS (void)
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION:
 *
 *
 *  RETURNS: NA
 *
 *****************************************************************************/
enum buttons placeGaugeinBS (void)
{
  enum buttons button;
  uint8 depth;
  do
  {
    // Place Gauge
    // in BS Position
    // Press Enter to Start
    gauge_in_bs_text();
    
    // get ENTER or ESC key
    do
    {
      button = getKey(TIME_DELAY_MAX);
    }
    while ( (button != ESC) && (button != ENTER ) );
    if ( button == ESC )
    {
      return ESC;
    }
    depth =  get_depth ( );
    
  } while ( depth != BS_POSITION ) ;
  
  return ENTER ;
}

/******************************************************************************
 *
 *  Name: void projectSetup(void)
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: 
 *
 *
 *  RETURNS: NA
 *
 *****************************************************************************/
uint8 projectSetup ( void )
{
  int str_equal;  
  uint8_t lcd_line;
  
  if ( Features.auto_store_on && (Spec_flags.recall_flag == 0)) 
  { // Automatic Project storage on
      if ( sdOpened == OFF ) 
      {
        SDstart(); 
      }
      updateProjectInfo();
      str_equal= strcmp ((const char*)& project_info.current_project,  "none_selected" );
      if ( ( project_info.number_of_projects == 0 ) || (str_equal == 0 ) ) 
      { //no project has been created and count is started... auto name starting at 1
          start_new_project();
          if( getLastKey() == ESC) 
          {
              if(sdOpened == ON) 
              { SDstop(null); }
              return 0;
          }
      }
      if ( project_info.station_index >= MAX_STATIONS ) 
      { // Check to see if all station positions are full
          max_stations_text( project_info.current_project ); // display "Max # of Stations\nFor %s Has\nBeen Exceeded.\nStart New Project",current_project
          delay_ms(1500);
          if (  sdOpened == ON  ) { SDstop(null); }
          return 0;
      }
      if ( SD_CheckIfProjExists ( project_info.current_project ) == FALSE ) 
      { // auto store is on and a valid project is not selected
          no_project_selected();  //display "No Project Has Been\nSelected. Please\nCreate or Select\nProject."
          delay_ms(1500);
          if (  sdOpened == ON  ) 
          { 
            SDstop(null); 
          }
          return 0;
      }
      
     if ( !Flags.auto_number_stations ) 
     { // auto name is off
          enter_station_name_text();  //TEXT// display "Enter Station\nName:" LINE1,2
          YES_to_Accept(LINE3);              //TEXT// display "YES to Accept"
          ESC_to_Exit(LINE4);                //TEXT// display "ESC to Exit"
          lcd_line = (Features.language_f) ? LINE2+6 : LINE2+10; // project_info.current_station_name[1] = 0;
          enter_name ( project_info.current_station_name, lcd_line ); //write entered name of station
          if ( getLastKey() == ESC) 
          { //prompt_for_start = TRUE;
              if (  sdOpened == ON  ) 
              { 
                SDstop(null); 
              }
              return 0;
          }
      }
      else 
      { // auto name is on //read last station and increment
          uint16 auto_number_temp;
          auto_number_temp = project_info.station_index + project_info.station_start ;  // equals index of last station
          itoa(auto_number_temp, project_info.current_station_name, 10);
      }
  } // end project setup
  return 1;
}


/******************************************************************************
 *
 *  Name: void projectUpdate(void)
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION:
 *
 *
 *  RETURNS: NA
 *
 *****************************************************************************/
uint8 projectUpdate ( void )
{
  int str_equal;
  uint8_t lcd_line;
  
  // Automatic Project storage on
  if ( sdOpened == OFF )
  { 
    SDstart(); 
  }
    
  updateProjectInfo();
    
  str_equal= strcmp ((const char*)& project_info.current_project,  "none_selected" );
  
  if ( ( project_info.number_of_projects == 0 ) || (str_equal == 0 ) )
  { //no project has been created and count is started... auto name starting at 1
    start_new_project();
    if( getLastKey() == ESC)
    {
      if(sdOpened == ON)
      { 
        SDstop(null);
      }
      return 0;
    }
  }
  
  if ( project_info.station_index >= MAX_STATIONS ) 
  { // Check to see if all station positions are full
    max_stations_text( project_info.current_project ); // display "Max # of Stations\nFor %s Has\nBeen Exceeded.\nStart New Project",current_project
    delay_ms(1500);
    if (  sdOpened == ON  ) 
    { 
      SDstop(null); 
    }
    return 0;
  }
  
  if ( SD_CheckIfProjExists ( project_info.current_project ) == FALSE ) 
  { 
    // auto store is on and a valid project is not selected
    no_project_selected();  //display "No Project Has Been\nSelected. Please\nCreate or Select\nProject."
    delay_ms(1500);
    if (  sdOpened == ON  ) 
    { 
      SDstop(null); 
    }
    return 0;
  }
  
 if ( !Flags.auto_number_stations ) 
 { 
  // auto name is off
  enter_station_name_text();  //TEXT// display "Enter Station\nName:" LINE1,2
  YES_to_Accept(LINE3);              //TEXT// display "YES to Accept"
  ESC_to_Exit(LINE4);                //TEXT// display "ESC to Exit"
  lcd_line = (Features.language_f) ? LINE2+6 : LINE2+10; // project_info.current_station_name[1] = 0;
  enter_name ( project_info.current_station_name, lcd_line ); //write entered name of station
  
  if ( getLastKey() == ESC) 
  { //prompt_for_start = TRUE;
    if (  sdOpened == ON  )
    { 
      SDstop(null); 
    }
    return 0;
  }
 }
 else 
 { 
   // auto name is on //read last station and increment
   uint16 auto_number_temp;
   auto_number_temp = project_info.station_index + project_info.station_start ;  // equals index of last station
   itoa(auto_number_temp, project_info.current_station_name, 10);
 }
 return 1;
}

/******************************************************************************
 *
 *  Name: void measureDensity(void)
 *
 *  PARAMETERS: calibration constants, top layer thickness, counts, std counts, offset
 *
 *  DESCRIPTION: Cal consts are in PCF. Thickness is in mm.
 *               Adds offset if enabled
 *
 *  RETURNS: denisty in KG/M3
 *
 *****************************************************************************/
void  calcDensityProfile ( float DT[] ) 
{
  double A1, B1, C1, A11, A12, A13;
  double A2, B2, C2, A21, A22, A23;
  double S11,S12,S13,S21,S22,S23;
  double stand_cnt_1, stand_cnt_2;
  double thickness ;
  double DG1, K1, DG2, K2;
  float dcr_1, dcr_2;
  float dcr_1_d, dcr_2_d;
  uint32 sys_count2,  sys_count1;
  uint8 profile;
  
  // Get the calibration constants from eeprom
  // Units are in GCC
  A1 = (double)NV_CONSTANTS ( A1 );
  B1 = (double)NV_CONSTANTS ( B1 );
  C1 = (double)NV_CONSTANTS ( C1 );
  
  A2 = (double)NV_CONSTANTS ( A2 );
  B2 = (double)NV_CONSTANTS ( B2 );
  C2 = (double)NV_CONSTANTS ( C2 );
  
  A11 = (double)NV_CONSTANTS ( A11 );
  A12 = (double)NV_CONSTANTS ( A12 );
  A13 = (double)NV_CONSTANTS ( A13 );
  
  A21 = (double)NV_CONSTANTS ( A21 );
  A22 = (double)NV_CONSTANTS ( A22 );
  A23 = (double)NV_CONSTANTS ( A23 );

  S11 = (double)NV_CONSTANTS ( S11 );
  S12 = (double)NV_CONSTANTS ( S12 );
  S13 = (double)NV_CONSTANTS ( S13 );  
  
  S21 = (double)NV_CONSTANTS ( S21 );
  S22 = (double)NV_CONSTANTS ( S22 );
  S23 = (double)NV_CONSTANTS ( S23 );  
  
  // get the current standard counts
  stand_cnt_1 =(double) NV_RAM_MEMBER_RD ( DEN1_STAND );
  stand_cnt_2 = (double)NV_RAM_MEMBER_RD ( DEN2_STAND );
  

  // get current density counts
  sys_count1 = NV_RAM_MEMBER_RD ( D1_CNT_AVG );
  sys_count2 = NV_RAM_MEMBER_RD ( D2_CNT_AVG );
  
 
  checkFloatLimits ( & dcr_1 );
  checkFloatLimits ( & dcr_2 );
  
  dcr_1_d = (double)sys_count1 / stand_cnt_1;
  dcr_2_d = (double)sys_count2 / stand_cnt_2;  
  
  DG1 = (1 /B1 ) * log ( A1 / ((double)dcr_1_d + C1 ) );
  DG2 = (1 /B2 ) * log ( A2 / ((double)dcr_2_d + C2 ) );
  
  thickness = PROFILE_DEN_START;
  // .5 in to 4.00, in .25" increments
  // y = ( 4.00 - .5) * 4 + 1
  for ( profile = 0; profile < PROFILE_DENSITIES; profile++ )
  {
    if ( thickness >= 1 )
    {
      K1 = A11 * exp ( -A12 * thickness ) - A13;
      K2 = A21 * exp ( -A22 * thickness ) - A23;  
    }  
    else
    {
      K1 = S11 * exp ( -S12 * thickness ) - S13;
      K2 = S21 * exp ( -S22 * thickness ) - S23; 
    }
    
    // IF K1 goes negative, there is no effect from the bottom layer.
    if ( K1 < 0 )
    {
      DT[profile] = DG2 ;
    }
    else
    {
      DT[profile] = ((K2 * DG1) - (K1 * DG2) ) / ( K2 - K1 );
    }  
    
    DT[profile] /= KG_PCF_CONV;  // density in KGM3 // All offsets are stored in Kg/M3 units. So, change PCF units to KG/M3

    // add offset to the density if enabled
    if ( Offsets.den_offset_pos ) 
    { // density calculation with density offset. Offset should be stored in kg
      DT[profile] +=  NV_RAM_MEMBER_RD(D_OFFSET);
    }
    thickness += PROFILE_DEN_INCREMENT;  
  }  

}


/******************************************************************************
 *
 *  Name: void measureDensity(void)
 *
 *  PARAMETERS: calibration constants, top layer thickness, counts, std counts, offset
 *
 *  DESCRIPTION: Cal consts are in PCF. Thickness is in mm.
 *               Adds offset if enabled
 *
 *  RETURNS: denisty in KG/M3
 *
 *****************************************************************************/
float  calcDensityNormal ( ) 
{
  double A1, B1, C1, A11, A12, A13;
  double A2, B2, C2, A21, A22, A23;
  double S11,S12,S13,S21,S22,S23;
  double stand_cnt_1, stand_cnt_2;
  double thickness, density;
  double DG1, K1, DG2, K2;
  float dcr_1, dcr_2;
  float dcr_1_d, dcr_2_d;
  uint32 sys_count2,  sys_count1;

  
  // Get the calibration constants from eeprom
  // Units are in GCC
  A1 = (double)NV_CONSTANTS ( A1 );
  B1 = (double)NV_CONSTANTS ( B1 );
  C1 = (double)NV_CONSTANTS ( C1 );
  
  A2 = (double)NV_CONSTANTS ( A2 );
  B2 = (double)NV_CONSTANTS ( B2 );
  C2 = (double)NV_CONSTANTS ( C2 );
  
  A11 = (double)NV_CONSTANTS ( A11 );
  A12 = (double)NV_CONSTANTS ( A12 );
  A13 = (double)NV_CONSTANTS ( A13 );
  
  A21 = (double)NV_CONSTANTS ( A21 );
  A22 = (double)NV_CONSTANTS ( A22 );
  A23 = (double)NV_CONSTANTS ( A23 );

  S11 = (double)NV_CONSTANTS ( S11 );
  S12 = (double)NV_CONSTANTS ( S12 );
  S13 = (double)NV_CONSTANTS ( S13 );  
  
  S21 = (double)NV_CONSTANTS ( S21 );
  S22 = (double)NV_CONSTANTS ( S22 );
  S23 = (double)NV_CONSTANTS ( S23 );  
  
  // get the current standard counts
  stand_cnt_1 =(double) NV_RAM_MEMBER_RD ( DEN1_STAND );
  stand_cnt_2 = (double)NV_RAM_MEMBER_RD ( DEN2_STAND );
  
  // get current layer thickness
  thickness = (double)NV_RAM_MEMBER_RD ( MAT_THICK ) ;   // constant is in mm * 10. (254 = 25.4mm)
  thickness /= 25.4; // convert to inches
  // get current density counts
  sys_count1 = NV_RAM_MEMBER_RD ( D1_CNT_AVG );
  sys_count2 = NV_RAM_MEMBER_RD ( D2_CNT_AVG );
  
   dcr_1_d = (double)sys_count1 / stand_cnt_1;
  dcr_2_d = (double)sys_count2 / stand_cnt_2;  
    checkFloatLimits ( & dcr_1 );
  checkFloatLimits ( & dcr_2 );
  
  
  DG1 = (1 /B1 ) * log ( A1 / ((double)dcr_1_d + C1 ) );
  DG2 = (1 /B2 ) * log ( A2 / ((double)dcr_2_d + C2 ) );
  if ( thickness >= 1 )
  {
    K1 = A11 * exp ( -A12 * thickness ) - A13;
    K2 = A21 * exp ( -A22 * thickness ) - A23;  
  }  
  else
  {
    K1 = S11 * exp ( -S12 * thickness ) - S13;
    K2 = S21 * exp ( -S22 * thickness ) - S23; 
  }
  
  if ( K1 < 0 )
  {
   density = DG2 ;
  }
  else
  {
    density = ((K2 * DG1) - (K1 * DG2) ) / ( K2 - K1 );
  }  
  
  density /= KG_PCF_CONV;  // density in KGM3 // All offsets are stored in Kg/M3 units. So, change PCF units to KG/M3

  // add offset to the density if enabled
  if ( Offsets.den_offset_pos ) 
  { // density calculation with density offset. Offset should be stored in kg
    density +=  NV_RAM_MEMBER_RD(D_OFFSET);
  }
  
  return density;
}


/******************************************************************************
 *
 *  Name: void measureDensity(void)
 *
 *  PARAMETERS: calibration constants, top layer thickness, counts, std counts, offset
 *
 *  DESCRIPTION: Cal consts are in PCF. Thickness is in mm.
 *               Adds offset if enabled
 *
 *  RETURNS: denisty in KG/M3
 *
 *****************************************************************************/
float  calcDensityMetal ( ) 
{
  double A1, B1, C1, A11, A12, A13;
  double A2, B2, C2, A21, A22, A23;
  double S11,S12,S13,S21,S22,S23;
  double L150,L250,L175,L275,L110,L210;
  double H150,H250,H175,H275,H110,H210;  
  double stand_cnt_1, stand_cnt_2;
  double thickness, density;
  double DG1, K1, DG2, K2;
  float dcr_1, dcr_2;
  float dcr_1_d, dcr_2_d;
  uint32 sys_count2,  sys_count1;
  double F;
  
  #define CORRECTION_LIMIT 2  // 2 PCF limit. Don't use correction if under this value
  
  // Get the calibration constants from eeprom
  // Units are in GCC
  A1 = (double)NV_CONSTANTS ( A1 );
  B1 = (double)NV_CONSTANTS ( B1 );
  C1 = (double)NV_CONSTANTS ( C1 );
  
  A2 = (double)NV_CONSTANTS ( A2 );
  B2 = (double)NV_CONSTANTS ( B2 );
  C2 = (double)NV_CONSTANTS ( C2 );
  
  A11 = (double)NV_CONSTANTS ( A11 );
  A12 = (double)NV_CONSTANTS ( A12 );
  A13 = (double)NV_CONSTANTS ( A13 );
  
  A21 = (double)NV_CONSTANTS ( A21 );
  A22 = (double)NV_CONSTANTS ( A22 );
  A23 = (double)NV_CONSTANTS ( A23 );

  S11 = (double)NV_CONSTANTS ( S11 );
  S12 = (double)NV_CONSTANTS ( S12 );
  S13 = (double)NV_CONSTANTS ( S13 );  
  
  S21 = (double)NV_CONSTANTS ( S21 );
  S22 = (double)NV_CONSTANTS ( S22 );
  S23 = (double)NV_CONSTANTS ( S23 ); 

  L150 = (double)NV_CONSTANTS ( L150 );  
  L175 = (double)NV_CONSTANTS ( L175 ); 
  L110 = (double)NV_CONSTANTS ( L110 ); 
   L250 = (double)NV_CONSTANTS ( L250 );  
  L275 = (double)NV_CONSTANTS ( L275 ); 
  L210 = (double)NV_CONSTANTS ( L210 );   
  
  H150 = (double)NV_CONSTANTS ( H150 );  
  H175 = (double)NV_CONSTANTS ( H175 ); 
  H110 = (double)NV_CONSTANTS ( H110 ); 
  
  H250 = (double)NV_CONSTANTS ( H250 );  
  H275 = (double)NV_CONSTANTS ( H275 ); 
  H210 = (double)NV_CONSTANTS ( H210 );   
      
  // get the current standard counts
  stand_cnt_1 =(double) NV_RAM_MEMBER_RD ( DEN1_STAND );
  stand_cnt_2 = (double)NV_RAM_MEMBER_RD ( DEN2_STAND );
  
  // get current layer thickness
  thickness = (double)NV_RAM_MEMBER_RD ( MAT_THICK ) ;   // constant is in mm * 10. (254 = 25.4mm)
  thickness /= 25.4; // convert to inches
  // get current density counts
  sys_count1 = NV_RAM_MEMBER_RD ( D1_CNT_AVG );
  sys_count2 = NV_RAM_MEMBER_RD ( D2_CNT_AVG );
  
 
  checkFloatLimits ( & dcr_1 );
  checkFloatLimits ( & dcr_2 );
  
  dcr_1_d = (double)sys_count1 / stand_cnt_1;
  dcr_2_d = (double)sys_count2 / stand_cnt_2;  
  
  DG1 = (1 /B1 ) * log ( A1 / ((double)dcr_1_d + C1 ) );
  DG2 = (1 /B2 ) * log ( A2 / ((double)dcr_2_d + C2 ) );
  
  uint16 thick_int = (uint16)((thickness * 100 + .001 ));
  // The metal thickness requires different calculations
  if ( thick_int > 100 )
  {
    K1 = A11 * exp ( -A12 * thickness ) - A13;
    K2 = A21 * exp ( -A22 * thickness ) - A23;  
    
   if ( K1 < 0 )
   {
     density = DG2 ;
   }
   else
   {
     density = ((K2 * DG1) - (K1 * DG2) ) / ( K2 - K1 );
   }

    density /= KG_PCF_CONV;  // density in KGM3 // All offsets are stored in Kg/M3 units. So, change PCF units to KG/M3
   // add offset to the density if enabled
    if ( Offsets.den_offset_pos ) 
    { // density calculation with density offset. Offset should be stored in kg
      density +=  NV_RAM_MEMBER_RD(D_OFFSET);
    } 
    
  }  
  else  if ( thick_int == 100 || thick_int == 75 || thick_int == 50  )
  {
    // check that the thickness is 1",.75",or .50" 
    
    K1 = S11 * exp ( -S12 * thickness ) - S13;
    K2 = S21 * exp ( -S22 * thickness ) - S23; 
    
    if ( K1 < 0 )
    {
      density = DG2 ;
    }
      else
    {
      density = ((K2 * DG1) - (K1 * DG2) ) / ( K2 - K1 );
    }
    
    double metal_correction = fabs ( DG1 - DG2 );
    
    // Don't use correction if fabs(DG1-DG2) < 2PCF
    if ( metal_correction <= CORRECTION_LIMIT )
      metal_correction = 0;
    
    F = density - metal_correction;
    
    float CG;
    if ( F < 128 )
    {
      if ( thick_int == 50 )
      {
        CG = L150*F + L250;
      }
      else if (thick_int == 75 )
      {
       CG = L175*F + L275;
      }
      else // 1.00"
      {
       CG = L110*F + L210;
      }
      density -= CG;
    }
    else // F >=128
    {
      if ( thick_int == 50 )
      {
        CG = H150*F + H250;
      }
      else if ( thick_int == 75 )
      {
       CG = H175*F + H275;
      }
      else //1.00"
      {
       CG = H110*F + H210;
      }
      density -= CG;
    
    }
    density /= KG_PCF_CONV;  // density in KGM3 // All offsets are stored in Kg/M3 units. So, change PCF units to KG/M3

    // add offset to the density if enabled
    if ( Offsets.den_offset_pos ) 
    { // density calculation with density offset. Offset should be stored in kg
      density +=  NV_RAM_MEMBER_RD(D_OFFSET);
    } 
  }
  else
  {
    LCD_PrintAtPositionCentered("Metal Density Mode",LINE1+10);
    LCD_PrintAtPositionCentered("must have a Thickness",LINE2+10);
    if ( Features.SI_units == FALSE )
    {
      LCD_PrintAtPositionCentered("of .50,\".75\" ",LINE3+10);
      LCD_PrintAtPositionCentered("or 1.0\"",LINE3+10);
    }
    else
    {
      LCD_PrintAtPositionCentered("of 12.7mm,19mm",LINE3+10);
      LCD_PrintAtPositionCentered("or 25mm",LINE4+10);
    }
    
    CyDelay ( 1500 );
    density = 0;
  }

  
  return density;
}





/******************************************************************************
 *
 *  Name: void enterMatThickness(void)
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Get the user entered top mat thickness. Store in NV memory in mm units
 *
 *
 *  RETURNS: NA
 *
 *****************************************************************************/
uint8 enterMatThickness ( void )
{
  double x = 0;
  char number_ptr[20] = NULL_NAME_STRING;
  enum buttons button;
  char buff[21];
  while ( 1 )
  {
    x = NV_RAM_MEMBER_RD (MAT_THICK ) ; // get the stored thickness in mm. 
     
    enter_mat_thickness_text(); //TEXT// display "Enter Material\nThickness:" LINE1,2    
    Enter_to_Accept(LINE3);     //TEXT// display "Enter to Accept"
    ESC_to_Exit(LINE4);         //TEXT// display "ESC to Exit"

    // display thickness
    if ( Features.SI_units == FALSE )
    {
     // Convert mm to inches
     sprintf ( number_ptr, "%.2f in", (x / 25.4) );
     x = enterNumber ( number_ptr, LINE2+11, 6 );  // prompt user to enter mat thick value in inches
     if ( x == 999999.0 )
     {
      button = ESC;
     } 
    
     x *= 25.4; // convert x to mm
    }
    else 
    {
     // Display mm
     sprintf ( number_ptr, "%.2f mm", x );
     x = enterNumber ( number_ptr, LINE2+11, 6 );  // prompt user to enter mat thick value in mm
     if ( x == 999999.0 )
     {
       button = ESC;
     } 
    }
    
    // Skip if ESC pressed
    if ( button == ESC )
    { 
      return 0 ; 
    }

    // if in metal mode thickness must be increments of .25" below 1.00"  
    if ( NV_RAM_MEMBER_RD (measure_mode) == METAL_DENSITY_MODE )
    {
      double thick = x/25.4 ; // convert back to inches for testing
      uint thick_int = (uint16)((thick * 100) + .001 );
      if ( ( thick_int < 100 ) && !( (thick_int == 50) || (thick_int == 75)) )
      {
        CLEAR_DISP;
        if ( Features.SI_units == FALSE )
        {
          LCD_PrintAtPositionCentered ( "If Thick. is < 1.00\"", LINE1 + 10);
          LCD_PrintAtPositionCentered ( "Thickness must be in", LINE2 + 10);
          LCD_PrintAtPositionCentered ( ".25 inch increments", LINE3 + 10);
        }  
        else
        {
          LCD_PrintAtPositionCentered ( "If Thick. is < 25mm\"", LINE1 + 10);
          LCD_PrintAtPositionCentered ( "Thickness must be in", LINE2 + 10);
          LCD_PrintAtPositionCentered ( "6.35mm increments", LINE3 + 10);
        }
        CyDelay ( 3000 );
        continue;
      }
    }
    
    if (( x >= MIN_THICKNESS ) && ( x <= MAX_THICKNESS ))
      break;
    else
    {
      thickness_out_of_range_text();
      delay_ms ( 1500 );
      if ( Features.sound_on ) 
      { 
        output_high(BUZZER); 
        delay_ms ( 1500 );
        output_low(BUZZER); 
      }
      else
      {
       delay_ms ( 3000 );
      }
      
    }
  }
  // store thickness in mm's
  NV_MEMBER_STORE( MAT_THICK, x );  
  // Display Thickness
  CLEAR_DISP;
  count_text(7);  //TEXT// display "THICKNESS"
  if ( Features.SI_units == FALSE )
  {
   snprintf( buff,20," %.2f in.",x / 25.4);
   LCD_print ( buff ); 
  }
  else 
  {
   snprintf( buff,20," %.2f mm",x);
   LCD_print ( buff ); 
  }
  
  CyDelay ( 1500 );  
  return 1;
}


/******************************************************************************
 *
 *  Name: void saveStationData(void)
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION: save the data to a new station number
 *
 *
 *  RETURNS: NA
 *
 *****************************************************************************/

void saveStationData (float density, float DT[] )
{
  station_d.density1_count   = NV_RAM_MEMBER_RD ( D1_CNT_AVG );
  station_d.density2_count   = NV_RAM_MEMBER_RD ( D2_CNT_AVG );
  station_d.density          = (uint16)density;
  station_d.density1_stand   = NV_RAM_MEMBER_RD ( DEN1_STAND );
  station_d.density2_stand   = NV_RAM_MEMBER_RD ( DEN2_STAND ); 
  station_d.mode             = project_info.station_measurement_mode;
   
  
  // Save all the profile densities
  for ( uint8 profile = 0; profile < PROFILE_DENSITIES; profile++ )
  {
    station_d.DT[profile] = (uint16)DT[profile]; // in Kg/m3 units. So, use 16bit integer to store the data.
  }
  
  float ma =  NV_RAM_MEMBER_RD ( MARSHALL );
  checkFloatLimits ( & ma );
  station_d.MA               = ma;
 
  // update the units in which the density is displayed 
  if ( Features.SI_units == FALSE ) 
  { 
    station_d.units           = PCF;
  }
  else if ( Features.kg_gcc_units == GCC_UNITS ) 
  { 
    station_d.units           = GM_CC; 
  }
  else 
  { 
    station_d.units          = KG_M3;
  }

  station_d.den_off          = 0;

  if ( Offsets.den_offset_pos ) 
  { // set the offset mask for the station         /*/*/          station_d.offset_mask = 0;
    station_d.den_off = NV_RAM_MEMBER_RD(D_OFFSET);
  }
 
  station_d.date             = date_time_g;                  // global variable updated before count taken
  station_d.thickness          = (float)NV_RAM_MEMBER_RD (MAT_THICK ); // mm units
  station_d.battery_voltage[0] =  readBatteryVoltage(NICAD) ;
  station_d.battery_voltage[1] =  readBatteryVoltage(ALK) ;
  
  int8_t x = sizeof(GPSDATA); // need to store the GPS reading for recall and project storage
  
  memcpy ( &station_d.gps_read, &gdata , x );
  memcpy ( &eepromData.LAST_GPS_READING,  &gdata , x );
  SavePartialEepromData((uint8*)&eepromData.LAST_GPS_READING, x, offsetof( EEPROM_DATA_t,LAST_GPS_READING)) ;
  if ( !Spec_flags.recall_flag )
  {
    if ( Features.auto_store_on )
    { 
      // See if there is a conflict between the current measurement mode setting and the mode that the project is using
      if ( station_d.mode == NV_RAM_MEMBER_RD (measure_mode))
      {
        strcpy ( station_d.name, project_info.current_station_name );
        writeStation ( project_info.current_project, project_info.station_index, &station_d );
        incrementStationNumber ( project_info.current_project );            //increment number of stations within project
        project_info.station_index     = getStationNumber ( project_info.current_project );
      }  
      else
      {
        // warn the user that they must change the measurement mode or select a new project
        // "Project Storage "
        // "Measurement Mode"
        // "Does not Match "
        // "Current Mode Setting"
        error_in_measurement_mode_text();
        output_high(BUZZER); 
        CyDelay ( 3000 );
        output_low(BUZZER); 
      }
    }  
  }  
}

/******************************************************************************
 *
 *  Name: void displayCounts6 (void)
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION: 
 *
 *
 *  RETURNS: 
 *
 *****************************************************************************/
void displayCounts6 ( uint16 div )
{
  char temp_str[21];
  uint32 gm[6], sum[2];
  
  enum buttons button;
  
 // get the 6 gm tube counts;
  getGMPulseCounts ( &gm[0]);
  
  sum[0] = gm[0]/div + gm[1]/div + gm[2]/div;
  sum[1] = gm[3]/div + gm[4]/div + gm[5]/div;
  
  CLEAR_DISP;
  LCD_position (LINE1);
  sprintf(temp_str,"A1:%6lu", gm[0]/div );
  _LCD_PRINTF ( "%s",temp_str);
  
  LCD_position (LINE1 + 10 );
  sprintf(temp_str,"B1:%6lu", gm[3]/div );
  _LCD_PRINTF ( "%s",temp_str);
  
  LCD_position (LINE2);
  sprintf(temp_str,"A2:%6lu", gm[1]/div );
  _LCD_PRINTF ( "%s",temp_str);

  LCD_position (LINE2 + 10 );
  sprintf(temp_str,"B2:%6lu", gm[4]/div );
  _LCD_PRINTF ( "%s",temp_str);

  LCD_position (LINE3);
  sprintf(temp_str,"A3:%6lu", gm[2]/div );
  _LCD_PRINTF ( "%s",temp_str);

  LCD_position (LINE3 + 10 );
  sprintf(temp_str,"B3:%6lu", gm[5]/div );
  _LCD_PRINTF ( "%s",temp_str);
  
  LCD_position (LINE4);
  sprintf(temp_str,"SA:%6lu", sum[0] );
  _LCD_PRINTF ( "%s",temp_str);

  LCD_position (LINE4 + 10 );
  sprintf(temp_str,"SB:%6lu", sum[1] );
  _LCD_PRINTF ( "%s",temp_str); 


  while(1)
  {
      button = getKey( TIME_DELAY_MAX );

     if( (button == ESC) || (button == ENTER) )
     {
      break;
     }
  }
  
  
}

/******************************************************************************
 *
 *  Name: void measureDensity(void)
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Cal consts are in GCC. Collect counts and get density
 *
 *
 *  RETURNS: NA
 *
 *****************************************************************************/
uint8_t measureDensity (void) 
{
  uint32_t density_2_count, density_1_count;
  float density;
  float prof_density[PROFILE_DENSITIES]; // there are 15 densities from .50 to 4.00 in .25" increments
  float mode = NV_RAM_MEMBER_RD (measure_mode );
  enum buttons button;
  float thickness;
  // get current layer thickness
  thickness = (double)NV_RAM_MEMBER_RD ( MAT_THICK ) ;   // constant is in mm * 10. (254 = 25.4mm)
  thickness /= 25.4; // convert to inches
   // convert to integer for comparison
   uint16 thick_int = (uint16)((thickness * 100) + .001 );
  if ( !Spec_flags.recall_flag )
  {
    if ( mode == METAL_DENSITY_MODE )
    {
       // if in metal mode thickness must be increments of .25" below 1.00"  
        if ( ( thick_int < 100 ) && !( (thick_int == 50) || (thick_int == 75)) )
        {
          CLEAR_DISP;
          if ( Features.SI_units == FALSE )
          {
            LCD_PrintAtPositionCentered ( "If Thick. is < 1.00\"", LINE1 + 10);
            LCD_PrintAtPositionCentered ( "Thickness must be in", LINE2 + 10);
            LCD_PrintAtPositionCentered ( ".50inch increments", LINE3 + 10);
          }  
          else
          {
            LCD_PrintAtPositionCentered ( "If Thick. is < 25mm\"", LINE1 + 10);
            LCD_PrintAtPositionCentered ( "Thickness must be in", LINE2 + 10);
            LCD_PrintAtPositionCentered ( "12.7mm increments", LINE3 + 10);
          }
          CyDelay ( 3000 );
          return 0;
        }
    }

    if (( get_depth( ) != BS_POSITION ) && ( Features.auto_depth == TRUE ))             // test for SAFE position
    {
      depth_not_in_bs_text();                 // TEXT: "NOT IN BS POSITION
      delay_ms(1000);
      return 0;
    }
    else if (  Features.auto_depth == 0 ) 
    {
   
      // Place Gauge
      // in BS Position
      // Press Enter to Start
      gauge_in_bs_text();
      
      // get ENTER or ESC key
      do
      {
        button = getKey(TIME_DELAY_MAX);
      }  while ( (button != ESC) && (button != ENTER ) );
      if ( button == ESC )
      {
        return ESC;
      }
     }

    // Update the project storage station
    if ( Features.auto_store_on )
    {
      projectUpdate();
    }
   }
  
 // If standard count is 0, display the error, and exit 
 if (( NV_RAM_MEMBER_RD (DEN1_STAND) == 0) || (NV_RAM_MEMBER_RD (DEN2_STAND) == 0))  
 {
   invalid_den_std_text();
   delay_ms(2000);
   hold_buzzer();
   delay_ms(2000);
   while(Flags.button_pressed)
   {
     delay_ms(10);
   }
   return 0;
 }

  if ( !Spec_flags.recall_flag )
  {
    // get the counts from each tube.
    measurePulsesForDensity ( &density_2_count, &density_1_count );
    read_RTC( &date_time_g );  //read time at beginning of count
  
    // store the counts in eeprom
    if ( Features.dummy_mode == TRUE )
    {
     NV_MEMBER_STORE(D2_CNT_AVG, DUMMY_COUNT_2 );   
     NV_MEMBER_STORE(D1_CNT_AVG, DUMMY_COUNT_1 );
    } 
    else
    {
      // save the counts for RECALL function
      NV_MEMBER_STORE ( D2_CNT_AVG, density_2_count );   
      NV_MEMBER_STORE ( D1_CNT_AVG, density_1_count );
    }  

    //exit if esc or enter pressed during count
    if (getLastKey() == ESC || getLastKey() == ENTER )
    {
      return 0;
    }
  }
  
  // calculate the top layer density
  // density returned in KG/M3
  // there are three density modes - Normal, Metal, and Profile.
  if ( mode == NORMAL_DENSITY_MODE )
  {
    density = calcDensityNormal ( );
  }
  else if ( mode == METAL_DENSITY_MODE )
  {
    if ( ( thick_int <= 100 ) && !( (thick_int != 50) || (thick_int != 75)) )
    {
        LCD_PrintAtPositionCentered("Metal Density Mode",LINE1+10);
        LCD_PrintAtPositionCentered("must have a Thickness",LINE2+10);
        if ( Features.SI_units == FALSE )
        {
          LCD_PrintAtPositionCentered("of .50\",.75\"",LINE3+10);
          LCD_PrintAtPositionCentered("or 1.0\"",LINE3+10);
        }
        else
        {
          LCD_PrintAtPositionCentered("of 12.7mm, 19mm",LINE3+10);
          LCD_PrintAtPositionCentered("or 25mm",LINE4+10);
        }
        CyDelay ( 2000 );
        return 0;
    }
    else
    {
      // Metal Mode must be >1 inch or 50" or .75"
      density = calcDensityMetal ( );
    }
  }

  calcDensityProfile( prof_density ); // always need the profile densities
  
  checkFloatLimits ( & density );

  if ( Features.sound_on ) 
  { 
    output_high(BUZZER); 
  } //beep at end of test, use delay to store station data in secret
  
  LCD_light_timer(15);                // 15 seconds
  if ( Controls.LCD_light )
  { 
    KEY_B_LIGHT_ENABLE();
  }  // turn on backlight at end of test if enabled
  
  // need to store the GPS reading for recall and project storage
  uint8 x = sizeof(GPSDATA); 
  if(!Spec_flags.recall_flag) 
  { 
    station_d.gps_read.altitude = 0;
    station_d.gps_read.longitude = 0;
    station_d.gps_read.latitude = 0;
    
    if (  Features.gps_on == TRUE ) 
    {
      if ( gps_fix == '1' )
      {
        Global_ID();
        parseGPSString();
        Global_IE();
      }
      else 
      {
        CyDelay( 1100 ); // The GPS data comes in at 1PPS
        if ( gps_fix == '1' )
        {
          Global_ID();
          parseGPSString();
          Global_IE();
        }
      }
    }
    memcpy ( &station_d.gps_read, &gdata , x );
    memcpy ( &eepromData.LAST_GPS_READING,  &gdata , x );
    SavePartialEepromData((uint8*)&eepromData.LAST_GPS_READING, x, offsetof( EEPROM_DATA_t,LAST_GPS_READING)) ;
  }
  else 
  { // recall last GPS reading
    memcpy(station_d.name,project_info.current_station_name,PROJ_NAME_LENGTH);
    memcpy ( &station_d.gps_read, &eepromData.LAST_GPS_READING , x );
  }
 
  // Make density the profile denisty at 3.00 inches.
  if ( mode == PROFILE_DENSITY_MODE )
    density = prof_density[10];
  
  // store the project - station data
  saveStationData ( density,  prof_density);
  SendBLEData ( &station_d, Spec_flags.recall_flag ) ;
  
  CyDelay ( 800 );  // added longer delay for beeper (2-14-2022)
  output_low(BUZZER);
  
  if ( ( mode == METAL_DENSITY_MODE ) || ( mode == NORMAL_DENSITY_MODE ) )
  {
    displayResults ( );
  }
  else
  {
    displayProfileResults ( prof_density );
  }
  
  return 1;
}
