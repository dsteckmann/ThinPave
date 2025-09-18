
 /******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  Depth.c
 *  Originating Author:  
 *       Creation Date:  
 *
 ******************************************************************************/ 
 
 /*--------------------------------------------------------------------------*/
/*---------------------------[  Revision History  ]--------------------------*/
/*---------------------------------------------------------------------------*/
/*
 *  when?       who?    what?
 *  ----------- ------- ------------------------------------------------------
 *  
 *
 *----------------------------------------------------------------------------*/
 
/*----------------------------------------------------------------------------*/
/*-------------------------[   Include Files   ]------------------------------*/
/*----------------------------------------------------------------------------*/

#include "Globals.h"
#include "DataStructs.h"
#include "prompts.h"
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "Utilities.h"



/*----------------------------------------------------------------------------*/
/*----------------------------------[   Macro   ]-----------------------------*/

/*----------------------------------------------------------------------------*/

#define SAFE_DEPTH    0
#define BS_DEPTH      1.65
#define delta_v       .2



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

/////DEPTH and TIME FUNCTIONS///////////////////////////////////////////////////
void set_count_time(void)  // set duration of count (TIME button initiates)
{
  uint8_t value;      //30,60,120,240.  
  enum buttons button;
                  
  value = cnt_time;  
 // select_test_or_spec_cal(Flags.in_spec_cal);  // display "Select Test" or "Select Spec. Cal" on LINE1
  
  CLEAR_DISP;  

  LCD_position(LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("UP/DOWN TO CHANGE   "); 
  }
   else 
  {
    _LCD_PRINT ("Arriba/Abajo       ");       
  } 
   
  YES_to_Accept(LINE3);   
  ESC_to_Exit(LINE4);
 
  while(1)                                   // neither abort or enter key is pressed
  {            
    display_count_time(value,LINE1);  // display "Count Time: %u sec. " or "Count Time:  %u min. "      
    while(1)
    {
      button = getKey(TIME_DELAY_MAX);  
                    
      if((button == YES) || (button == ESC) || (button == UP) || (button == DOWN))
      {
        break;
      }    
    }       
    if(button == ESC)        //button is not enter, down or up
      break;
    else if(button == YES)                                // Is button enter?
    {      
      cnt_time = value;
      NV_MEMBER_STORE(COUNT_TIME, cnt_time);      
      break;
    }
    else if(button == DOWN)
    {        
      if ( value == 30)
      {
        value = 240;
      }  
      else if(value == 60)
      {
        value = 30; 
      }  
      else if(value == 120)
      {
        value = 60;
      }  
      else if(value == 240)
      {
        value = 120;
      }  
      else
      {
        value = 60;
      }
    }
    else  //UP was pressed
    {        
      if(value == 30)
      {
        value = 60;
      }  
      else if(value == 60)
      {
        value = 120;
      }  
      else if(value == 120)
      {
        value = 240;
      }  
      else if(value == 240)
      {
        value = 30;
      }  
      else 
      {
       value = 60;
      }
    }    
  }     
}



 /******************************************************************************
 *  Name:
 *  
 *  PARAMETERS:None
 *
 *  DESCRIPTION: 
 *	
 *  The values for the depth strip are   SAFE = 0  
 *                                   BSCATTER = 1 
 *                                      
 *	                         
 *	
 *  RETURNS: 
 *
 *****************************************************************************/ 

uint8_t get_depth ( void )  // reads depth when in auto depth mode
{
  uint8 depth;
  
  float volts = readADCVolts( DEPTH_SENS_ADC_CHAN );   // read the depth strip
  
  if ( ( volts > SAFE_DEPTH - delta_v ) && ( volts < SAFE_DEPTH + delta_v ) )
  {
    depth = SAFE_POSITION;  
  }
  else if ( ( volts > BS_DEPTH - delta_v) && ( volts < BS_DEPTH + delta_v) )
  {
    depth = BS_POSITION;  
  }
  else
  {
    depth = NO_POSITION;
  }
  
  return depth;
  
}


 /******************************************************************************
 *  Name:
 *  
 *  PARAMETERS:None
 *
 *  DESCRIPTION: 
 *	
 *  The values for the depth strip are   SAFE   
 *                                   BSCATTER  
 *                                   NO_POSITION   
 *	                         
 *	
 *  RETURNS: 
 *
 *****************************************************************************/ 

void check_depth ( void )  // reads depth when in auto depth mode
{
  uint8 depth;
  float volts  ;
  wait_for_key_release();
  global_special_key_flag = FALSE;

  while ( global_special_key_flag == FALSE )
  {
    depth = get_depth();
    if ( depth == BS_POSITION )
      depth_in_bs_text();
    else  if ( depth == SAFE_POSITION )
      depth_in_safe_text ( );
    else
      depth_in_no_text (  );
      
  
    volts = readADCVolts( DEPTH_SENS_ADC_CHAN );   // read the depth strip  
    LCD_position (LINE3);
    
    // Print the NICAD and ALKALINE Voltages
     _LCD_PRINTF ( "DS Volts: %5.3f",volts );  
    
    CyDelay ( 500 );  
  }    
    
}

 /******************************************************************************
 *  Name:
 *  
 *  PARAMETERS:display_depth
 *
 *  DESCRIPTION: 
 *	
 *  The values for the depth strip are   SAFE   
 *                                   BSCATTER  
 *                                   NO_POSITION    
 *                                         
 *	                                      
 *	
 *  RETURNS: 
 *
 *****************************************************************************/ 
void display_depth( uint8 depth  )
{  
  
  if ( Features.language_f )
  {
    if ( depth == SAFE_POSITION )
    {
      _LCD_PRINT("Depth:SAFE "); 
    }
    else  if ( depth == BS_POSITION )    
    {
      _LCD_PRINT("Depth:BS   ");
    }
    else 
    {
      _LCD_PRINT("Depth:None ");
    }
  }  
  else
  {
    if ( depth == SAFE_POSITION )
    {
      _LCD_PRINT("Prof.:SEG. "); 
    }
    else  if ( depth == BS_POSITION )    
    {
      _LCD_PRINT("Prof.:RD   ");
    }
    else 
    {
      _LCD_PRINT("Prof.:None ");
    }
  }

}

