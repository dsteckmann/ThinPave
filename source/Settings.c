/**************************   ***************************************************************** ***************************/
//
// Title:        Elite
// Company:     Instrotek
//
// Document No.:
// Issue:
// Description:  
//
// Filename:   Settings.c
// Author:      
//
// Processor:   Microchip PIC24HJ256GP206    
// Clock:	    12MHz	
//
//
// History: date, version, personal initials with comments about any changes
//
/**********************************************************************************************************************/

/*********************************************  INCLUDE FILES  ***************************************************/
#include "project.h"
#include "Globals.h"
#include "DataStructs.h"
#include "conversions.h"
#include "prompts.h"
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "Tests.h"
#include "Utilities.h"
#include "UARTS.h"
#include "Alfat.h"

/************************************* EXTERNAL FUNCTION DECLARATIONS  *************************************/
extern float convertDensityToPCF ( float value_in_operational_units);
extern  void calibrateDepthSensor ( void ) ;
extern  int isNaN ( uint8_t  f[]) ;
extern void disableGPS ( void ) ;
extern void initGPS ( void ) ;

/************************************* EXTERNAL VARIABLE AND BUFFER DECLARATIONS  *************************************/

/************************************************  LOCAL DEFINITIONS  *************************************************/

/*****************************************  VARIABLE AND BUFFER DECLARATIONS  *****************************************/

struct ALFAT_DATE 
{
  uint32_t  second2   : 5  ;  
  uint32_t  minute    : 6  ;
  uint32_t  hour      : 5  ;
  uint32_t  day       : 5  ;
  uint32_t  month     : 4  ;
  uint32_t  year      : 7  ;

}  s_alfat_date;





 /******************************************************************************
 *  Name:     void displayValueWithUnits ( float value_in_kg, uint8_t position )
 *  
 *  PARAMETERS: A value in kg/m3, the LCD display position, string that is displayed
 *
 *  DESCRIPTION: Convert the kg/m3 value to the units that are operational
 *               Display the new values to the appropriate decimal place
 *               at the LCD position, and then add the unit tag (kg,pcf,gcc)
 *
 *  RETURNS: density in operational units, density displayed
 *
 *****************************************************************************/ 
 
float displayValueWithUnits ( float value_in_kg, uint8_t position, char * display)
{
  float value;
  
  LCD_position ( position );
  
  if ( Features.SI_units == FALSE )
  {
     value = value_in_kg *  KG_PCF_CONV;
     sprintf ( display, "%.1f PCF", value );
  }
  else 
  {
    if ( Features.kg_gcc_units == GCC_UNITS )
    {
      value = value_in_kg / 1000;
      sprintf(display,"%.3f GCC", value );
    }
    else
    {
      value = value_in_kg;
      sprintf(display,"%.0f KG/M3", value );
    }
  }
 
  LCD_print ( display );
  
  return value;
  
  
}
 
  /******************************************************************************
 *  Name:     void displayValueWithUnitsBW ( float value_in_kg, uint8_t position )
 *  
 *  PARAMETERS: A value in kg/m3, the LCD display position, string that is displayed
 *
 *  DESCRIPTION: Convert the kg/m3 value to the units that are operational
 *               Display the new values to the appropriate decimal place
 *               at the LCD position, and then add the unit tag (kg,pcf,gcc)
 *
 *  RETURNS: density in operational units, density displayed
 *
 *****************************************************************************/ 
 
float displayValueWithUnitsBW ( float value_in_kg, uint8_t position, char * display)
{
  float value;
  
  
  if ( Features.SI_units == FALSE )
  {
     value = value_in_kg *  KG_PCF_CONV;
     sprintf ( display, "%.1f PCF", value );
  }
  else 
  {
    if ( Features.kg_gcc_units == GCC_UNITS )
    {
      value = value_in_kg / 1000;
      sprintf(display,"%.3f GCC", value );
    }
    else
    {
      value = value_in_kg;
      sprintf(display,"%.0f KGM3", value );
    }
  }
 
  print_string_backward( display, position);    
  return value;
  
  
}
 
 /******************************************************************************
 *  Name:    convertDensityToKgM3 ( float value_in_operational_units)
 *  
 *  PARAMETERS: A density value in kg/m3,pcf, or gcc ( depends on current units setting )
 *
 *  DESCRIPTION: Converts the unit settings density to kg/m3
 *
 *  RETURNS: density in kg/m3
 *
 *****************************************************************************/ 
 
