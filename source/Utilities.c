

/**************************   ***************************************************************** ***************************/
//
// Title:       Elite Explorer
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
// Clock:	    12MHz	
//
//
// History: date, version, personal initials with comments about any changes
//
/**********************************************************************************************************************/

/*********************************************  INCLUDE FILES  ***************************************************/
#include "project.h"
#include "Globals.h"
#include "conversions.h"
#include "DataStructs.h"
#include "StoreFunctions.h"
#include "Alfat.h"
#include "prompts.h"
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "Tests.h"
#include "Utilities.h"
#include "Interrupts.h"
#include "SDcard.h"
#include "UARTS.h"
#include "BlueTooth.h"
#include "Batteries.h"
#include <FS.h>
/************************************* EXTERNAL VARIABLE AND BUFFER DECLARATIONS  *************************************/
 extern uint8_t getCalibrationDepth ( uint8_t depth_inches );
 extern void initPulseCntStrt( void );
 extern  void resetPacketCheck ( void ); 

 extern void shut_down_state ( void );
 extern uint32 getSerialNumber ( void );
 extern uint16 BleVersionHi;
 extern uint16 BleVersionLo;
 extern EEPROM_DATA_t  eepromData; 

/************************************************  LOCAL DEFINITIONS  *************************************************/


/*****************************************  VARIABLE AND BUFFER DECLARATIONS  *****************************************/


#ifndef language
#define language  1
#endif

// Make this a power of 2 for binary math
#define BUFFER_SIZE  16
#define BUFFER_SHIFT 4  // 2^4 = 16

typedef struct 
{
  int32_t filter[ADC_CHANNELS];
  int32_t average;
  uint8_t index;
} filter_t ;
typedef struct
{
 uint16 buffer_size;
 uint32 value_array[BUFFER_SIZE];
 uint16 value_index;
 uint32 adc_filter;
 uint8 filter_filled;
 volatile uint32 filtered_value;
} circular_buffer_t;

static circular_buffer_t filtered_temperature;
static int32 filt[ADC_CHANNELS] = {0,0,0,0};
static int32 filteredADC[ADC_CHANNELS];

/////UTILITY FUNCTIONS//////////////////////////////////////////////////////////

// return the temperature filtered data
uint32 getTemperature_FilteredData ( void )
{
 uint32 value = filtered_temperature.adc_filter;
 value /= BUFFER_SIZE;
 return value ;
}
static void init_data_filter ( circular_buffer_t * strct_data  )
{
  uint16 n;
  // set all values to zero
  for ( n = 0; n < BUFFER_SIZE; n++ )
  {
    strct_data->value_array[n] = 0;
  }
  strct_data->value_index = 0;
  strct_data->adc_filter = 0;
  strct_data->filter_filled = 0;
  strct_data->buffer_size = BUFFER_SIZE;
}
/*******************************************************************************
* Function Name: void circular_buffer_filter
********************************************************************************
*
* Summary: Implements a MA filter on the incoming data
*          It uses a circular buffer to make this fast.
*          Noise attenuation = sqrt(number of samples)
*          100 points has an attenuation of 10
*          64 has an attenuation of 8
*          1024 = 32
* Parameters:  
*
* Return:
*   None
*
*******************************************************************************/

static void circular_buffer_filter ( uint32 value, circular_buffer_t * stct_data )
{
  
  uint32 value32 = (uint32)(value) ;
  
  // the circular buffer has been filled
  if ( stct_data->filter_filled == 1 )
  {
    stct_data->adc_filter -= stct_data->value_array[stct_data->value_index];
    stct_data->adc_filter += value32;
    stct_data->value_array[stct_data->value_index++] = value32;
    if ( stct_data->value_index >= stct_data->buffer_size )
    {
      stct_data->value_index = 0;
    }
  }
  else
  {
    stct_data->adc_filter -= stct_data->value_array[stct_data->value_index];
    stct_data->adc_filter += value32;
    stct_data->value_array[stct_data->value_index++] = value32;

    
    // Check if the filter aray is filled
    if ( stct_data->value_index >= stct_data->buffer_size )
    {
      stct_data->value_index = 0;
      stct_data->filter_filled = 1;
    }
   
  }
  
}


