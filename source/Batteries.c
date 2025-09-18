

/**************************   ***************************************************************** ***************************/
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
// Processor:       
// Clock:	    
//
//
// History: date, version, personal initials with comments about any changes
//
/**********************************************************************************************************************/

/*********************************************  INCLUDE FILES  ***************************************************/
#include "Globals.h"
#include "DataStructs.h"
#include "prompts.h"
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "Utilities.h"
#include "Batteries.h"

/************************************* EXTERNAL VARIABLE AND BUFFER DECLARATIONS  *************************************/

/************************************************  LOCAL DEFINITIONS  *************************************************/

#define ALKALINE_LIMIT             6.7
#define ALKALINE_WARNING_LIMIT     7.0

#define NICAD_LIMIT            6.6
#define NICAD_WARNING_LIMIT    7.1

#define NICAD_BATTERY_DIVIDER       .177

#define ALKALINE_BATTERY_DIVIDER     .177

#define CHARGING_TIME_SEC   36000 // 10 hours

/*****************************************  VARIABLE AND BUFFER DECLARATIONS  *****************************************/

static uint16 charge_timer = 0;

/******************************************************************************
 *
 *  Name: checkBatterySelected ( )  
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Check the ALKALINE_ON line to see which battery is powering the gauge
 *               The ALKALINE_ON line is controlled by the LTC4419. It turns on around 
 *               when the Nicads are  below 5.8V to 6.0v
 *            
 *  RETURNS: ALK or NICAD 
 *
 *****************************************************************************/ 
uint8 checkBatterySelected ( void )
{
 if ( ALKALINE_ON_Read () == 1 )
 {
  return ALK;
 }
 else
 {
  return NICAD;
 }
}  


// Shut down if batteires are low
void shutdownLowBatteries ( void )
{
   int8_t i;
  
  // Don't SHUT down if the charger is enabled
  if( IS_CHARGER_DETECTED()  ) 
  {
    return;
  }
  
   // ***PANIC***
   // do something here alkaline is also too low, we've run out of energ...
   //TEXT// display "    Battery Low\n  Shutdown: %d sec.",5 LINE2,3   
    battery_shutdwn_text();  
	  delay_ms(1000);
	    
	  // start 5 second count down to turn off
	  for(i = 5; i >= 0; i--)
	  {
	    if(Features.language_f)
	    { 
	    LCD_position(LINE3 + 12);
	    }  
	    else
	    { 
	     LCD_position(LINE3 + 14);
	    }  
	    _LCD_PRINTF ( "%d", i );
	     
	    delay_ms(1000);
	   }
  
    Controls.shut_dwn = TRUE;                           // set when "auto" shut off is enabled   
  
    // update status of shutdown, 1 = auto shutdown 
    NV_MEMBER_STORE(OFF_MODE,1);
      
    // write all flag settings to memory      
    NV_MEMBER_STORE( OFFSET_SETTINGS,Offsets);
   
   // NV_MEMBER_STORE( FEATURE_SETTINGS, Features );
     
    NV_MEMBER_STORE( FLAG_SETTINGS, Flags );          
   
    Controls_U.controls_bitfield = &Controls;     
    NV_MEMBER_STORE( CONTROL_SETTINGS, Controls );
  
    // Both sources are enabled, so that they will be on when power is turned back on.
    // The ON state is 0 v, which should be the condition when no power is applied.
    Global_ID();
    MICRO_POWER_DISABLE();    
        
    while(1) 
    {
     delay_ms(100);  
     MICRO_POWER_DISABLE();
    };    
 
  }
 