float convertDensityToKgM3 ( float value_in_operational_units)
{
  
  float value_in_kg;
  
  if ( Features.SI_units == FALSE )
  {
     // convert PCF to KG/M3
     value_in_kg = value_in_operational_units /  KG_PCF_CONV;
  }
  else 
  {
    if ( Features.kg_gcc_units == GCC_UNITS )
    {
     // convert GCC to KG/M3
      value_in_kg = value_in_operational_units * 1000;
    }
    else
    {
      // convert KG/M3 to KG/M3
      value_in_kg = value_in_operational_units;
    }
  }
  return value_in_kg;

}

 /******************************************************************************
 *  Name:    convertDensityToPCF ( float value_in_operational_units)
 *  
 *  PARAMETERS: A density value in kg/m3,pcf, or gcc ( depends on current units setting )
 *
 *  DESCRIPTION: Converts the unit settings density to PCF
 *
 *  RETURNS: density in PCF
 *
 *****************************************************************************/ 
 
float convertDensityToPCF ( float value_in_operational_units)
{
  
  float value_in_pcf;
  
  if ( Features.SI_units == FALSE )
  {
     // value is alreasy in PCF
     value_in_pcf = value_in_operational_units ;
  }
  else 
  {
    if ( Features.kg_gcc_units == GCC_UNITS )
    {
     // convert GCC to PCF
     value_in_pcf = (value_in_operational_units * 1000) * KG_PCF_CONV;
    }
    else
    {
      // convert KG/M3 to PCF
      value_in_pcf = value_in_operational_units * KG_PCF_CONV ;
    }
  }
  return value_in_pcf;

}

/******************************************************************************
 *  Name:    convertKgM3DensityToUnitDensity ( float value_in_kg )
 *  
 *  PARAMETERS: A density value in kg/m3
 *
 *  DESCRIPTION: Converts the unit settings density to kg/m3,pcf, gcc
 *               depending on syytems units setting
 *            
 *  RETURNS: density in system units
 *
 *****************************************************************************/ 
 
float convertKgM3DensityToUnitDensity ( float value_in_kg, uint8_t units)
{
  
  float value;
  
  if ( units == PCF )
  {
     value = value_in_kg *  KG_PCF_CONV;
  }
  else if ( units == GM_CC )
  {
   value = value_in_kg / 1000;
  }
  else
  {
    value = value_in_kg;
  }
  
  
  return value;  // density in kg,pcf, or gcc

}
 
 /******************************************************************************
 *  Name:     enable_disable_features()
 *  
 *  PARAMETERS: None
 *
 *  DESCRIPTION: enable/disable LCD backlight, auto depth, or avg std mode, gps,auto scroll
 *
 *  RETURNS: key
 *
 *****************************************************************************/ 
 

enum buttons enable_disable_features(char c)  
{  
 // BYTE enable = TRUE;
  char temp_str[20];
  enum buttons button;
 
 
if ((Controls.LCD_light && (c == 'L')) || (Features.auto_scroll && (c == 'S')) || (Features.avg_std_mode && (c == 'A')) 
   || (Features.auto_store_on && (c == 'O')) || (Features.sound_on && (c == 'B')) ||  (Features.gps_on == 1 && (c == 'G'))
   ||  (Features.auto_depth == 1 && (c == 'D')) )  //the feature in question is enabled
 {
  //  enable=FALSE;
  if(Features.language_f)
  {
    sprintf(temp_str,"Disable");
  }   
  else
  { 
    sprintf(temp_str,"Inhabilte");
  }  
 }
 else
 {
    if(Features.language_f)
    {
      sprintf(temp_str,"Enable");
    }    
    else
    {
      sprintf(temp_str,"Permita");
    }  
 }
  
  enable_disable_text(temp_str,c);  //TEXT// display Enable/Disable screen for selected feature  
  Press_YES_or_NO(LINE4);
  
  while(1)
  {
   button = getKey(TIME_DELAY_MAX);
   
   if ( (button == ESC) || (button == YES) || (button == NO) || (button == MENU) )
   {
     break;
   }     
  }    
  
  if ( button == YES )
  {
    if(c == 'L')
    {        
      Controls.LCD_light ^= 1;                    // toggle LCD backlight flag
      if(Controls.LCD_light)
	    {
        KEY_B_LIGHT_ENABLE();
	    }
      else
	    {
        KEY_B_LIGHT_DISABLE();
	    }
    }
    else if(c=='S')
    {
      Features.auto_scroll ^= 1;                   // toggle scroll feature
    }    
    else if(c=='A')
    {
      Features.avg_std_mode ^= 1;                  // toggle average standard mode
    }  
    else if(c=='O')
    {
      Features.auto_store_on ^= 1;
    }   
    else if(c=='B')
    {
      Features.sound_on ^= 1;

    } 
    else if(c=='G')
    {
      Features.gps_on ^= 1;
    }
    else if(c=='D')
    {
      Features.auto_depth ^= 1;
    }  
  }  
  