/******************************************************************************
 *
 *  Name: readADC()
 *
 *  PARAMETERS: ADC channel
 *
 *  DESCRIPTION: Gets the filtered value, then converts it to a voltage 
 *            
 *  RETURNS:    voltage as a float value
 *
 *****************************************************************************/ 

float readADCVolts ( uint8_t channel )
{

  int32 k;
  float volts;
  
  k  = filteredADC[channel];
  
  if ( channel == TEMPERATURE_ADC_CHAN )
  {
   volts = ADC_DelSig_CountsTo_Volts ( (int32)getTemperature_FilteredData() );
  }
  else
  {
    volts = ADC_DelSig_CountsTo_Volts(k);
  }  

  return volts;
}

/*******************************************************************************
* Function Name: LowPassFilter()
*
*
* Summary:
*  IIR filter for the adc values
*  At 33HZ smple rate, it takes about 3 seconds to settle down 
*
* Parameters:  THe latest adc value and the RTD channel
*
* Return:
*   The filtered ADC value
*
*******************************************************************************/
void LowPassFilter(int32 input, uint8 channel)
{
   
    input <<= 8;
    filt[channel] = filt[channel] + (((input-filt[channel]) >> 8) * 17); // higher the multiplier the faster the filter
    filteredADC[channel] = (filt[channel]>>8) + ((filt[channel] & 0x00000080) >> 7);
    
}

/*******************************************************************************
* Function Name: LowPassFilter()
*
*
* Summary:
*  IIR filter for the adc values
*  At 33Hz sample rate, it takes about 1.5s to settle down
*  Low Pass Filter 2 simplies the IIR filter. It probably looses a little resolution
*  Changing the multiplier coeffcient, changes the step response. The larger, the faster the response 
* Parameters:  THe latest adc value. 
*  
* Return:
*   The filtered ADC value
*
*******************************************************************************/
void LowPassFilter2(int32 input, uint8 channel)
{
   
    input <<= 8; // prescale * 256 to keep precision
    filt[channel] = filt[channel] + (((input-filt[channel]) >> 8) * 32); // higher the multiplier the faster the filter
    
    // This removes the prescale
    filteredADC[channel] = (filt[channel]>>8) ; 
    
}
/*******************************************************************************
* Function Name: ADC_RTD_ISR
*
*
* Summary:
*   The default Interrupt Service Routine for ADC.
*
*  The ADC will be halted. THe 32 bit result is read. THis also clears the interrupt flag
*  The analog mux will be incremented to the next channel. The next conversion is started.  
*
* Parameters:  
*
* Return:
*   None
*
*******************************************************************************/
static CY_ISR( ADC_ISR)
{
  static uint8_t mux = 0;
  static int32_t adc;
 // static float volts;
  ADC_DelSig_StopConvert();
   
  adc = ADC_DelSig_GetResult32();
 // volts = ADC_DelSig_CountsTo_Volts (adc);
  
  // filter the ADC value. Use the filteredADC value to get voltage
  LowPassFilter((uint32)adc,mux);
  
  if ( mux == TEMPERATURE_ADC_CHAN )
  {
    circular_buffer_filter ( adc, &filtered_temperature);
  }  
  
  mux++ ;

  if ( mux >= ADC_CHANNELS )
  {
    mux = 0;
  }
   
  AMux_Select(mux);

  DEPTH_PWR_EN_SEL_Write ( 1 );  // Selects Depth sensor channel and turns on Deptrh Sensor power.

  ADC_DelSig_StartConvert();
 
}

/******************************************************************************
 *
 *  Name: initADC ( )
 *
 *  PARAMETERS: None
 *
 *  DESCRIPTION: Initialize the DelSig ADC. Start ISR to Read values
 *               
 *               
 *               CH0 - NICAD_ADC
 *               CH1 - TEMP_SENS_ADC      
 *               CH2 - ALK_ADC   
 *               CH2 - DEPTH_SENS_ELITE
 *              
 *
 *  RETURNS:None
 *
 *****************************************************************************/ 