/******************************************************************************
 *
 *  Name: void batt_volt(void)  
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: 
 
 *               
 *  Read/Display Voltage of NiCad and Alkaline Batteries
 *  Whichever battery is selected, the minimum voltage at the 5VDC bus needs to be 5.7VDC in
 *  order for the Regulator to work properly. For a conservative state of charge, we use a window  
 *  from [7.0,6.0) as a user-notification that the NiCd battery charge is low, and go into an automatic
 *  shutdown when the NiCd battery voltage reaches 6.0V.
 * 
 *  In the case of the alkaline, we really don't care about a deep discharge, they're a primary battery
 *  so we can run the voltage down to the minimum discharge voltage of 0.9V per battery without problems.
 *  There are 6 batteries in the pack for maximum voltage benefit 0.8*9=7.2V plus the schottky drops
 *  to get the drop down to 6.0VDC for LDO dropout purposes.
 * 
 *  The question of what the 'true' battery voltage is somewhat problematic.  The circuits see a battery voltage
 *  minus a few FET, so there has to be some headroom for those.  The complicating factor 
 *  is that when the backlights turn on, the FET/diode forward voltage drop increases by another few tenths.  This has the unfortunate 
 *  effect that if the battery is right at one of the thresholds, and the user pushes a button, then the voltage can drop right below the 
 *  second threshold if we're not careful, and shut down the gauge.  
 *  On the other hand, if somebody measures the actual battery voltage and we are displaying our usable voltage, then there will be a
 *  discrepancy of a few tenths.  We can compensate for that, as long as we make sure that our minimum voltage requirements 
 *  reflect the same.
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void batt_volt(void)  

{
  float  volts_nicad, volts_alk;

  
  Flags.bat_volt =  1;              //  TRUE
  
  batt_volt_text();                 //  "NiCad Volt =\n  Alk Volt ="  LINE1,2  
  
  ESC_to_Exit(LINE4);               //  "ESC to Exit"
       
   wait_for_key_release();
   global_special_key_flag = FALSE;

   while ( global_special_key_flag == FALSE )
   { 
    volts_nicad = readBatteryVoltage(NICAD) ;   
    volts_alk = readBatteryVoltage(ALK) ;    
    
    LCD_position (LINE1 + 13);
    
    // Print the NICAD and ALKALINE Voltages
     _LCD_PRINTF ( "%5.3f",volts_nicad );       
    
    if ( checkBatterySelected() == NICAD )
    {
        _LCD_PRINT ( " *");
    }    
    else _LCD_PRINT ( "  ");
    {
        LCD_position (LINE2 + 13);
    }    
    
    _LCD_PRINTF ( "%4.2f",volts_alk); 
    
    if ( checkBatterySelected() == ALK )
    {
        _LCD_PRINT ( " *");
    }    
    else 
    {
      _LCD_PRINT ( "  ");
    }    
    
    // Check if charger plugged in
    if ( CHARGER_DETECT_Read() == CHARGER_ON )
    {
       LCD_position (LINE3 + 0);
       _LCD_PRINT ( "C ");
      // Check if charger charger enabled
      if ( CHARGER_ON_IO_Read() == 0 )
      {
       LCD_position (LINE3 + 2);
       _LCD_PRINT ( "E ");
      }
      else
      {
        LCD_position (LINE3 + 2);
       _LCD_PRINT ( "D ");
      }
    }
    else
    {
      LCD_position (LINE3 + 0);
     _LCD_PRINT ( "         ");
    }
    
    
    delay_ms(500);
    checkBatteriesToShutDown();    // select battery pack to run off of
          
  }   
  
  Flags.bat_volt = 0; //FALSE;
}


/******************************************************************************
 *
 *  Name: readBatteryVoltage ( )  
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Get the battery voltage of the NICAD or AKALINE pack
 *               
 *            
 *  RETURNS: volts 
 *
 *****************************************************************************/ 
 float readBatteryVoltage ( uint8 battery_type )
 {
 
  float battery_volts;
  float divider = NICAD_BATTERY_DIVIDER;
  
  if ( battery_type == NICAD )
  {
    battery_volts = readADCVolts( NICAD_ADC_CHAN );   
    battery_volts /=  divider;                     // voltage went through a divider, so get actual voltage 
 
  }  
  else
  {
    battery_volts = readADCVolts( ALKALINE_ADC_CHAN );    
    battery_volts /= ALKALINE_BATTERY_DIVIDER;                     // voltage went through a divider, so get actual voltage 
 
  }  
  
  
  return ( battery_volts );
 }