  // Display if feature is enabled or disabled
  if(c == 'L')
  {       
        
    CLEAR_DISP;
    if ( Controls.LCD_light == TRUE )
    {
      if ( Features.language_f )
        LCD_PrintAtPositionCentered("Light Enabled",LINE2+10); 
      else  
        LCD_PrintAtPositionCentered("LCD habilitada",LINE2+10); 
    }
    else
    {
      if ( Features.language_f )
        LCD_PrintAtPositionCentered("Light Disabled",LINE2+10); 
      else
        LCD_PrintAtPositionCentered("LCD desactivada",LINE2+10); 
    }
    
  }
  else if(c=='S')
  {
    
    CLEAR_DISP;
    if ( Features.auto_scroll == TRUE )
    {
      if ( Features.language_f )
        LCD_PrintAtPosition("Auto Scroll Enabled",LINE2);
      else
        LCD_PrintAtPosition("Auto Scroll activado",LINE2);
    }
    else
    {
      if ( Features.language_f )
        LCD_PrintAtPosition("Auto Scroll Disabled",LINE2);
      else
       LCD_PrintAtPosition("Auto Scroll desactiv",LINE2); 
    }
  }    
  else if(c=='D')
  {
    CLEAR_DISP;
    if ( Features.auto_depth  == 1 )
    {
      if ( Features.language_f )
        LCD_PrintAtPositionCentered("Auto Depth Enabled",LINE2+10);
      else  
        LCD_PrintAtPositionCentered("Auto Prof. activado",LINE2+10); 
    }
    else
    {
      if ( Features.language_f )
        LCD_PrintAtPositionCentered("Auto Depth Disabled",LINE2+10);
      else
        LCD_PrintAtPositionCentered("Auto Prof. desactiv.",LINE2+10);
    } 
  }
  else if(c=='A')
  {
    
  }  
  else if(c=='O')
  {

    CLEAR_DISP;
    if ( Features.auto_store_on == TRUE )
    {
      if ( Features.language_f )
        LCD_PrintAtPosition( "Auto Store Enabled  ",LINE2);
      else
        LCD_PrintAtPosition( "Auto Store activado",LINE2);
    }
    else
    {
      if ( Features.language_f )
        LCD_PrintAtPosition("Auto Store Disabled ",LINE2);
      else
        LCD_PrintAtPosition("Auto Store desactiv.",LINE2);
    } 
  }   
  else if(c=='B')
  {
    
    CLEAR_DISP;
    if ( Features.sound_on == TRUE )
    {
      if ( Features.language_f )
        LCD_PrintAtPositionCentered("BUZZER Enabled",LINE2+10);
      else   
        LCD_PrintAtPositionCentered("BUZZER activado",LINE2+10);
    }
    else
    {
      if ( Features.language_f )
        LCD_PrintAtPositionCentered("Alarma Disabled",LINE2+10);
      else
        LCD_PrintAtPositionCentered("Alarma desactivado",LINE2+10);
        
    }
  }
   else if(c=='G')
  {
    
    CLEAR_DISP;
    if ( Features.gps_on  == 1 )
    {
      if ( Features.language_f )
        LCD_PrintAtPositionCentered("GPS Enabled",LINE2+10);
      else
        LCD_PrintAtPositionCentered("GPS activado",LINE2+10);
      CyDelay ( 100 );
      initGPS ();
    }
    else
    {
      if ( Features.language_f )
        LCD_PrintAtPositionCentered("GPS Disabled",LINE2+10);
      else
        LCD_PrintAtPositionCentered("GPS desactivado",LINE2+10);
     disableGPS();
    }

  }  
 // save struct Features to eeprom
 NV_MEMBER_STORE( FEATURE_SETTINGS, Features );
         
 // enable shut down feature when NO is pressed                              
 Controls.shut_dwn = TRUE;                               
 delay_ms(1000);
 return button;
}


 /******************************************************************************
 *  Name:
 *  
 *  PARAMETERS:None
 *
 *  DESCRIPTION: 
 *
 *  RETURNS: 
 *
 *****************************************************************************/ 
 

