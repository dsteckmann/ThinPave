/******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  Globals.c
 *  Originating Author:  Chris Churavy
 *       Creation Date:  10/21/2010 
 *
 ******************************************************************************/ 
 
 /*--------------------------------------------------------------------------*/
/*---------------------------[  Revision History  ]--------------------------*/
/*---------------------------------------------------------------------------*/
/*
 *  when?       who?    what?
 *  ----------- ------- ------------------------------------------------------
 *  10/21/2010  CC      Original
 *
 *----------------------------------------------------------------------------*/
 
/*----------------------------------------------------------------------------*/
/*-------------------------[   Include Files   ]------------------------------*/
/*----------------------------------------------------------------------------*/


#include "Globals.h"


/* Some Key Global Variables */
uint8   KeyActive;                       // Key active, 0 if none pending
uint8   KeyBuffer;                       // Keypad character, 0 if none valid


#define MAX_FLOAT_VALUE  10000.0
#define MIN_FLOAT_VALUE .001
/*----------------------------------------------------------------------------*/
/*----------------------[   Global Variables   ]------------------------------*/
/*----------------------------------------------------------------------------*/

//enum eADC { HV, TEMPERATURE, DEPTH_SENSOR , BATTERY };
//enum eADC eADC_CHANNEL;




/*********************************************************************/
 date_time_t date_time_g;
 
struct control_struct Controls;
struct flag_struct Flags;
struct features_struct Features;
struct offsets_struct Offsets;
struct spec_flag_struct Spec_flags;

volatile uint32 msTimer;

 uint8_t usb_start = 0;
 uint8_t global_special_key_flag;
 uint8_t tst_depth_g;
 uint8_t cnt_time;
 uint16_t  g_stop_time_secs;
 uint8_t ok_2_print;
 uint8_t gp_disp; 

 uint8_t test_number;
 uint8_t in_menu;
 uint8_t next_in2; 
 uint8_t next_out2;

 uint16_t special_depth=0, temp=0;				

 uint8_t	splashScreen=TRUE, nicadVoltageFlag=TRUE, alkVoltageFlag=TRUE;
           
 uint32_t shutdown_timer;
 uint32_t g_turn_off_cnts;
 uint16_t valid_depth;

 uint16_t total_seconds;
 uint16_t depth_setting;

 uint32_t date_unformatted; 
 uint32_t day_counter_g;
 
 fp32_t calib_depth;

 fp32_t volts_calc;

 uint16_t alfat_errors;
 uint8_t alfat_error;

char s1[4] = "!00";

 
char lcdstr[80];

char gps_fix = FALSE;

uint8 PCB_REV;

uint8 charger_state = 0;

BOOL g_remote_start_flag;
BOOL g_remote_recall_flag;
BOOL g_recall_flag;

/*---------------------------------------------------------------------------*/
/*------------------------------[  Global Macros  ]--------------------------*/
/*---------------------------------------------------------------------------*/

void append_char(char* s, char c)    
{  
  uint8_t len = strlen(s);
  s[len] = c;
  s[len+1] = '\0';
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
void nullString ( char * string, uint16_t length )
{

  
  while ( length-- )
  {
    string[length] = 0;
  }

}




/*****************************************  VARIABLE AND BUFFER DECLARATIONS  *****************************************/

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
 *  Name: checkFloatLimits
 *
 *  PARAMETERS: address of a value to be checked
 *
 *  DESCRIPTION: If the floating point values are over the expected limits for the 
 *               density calculations, set them to zero 
 *               
 *               Limit scope to this module.
 *  RETURNS: changes the passed value if limits not met
 *
 *****************************************************************************/ 
 void checkFloatLimits ( float * value_to_check )
 {
    if ( Features.dummy_mode == TRUE ) return;
    if (  *value_to_check > MAX_FLOAT_VALUE  )
    {
      *value_to_check = MAX_FLOAT_VALUE;
    } 
    else if ( *value_to_check < MIN_FLOAT_VALUE )
    {
      *value_to_check = MIN_FLOAT_VALUE;
    }
    else if ( !(*value_to_check == *value_to_check) )
    {
      *value_to_check = 99999.0;
    }
 
 }
 
 