/******************************************************************************
 *
 *  Name: checkAlkalines ()
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Get the alkaline battery volatage
 *               Display a warning if voltage is too low
 *               Disable alkaline if too low
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 


void checkAlkalines (void)	// bootup only, check for alkaline battery state to warn user (otherwise we don't have a low alkaline warning, just a status screen.
{
  
  float battery_volts;

  battery_volts = readBatteryVoltage( ALK );
  
  if ( battery_volts < ALKALINE_WARNING_LIMIT )    // Alk battery is too low, display splash message on boot.
  { 
	
    CyDelay ( 500 );
    battery_volts = readBatteryVoltage( ALK ); // check again
    if ( battery_volts < ALKALINE_WARNING_LIMIT ) 
    {
      // display a warning on bootup if the alkaline battery is too low, 
      // just so the user is aware of the situation.
  	  display_alk_warning();
      delay_ms(1500);
    }  
  }
  
}


// Check to see if Nicads should be charged
// Return TRUE if charging is needed
BOOL checkNICADVoltageWarning (void)	
{
  
  float battery_volts;
  float warning_limit =  NICAD_WARNING_LIMIT; 
  
  
  battery_volts = readBatteryVoltage( NICAD );
  
  
  if ( battery_volts < warning_limit )    // NiCd battery is too low
  { 
	  return TRUE;
  }
  else
  { 
	  return FALSE;
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
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void checkBatteriesToShutDown ( void )
{
 float nicad_value, alk_value;
  uint8_t  nicadVoltageLowFlag = FALSE;
  uint8_t  AlkalineVoltageLowFlag = FALSE;  
  float low_warning_limit =  NICAD_LIMIT; 
  

 // Don't SHUT down if the charger is enabled
 if( IS_CHARGER_DETECTED()  ) 
 {
    return;
 }   

  nicadVoltageLowFlag = FALSE;
  nicad_value = readBatteryVoltage(NICAD);
  
  
  if ( nicad_value < low_warning_limit )
  {
    nicadVoltageLowFlag = TRUE;
  }  
  
  AlkalineVoltageLowFlag = FALSE; 
  alk_value = readBatteryVoltage(ALK);
  if ( alk_value < ALKALINE_LIMIT )   //Alk battery is low
  {
    AlkalineVoltageLowFlag = TRUE; 
  }  

  Controls.bat_sel = checkBatterySelected();
  
  // SHUT DOWN if both systems are low  
  if ( ( TRUE == nicadVoltageLowFlag ) && ( TRUE == AlkalineVoltageLowFlag ) )
  {
    shutdownLowBatteries ( );
  }  
   

}  

/******************************************************************************
 *
 *  Name:   batteryCharging
 *
 *  PARAMETERS: The battery charging routine. It is called once a second from T1 timer interrupt
 *
 *  DESCRIPTION: Monitors time and the battery voltage to enable and disable the charging circuit
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void batteryCharging ( void )
{
  
  switch ( charger_state )
  {
    case 0: //(charger is off )
          if ( CHARGER_DETECT_Read() == CHARGER_ON )
          {
            charger_state = 1; // If the charger is plugged in, go to the next state.
            CHARGER_ON_IO_Write ( 0 ); // turn on charger enable
            charge_timer = 0;   // reset charge timer to 0 seconds.  
          }
          else
          {
           CHARGER_ON_IO_Write ( 1 ); // turn off charger enable
           charge_timer = 0;   // reset charge timer to 0 seconds.  
          }
          break;
          
    case 1: // Charger is on, starting to charge.
          if ( CHARGER_DETECT_Read() == CHARGER_OFF )  // If no charger stop the charging routine and go back to state 0
          {
            charger_state = 0;               // return to non-charging state
            CHARGER_ON_IO_Write ( 1 );      // turn off charger enable
            charge_timer = 0;   // reset charge timer to 0 seconds. 
          }
          else
          {
           CHARGER_ON_IO_Write ( 0 ); // turn on charger enable
           charge_timer++ ;
           if ( charge_timer >= CHARGING_TIME_SEC )   // 4 hours = 14400 seconds
           {
            CHARGER_ON_IO_Write ( 1 ); // turn off charger enable
            charger_state = 2;         // Goto state 2 and wait for charger to be removed;
           }
          }
          break;
          
     case 2: // Charger is plugged in, but charging is done. Wait for it to be unplugged to chage state.
          if ( CHARGER_DETECT_Read() == CHARGER_OFF )  // If no charger stop the charging routine and go back to state 0
          {
            charger_state = 0;               // return to non-charging state
          }
          CHARGER_ON_IO_Write ( 1 );      // turn off charger enable
          
          break;
          
     default: 
          break;     
  
  }
  
}