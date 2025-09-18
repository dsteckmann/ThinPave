/**********************************************************************************************************************
 Title:       ThinPAVE
 Company:     Instrotek
 Document No.:
 Issue:
 Description:  
 Filename:   MAin.c
 Author:      
 Processor:  PSOC 5LP   
 History: date, version, personal initials with comments about any changes
 
**********************************************************************************************************************/
/*********************************************  INCLUDE FILES  ***************************************************/

#include "Globals.h"
#include "DataStructs.h"
#include "StoreFunctions.h"
#include "Utilities.h"
#include "Interrupts.h"
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "prompts.h"
#include "Alfat.h"
#include "Utilities.h"
#include "Batteries.h"
#include "UARTS.h"
#include "ProjectData.h"
#include "Tests.h"

/*-------------------------[   Global Functions   ]---------------------------*/
extern void pulseBuzzer ( void );
extern void print_menu ( void ); 
extern void initUARTPlus ( void );
extern uint8 enterMatThickness ( void );

extern uint8 read_I2C_RTC ( date_time_t* d_time );
/*----------------------------------------------------------------------------*/
/*-------------------------[   Global variables   ]------------------------------*/

#define debug_mode 0

uint16 BleVersionHi = 0;
uint16 BleVersionLo = 0;
uint32 enableFlags;
uint32 priority;
int8 i;
char buf[128];

/*-------------------------[   Local variables   ]------------------------------*/
char wrBuffer[80];      // these are actually global
char menu = UNUSED ;
char t[DISPLAY_LENGTH];

/*-------------------------[   Local function Headers   ]------------------------------*/
/* Add an explicit reference to the floating point printf library */
/* to allow the usage of floating point conversion specifiers. */
/* This is not linked in by default with the newlib-nano library. */
asm (".global _printf_float");
/************************************* EXTERNAL VARIABLE AND BUFFER DECLARATIONS  *************************************/
extern void initTimers ( void );
extern void enter_cal_const(void) ;
extern void initGPS ( void ) ;

/************************************************  LOCAL DEFINITIONS  *************************************************/
/************************************************************************/
//  Functions Name: main()
//  Description:  The main loop in the program
//  Parameters:
//  Returns:   
/***************************************************************************/

void GetLastStationName() 
{
  // project_info.station_index updated in updateProjectInfo(), called just before this function
  if (( project_info.station_index == 0 ) || (Features.auto_store_on == 0 ))
  {
    strcpy(project_info.current_station_name, "" );
  }
  else
  {
    getStationName(project_info.current_project,project_info.current_station_name,project_info.station_index - 1);
  }  
}