void standCountMode(void)
{  
  enum buttons button;

  stand_mode_settings_text(); //TEXT// display "Std Count Settings\n1. Average Mode n2.Decay Mode " on LINE1,2,3
  ESC_to_Exit(LINE4);
       
  while(1)
  {
    button = getKey(TIME_DELAY_MAX);
    
    if( (button == 1) || (button == 2) ||  (button == ESC) || (button == MENU) )
      break;
  }
  
  if((button == ESC) || (button == MENU))
  {
    return ;
  } 
     
  else if(button == 1)
  {
    Features.avg_std_mode = TRUE;
    avg_stand_mode_enabled_text();
  }  
  else if (button == 2)
  {   
   decay_stand_mode_enabled_text();
   Features.avg_std_mode = FALSE;  
  }


  // Put the data into EEPROM
  NV_MEMBER_STORE (FEATURE_SETTINGS, Features);     // write kg or GCC as active units and save to memory          
  delay_ms ( 1000 );

}

 /******************************************************************************
 *  Name:
 *  
 *  PARAMETERS:None
 *
 *  DESCRIPTION: 
 *
 *  RETURNS: 
 *
 *****************************************************************************/ 
 
void set_units(void)  // Set/Store Gauge Units as SI or English
{  
  enum buttons button;

  set_units_text();  //TEXT// display "1. PCF\n2. kg/m3" on LINE1 and LINE2
  up_down_select_text(0);  //TEXT// display "Select #, ESC Exit" LINE4();  
    
  while(1)
  {
    button = getKey(TIME_DELAY_MAX);
    
    if((button == 1) || (button == 2) || (button == 3) || (button == ESC) || (button == MENU))
    {
      break;
    }  
  }   
  
  if ( button == 1 )  // "Enter" was pressed, selection was made
  {  
    // PCF selected
 
    Features.SI_units = FALSE;    
    NV_MEMBER_STORE(FEATURE_SETTINGS,Features);     // write PCF as active units and save to memory          
    CLEAR_DISP;
    LCD_PrintAtPositionCentered("PCF",LINE2+10);
    delay_ms(2000);
  }
  else if ( button == 2 )
  {   
    // KG/M3
    Features.SI_units = TRUE;    
    Features.kg_gcc_units = KG_UNITS;
    NV_MEMBER_STORE (FEATURE_SETTINGS, Features);     // write kg or GCC as active units and save to memory          
    CLEAR_DISP;
    LCD_PrintAtPositionCentered("kgm3",LINE2+10);
    delay_ms(2000);
    
  }   
  else if ( button == 3 )
  {
    // GCC
    Features.SI_units = TRUE;    
    Features.kg_gcc_units = GCC_UNITS;
    NV_MEMBER_STORE (FEATURE_SETTINGS, Features);     // write kg or GCC as active units and save to memory          
    CLEAR_DISP;
    LCD_PrintAtPositionCentered("GCC",LINE2+10);
    delay_ms(2000);
  }
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
 *  RETURNS: 
 *
 *****************************************************************************/ 


void select_mode(void)
{
  uint8_t menu_track = 1, menu_n = 1;    
  enum buttons button;
  
  in_menu = TRUE;
  
  while(1)                    // only exit menu when ESC is pressed
  { 
    mode_menu_display(menu_track);
    in_menu = FALSE;
    while(1)
    {
      button = getKey( TIME_DELAY_MAX );
      if((button <= 2) || (button == ESC) || (button == UP) || (button == DOWN) )
      {
          break;
      }    
    }          
    if((button == ESC) || (button == MENU))             // ESC->ready screen, MENU->main menu
    {
        break;  
    }    
    else if(button == UP)                 
    {
        menu_track = ((menu_track + menu_n) - 1)%menu_n;     
    }    
    else if(button == DOWN)                
    {
        menu_track = (menu_track + 1)%menu_n;                  
    }    
    else if(button <= 5)                // selection was made
    { 
        if(button == 2)  // DUMMY Selected
        {
          Features.dummy_mode = TRUE;
          CLEAR_DISP;
          LCD_PrintAtPositionCentered("DEMO  Mode          ",LINE2+10);
          NV_MEMBER_STORE(FEATURE_SETTINGS,Features);  //save settings to EEPROM
        }     
        else if(button == 1)  // DUMMY Disable
        {
          Features.dummy_mode = FALSE;
          CLEAR_DISP;
          LCD_PrintAtPositionCentered("Normal Mode          ",LINE2+10);
          NV_MEMBER_STORE(FEATURE_SETTINGS,Features);  //save settings to EEPROM
        }
      
     
     delay_ms ( 1000 );
     break;  
   }
     
    in_menu = TRUE;
  } // loop      
   
}
 /******************************************************************************
 *  Name:
 *  
 *  PARAMETERS:None
 *
 *  DESCRIPTION: 
 *
 *  RETURNS: 
 *
 *****************************************************************************/ 
 
void select_language(void)
{
  enum buttons button;
  
  select_language_text();  //TEXT// display "1. English\n2. Spanish"  LINE1,2

  up_down_select_text(0);  //TEXT// display "Select #, ESC Exit"

  while(1)
  {
   button = getKey ( TIME_DELAY_MAX );
    if((button == 1) || (button == 2) || (button == ESC) || (button == MENU))
    {
      break;
    }    
  }
  if(button > 2)  //ESC or MENU pressed
  {
    return;
  }    
  else if(button == 1)  //english selected
  {
    Features.language_f = TRUE;
    CLEAR_DISP;
    LCD_PrintAtPositionCentered("ENGLISH",LINE2+10);
  }    
  else                  //spanish selected
  {
    if(enter_access_code())                   // confirm access code
    {
      Features.language_f = FALSE;  
      CLEAR_DISP;
      LCD_PrintAtPositionCentered("Espanol",LINE2+10);
    }
    else
    {
      return;
    }  
  }    
  NV_MEMBER_STORE(FEATURE_SETTINGS,Features);  //save settings to EEPROM
  delay_ms ( 1000 );
  
}

 /******************************************************************************
 *  Name:
 *  
 *  PARAMETERS:None
 *
 *  DESCRIPTION: 
 *
 *  RETURNS: 
 *
 *****************************************************************************/ 
 
void offset(void)  // leads user through process to enable/disable or enter offsets
{          
  char number_ptr[20] = "\0";
  char temp_str[20];
  enum buttons button;
  float num_temp;
  Bool enabled = 0;
  Controls.shut_dwn = TRUE;               // enable shut down feature when NO is pressed  
 
  while(1)                             
  {           
   
    if(Features.language_f)
    {
        sprintf(temp_str,"Density");
    }    
    else
    {
       sprintf(temp_str,"Densid.");
    }   
    enabled = (Offsets.den_offset_pos) ? 1 : 0;
  
    //if enable = 1, then offset is enabled
    offset_on_off_text(temp_str,enabled);  //TEXT// display "%s Offset ON\nWant to Disable?",temp_str or "%s Offset OFF\,Want to Enable?",temp_str LINE1,2
    Press_YES_or_NO(LINE4);  //TEXT// display "Press YES or NO" on LINE4         
      
    while(1)
    {
      button = getKey(TIME_DELAY_MAX);
      if((button == YES) || (button == NO) || (button == ESC) || (button == MENU))
      {
       break;
      }    
    } 
    
    if((button == NO) || (button == YES))
    {
      CLEAR_DISP;
      if(((button == YES)  && enabled) || ((button == NO) && !enabled))  // Disable density offset
      {
        Offsets.den_offset_pos = FALSE;             // clear density offset bit to "OFF"
        offset_enable_disable_text(temp_str,0);     // TEXT// display "   %s Offset\n      Disabled",temp_str on LINE2,3            
        delay_ms(1200);                             // show enabled/disabled screen for 1.2 seconds  
        break;   // leave the while loop
      }   
      else if(((button == YES) && !enabled) || ((button == NO) && enabled))  // enable density offset
      {         
        enter_offset_text(temp_str);  //TEXT// display "Enter %s\nOffset:",temp_str  on LINE1 and LINE2         
        Enter_to_Accept(LINE3);       //TEXT// display "Enter to Accept"
        ESC_to_Exit(LINE4);           //TEXT// display "ESC to Exit"
        number_ptr[0] = '\0';

        // Get the Density offset
         
        // This is in kg_m3 units
        num_temp = NV_RAM_MEMBER_RD(D_OFFSET);  // Get the density offset from NV memory
          
        // Convert the density value to the proper units and display
        // returns the value in proper units (kg,pcf,gcc)
        num_temp = displayValueWithUnits ( num_temp, LINE2 + 9, number_ptr );
          
        // enter density offset
        num_temp = enterNumber ( number_ptr, LINE2 + 9, 6 );
        
        if( (getLastKey() == ENTER)  || (button == YES)  )                         // a number was entered and accepted
        {
           // update the flag bit and NV memory
             
            // Convert to KG/M3 before storing the density
            num_temp = convertDensityToKgM3 ( num_temp );
            NV_MEMBER_STORE(D_OFFSET, num_temp);
            Offsets.den_offset_pos = TRUE;                     
          
            // Convert the density value to the proper units and display
            // returns the value in proper units (kg,pcf,gcc)
            CLEAR_DISP;
            displayValueWithUnits ( num_temp, LINE2+3, number_ptr );
            delay_ms(1000);

          offset_enable_disable_text(temp_str,1);  //TEXT// display "   %s Offset\n      Enabled",temp_str on LINE2,3
          delay_ms(1200);                                   // show enabled/disabled screen for 1.2 seconds  
          break;
        }
        else
        {  
          break;
        }
    }
  } 

 }  // end while(1) loop
  
  Controls.shut_dwn = TRUE;                                    // enable shut down feature when NO is pressed  
} // end of function



 /******************************************************************************
 *  Name:
 *  
 *  PARAMETERS:None
 *
 *  DESCRIPTION: 
 *
 *  RETURNS: 
 *
 *****************************************************************************/ 
 
char select_measurement_mode(void)
{
  enum buttons button;
  uint8 mode = NORMAL_DENSITY_MODE;
  select_measurement_mode_text();  //TEXT// display "1. Normal\n2. Metal" \n. Metal LINE1,2,3

  up_down_select_text(0);  //TEXT// display "Select #, ESC Exit"

  while(1)
  {
   button = getKey ( TIME_DELAY_MAX );
    if((button == 1) || (button == 2) || (button == 3) || (button == ESC) || (button == MENU))
    {
      break;
    }    
  }
  if(button > 3)  //ESC or MENU pressed
  {
    NV_MEMBER_STORE(measure_mode,mode);  //save settings to EEPROM
    return mode; // Default to normal mode i
  }    
  else if(button == 1)  // Normal Mode selected
  {
    CLEAR_DISP;
    if ( Features.language_f )
      LCD_PrintAtPositionCentered("Normal Density Mode",LINE2+10);
    else
      LCD_PrintAtPositionCentered("Modo de Dens. Normal",LINE2+10);
    mode = NORMAL_DENSITY_MODE;
  }    
  else if(button == 2)  // Metal Mode selected                
  {
    CLEAR_DISP;
    if ( Features.language_f )
      LCD_PrintAtPositionCentered("Metal Density Mode",LINE2+10);
    else      
      LCD_PrintAtPositionCentered("Modo de Dens. Metal",LINE2+10);
    mode = METAL_DENSITY_MODE;
  }    
  else if(button == 3)  // Metal Mode selected                
  {
    CLEAR_DISP;
    if ( Features.language_f )
      LCD_PrintAtPositionCentered("Dens. Profile Mode",LINE2+10);
    else      
      LCD_PrintAtPositionCentered("Modo de Dens. Perfil",LINE2+10);      
    mode = PROFILE_DENSITY_MODE;    
  }    
  
  NV_MEMBER_STORE(measure_mode,mode);  //save settings to EEPROM
  delay_ms ( 1000 );
  return mode;
}




 /******************************************************************************
 *  Name:
 *  
 *  PARAMETERS:None
 *
 *  DESCRIPTION: 
 *
 *  RETURNS: 
 *
 *****************************************************************************/ 
Bool enter_access_code(void)
{
  uint16_t access_number;
  char number_ptr[11] = "\0\0\0\0\0\0";  // enter a code here when ready to test
  
  enter_access_number_text();  //TEXT// display "Enter Access\,Number: " LINE1 and LINE2
  YES_to_Accept(LINE3);        //TEXT// display "YES to Accept"  
  ESC_to_Exit(LINE4);          //TEXT// display "ESC to Exit"      
  
  access_number = enter_number_std(number_ptr,LINE2+8,4,0);  // prompt user to enter access number 
  
  if( getLastKey() ==ESC)
  {
     return(0);
  }   
  else if(access_number==ACCESS_CODE)
  {
     return(1);
  }   
  else if(access_number==ACCESS_CODE_2)
  {
     return(2);
  } 
  else
  {
     access_number_incorrect_text();  //TEXT// display "   "Access Code\n    Incorrect" LINE2 and LINE3
  }   
  return(0);  
}


 
 /******************************************************************************
 *  Name:
 *  
 *  PARAMETERS:None
 *
 *  DESCRIPTION: 
 *
 *  RETURNS: 
 *
 *****************************************************************************/ 
 
void memory_reset(void)  // Clears all Calibration Constants from Memory
{        
  enum buttons button;
  
  if(enter_access_code())
  {  
    Controls.shut_dwn = FALSE;                                // disable shutdown if no is pressed
    memory_reset_text();     //TEXT// display "Reset Memory\nto Default? on LINE1,2
    Press_YES_or_NO(LINE3);  //TEXT// display "Press YES or NO"
    ESC_to_Exit(LINE4);      //TEXT// display "ESC to Exit"
        
    while(1)
    {
      button = getKey(TIME_DELAY_MAX);   
      if((button == ESC) || (button == YES) || (button == NO))
      {
         break;
      }   
    }  
    if(button == YES)
    {      
      auto_initialization();
      delay_ms(250);      
    }  
    Controls.shut_dwn = TRUE;                              // enable shut down feature when NO is pressed
  }
  else if( getLastKey() ==ESC)
  {
    return;
  }   
  else
  {
    hold_buzzer();
    delay_ms(2000);
  }  
}



 /******************************************************************************
 *  Name:
 *  PARAMETERS:None
 *  DESCRIPTION: 
 *  RETURNS: 
 *****************************************************************************/ 
uint32 getSerialNumber ( void )
{
  // serial_number can only be up to 65535. If it needs to be bigger, than the serial number is stored into 
  // serial_number_2. If serial_number byte size changed, it would mess up the other eeprom data, so new
  // serial_number_2 uint32 is used.
  uint32 serial_number = (uint32)NV_RAM_MEMBER_RD(Constants.SERIAL_NUM);  // get serial number
   return serial_number;
}

 /******************************************************************************
 *  Name:
 *  PARAMETERS:None
 *  DESCRIPTION: 
 *  RETURNS: 
 *****************************************************************************/ 
void setSerialNumber ( uint32 serial_number )
  {
  // store both the 16 and 32 bit serial numbers
  NV_MEMBER_STORE(Constants.SERIAL_NUM, serial_number);        
}  


/******************************************************************************
 *  Name: ma(void)
 *  
 *  PARAMETERS: Enter Max (MAX button initiates)
 *
 *  DESCRIPTION: 
 *
 *  RETURNS: 
 *
 *****************************************************************************/ 
 
void ma(void)  // 
{ 
  float  num_temp;
  char number_ptr[20] =  NULL_NAME_STRING ;
  enum buttons button;
  
  Controls.shut_dwn = FALSE;
    while(1)
    {
      // This is in kg_m3 units
      num_temp = NV_RAM_MEMBER_RD(MARSHALL);    
      
      CLEAR_DISP;
      LCD_position(LINE1);
      _LCD_PRINT ( "MAX:");
      
      // Convert the density value to the proper units and display
      // returns the value in proper units (kg,pcf,gcc)
      num_temp = displayValueWithUnits ( num_temp, LINE1 + 4, number_ptr );
      
      change_val_text();       //TEXT// display "Change Value?" LINE2
      Press_YES_or_NO(LINE3);  //TEXT// display "Press YES or NO"
      ESC_to_Exit(LINE4);      //TEXT// display "ESC to Exit" 
      
      while(1)
      {
        button = getKey(TIME_DELAY_MAX);
        if((button == ESC) || (button == YES) || (button == NO))
        {
          break;
        }  
      }    
      if(button != YES)   // NO or ESC was pressed, exit to menu
      {
        break;
      }  
      
      enter_value_text();   //TEXT// display "Enter Value for\nMarshall:" LINE1,2
      Enter_to_Accept(LINE3);  //TEXT// display "YES to Accept" 
      ESC_to_Exit(LINE4);    //TEXT// display "ESC to Exit"
       
      num_temp = enterNumber ( number_ptr, LINE2 + 10, 6 );
      
     // If esc hit, don't save a new ENTRY
      if ( getLastKey() == ESC )
      {
        break;      
      }      
       // Convert to KG/M3 before storing the density
      num_temp = convertDensityToKgM3 ( num_temp );
      NV_MEMBER_STORE(MARSHALL,num_temp);
      
      CLEAR_DISP;
      LCD_position(LINE1);
      _LCD_PRINT ( "MAX:");
      
      // Convert the density value to the proper units and display
      // returns the value in proper units (kg,pcf,gcc)
      displayValueWithUnits ( num_temp, LINE1 + 4, number_ptr );
      delay_ms(1000);
      
      break;   
    }
   
      
  Controls.shut_dwn = TRUE;                      // enable shut down feature when NO is pressed  
}
  
 /******************************************************************************
 *  Name:
 *  
 *  PARAMETERS:None
 *
 *  DESCRIPTION: 
 *
 *  RETURNS: 
 *
 *****************************************************************************/ 
 
void serial_number ( void )  //Enter or Edit Serial Number 0 is gauge, 1 is computer
{
  char num[9] = "        "; // 8 + \0
  uint32 serial_number;
  enum buttons button;
  
  serial_number = getSerialNumber();        
     
  //////////////Confirm Access code then display serial number//////////////////////////////////////
  if(enter_access_code())                   // confirm access code
  {
    Controls.shut_dwn = FALSE;                             
    change_serial_number_text ( serial_number );
     
    while(1)
    {
      button = getKey(TIME_DELAY_MAX);
      if((button == ESC) || (button == YES) || (button == NO))
      { break ; }
    }    
    Controls.shut_dwn = TRUE;
    if(button != YES)  // button other than YES was pressed, exit to menu
      return;     
   
    while ( 1 )
    {
      enter_new_serial_number_text();
      sprintf(num,"        ");  //8 + \0
      
      serial_number = enter_number_std(num,LINE2+8,8,FALSE);   // prompt user to enter new serial number
         
      if(button == ESC)
        return;

      break;
    }
  setSerialNumber ( serial_number );
  SendBleSn();
    
  }
  else if( getLastKey()==ESC)
  {
     return;
  }   
  else
  {
    hold_buzzer();
     
    delay_ms(2000);
  }     
 
  Controls.shut_dwn = TRUE;                                    // enable shut down feature when NO is pressed   
}
 

/*
*  FUNCTION: changeMonthDayOrder()
*
*  PARAMETERS: 
*
*  DESCRIPTION: Change the order MM-DD or DD-MM is displayed
*               
*
*  RETURNS: None
*
*/
void changeMonthDayOrder ( void )
{

  enum buttons key;
  
  if ( ON == Features.day_month_year_flag )
  {
    //  DD/MM/YEAR ORDER
    // <YES> to Change
    // 
    // <ESC> to Exit
    sDayMonthOrderChange ();
  }
  else
  {
    //  MM/DD/YEAR ORDER
    // <YES> to Change
    // 
    // <ESC> to Exit
    sMonthDayOrderChange ();
  }
  while ( 1 )
  {
    key = getKey ( TIME_DELAY_MAX );
    if ( key == YES || key ==ESC )
      break;
  }
  
  if ( YES == key ) // change 
  {
    clearlcd();
    gotoloc (1,0);
    if ( ON == Features.day_month_year_flag )
    {
      // change to off
      Features.day_month_year_flag = OFF ;
      if ( Features.language_f )
      {
        displine (1, &month_day_YearOrder, 1 );
      }
      else
      {
        displine (1, &month_day_YearOrder_sp, 1 );
      }
      
      CyDelay ( 1500 );
    } 
    else
    {
      // change to on
      Features.day_month_year_flag =  ON ;
      if ( Features.language_f )
      {
        displine (1, &day_month_YearOrder, 1 );
      }
      else
      {
        displine (1, &day_month_YearOrder_sp, 1 );
      }
      CyDelay ( 1500 );
    } 
  }
  
  NV_MEMBER_STORE( FEATURE_SETTINGS, Features );
}



 
/*
*  FUNCTION: changeAMPM_24Hour()
*
*  PARAMETERS: 
*
*  DESCRIPTION: Change if time is 24hour or AM-PM mode
*               
*
*  RETURNS: None
*
*/
void changeAMPM_24Hour ( void )
{
  enum buttons key;
  
  if ( OFF == Features.flag_24HourMode)
  {
    // Time Displayed in 
    // AM-PM Mode
    // 
    // <ESC> to Exit
     sAmPmModeChange ();
  }
  else
  {
    // Time Displayed in 
    // 24 Hour Mode
    // <YES> to Change
    // <ESC> to Exit
    s24HourModeChange ();
  }
  
  key = getKey ( TIME_DELAY_MAX);
  
  if ( YES == key ) // change 
  {
    clearlcd();
    gotoloc (1,0);
    if ( ON == Features.flag_24HourMode )
    {
      // change to off
      Features.flag_24HourMode = OFF ;
      if(Features.language_f)
      {
        displine (1, &mAM_PM_Mode, 1 );
      }
      else
      {
        displine (1, &mAM_PM_Mode_sp, 1 );
      }
      CyDelay ( 1500 );
    } 
    else
    {
      // change to on
      Features.flag_24HourMode = ON ;
      if(Features.language_f)
      {
        displine (1, &m24_Hour_Mode, 1 );
      }
      else
      {
        displine (1, &m24_Hour_Mode_sp, 1 ); 
      }
      CyDelay ( 1500 );
    } 
  }

}
  