void initADC ( void )
{

  AMux_Start();
  AMux_Select(0);
  
  init_data_filter ( &filtered_temperature );
  
  // Start the ADC
  ADC_DelSig_Start();
  
  // Enable the ADC interrupt,and give the ISR vestor
  // This must be after ADC_START
  ADC_DelSig_IRQ_StartEx( ADC_ISR );
  
  // Start the conversion    
  ADC_DelSig_StartConvert();
}


/******************************************************************************
 *
 *  Name: initTimers ( void )
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Initialize the timers
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void initTimers ( void )
{
 
 // key scan timer
 initInterrupts();
  //Counter_Divider_Start();
}  



/******************************************************************************
 *
 *  Name: initIOLines ( void )
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Initialize the GPIO
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void initIOLines ( void )
{
 uint8 n;
 
 DIS_YES_ON_Write ( 1 ); // Disable the YES/ON key from toggling the LTC2951 power controller.
 MICRO_POWER_ENABLE() ;
 KEY_B_LIGHT_DISABLE(); 

 for ( n=0; n < 4; n++ )
 {
  output_high( BUZZER ); 
  CyDelay(50);
  output_low( BUZZER ); 
  CyDelay(50);
 }


}

/******************************************************************************
 *
 *  Name: getPCB_Rev()
 *  PARAMETERS: 
 *
 *  DESCRIPTION: Read the rev 0,1,2 inputs to get the PCB revision number
           
 *  RETURNS: NA 
 *
 *****************************************************************************/ 
void getPCB_Rev ( void )
{
  uint8 rev = 0;
  
  rev = REV_0_Read() & 0x01;
  rev |= ( REV_1_Read() & 0x01 ) << 1 ;
  rev |= ( REV_2_Read() & 0x01 ) << 2 ;
  PCB_REV = rev;
}