int main() { //initialization and main loop
  //uint32_t size_proj;
  uint8_t last_power_down, refresh = 0;
  uint16_t  clock_timer = 0;
  enum buttons button;
  static uint8_t b_counter = 25;
  //date_time_t calibration_date;
  uint8 auto_depth_timer = 0;
  GET_BITFIELD_ADDRESSES(); // assign addresses of unions of bitfield structs
  initSystem();             // starts the interrupts, prints the version
  GPS_EN_REG_Write ( 1 );
  
  Global_IE();
  last_power_down = NV_RAM_MEMBER_RD( OFF_MODE );      //check to see if auto shutdown or operator shutdown
  Features        =  NV_RAM_MEMBER_RD(FEATURE_SETTINGS);
  NV_MEMBER_STORE(measure_mode, NORMAL_DENSITY_MODE);  // set measurement mode to Normal
 
  if ( Features.gps_on == TRUE ) 
  { 
    initGPS ();
  }

  // retrieve last display position
  gp_disp      =  NV_RAM_MEMBER_RD( GP_DISPLAY );
  splashScreen = TRUE;              // do the alkaline notification on bootup.
  SPEC_FLAG = 0;     
  // auto turn off is disabled
  if ( last_power_down == MANUAL )
  {
    OFFSETS = FLAGS = 0;            // reinitialize all settings to "0" if operator shut down
    CONTROL = 0x02;                 // initialize control register, all 0 except shut_dwn  
  }
  else 
  {                            // reinitialize settings to previous settings if auto shutdown
    Flags              = NV_RAM_MEMBER_RD( FLAG_SETTINGS  );      
    Flags.bat_volt     = 0; //FALSE;
    Flags.drift_flag   = FALSE;
    Flags.stat_flag    = 0;
    Flags.stand_flag   = 0;
    Flags.in_spec_cal  = 0;
    Controls           = NV_RAM_MEMBER_RD( CONTROL_SETTINGS);
    Offsets            = NV_RAM_MEMBER_RD( OFFSET_SETTINGS );    
    Controls.LCD_light = 0;
  }
  
  Controls.shut_dwn = TRUE;
  cnt_time          =  NV_RAM_MEMBER_RD( COUNT_TIME );   // check to see if memory is initialized
  KEYSCAN_DISABLE();                            // Keyscan disabled

  //calibration_date  =  NV_RAM_MEMBER_RD  (Constants.CAL_DATE ); //read date of last calibration date from EEPROM 
  updateProjectInfo  ( );
  GetLastStationName ( );
  CLEAR_DISP;
  KEYSCAN_ENABLE();                     // This enables the keypad task
  
  if ( last_power_down == MANUAL ) 
  {     
    read_RTC( &date_time_g );
    day_counter_g = decode_date( date_time_g ) - decode_date ( NV_RAM_MEMBER_RD (Constants.CAL_DATE) ); // gets days since last calibration
    #if 0
    if ( day_counter_g >= (365 -30) )
    {
        CLEAR_DISP;
        calibration_due ( );  //display calibration due text
        LCD_position (LINE3);
        printTimeDate( calibration_date );
        ESC_to_Exit (LINE4+4);  // display "ESC tp EXIT"  
        button = getKey(TIME_DELAY_MAX);  
    }
    #endif
  }

  Controls.buzz_enable = TRUE;
 
  // clock timer is set to read date immediatley
  clock_timer = 100;
  pulseBuzzer();
  
  checkAlkalines();
  
  //InitBleCharacteristics(); // sends BLE this gauges SN, allowing it to run and advertise

  while(1)  //main loop
  {    

   hold_buzzer();

   LCD_position(LINE1);
   ready( );  //display <READY> Time

   LCD_position(LINE2);
     
   display_offset_on ( Offsets );     // display OFFSET
   LCD_light_timer(5);
    
   if ( Controls.LCD_light )
   {
    KEY_B_LIGHT_ENABLE(); 
	 }

    refresh = 0;
    
    while ( !refresh )
    {
      // enable the shut down timer
      // enable automatic timeout and shut down feature  
      g_turn_off_cnts = SIXTY_MINUTES;       
      shutdown_timer = 0;    
      Spec_flags.auto_turn_off = TRUE;  
      global_special_key_flag = FALSE;
      while (  global_special_key_flag == FALSE )
      {  
       
        auto_depth_timer++;
        if(Features.auto_depth && (auto_depth_timer >= 18)) 
        {
          // display auto depth every 1 second
          uint8 depth = get_depth();               
          // display depth
          LCD_position(LINE2);
          display_depth( depth );   
          auto_depth_timer = 0;    
        }
          
        // read clock every 10 seconds
        if ( ++clock_timer >= 100 )
        {
          read_RTC( &date_time_g ); 
          LCD_position(LINE3);
          printTimeDate ( date_time_g );
          clock_timer = 0;
         }

         // check for GPS satellite FIX
         if ( Features.gps_on == TRUE )
         {
          if( gps_fix == '0' )
          {
             LCD_position( LINE1);  
            _LCD_PRINT ( "N");    
      		} 
          else if ( gps_fix == '1' )
      	  {
            LCD_position( LINE1);  
            _LCD_PRINT ( "F");    
          }
          else
          {
            LCD_position( LINE1);  
            _LCD_PRINT ( "E");    
          }
         }
        
         // check for charging mode
         if ( IS_CHARGER_DETECTED() )                          
         {
           LCD_position( LINE1 + 18);  
           shutdown_timer = 0;          
           _LCD_PRINT ( "C ");    
          Press_Start_to_Test_text ( LINE4 ); //"Press START to Test"
         } 
         else
    	   {
        	// read battery every 5 s
        	if ( b_counter >= 50 )
        	{
          	static uint8_t flip = 0;
          	
          	// If running off of NICADS, 
          	if ( ( Controls.bat_sel == NICAD )  && ( checkNICADVoltageWarning () == TRUE ) )
          	{
              if ( flip == 0 )
              {
                Recharge_Battery_text ( LINE4 ); //RECHARGE BATTERY
              }
              else
              {
                Press_Start_to_Test_text ( LINE4 ); //"Press START to Test"
              }  
      		  }
      	    else
      		  {
             Press_Start_to_Test_text ( LINE4 ); //"Press START to Test"
      		  }
  		  
      		  if ( Controls.bat_sel == ALK )  
          	{	
      		   if ( 0 == flip )
      		   { 
        		   LCD_PrintAtPosition( "ALK", LINE1 + 17);
        		   Recharge_Battery_text ( LINE4 ); //RECHARGE BATTERY
         		 }
        		 else
        		 {
        		   LCD_PrintAtPosition( "   ", LINE1 + 17);
               Press_Start_to_Test_text ( LINE4 ); //"Press START to Test"
        		 }  
      		  }
      		  else
      		  {
      		     LCD_PrintAtPosition( "   ", LINE1 + 17);
      		  }
      		  
      		  flip ^= 0xFF;
      		  b_counter = 0;
       		}       		  
    		  else
    		  {
    		    b_counter++;
    		  }
   	 	}
       CyDelay(100);
       Controls.buzz_enable = TRUE;       
       scan_keys();
      }   // loop for a key press       
      
      shutdown_timer = 0;    
      // enable the shut down timer
      // enable automatic timeout and shut down feature  
      g_turn_off_cnts = (uint32_t)SIXTY_MINUTES * (uint32_t)4; // non sleep mode is 66ms       
      shutdown_timer = 0;    
      Spec_flags.auto_turn_off = TRUE;  
     
      //button was pressed, determine what to do
      LCD_light_timer(30); 
      if(Controls.LCD_light)
	    {
        KEY_B_LIGHT_ENABLE(); 
	    }

      button = getLastKey();
      do
      {
          switch(button)
          {
            case THICKNESS:
                    // Get the top layer thickness
                     enterMatThickness ();
                     break;
            
            case PROJ:
                      project_menu(); 
                      refresh = TRUE;
                      break;      
            case MENU:
                      main_menu();   
                      refresh = TRUE;
                      break;
            case STAND:
                      standard_count_test();
                      refresh = TRUE;
                      break;
            case TIME:               
                      set_count_time();
                      refresh = TRUE;
                      break;
            case MA:
                      ma();
                      refresh = TRUE;
                      break;
            case STORE:        
                      no_data_to_store_text( );
                      delay_ms ( 1500 );
                      refresh = TRUE;
                      break;
            case ENTER:
                      #if 1
                      {
                        uint8 flag;
                        Spec_flags.recall_flag = FALSE;
                        while ( 1 )
                        {
                          flag = measureDensity(); 
                          button = getLastKey();
                          if ( getLastKey() == ESC )
                          {
                            refresh = TRUE;
                            Flags.button_pressed = TRUE;
                            break;
                          }
                          if ( flag == 0 )
                          {
                           refresh = TRUE;
                          }
                        }
                      }  
                      break;
                    #else
                     while ( getLastKey() == ENTER)   //start another count if enter is pressed
                     { 
                         Spec_flags.recall_flag = FALSE;
                         measurePulses6Tubes( ) ;
                     }  
                     refresh = TRUE;
                     Flags.button_pressed = TRUE;
                     break;
                      
                     #endif
            default:  
                      delay_ms(10);
                      break;          
      
        }
      button = getLastKey();
    } while ( button == TIME || button == STAND || button == MA || button == MENU || button == PROJ || button == THICKNESS );
  	// set to read clock immediately
    clock_timer = 100;
    wait_for_key_release();
    refresh = TRUE;
  }  // end of main loop  
 }

}                  // end of Main