/******************************************************************************
 *
 *  Name: initSystem()
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: initializt the GPIO lines
 *               
 *
 *
 *
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 
void initSystem ( void )
{
  initPulseCntStrt(  );
  initIOLines();
  
  CHARGER_ON_IO_Write ( 0 ); // turn on charger enable
  
  getPCB_Rev();
  
  initTimers ( );
  init_lcd ( );
 
  I2C_1_Start();
  I2C_1_DisableInt();

  initAlfat();
   
  SDstart();
  // Check three times to see if card is detected.
  if ( SD_CARD_DETECT_Read() == SD_CARD_OUT) 
  {
    CyDelay(250);
    if ( SD_CARD_DETECT_Read() == SD_CARD_OUT) 
    {
      CyDelay(250);
      if ( SD_CARD_DETECT_Read() == SD_CARD_OUT) 
      {
        //SD Card Not Detected;
        no_sd_card_detected_count_text ( );
        CyDelay ( 1000 );
      }   
    }    
  }
  else
  {
    CreateDir( "Project" );
  }  
 SDstop(0);

  // Copy EEPROM NV memory data to RAM copy nv_storage
  getNVFromEEProm();
  
  display_version();
  delay_ms(1500);

 initADC ( ); 

 BlueToothUart_Start();
 resetPacketCheck ( );
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

Bool check_temp(Bool display)  // Checks Internal Temperature of Gauge
{
  //uint8_t key_timer = 249;
  float voltage = 0;     
  enum buttons button;
  float temp_calc;
 
  CyDelay ( 50 );
  Global_ID();
  init_data_filter ( &filtered_temperature );
  Global_IE();  
  
  while ( filtered_temperature.filter_filled != 1 )
  {};
  
  Global_ID();
  voltage = readADCVolts(TEMPERATURE_ADC_CHAN);
  Global_IE();  
  // °C = (Vout - 500mV) / 10 mv/°C
  temp_calc = (voltage - .5) * 100;

  if ( Spec_flags.self_test)
  {
    if((temp_calc < 65.0)  && (temp_calc > -35.0))
      return(1);  
    else
      return(0);
  }
  else
  {
    if( !display )
    {   
      
      if ( temp_calc >= 65.0 )                           
      {    
        warning_text(0);  //TEXT// display "  *** WARNING ***\n   Gauge Too HOT" LINE2,3       
        hold_buzzer();
        delay_ms(3000);      
        CLEAR_DISP;;
      }   
    }
    else
    {
      gauge_temp_text();  //TEXT// display "Gauge Temp =" LINE2     
      ESC_to_Exit(LINE4); //TEXT// display "ESC to Exit"
 
      while(1)
      {
        LCD_position(LINE2+13);
        
        sprintf ( lcdstr, "%3.0f %cC ",(double)temp_calc,0xDF);
        LCD_print ( lcdstr );
        Global_ID();
        voltage = readADCVolts(TEMPERATURE_ADC_CHAN);
        Global_IE(); 
        // °C = (Vout - 500mV) / 10 mv/°C
        temp_calc = (voltage - .5) * 100;
        
        button = getKey ( 1500 );
        
        if((button == ENTER) || (button == ESC) || (button == MENU))
		    {
		     break;
	     	};
      }    
    }   
  }
 
  return(1); 
}

/******************************************************************************
 *
 *  Name: shut_down_test ()
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Does a HARD shut down
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void shut_down_test (void ) 
{
 
 char time = 0;
 enum buttons button;
 
 //"  Hold OFF Key Down "        
 //"    for 25secs      "        
 //"                    "
 //"  Press ESC to Exit "    
 shut_down_txt();

 button = getKey ( TIME_DELAY_MAX ) ;

 // If OFF/NO key was hit do test
 if ( button == NO )
 {
   
   //"  TEST IN PROCESS   ");        
   //" Keep Pressing OFF  ");        
   //"  Press ESC to Exit ");  
   
   press_off_txt();
   LCD_position (LINE3) ;

   // Disable auto shut down
   Controls.shut_dwn = FALSE; 
   // Keyscan disabled
   KEYSCAN_DISABLE();  
   while(1)
   {
  
    delay_ms ( 1300 );
    LCD_position (LINE3+ 8) ;
    if(Features.language_f)
    {
      _LCD_PRINTF( "%u sec. ",++time );
    
    }
    else
    {
      _LCD_PRINTF( "%u seg.",++time ); 
    }  
    
    scan_keys();  
    button = getLastKey();
    
    if ( button == ESC )               // ENTER was pressed, reset count
    { 
      break;
    }  
   }
 } 

  // re-enable auto shut down and key scan
  Controls.shut_dwn = TRUE; 
  KEYSCAN_ENABLE() ;

}

/******************************************************************************
 *
 *  Name: light_test ()
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Turns on the LCD light and LEDs for 5 seconds 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void light_test (void ) 
{
  enum buttons button;
  
  // Press START 
  // to turn on LIGHT
  // for 5 seconds
  light_test_txt();
  
  while(1)
  {
    button = getKey( TIME_DELAY_MAX );
    if ( button == ESC )
    {
      return;
    }
    else if ( button == ENTER )
    {
      break;
    }
  }
  
  lights_are_on_txt(  );
  
  KEY_B_LIGHT_ENABLE()  ;  
  
  delay_ms ( 4000 );
 

  KEY_B_LIGHT_DISABLE();
}

/******************************************************************************
 *
 *  Name: raw_count_test ()
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: takes a 5 second count 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void raw_count_test (void ) 
{
  uint32_t density2_cnt = 0;
  uint32_t density1_cnt = 0;
  
  enum buttons button;
    
  // Press START to take 10 second count
  count_test_txt();
  
  while(1)
  {
    button = getKey( TIME_DELAY_MAX );
    if ( button == ESC )
    {
      return;
    }
    else if ( button == ENTER )
    {
      break;
    }
  }
  
  CLEAR_DISP;
  Spec_flags.self_test = FALSE;
  self_test_text(2);  // display "     Tube Test\n    In Progress" on LINE2 and LINE3    

  // run count for 30 seconds
  measurePulses ( LINE3, 30, &density2_cnt, &density1_cnt);  
 
  Spec_flags.self_test = FALSE;
  
  display_count_text ( density1_cnt, density2_cnt );

  while(1)
  {
    button = getKey( TIME_DELAY_MAX );
    break;
  }

}

/******************************************************************************
 *
 *  Name: USB_store_test ()
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Press a key and the KEY name appears.
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void USB_store_test (void )  
{
  char j;
  char date_string[30];
  char name_string[30];
  char temp_str[30];
  enum buttons button;
  FILE_PARAMETERS fp; 
  uint8_t n;
  
    // turn on power
    AlfatStart();
    //center the string
    
    char alfat_error = NO_ERROR;
    if ( AlfatGetVersion( temp_str ) != 0x3030 )
    {
      alfat_error = ERROR;
    }  
    
    temp_str[6] = 0;
      
    if ( NO_ERROR == alfat_error )
    {
      CLEAR_DISP;
      printOnLCDLineLocString ( 1, centerStart( strlen(temp_str)), temp_str );
      CyDelay(2000);
    }  
    else
    {
      printOnLCDLineLocString ( 1,centerStart(11), "ALFAT Error");
      CyDelay(2000);
      AlfatStop();   
      return;
    }
   
    
    USB_text(0); // display "  Insert External\n Drive in USB Port\n     Press ENTER" on LINE1, LINE2 and LINE4 
    while ( 1 )
    {
      button = getKey ( TIME_DELAY_MAX );
      
      if((button == ENTER) || (button == ESC))
      {
        break;
      }  
        
    }      
    
    if(button == ESC)
    {
     AlfatStop();   
     return;
    }  
    
    
    // If the drive can be mounted, stat test data will be stored.
    j = 0;
    while ( !check_for_USB() && j < 5 ) 
    { 
     
     USB_text(2);  // display " No USB Device "
     delay_ms ( 100 );  
     j++;   
    }; 
    
    if ( j >= 5 )
    {
      // Put ALFAT in reduced power mode
      AlfatStop();   
      return;
    }
    
    USB_text(1);  // display "    Writing Data\n    To USB Drive" on LINE2 and LINE3
    hold_buzzer();
    
    if ( initialize_USB( TRUE ) )
    {
      // make the file name string
      nullString(name_string, sizeof(name_string) );
      strcat( name_string, "USB STORE_" );
      
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
      USB_open_file  ( name_string , &fp );  
    
      // make header
      // get the current time, and turn into a string
      read_RTC( &date_time_g );
      getTimeDateStr ( date_time_g, date_string );
      sprintf( temp_str, "%s\t\r\n", date_string );
      AlfatWriteStr(&fp,temp_str);
    
      nullString(temp_str, sizeof(temp_str));
      sprintf( temp_str, "Storage Test\t\r\n" );
      
      AlfatWriteStr(&fp,temp_str);
      // close file     
      AlfatFlushData(fp.fileHandle);
      AlfatCloseFile(fp.fileHandle);  
      CyDelay ( 1500 );
    }   
   
    AlfatStop();   
  
}

/******************************************************************************
 *
 *  Name: key_padtest()
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Press a key and the KEY name appears.
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void key_pad_test (void )  
{
    enum buttons button;
    press_key_text();     // PRESS any KEY
    do
    {
      LCD_PrintBlanksAtPosition ( 20, LINE3 );
      button = getKey ( TIME_DELAY_MAX );
      
      switch ( button )
      {
        case ZERO:
                  LCD_PrintAtPositionCentered ( "0", LINE3+10 );
                  break;
        case ONE:
                  LCD_PrintAtPositionCentered ( "1", LINE3+10 );
                  break;
        case TWO:
                  LCD_PrintAtPositionCentered ( "2", LINE3+10 );
                  break;
        case THREE:
                  LCD_PrintAtPositionCentered ( "3", LINE3+10 );
                  break;
        case FOUR:
                  LCD_PrintAtPositionCentered ( "4", LINE3+10 );
                  break;
        case FIVE:
                  LCD_PrintAtPositionCentered ( "5", LINE3+10 );
                  break;          
        case SIX:
                  LCD_PrintAtPositionCentered ( "6", LINE3+10 );
                  break;
        case SEVEN:
                  LCD_PrintAtPositionCentered ( "7", LINE3+10 );
                  break;
        case EIGHT:
                  LCD_PrintAtPositionCentered ( "8", LINE3+10 );
                  break;
        case NINE:
                  LCD_PrintAtPositionCentered ( "9", LINE3+10 );
                  break;
        case UP:
                  LCD_PrintAtPositionCentered ( "UP", LINE3+10 );
                  break;
        case DOWN:
                  LCD_PrintAtPositionCentered ( "DOWN", LINE3+10 );
                  break;          
        case THICKNESS:
                  LCD_PrintAtPositionCentered ( "THICKNESS", LINE3+10 );
                  break;          
        case CE:
                  LCD_PrintAtPositionCentered ( "CE", LINE3+10 );
                  break;
        case ESC:
                  LCD_PrintAtPositionCentered ( "ESC", LINE3+10 );
                  break;
        case PROJ:
                  LCD_PrintAtPositionCentered ( "PROJ", LINE3+10 );
                  break;
        case MENU:
                  LCD_PrintAtPositionCentered ( "MENU", LINE3+10 );
                  break;
        case STORE:
                  LCD_PrintAtPositionCentered ( "STORE", LINE3+10 );
                  break;
        case YES:
                  LCD_PrintAtPositionCentered ( "YES", LINE3+10 );
                  break;                                                        
        case NO:
                  LCD_PrintAtPositionCentered ( "NO", LINE3+10 );
                  break;                                                        
        case STAND:
                  LCD_PrintAtPositionCentered ( "STD", LINE3+10 );
                  break; 
        case TIME:
                  LCD_PrintAtPositionCentered ( "TIME", LINE3+10 );
                  break;           
        case MA:
                  LCD_PrintAtPositionCentered ( "MAX", LINE3+10 );
                  break;           
        case ENTER:
                  LCD_PrintAtPositionCentered ( "START/ENTER", LINE3+10 );
                  break;           
        
       default:   LCD_PrintAtPositionCentered ( "UNKNOWN KEY", LINE3+10 );
                  break;   

      }
      delay_ms(500);
      
    } 
    while ( ESC != button );

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

void shut_down_text(void)  // sets OFF_MODE and powers OFF gauge
{
  uint8 n;
  
  isr_ON_OFF_Disable();
  isrTIMER_1_Disable();
  
  NV_MEMBER_STORE(OFF_MODE, 0);                    // update status of shutdown, 0 = operator shutdown

  Controls.shut_dwn = TRUE;                           // set when "auto" shut off is enabled   
      
  // write all flag settings to memory      
  NV_MEMBER_STORE( OFFSET_SETTINGS,Offsets);
       
  //NV_MEMBER_STORE( FEATURE_SETTINGS, Features );
         
  NV_MEMBER_STORE( FLAG_SETTINGS, Flags );          
     
 Controls_U.controls_bitfield = &Controls;     
 NV_MEMBER_STORE( CONTROL_SETTINGS, Controls );     
 Global_ID();

 shut_down_text_text();  //TEXT// display "      Shutdown" LINE2  
 delay_ms(500);
 for ( n=0; n < 5; n++ )
 {
  output_high( BUZZER ); 
  CyDelay(100);
  output_low( BUZZER ); 
  CyDelay(100);
 }

  delay_ms(1500);
  while(1) 
  {
   MICRO_POWER_DISABLE();   
   delay_ms(1000);  
  };    
  
  
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

void print_string_backward(char *temp_str, uint8_t position)  //prints string from right to left starting at position 
{  
  uint8_t length;
  length = strlen(temp_str);
  LCD_position(position + (1 - length));
  _LCD_PRINTF ( "%s",temp_str);
}


/******************************************************************************
 *
 *  Name: review_std_counts(void)
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: There are up to 30 standard counts stored
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 
void review_std_counts(void)
{
  Bool auto_scroll_advance;
  uint8_t i, j, number_of_counts, index;
  uint16_t D1SC[30],D2SC[30],loop_cnt;
  date_time_t TSC[30];
  enum buttons button;
   
  index = NV_RAM_MEMBER_RD(stand_test.std_index); // index is always 1-30
  index -= 1;  //match index to array element number
  number_of_counts = NV_RAM_MEMBER_RD(stand_test.std_counts);   
  if ( number_of_counts != 0 )
  {        
    i = index;
    j = 0;
   
    do   //read all valid counts from memory
    {
      D1SC[i] = NV_RAM_MEMBER_RD(stand_test.dense_count1_x[i] );  //each count is 2 bytes long
      D2SC[i] = NV_RAM_MEMBER_RD(stand_test.dense_count2_x[i] );  //each count is 2 bytes long   
      TSC[i] = NV_RAM_MEMBER_RD(stand_test.date_count_x[i]  );   //each date is 4 bytes long
      i = (i!=0) ? (i-1) : 29;
      j++;
    } while((i!=index) && (j<number_of_counts));  //index is array element with most recent data      
   
    i = index;
    j = 1;
   
    while(1)                                   //remains in display mode until button is pressed
    {
      CLEAR_DISP;
      LCD_position(LINE1);
      
      printTimeDate ( TSC[i] );
  
      LCD_position(LINE2);
      
       sprintf( lcdstr, "D1 %u  D1 %u", D1SC[i], D1SC[i] ) ; 
       LCD_print ( lcdstr ) ;
    
      if(2*j <= number_of_counts)       
      {          
        i = (i!=0) ? (i-1) : number_of_counts - 1;
        
        LCD_position(LINE3);
        printTimeDate ( TSC[i] );
        
        LCD_position(LINE4);
        sprintf (lcdstr, "D1 %u  D2 %u",D1SC[i],D2SC[i]);
        LCD_print ( lcdstr );
     }       
      auto_scroll_advance = FALSE;     
      if(Features.auto_scroll)
      {
        loop_cnt = 0;
        while(1)
        {          
          button = getKey(50);
          if( (button == STORE) || (button == ESC) || (button == MENU) || (button == UP) || (button == DOWN) || (++loop_cnt >= 100))
            break;
        }
        if(loop_cnt >= 100)
          auto_scroll_advance = TRUE;
        else
          auto_scroll_advance = FALSE;
      }
      else
      {
        while(1)
        {
          button = getKey(TIME_DELAY_MAX);
          if((button == ESC) || (button == UP) || (button == DOWN) || (button == MENU) || (button == STORE) )
            break;
        }        
      }
      if((button == DOWN) || auto_scroll_advance)     
      {
        i = (i!=0) ? (i-1) : number_of_counts - 1;         
        j = (j+1)%((number_of_counts+1)/2);
        if(j==0)
          j = (number_of_counts+1)/2;
      }     
      else if ( button == STORE )
      {
       storeStdCountsToUSB( 1 );
      }
      else if(button == UP)       
      {       
        if(number_of_counts > 2)
        {
          if((i<(number_of_counts-2)) && i!=0)
            i = (i+3)%30;
          else if(i!=0)
            i = (i+3)%((number_of_counts+1)/2);
          else
            i = ((number_of_counts%2)==0) ? i+3 : i+2;             
        }
        else
        {
          i = (i+1)%number_of_counts;
        }   
        j -= 1;
        if(j==0)
        {
           j = (number_of_counts+1)/2;
        }   
      }
      else
        break;
    } 
  }
  // There are no standard counts available
  else
  {
    CLEAR_DISP;
    no_valid_std_count_text();
    CyDelay (2000);
  }
}

