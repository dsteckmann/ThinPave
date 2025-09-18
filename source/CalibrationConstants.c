/**********************************************************************************************************************/
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
#include "conversions.h"
#include "prompts.h"
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "Tests.h"
#include "Utilities.h"
#include "UARTS.h"
#include "Alfat.h"
#include "CalibrationConstants.h"
#define NAME_LENGTH 15

#define MAX_LINE_LENGTH 24
#define MAX_LINES 35
typedef struct 
{
  char buffer[2];
  bool file_end;
  uint16 line_start;
} t_file_line;

t_file_line file_line;

static char const_buffer[32];
static char name_string[30];
static char line_str[10];

/************************************* EXTERNAL VARIABLE AND BUFFER DECLARATIONS  *************************************/

/************************************************  LOCAL DEFINITIONS  *************************************************/

#define GCC_PCF		 62.42796

/*****************************************  VARIABLE AND BUFFER DECLARATIONS  *****************************************/

void enter_calibration_time(void);
void enter_cal_stand (void);
void enter_cal_const_hidden ();


t_const_key_value const_name_value[37] = 
{
  {"A1", 0},
  {"B1", 1},
 { "C1", 2},
  {"A2", 3},
  {"B2", 4},
  {"C2", 5},
  {"A11",6},
 { "A12",7},
 { "A13",8},                    
 { "A21",9},
 { "A22",10},
 { "A23",11},
 { "S11",12},
 { "S12",13},
 { "S13",14},
 { "S21",15},
 { "S22",16},
 { "S23",17}, 
 { "L125",18},
 { "L225",19},
 { "L150",20},
 { "L250",21},
 { "L175",22},
 { "L275",23},
 { "L110",24},
 { "L210",25},
 { "H125",26},
 { "H225",27},
 { "H150",28},
 { "H250",29},
 { "H175",30},
 { "H275",31},
 { "H110",32},
 { "H210",33},
 { "SN",34},
 { "STND1",35},
 { "STND2",36},
 };



void setDefaultConstants ( void )
{

  eepromData.Constants.A1 = A1_DEFAULT;
  eepromData.Constants.B1 = B1_DEFAULT;
  eepromData.Constants.C1 = C1_DEFAULT;
  
  eepromData.Constants.A2 = A2_DEFAULT;
  eepromData.Constants.B2 = B2_DEFAULT;
  eepromData.Constants.C2 = C2_DEFAULT;
  
  eepromData.Constants.A11 = A11_DEFAULT;
  eepromData.Constants.A12 = A12_DEFAULT;
  eepromData.Constants.A13 = A13_DEFAULT;
  
  eepromData.Constants.A21 = A21_DEFAULT;
  eepromData.Constants.A22 = A22_DEFAULT;
  eepromData.Constants.A23 = A23_DEFAULT;
  
  eepromData.Constants.S11 = S11_DEFAULT;
  eepromData.Constants.S12 = S12_DEFAULT;
  eepromData.Constants.S13 = S13_DEFAULT;
  
  eepromData.Constants.S21 = S21_DEFAULT;
  eepromData.Constants.S22 = S22_DEFAULT;
  eepromData.Constants.S23 = S23_DEFAULT;
  
  eepromData.Constants.L125 = L125_DEFAULT;
  eepromData.Constants.L225 = L225_DEFAULT;
  eepromData.Constants.L150 = L150_DEFAULT;
  eepromData.Constants.L250 = L250_DEFAULT;
  
  eepromData.Constants.L175 = L175_DEFAULT;
  eepromData.Constants.L275 = L275_DEFAULT;
  eepromData.Constants.L110 = L110_DEFAULT;
  eepromData.Constants.L210 = L210_DEFAULT;  
  
  eepromData.Constants.H125 = H125_DEFAULT;
  eepromData.Constants.H225 = H225_DEFAULT;
  eepromData.Constants.H150 = H150_DEFAULT;
  eepromData.Constants.H250 = H250_DEFAULT;
  
  eepromData.Constants.H175 = H175_DEFAULT;
  eepromData.Constants.H275 = H275_DEFAULT;
  eepromData.Constants.H110 = H110_DEFAULT;
  eepromData.Constants.H210 = H210_DEFAULT;    
  

  
}


 // Looks to see if the EEPROM data is all ff
 int isNaN ( uint8_t  f[]) 
 {
    if ( (f[0] == 255 ) && ( f[1] == 255) && ( f[2] == 255 ) && (f[3] == 255 ) ) 
    {
      return 1;
    }
    
    return 0;
      
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
 
void enter_cal_const(void)  // Manually Enter Calibration Constants
{
  uint8_t display_set = 1, selection;
  float a_temp, b_temp, c_temp;
  float t1_temp, t2_temp, t3_temp;
  
  char number_ptr[11] = "\0";
  enum buttons button;
  
  in_menu = TRUE;
  uint8 result = enter_access_code();
  if ( result == 1 )
  {
    while(1)
    {
      CLEAR_DISP;
      while(1)
      {               
        cal_menu_text(display_set);  //TEXT// display calibration menu
        if (in_menu)
        {
         up_down_select_text(1);
        }    
        in_menu = FALSE;      
      
        while(1)
        {
          button = getKey( TIME_DELAY_MAX );
         
          if(button == ESC || button <= 11)
          {
            break;
          }  
        }            
       
        // ESC pressed, exit to menu       
        if(button == ESC)                             
        {  
          break;
        }  
        else if(button == UP)                 
        {
            display_set = ((display_set + 3) - 1)%3;     // There are 3 menus to scroll through
        }    
        else if(button == DOWN)                
        {
            display_set = (display_set + 1)%3;                  
        }    
        else if(button <= 5)                          // selection was made
        { 
          selection = button;           
          in_menu = TRUE;
          break;
        } 
      } // end of while(1)      
      
      if(button == ESC)
      {
          break;
      }    
      
  		LCD_position(LINE1 + 17);
      LCD_print ( "GCC" );
  	
      if ( selection == 0 )
      {          
        CLEAR_DISP; 
        LCD_position(LINE2);
        _LCD_PRINTF ( "A1 =  %+9.7f", (float)NV_CONSTANTS(A1));
           
    		LCD_position(LINE3);  
        _LCD_PRINTF ( "B1 =  %+9.7f", (float)NV_CONSTANTS(B1));
    	                       
        LCD_position(LINE4);
        _LCD_PRINTF ( "C1 =  %+9.7f", (double)NV_CONSTANTS(C1));

        sprintf ( number_ptr, "%+9.7f", (double)NV_CONSTANTS(A1) ); 
        a_temp = enterNumber (number_ptr,LINE2 + 6, 13);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
            //return;         
            continue;
        }    
        
    		sprintf ( number_ptr, "%+9.7f", (double)NV_CONSTANTS(B1) ); 
    	
    		b_temp = enterNumber(number_ptr,LINE3 + 6, 13);    // *number_ptr1 will hold the entered number

        button = getLastKey();
        if(button == ESC)                                   //ESC was pressed, exit to menu
        {
          //return;              
          continue;
        }  
        
        sprintf ( number_ptr, "%+9.7f", (double)NV_CONSTANTS(C1) ); 
        c_temp = enterNumber( number_ptr,LINE4 + 6, 13);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
          //return;        
          continue;
        }   
        
        // if enter is pressed the third time, the values will be saved
        // Only write to EEPROm if values have changed
        if ((NV_CONSTANTS(A1) != a_temp ) )
        {
          NV_MEMBER_STORE ( Constants.A1, a_temp );
        }
        
        if ((NV_CONSTANTS(B1) != b_temp ) )
        {
          NV_MEMBER_STORE ( Constants.B1, b_temp );
        } 
         
        if ((NV_CONSTANTS(C1) != c_temp ) )
        {
          NV_MEMBER_STORE ( Constants.C1, c_temp );
        } 
        
      }
      
      else if(selection == 1)
      {       
     
        CLEAR_DISP; 
        LCD_position(LINE2);
        _LCD_PRINTF ( "A11 =  %+9.7f", (float)NV_CONSTANTS(A11));
           
    		LCD_position(LINE3);  
        _LCD_PRINTF ( "A12 =  %+9.7f", (float)NV_CONSTANTS(A12));
    	                       
        LCD_position(LINE4);
        _LCD_PRINTF ( "A13 =  %+9.7f", (double)NV_CONSTANTS(A13));

        sprintf ( number_ptr, "%+9.7f", (double)NV_CONSTANTS(A11) ); 
        
        t1_temp = enterNumber (number_ptr,LINE2 + 7, 13);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
            //return;         
            continue;
        }    
        
    		sprintf ( number_ptr, "%+9.7f", (double)NV_CONSTANTS(A12) ); 
    	
    		t2_temp = enterNumber(number_ptr,LINE3 + 7, 13);    // *number_ptr1 will hold the entered number

        button = getLastKey();
        if(button == ESC)                                   //ESC was pressed, exit to menu
        {
          //return;              
          continue;
        }  
        
        sprintf ( number_ptr, "%+9.7f", (double)NV_CONSTANTS(A13) ); 
        t3_temp = enterNumber( number_ptr,LINE4 + 7, 13);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
          //return;        
          continue;
        }   
        
        // if enter is pressed the third time, the values will be saved
        // Only write to EEPROm if values have changed
        if ((NV_CONSTANTS(A11) != t1_temp ) )
        {
          NV_MEMBER_STORE ( Constants.A11, t1_temp );
        }
        
        if ((NV_CONSTANTS(A12) != t2_temp ) )
        {
          NV_MEMBER_STORE ( Constants.A12, t2_temp );
        } 
         
        if ((NV_CONSTANTS(A13) != t2_temp ) )
        {
          NV_MEMBER_STORE ( Constants.A13, t3_temp );
        } 
      
      }
      else if ( selection == 2)
      {
        CLEAR_DISP; 
        LCD_position(LINE2);
        _LCD_PRINTF ( "A2 =  %+9.7f", (float)NV_CONSTANTS(A2));
           
    		LCD_position(LINE3);  
        _LCD_PRINTF ( "B2 =  %+9.7f", (float)NV_CONSTANTS(B2));
    	                       
        LCD_position(LINE4);
        _LCD_PRINTF ( "C2 =  %+9.7f", (double)NV_CONSTANTS(C2));

        sprintf ( number_ptr, "%+9.7f", (double)NV_CONSTANTS(A2) ); 
        a_temp = enterNumber (number_ptr,LINE2 + 6, 13);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
            //return;         
            continue;
        }    
        
    		sprintf ( number_ptr, "%+9.7f", (double)NV_CONSTANTS(B2) ); 
    	
    		b_temp = enterNumber(number_ptr,LINE3 + 6, 13);    // *number_ptr1 will hold the entered number

        button = getLastKey();
        if(button == ESC)                                   //ESC was pressed, exit to menu
        {
          //return;              
          continue;
        }  
        
        sprintf ( number_ptr, "%+9.7f", (double)NV_CONSTANTS(C2) ); 
        c_temp = enterNumber( number_ptr,LINE4 + 6, 13);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
          //return;        
          continue;
        }   
        
        // if enter is pressed the third time, the values will be saved
        // Only write to EEPROm if values have changed
        if ((NV_CONSTANTS(A2) != a_temp ) )
        {
          NV_MEMBER_STORE ( Constants.A2, a_temp );
        }
        
        if ((NV_CONSTANTS(B2) != b_temp ) )
        {
          NV_MEMBER_STORE ( Constants.B2, b_temp );
        } 
         
        if ((NV_CONSTANTS(C2) != c_temp ) )
        {
          NV_MEMBER_STORE ( Constants.C2, c_temp );
        } 
       
      }  
      else if ( selection == 3)
      {
        CLEAR_DISP; 
        LCD_position(LINE2);
        _LCD_PRINTF ( "A21 =  %+9.7f", (float)NV_CONSTANTS(A21));
           
    		LCD_position(LINE3);  
        _LCD_PRINTF ( "A22 =  %+9.7f", (float)NV_CONSTANTS(A22));
    	                       
        LCD_position(LINE4);
        _LCD_PRINTF ( "A23 =  %+9.7f", (double)NV_CONSTANTS(A23));
    		
        sprintf ( number_ptr, "%+9.7f", (double)NV_CONSTANTS(A21) ); 
        t1_temp = enterNumber (number_ptr,LINE2 + 7, 13);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
            //return;         
            continue;
        }    
        
    		sprintf ( number_ptr, "%+9.7f", (double)NV_CONSTANTS(A22) ); 
    	
    		t2_temp = enterNumber(number_ptr,LINE3 + 7, 13);    // *number_ptr1 will hold the entered number

        button = getLastKey();
        if(button == ESC)                                   //ESC was pressed, exit to menu
        {
          //return;              
          continue;
        }  
        
        sprintf ( number_ptr, "%+9.7f", (double)NV_CONSTANTS(A23) ); 
        t3_temp = enterNumber( number_ptr,LINE4 + 7, 13);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
          //return;        
          continue;
        }   
        
        // if enter is pressed the third time, the values will be saved
        // Only write to EEPROm if values have changed
        if ((NV_CONSTANTS(A21) != t1_temp ) )
        {
          NV_MEMBER_STORE ( Constants.A21, t1_temp );
        }
        
        if ((NV_CONSTANTS(A22) != t2_temp ) )
        {
          NV_MEMBER_STORE ( Constants.A22, t2_temp );
        } 
         
        if ((NV_CONSTANTS(A23) != t2_temp ) )
        {
          NV_MEMBER_STORE ( Constants.A23, t3_temp );
        } 
      
       
      }  
      else if ( selection == 4 )
      {
        enter_cal_stand();
      } 
      else if ( selection == 5 )
      {
        CLEAR_DISP;
        // Display the Calibration Date
        calibration_date_txt();
        LCD_position (LINE1+9);
        printTimeDate (  NV_RAM_MEMBER_RD (Constants.CAL_DATE) );
        change_val_text();       //TEXT// display "Change Value?" LINE2     
        Press_YES_or_NO (LINE3);  // display "Press YES or NO"
        ESC_to_Exit(LINE4);
        

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
         continue;
        }  
        
        // Enter the calibration date and write to EEPROM
        enter_calibration_time();
        hold_buzzer();
          
        delay_ms(2000); 
      }
         
    }  // end of while(1)
  }  // end of if(acces_number == ACCESS_CODE)  
  else if ( result == 2 )
  {
   enter_cal_const_hidden ();
  }
  else if( getLastKey() == ESC)
  {
      return;
  }    
  else
  {
    hold_buzzer();
      
    delay_ms(2000);
  }  
}


/*****************************************************************************/
/* FUNCTION   : parse_date                                                   */
/*                                                                           */
/* DESCRIPTION: converts an ASCII character string into a date               */
/*              yyyy-mm-dd  (hyphens delimit the values)                                                              */
/* PARAMETERS : buffer - the ASCII string  YYYY-MM-DD                        */
/*                                                                            */
/* RETURNS    : 1 if conversion was successful, 0 otherwise                   */
/*                                                                            */
/* NOTES      :                                                               */
/*                                                                            */
/******************************************************************************/
uint8 parse_date ( char *myStr, char* year, char* day, char* month )
{
 
  char* myPtr = strtok(myStr, "-");
  if ( myPtr != NULL )
  {
    sprintf( year,"%s", myPtr);
  }
  else
  {
    return 0; //error
  }
  
  myPtr = strtok(NULL, "-");
  if ( myPtr != NULL )
  { 
    sprintf( month,"%s", myPtr);
  }  
  else
  {
    return 0; //error
  }
  
  myPtr = strtok(NULL, "-"); 
  if ( myPtr != NULL )
  { 
    sprintf( day,"%s", myPtr);
  }  
  else
  {
    return 0; //error
  }
 
  return 1;

}   

/******************************************************************************/
/* FUNCTION   : parse_double_float                                            */
/*                                                                            */
/* DESCRIPTION: converts an ASCII character string into a double float        */
/*                                                                            */
/* PARAMETERS : buffer - the ASCII string                                     */
/*                   i - starting position in the string                      */
/*                   n - ending position in the string                        */
/*                work - pointer to location for the result                   */
/*                                                                            */
/* RETURNS    : 0 if conversion was successful, 0 otherwise                   */
/*                                                                            */
/* NOTES      :                                                               */
/*                                                                            */
/******************************************************************************/
unsigned char parse_double_float ( unsigned char *buffer, double *work )
{
  unsigned char status;
  uint16 k= 0,n;

  status = 0;   // assume success
  n =strlen ( (const char*)buffer);

  for ( k = 0; k < n ; k++ )
  {
     if ( buffer[k] != '-' &&
          buffer[k] != '+' &&
          buffer[k] != '.' &&
          buffer[k] != ' ' &&
          ( buffer[k] < 0x30 && buffer[k] > 0x39 ) )
     {
        status = 1;   // indicate bogus value
        break;
     }
     else
     {
       k++;
     } 
   }   // check for permissible characters (0-9, -, + and .)

   if  ( ! status )
   {
      * work = atof ( (char const *) buffer );   // save the converted double precision float
   }

  return status;

}   /* unsigned char parse_double_float ( ... ) */


/******************************************************************************
 *  Name: storeConstant
 *  
 *  PARAMETERS: the key name of the constant and the value
 *
 *  DESCRIPTION: 
 *                
 *  RETURNS: 
 *
 *****************************************************************************/  
void storeConstant ( char * name, double constant )
{
  uint8 n;
  uint8 off;
  for ( n = 0; n < THINPAVE_CONST_NUM; n++ )
  {
    if ( strcmp ( name, const_name_value[n].name ) == 0)
    {
      off = n;
      switch ( off )
      {
        case 0: NV_MEMBER_STORE(Constants.A1,constant );
                break;
        case 1: NV_MEMBER_STORE(Constants.B1,constant );
                break;
        case 2: NV_MEMBER_STORE(Constants.C1,constant );
                break;
        case 3: NV_MEMBER_STORE(Constants.A2,constant );
                break;
        case 4: NV_MEMBER_STORE(Constants.B2,constant );
                break;
        case 5: NV_MEMBER_STORE(Constants.C2,constant );
                break;        
        case 6: NV_MEMBER_STORE(Constants.A11,constant );
                break;
        case 7: NV_MEMBER_STORE(Constants.A12,constant );
                break;
        case 8: NV_MEMBER_STORE(Constants.A13,constant );
                break;

        case 9: NV_MEMBER_STORE(Constants.A21,constant );
                break;
        case 10: NV_MEMBER_STORE(Constants.A22,constant );
                break;
        case 11: NV_MEMBER_STORE(Constants.A23,constant );
                break;
        case 12: NV_MEMBER_STORE(Constants.S11,constant );
                break;
        case 13: NV_MEMBER_STORE(Constants.S12,constant );
                break;
        case 14: NV_MEMBER_STORE(Constants.S13,constant );
                break;
        case 15: NV_MEMBER_STORE(Constants.S21,constant );
                break;
        case 16: 
                NV_MEMBER_STORE(Constants.S22,constant );
                break;
        case 17: 
                NV_MEMBER_STORE(Constants.S23,constant );
                break;
        case 18: 
                NV_MEMBER_STORE(Constants.L125,constant );
                break;
        case 19: 
                NV_MEMBER_STORE(Constants.L225,constant );
                break;
        case 20: 
                NV_MEMBER_STORE(Constants.L150,constant );
                break;
        case 21:
                NV_MEMBER_STORE(Constants.L250,constant );
                break;
        case 22: 
                NV_MEMBER_STORE(Constants.L175,constant );
                break;
        case 23: 
                NV_MEMBER_STORE(Constants.L275,constant );
                break;
        case 24: 
                NV_MEMBER_STORE(Constants.L110,constant );
                break;
        case 25: 
                NV_MEMBER_STORE(Constants.L210,constant );
                break;
        case 26: 
                NV_MEMBER_STORE(Constants.H125,constant );
                break;
        case 27:
                NV_MEMBER_STORE(Constants.H225,constant );
                break;
        case 28: 
                NV_MEMBER_STORE(Constants.H150,constant );
                break;
        case 29: 
                NV_MEMBER_STORE(Constants.H250,constant );
                break;
        case 30:
                NV_MEMBER_STORE(Constants.H175,constant );
                break;
        case 31: 
                NV_MEMBER_STORE(Constants.H275,constant );
                break;
        case 32: 
                NV_MEMBER_STORE(Constants.H110,constant );
                break;
        case 33: 
                NV_MEMBER_STORE(Constants.H210,constant );
                break;        
        case 34:
                NV_MEMBER_STORE(Constants.SERIAL_NUM,constant );
                break; 
        case 35: 
                NV_MEMBER_STORE(Constants.DENS_1_CAL_STD,constant );
                break; 
        case 36: 
                NV_MEMBER_STORE(Constants.DENS_2_CAL_STD,constant );
                break;         
        
        default: break ;
      }
    }
  }
}

  /*******************************************************************
 *  Name:   getLineFromFile
 *
 *  PARAMETERS: struct with pointers to FILE_PARAMETERS of opened file
 *              and a file line struct. 
 *
 *  DESCRIPTION: A file is opened for read on the USB drive. 
 *               A file_line struct is created that starts at position 0 in the file.
 *               This struct is passed to this function to read a line from the file .
 *               Data is read into the buffer from the file until \r or \r\n is found.
 *               The \r is replaced with a NULL to indicate the end of the string.
 *               A check is made to see if the eof os reached.
 *               The position of the file after the \r\n is put in to the struct.
 *               THis is used the next time this function is called.
 *  RETURNS: 
 *
 ********************************************************************/ 
uint8 getLineFromFile (  FILE_PARAMETERS * fp )
{
 // uint16 error;
  uint8 n = 0;
  
  // read 1 bytes
  for ( n = 0; n < 24; n++ )
  {
    //error = AlfatReadFromFile ( fp );  
    AlfatReadFromFile ( fp );  
    file_line.file_end = FALSE;
    // Check if at eof
    if ( fp->numBytes < 1 )
    {
      file_line.file_end = TRUE;
      return 0;
    }
    if ( file_line.buffer[0] == '\n' )
    {
      return 1;
    }
    else if ( file_line.buffer[0] == '\r' )
    {
      const_buffer[n] = 0;
    }
    else
    {
     const_buffer[n] = file_line.buffer[0];
    }
  }  
 
  return 0;
}  


/******************************************************************************
 *  Name: parseConstants( void)
 *  
 *  PARAMETERS: Data on USB drive in file ThinPAVE.txt, opened file handle
 *
 *  DESCRIPTION: Read the 34 constants and save to eeprom
 *                
 *  RETURNS: 
 *
 *****************************************************************************/ 
void parseConstants (  )
{
  date_time_t date ;  
  int8 n;

  char const * string;
  char * k;
  uint8 status;
  double constant;
  char display_buffer[21];
  char name[10];
  char year[5],month[3], day[3];
 

  // Always set to zero to get first line
  file_line.line_start = 0;
   
  g_myfile.dataBuffer = (uint8 *)file_line.buffer;
  g_myfile.numBytes   = 1;
  g_myfile.fillerChar = '$';
  
  // Loop thtough all the constants except the date constant
  for ( n = 0; n < THINPAVE_CONST_NUM - 1; n++ )
  {
    file_line.buffer[0] = 0;
    getLineFromFile (  &g_myfile ); 
    
    string = const_buffer;  // The string is placed into the const_buffer array
    // the \t seperates the key from the value
    k = strchr(string, '\t');
    *k = 0; // null for the end of the string
    strcpy ( name, string );
    
    // get the number from the string
    parse_double_float ( (unsigned char * )k+1 , &constant) ; 
    storeConstant ( name, constant );
    if ( n < THINPAVE_CONST_NUM_DOUBLES ) // The first 34 constants are doubles
    {
      snprintf ( display_buffer, 21,"%s: %+12.8f", name,constant );
    }
    else
    {
      // This is the serial number, standard count 1 and 2
      snprintf ( display_buffer, 21,"%s: %lu", name,(uint32)constant );
    }
    CLEAR_DISP;
    printOnLCDLineLocString ( LINE1, 0, display_buffer );
    CyDelay(100);
   } 
  
   // The 38th constant is the calibration Date
   // The last line is the year/month/day of calibration
   file_line.buffer[0] = 0;
   getLineFromFile (  &g_myfile ); 
   string = const_buffer;
 
  // the \t seperates the key from the value
  k = strchr(string, '\t');
  *k = 0; // null for the end of the string
  strcpy ( name, string ); 
     
  status = parse_date ( (char * )k+1, year, day, month) ; 
 
  if ( status == 1 )
  {
    date.iday   = atoi ( day );
    date.imonth = atoi ( month );
    date.iyear  = atoi ( year );      
    date.iminute = 0;  
    date.ihour  = 12;
    date.isecond = 0;
    //write the date/time to memory
    NV_MEMBER_STORE(Constants.CAL_DATE, date);
    snprintf ( display_buffer, 21,"yymmdd:%s-%s-%s", year,month,day );
    CLEAR_DISP;
    printOnLCDLineLocString ( LINE1, 0, display_buffer );    
    CyDelay ( 100 );
  }  
  
  CLEAR_DISP;
  DisplayStrCentered(LINE1,"Finished Reading");
  DisplayStrCentered(LINE2,"ThinPAVE.txt");
  CyDelay(2000);
    
}




/******************************************************************************
 *  Name: writeLineConstant_to_USB
 *  
 *  PARAMETERS: the offset to the name,value pair, pointers to stor the name and value
 *
 *  DESCRIPTION: There is a structure with the Calibration name and its offset. This tells
 *                the case statement to select the proper calibration value
 *  RETURNS: 
 *
 *****************************************************************************/  
void getConstant_Name_from_index ( uint8 offset, char * cc_name, double * value )
{

  
  // get the name from the 
  strcpy ( cc_name, const_name_value[offset].name );
  
  // get the value
  switch ( offset )
  {
    case 0:
            *value = NV_CONSTANTS(A1);
            break;
    case 1: *value = NV_CONSTANTS(B1);
            break;
    case 2: *value = NV_CONSTANTS(C1);
            break;
    case 3: *value = NV_CONSTANTS(A2);
            break;
    case 4: *value = NV_CONSTANTS(B2);
            break;
    case 5: *value = NV_CONSTANTS(C2);
            break;
    case 6: *value = NV_CONSTANTS(A11);
            break;
    case 7: *value = NV_CONSTANTS(A12);
            break;
    case 8: *value = NV_CONSTANTS(A13);
            break;
    case 9: *value = NV_CONSTANTS(A21);
            break;
    case 10: *value = NV_CONSTANTS(A22);
            break;
    case 11: *value = NV_CONSTANTS(A23);
            break;
    case 12: *value = NV_CONSTANTS(S11);
            break;
    case 13: *value = NV_CONSTANTS(S12);
            break;
    case 14: *value = NV_CONSTANTS(S13);
            break;
    case 15: *value = NV_CONSTANTS(S21);
            break;
    case 16: 
            *value = NV_CONSTANTS(S22);
            break;
    case 17: 
            *value = NV_CONSTANTS(S23);
            break;
    case 18: 
            *value = NV_CONSTANTS(L125);
            break;
    case 19: 
            *value = NV_CONSTANTS(L225);
            break;
    case 20: 
            *value =  NV_CONSTANTS(L150);
            break;
    case 21:
            *value = NV_CONSTANTS(L250);
            break;
    case 22: 
            *value = NV_CONSTANTS(L175);
            break;
    case 23: 
            *value = NV_CONSTANTS(L275);
            break;
    case 24: 
            *value = NV_CONSTANTS(L110);
            break;
    case 25: 
            *value = NV_CONSTANTS(L210);
            break;
    case 26: 
            *value = NV_CONSTANTS(H125);
            break;
    case 27:
            *value = NV_CONSTANTS(H225);
            break;
    case 28: 
            *value = NV_CONSTANTS(H150);
            break;
    case 29: 
            *value = NV_CONSTANTS(H250);
            break;
    case 30:
            *value = NV_CONSTANTS(H175);
            break;
    case 31: 
            *value = NV_CONSTANTS(H275);
            break;
    case 32: 
            *value = NV_CONSTANTS(H110);
            break;
    case 33: 
            *value = NV_CONSTANTS(H210);
            break;        
    case 34:
            *value = NV_CONSTANTS(SERIAL_NUM);
            break; 
    case 35: 
            *value = NV_CONSTANTS(DENS_1_CAL_STD);
            break; 
    case 36: 
            *value = NV_CONSTANTS(DENS_2_CAL_STD);
            break;         
    
    default: break ;
  }

}


 /******************************************************************************
 *  Name: writeConst_USB ( )
 *  
 *  PARAMETERS: 
 *
 *  DESCRIPTION: write calibration coonstants to the USB drive.
 *               This prevents always doing hand entry.
 *  RETURNS: 
 *
 *****************************************************************************/ 
void writeConst_USB()
{
   char j;

  int button;
  FILE_PARAMETERS fp; 
  bool error;
  char string_v[20];
  char name[21];
  double value;
  // Write Calibration
  // Constants To USB?
  // <YES> or <NO>
  dispscrn ( s_writeCalibUSB );

  while ( 1 )
  {
    button = getKey(TIME_DELAY_MAX );
    
    if((button == YES) || (button == ESC) || (button == NO ) )
    {
      break;
    }  
  }      
  
  if( (button == ESC) || (button == NO ))
  {   
   return;
  } 
  
  // turn on power
   AlfatStart();
 
  //center the string
  
  char alfat_error = NO_ERROR;
  if ( AlfatGetVersion( string_v ) != 0x3030 )
  {
    alfat_error = ERROR;
  }  
  
  string_v[6] = 0;
    
  if ( NO_ERROR == alfat_error )
  {
    CLEAR_DISP;
    printOnLCDLineLocString ( 1, centerStart( strlen(string_v)), string_v );
    CyDelay(1000);
  }  
  else
  {
    printOnLCDLineLocString ( 1,centerStart(11), "ALFAT Error");
    CyDelay(2000);
    AlfatStop();   
    return;
  }
   
   CLEAR_DISP;
   DisplayStrCentered(LINE1,"Insert Drive into");
   DisplayStrCentered(LINE2,"USB Port");
   DisplayStrCentered(LINE3,"and press <YES>");
   DisplayStrCentered(LINE4,"<ESC> to Exit"); 
  
   button = getKey(TIME_DELAY_MAX );
      
  if(button == ESC)
  {
   AlfatStop();   
   return;
  }  
  read_RTC( &date_time_g );  //get date
  CLEAR_DISP;
  DisplayStrCentered(LINE1,"Creating File");
  DisplayStrCentered(LINE2,"ThinPAVE.csv");
  
  j = 0;
  while ( !check_for_USB() && j < 5 ) // check for USB device attached to Alfat.
  { 
   
   USB_text(2);  // display " No USB Device "
   CyDelay ( 100 );  
   j++;   
  }; 
     
  if ( j >= 5 )
  {
    // Put ALFAT in reduced power mode
    AlfatStop();   
    return;
  }
  
  if ( initialize_USB( TRUE ) ) // Mounts the USB device
  {
    // make the file name string
    snprintf(name_string,30,"ThinPAVE");
    Bool status  = USB_open_file  ( name_string, &fp );  
    if ( status == TRUE )
    {
      CLEAR_DISP;
    DisplayStrCentered(LINE1,"Writing Constants");
    CyDelay ( 1500 );

    // go through the double valued constants and store the name\tvalue
    for ( uint8 i=0; i < THINPAVE_CONST_NUM -1 ; i++ )
    {
      getConstant_Name_from_index ( i, name, &value );
      if ( i < THINPAVE_CONST_NUM_DOUBLES )
        {
          // write the line
          snprintf (string_v,20,"%s,%12.8f\r\n", name, value );
        }
        else
        {
          // These values are not doubles, but integers: SN,STD1,STD2
          // write the line
          snprintf (string_v,20,"%s,%lu\r\n", name, (uint32)value );
        }

        AlfatWriteStr ( &fp, string_v );
      }
      //write the date/time to USB
      date_time_t date =  NV_CONSTANTS(CAL_DATE);
      char day[3],month[3], year[5];
      itoa(date.iday,day,10);
      itoa(date.imonth,month,10);
      itoa(date.iyear,year,10);      
      snprintf ( string_v, 21,"yymmdd,%s-%s-%s", year,month,day );
      AlfatWriteStr ( &fp, string_v );
    }
 }  
  
  // Get the calibration date
  AlfatFlushData ( fp.fileHandle );
  AlfatCloseFile ( fp.fileHandle ); 
  AlfatStop();   
}


/******************************************************************************
 *  Name: readConstFromUSB ( void)
 *  
 *  PARAMETERS: Data on USB drive in file ThinPAVE.txt
 *
 *  DESCRIPTION: Read the constants, cal std counts, and save to eeprom
 *                
 *  RETURNS: 
 *
 *****************************************************************************/ 
void readConstFromUSB ( void )
{
  char j;

  int button;
  FILE_ENTRY fe;
  bool error;
  char string_v[20];
  // Read Calibration
  // Constants From USB?
  // <YES> or <NO>
  dispscrn ( s_readCalibUSB );

  while ( 1 )
  {
    button = getKey(TIME_DELAY_MAX );
    
    if((button == YES) || (button == ESC) || (button == NO ) )
    {
      break;
    }  
  }      
  
  if( (button == ESC) || (button == NO ))
  {   
   return;
  } 
  
  // turn on power
   AlfatStart();
 
  //center the string
  
  char alfat_error = NO_ERROR;
  if ( AlfatGetVersion( string_v ) != 0x3030 )
  {
    alfat_error = ERROR;
  }  
  
  string_v[6] = 0;
    
  if ( NO_ERROR == alfat_error )
  {
    CLEAR_DISP;
    printOnLCDLineLocString ( 1, centerStart( strlen(string_v)), string_v );
    CyDelay(1000);
  }  
  else
  {
    printOnLCDLineLocString ( 1,centerStart(11), "ALFAT Error");
    CyDelay(2000);
    AlfatStop();   
    return;
  }
   
   CLEAR_DISP;
   DisplayStrCentered(LINE1,"Insert Drive with");
   DisplayStrCentered(LINE2,"ThinPAVE.txt");
   DisplayStrCentered(LINE3,"and press <YES>");
   DisplayStrCentered(LINE4,"<ESC> to Exit"); 
  
   button = getKey(TIME_DELAY_MAX );
      
  if(button == ESC)
  {
   AlfatStop();   
   return;
  }  
 
   CLEAR_DISP;
   DisplayStrCentered(LINE1,"Opening File");
   DisplayStrCentered(LINE2,"ThinPAVE.txt");
  
  j = 0;
  while ( !check_for_USB() && j < 5 ) // check for USB device attached to Alfat.
  { 
   
   USB_text(2);  // display " No USB Device "
   CyDelay ( 100 );  
   j++;   
  }; 
  
  if ( j >= 5 )
  {
    // Put ALFAT in reduced power mode
    AlfatStop();   
    return;
  }
  
  if ( initialize_USB( TRUE ) ) // Mounts the USB device
  {
    // make the file name string
    snprintf(name_string,30,"U0:\\ThinPAVE.txt");
    fe.fname = name_string;   
    if ( AlfatFindFile(&fe) != ALFAT_ERR_SUCCESS )
    {
        CLEAR_DISP;
        DisplayStrCentered(LINE1,"Unable to find");
        DisplayStrCentered(LINE2,"ThinPAVE.txt");
        DisplayStrCentered(LINE3,"on USB Drive");
        CyDelay(3000);
        // Put ALFAT in reduced power mode
        AlfatStop();  
        return;
    }
    
    CLEAR_DISP;
    DisplayStrCentered(LINE1,"Reading Constants");
    CyDelay ( 1500 );
    g_myfile.fileAttr.fname = name_string;
    g_myfile.dataBuffer =(uint8*) line_str; 
    g_myfile.fileHandle = 0;   // This is the default for all files, since only one is opened at a time
    error = USB_open_file_for_read (  );
 
    if ( error == FALSE )
    {
     AlfatStop();
     return;
    }
    parseConstants( );
    // copy the data from the file into EEPROM
    
    AlfatCloseFile( g_myfile.fileHandle );  
  }   
 
  AlfatStop();   

}


 /******************************************************************************
 *  Name: get_store_Constants_USB ( )
 *  
 *  PARAMETERS: 
 *
 *  DESCRIPTION: Read from or write calibration coonstants to the USB drive.
 *               This prevents always doing hand entry.
 *  RETURNS: 
 *
 *****************************************************************************/ 

void  get_store_Constants_USB ( )
{
  // Press <1> to write
  // constants to USB
  // Press <2> to read
  // constants from USB
  enum buttons button;
  
  read_write_Constants_text ( );
  while(1)
  {
    button = getKey(TIME_DELAY_MAX);
      
    if ( button == ONE )
    {
      writeConst_USB();
        break;
    }  
    else if ( button == TWO )
    {
      readConstFromUSB();
      break;
    }
    else if ( ( button == ESC ) || ( button == ENTER ) )
      break ;
  } 

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
 
void enter_cal_const_hidden(void)  // Manually Enter Calibration Constants
{
  uint8_t display_set = 1, selection;
  DOUBLE_FLOAT a_temp, b_temp, c_temp;
  DOUBLE_FLOAT t1_temp, t2_temp, t3_temp;
  
  char number_ptr[11] = "\0";
  enum buttons button;
  
  in_menu = TRUE;
    while(1)
    {
      CLEAR_DISP;
      while(1)
      {               
        cal_menu_hidden_text(display_set);  //TEXT// display calibration menu
        if (in_menu)
        {
         up_down_select_text(1);
        }    
        in_menu = FALSE;      
      
        while(1)
        {
          button = getKey( TIME_DELAY_MAX );
         
          if( button == ESC || button <= 11 ||  button == STORE )
          {
            break;
          }  
        }            
       
        // ESC pressed, exit to menu       
        if(button == ESC)                             
        {  
          break;
        }  
        else if(button == UP)                 
        {
          if ( display_set == 1 )
            display_set = 4;
          else
            display_set -= 1;
             
        }    
        else if(button == DOWN)                
        {
           display_set += 1;
           if ( display_set > 4 )
            display_set = 1;
        } 
        else if ( button == STORE )
        {
          get_store_Constants_USB ();
        }
        else if(button <= 7)                          // selection was made
        { 
          selection = button;           
          in_menu = TRUE;
          break;
        } 
        
      } // end of while(1)      
      
      if(button == ESC)
      {
          break;
      }    
  	
      if ( selection == 0 )
      {          
        CLEAR_DISP; 
        LCD_position(LINE2);
        _LCD_PRINTF ( "L125 =  %+11.7f", (float)NV_CONSTANTS(L125));
           
    		LCD_position(LINE3);  
        _LCD_PRINTF ( "L225 =  %+11.7f", (float)NV_CONSTANTS(L225));
    	                       
        LCD_position(LINE4);
        _LCD_PRINTF ( "L150 =  %+11.7f", (double)NV_CONSTANTS(L150));

        sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(L125) ); 
        a_temp = enterNumber (number_ptr,LINE2 + 8, 12);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
            //return;         
            continue;
        }    
        
    		sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(L225) ); 
    	
    		b_temp = enterNumber(number_ptr,LINE3 + 8, 12);    // *number_ptr1 will hold the entered number

        button = getLastKey();
        if(button == ESC)                                   //ESC was pressed, exit to menu
        {
          //return;              
          continue;
        }  
        
        sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(L150) ); 
        c_temp = enterNumber( number_ptr,LINE4 + 8, 12);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
          //return;        
          continue;
        }   
        
        // if enter is pressed the third time, the values will be saved
        // Only write to EEPROm if values have changed
        if ((NV_CONSTANTS(L125) != a_temp ) )
        {
          NV_MEMBER_STORE ( Constants.L125, a_temp );
        }
        
        if ((NV_CONSTANTS(L225) != b_temp ) )
        {
          NV_MEMBER_STORE ( Constants.L225, b_temp );
        } 
         
        if ((NV_CONSTANTS(L150) != c_temp ) )
        {
          NV_MEMBER_STORE ( Constants.L150, c_temp );
        } 
        
      }
      
      else if(selection == 1)
      {       
     
        CLEAR_DISP; 
        LCD_position(LINE2);
        _LCD_PRINTF ( "L250 =  %+11.7f", (float)NV_CONSTANTS(L250));
           
    		LCD_position(LINE3);  
        _LCD_PRINTF ( "L175 =  %+11.7f", (float)NV_CONSTANTS(L175));
    	                       
        LCD_position(LINE4);
        _LCD_PRINTF ( "L275 =  %+11.7f", (double)NV_CONSTANTS(L275));

        sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(L250) ); 
        
        t1_temp = enterNumber (number_ptr,LINE2 + 8, 11);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
            //return;         
            continue;
        }    
        
    		sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(L175) ); 
    	
    		t2_temp = enterNumber(number_ptr,LINE3 + 8, 11);    // *number_ptr1 will hold the entered number

        button = getLastKey();
        if(button == ESC)                                   //ESC was pressed, exit to menu
        {
          //return;              
          continue;
        }  
        
        sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(L275) ); 
        t3_temp = enterNumber( number_ptr,LINE4 + 8, 11);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
          //return;        
          continue;
        }   
        
        // if enter is pressed the third time, the values will be saved
        // Only write to EEPROm if values have changed
        if ((NV_CONSTANTS(L250) != t1_temp ) )
        {
          NV_MEMBER_STORE ( Constants.L250, t1_temp );
        }
        
        if ((NV_CONSTANTS(L175) != t2_temp ) )
        {
          NV_MEMBER_STORE ( Constants.L175, t2_temp );
        } 
         
        if ((NV_CONSTANTS(L275) != t2_temp ) )
        {
          NV_MEMBER_STORE ( Constants.L275, t3_temp );
        } 
      
      }
      else if ( selection == 2)
      {
        CLEAR_DISP; 
        LCD_position(LINE2);
        _LCD_PRINTF ( "L110 =  %+11.7f", (float)NV_CONSTANTS(L110));
           
    		LCD_position(LINE3);  
        _LCD_PRINTF ( "L210 =  %+11.7f", (float)NV_CONSTANTS(L210));
    	                       
        LCD_position(LINE4);
        _LCD_PRINTF ( "H125 =  %+11.7f", (double)NV_CONSTANTS(H125));

        sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(L110) ); 
        a_temp = enterNumber (number_ptr,LINE2 + 8, 11);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
            //return;         
            continue;
        }    
        
    		sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(L210) ); 
    	
    		b_temp = enterNumber(number_ptr,LINE3 + 8, 11);    // *number_ptr1 will hold the entered number

        button = getLastKey();
        if(button == ESC)                                   //ESC was pressed, exit to menu
        {
          //return;              
          continue;
        }  
        
        sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(H125) ); 
        c_temp = enterNumber( number_ptr,LINE4 + 8, 11);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
          //return;        
          continue;
        }   
        
        // if enter is pressed the third time, the values will be saved
        // Only write to EEPROm if values have changed
        if ((NV_CONSTANTS(L110) != a_temp ) )
        {
          NV_MEMBER_STORE ( Constants.L110, a_temp );
        }
        
        if ((NV_CONSTANTS(L210) != b_temp ) )
        {
          NV_MEMBER_STORE ( Constants.L210, b_temp );
        } 
         
        if ((NV_CONSTANTS(H125) != c_temp ) )
        {
          NV_MEMBER_STORE ( Constants.H125, c_temp );
        } 
       
      }  
      else if ( selection == 3)
      {
        CLEAR_DISP; 
        LCD_position(LINE2);
        _LCD_PRINTF ( "H225 =  %+11.7f", (float)NV_CONSTANTS(H225));
           
    		LCD_position(LINE3);  
        _LCD_PRINTF ( "H150 =  %+11.7f", (float)NV_CONSTANTS(H150));
    	                       
        LCD_position(LINE4);
        _LCD_PRINTF ( "H250 =  %+11.7f", (double)NV_CONSTANTS(H250));
    		
        sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(H225) ); 
        t1_temp = enterNumber (number_ptr,LINE2 + 8, 11);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
            //return;         
            continue;
        }    
        
    		sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(H150) ); 
    	
    		t2_temp = enterNumber(number_ptr,LINE3 + 8, 12);    // *number_ptr1 will hold the entered number

        button = getLastKey();
        if(button == ESC)                                   //ESC was pressed, exit to menu
        {
          //return;              
          continue;
        }  
        
        sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(H250) ); 
        t3_temp = enterNumber( number_ptr,LINE4 + 8, 12);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
          //return;        
          continue;
        }   
        
        // if enter is pressed the third time, the values will be saved
        // Only write to EEPROm if values have changed
        if ((NV_CONSTANTS(H225) != t1_temp ) )
        {
          NV_MEMBER_STORE ( Constants.H225, t1_temp );
        }
        
        if ((NV_CONSTANTS(H150) != t2_temp ) )
        {
          NV_MEMBER_STORE ( Constants.H150, t2_temp );
        } 
         
        if ((NV_CONSTANTS(H250) != t2_temp ) )
        {
          NV_MEMBER_STORE ( Constants.H250, t3_temp );
        } 
      } 
      else if ( selection == 4)
      {
        CLEAR_DISP; 
        LCD_position(LINE2);
        _LCD_PRINTF ( "H175 =  %+11.7f", (float)NV_CONSTANTS(H175));
           
    		LCD_position(LINE3);  
        _LCD_PRINTF ( "H275 =  %+11.7f", (float)NV_CONSTANTS(H275));
    	                       
        LCD_position(LINE4);
        _LCD_PRINTF ( "H110 =  %+11.7f", (double)NV_CONSTANTS(H110));
    		
        sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(H175) ); 
        t1_temp = enterNumber (number_ptr,LINE2 + 8, 12);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
            //return;         
            continue;
        }    
        
    		sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(H275) ); 
    	
    		t2_temp = enterNumber(number_ptr,LINE3 + 8, 12);    // *number_ptr1 will hold the entered number

        button = getLastKey();
        if(button == ESC)                                   //ESC was pressed, exit to menu
        {
          //return;              
          continue;
        }  
        
        sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(H110) ); 
        t3_temp = enterNumber( number_ptr,LINE4 + 8, 12);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
          //return;        
          continue;
        }   
        
        // if enter is pressed the third time, the values will be saved
        // Only write to EEPROm if values have changed
        if ((NV_CONSTANTS(H175) != t1_temp ) )
        {
          NV_MEMBER_STORE ( Constants.H175, t1_temp );
        }
        
        if ((NV_CONSTANTS(H275) != t2_temp ) )
        {
          NV_MEMBER_STORE ( Constants.H275, t2_temp );
        } 
         
        if ((NV_CONSTANTS(H110) != t2_temp ) )
        {
          NV_MEMBER_STORE ( Constants.H110, t3_temp );
        } 
      }
      else if ( selection == 5)
      {
        CLEAR_DISP; 
        LCD_position(LINE2);
        _LCD_PRINTF ( "H210 =  %+11.7f", (float)NV_CONSTANTS(H210));
        sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(H210) ); 
        t1_temp = enterNumber (number_ptr,LINE2 + 8, 12);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
            //return;         
            continue;
        }    
       
        // if enter is pressed the third time, the values will be saved
        // Only write to EEPROm if values have changed
        if ((NV_CONSTANTS(H210) != t1_temp ) )
        {
          NV_MEMBER_STORE ( Constants.H210, t1_temp );
        }
      }
      else if ( selection == 6)
      {
        CLEAR_DISP; 
        LCD_position(LINE2);
        _LCD_PRINTF ( "S11 =  %+11.7f", (float)NV_CONSTANTS(S11));
           
    		LCD_position(LINE3);  
        _LCD_PRINTF ( "S12 =  %+11.7f", (float)NV_CONSTANTS(S12));
    	                       
        LCD_position(LINE4);
        _LCD_PRINTF ( "S13 =  %+11.7f", (double)NV_CONSTANTS(S13));
    		
        sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(S11) ); 
        t1_temp = enterNumber (number_ptr,LINE2 + 8, 12);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
            //return;         
            continue;
        }    
        
    		sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(S12) ); 
    	
    		t2_temp = enterNumber(number_ptr,LINE3 + 8, 12);    // *number_ptr1 will hold the entered number

        button = getLastKey();
        if(button == ESC)                                   //ESC was pressed, exit to menu
        {
          //return;              
          continue;
        }  
        
        sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(S13) ); 
        t3_temp = enterNumber( number_ptr,LINE4 + 8, 12);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
          //return;        
          continue;
        }   
        
        // if enter is pressed the third time, the values will be saved
        // Only write to EEPROm if values have changed
        if ((NV_CONSTANTS(H175) != t1_temp ) )
        {
          NV_MEMBER_STORE ( Constants.S11, t1_temp );
        }
        
        if ((NV_CONSTANTS(H275) != t2_temp ) )
        {
          NV_MEMBER_STORE ( Constants.S12, t2_temp );
        } 
         
        if ((NV_CONSTANTS(H110) != t2_temp ) )
        {
          NV_MEMBER_STORE ( Constants.S13, t3_temp );
        } 
      }
      
      else if ( selection == 7)
      {
        CLEAR_DISP; 
        LCD_position(LINE2);
        _LCD_PRINTF ( "S21 =  %+11.7f", (float)NV_CONSTANTS(S21));
           
    		LCD_position(LINE3);  
        _LCD_PRINTF ( "S22 =  %+11.7f", (float)NV_CONSTANTS(S22));
    	                       
        LCD_position(LINE4);
        _LCD_PRINTF ( "S23 =  %+11.7f", (double)NV_CONSTANTS(S23));
    		
        sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(S21) ); 
        t1_temp = enterNumber (number_ptr,LINE2 + 8, 12);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
            //return;         
            continue;
        }    
        
    		sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(S22) ); 
    	
    		t2_temp = enterNumber(number_ptr,LINE3 + 8, 12);    // *number_ptr1 will hold the entered number

        button = getLastKey();
        if(button == ESC)                                   //ESC was pressed, exit to menu
        {
          //return;              
          continue;
        }  
        
        sprintf ( number_ptr, "%+11.7f", (double)NV_CONSTANTS(S23) ); 
        t3_temp = enterNumber( number_ptr,LINE4 + 8, 12);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
          //return;        
          continue;
        }   
        
        // if enter is pressed the third time, the values will be saved
        // Only write to EEPROm if values have changed
        if ((NV_CONSTANTS(S21) != t1_temp ) )
        {
          NV_MEMBER_STORE ( Constants.S21, t1_temp );
        }
        
        if ((NV_CONSTANTS(S22) != t2_temp ) )
        {
          NV_MEMBER_STORE ( Constants.S22, t2_temp );
        } 
         
        if ((NV_CONSTANTS(S23) != t2_temp ) )
        {
          NV_MEMBER_STORE ( Constants.S23, t3_temp );
        } 
      }      
      
    }  // end of while(1)

}


 /******************************************************************************
 *  Name: enter_calibration_time
 *  
 *  PARAMETERS:None
 *
 *  DESCRIPTION: Enter the month, day, year of the calibration.
 *               Stores date in EEPROM
 *
 *  RETURNS: 
 *
 *****************************************************************************/ 
 
void enter_calibration_time(void)  // leads user through process to set the time and date
{
  Bool  escape = 0;
  uint8_t i, j, msb_yr, lsb_yr, msb_month, lsb_month, msb_day, lsb_day, month_temp, day_temp, date_reg = 0;  
    
  date_time_t date ;  
  
  enum buttons button;
  
/////////////////////////Enter the date///////////////////////////////////////////
  CURSOR_OFF;
  while(!escape)
  {
    CLEAR_DISP;
    LCD_position(LINE1);
    _LCD_PRINT ( "Enter Calibration");
    LCD_position(LINE2);  
    _LCD_PRINT ( "Date: MM/DD/20YY");       
    YES_to_Accept(LINE3);  
    ESC_to_Exit(LINE4);
 
    i = 0;  //initialize markers
    j = 1;
    while(1)                                    //loop for entering each cnt cycle digit
    {
      if(j == 7)
      {
         CURSOR_OFF;
      }   
      else 
      {
        CURSOR_ON;
      }  
      LCD_position(LINE2 + (i+6));                // position cursor and flashing
      while(1)
      {
        button = getKey(TIME_DELAY_MAX);
        if((button <= 9) || (button == ESC) || (button == YES) || (button == CE))
        { break ; }
      }      
      if(button <= 9)                             // digit was entered   
      {   
        switch(j)
        {
          case 1:
          if(button < 2)                          //number must be 0 or 1
          {
            _LCD_PRINTF ( "%c",key_char); 
            msb_month = button;   
            BIT_SET(date_reg, 0);    
            i++;
            j++;
          }
          else if(button > 1)                     //in case number is > 1
          {
            _LCD_PRINTF ( "0%c",key_char);
            msb_month = 0;
            lsb_month = button;
            BIT_SET(date_reg, 0);
            BIT_SET(date_reg, 1);
            i = 3;   // i += 3
            j = 3;   // j += 2
          }
          break;
          
          case 2:                      
          if((msb_month == 1) && (button < 3))        //number must be 0 - 2
          {
            _LCD_PRINTF ( "%c",key_char);
            lsb_month = button;
            BIT_SET(date_reg, 1);
            i = 3;  // i += 2
            j = 3;  // j++;
          }
          else if((msb_month == 0) && (button > 0))   //number must be 0 - 9
          {
            _LCD_PRINTF ( "%c",key_char);
            lsb_month = button;
            BIT_SET(date_reg, 1);
            i = 3;  // i += 2;
            j = 3;  // j++;
          }
          break;
                    
          case 3:
          month_temp = msb_month*10 + lsb_month;
          if((month_temp != 2) && (button < 4))        //if not February, number from 0 - 3
          {
            _LCD_PRINTF ( "%c",key_char);
            msb_day = button;
            BIT_SET(date_reg, 2);
            i = 4;
            j = 4;
          }
          else if((month_temp == 2) && (button < 3))  //if February, number from 0 - 2
          {
            _LCD_PRINTF ( "%c",key_char);
            msb_day = button;
            BIT_SET(date_reg, 2);
            i = 4;
            j = 4;
          }
          else if(((month_temp != 2) && (button > 3)) || ((month_temp == 2) && (button > 2)))
          {
            _LCD_PRINTF ( "0%c",key_char);
            msb_day = 0;
            lsb_day = button;
            BIT_SET(date_reg, 2);
            BIT_SET(date_reg, 3);
            i = 8;
            j = 5;
          }          
          break;
          
          case 4:
          if((msb_day != 0) && (msb_day < 3))           // day must be greater than 00
          {
            _LCD_PRINTF ( "%c",key_char);
            lsb_day = button;
            BIT_SET(date_reg, 3);
            i = 8;
            j = 5;
          }
          else if((msb_day > 2) && (button == 1) && (month_temp != 4) && (month_temp != 6) && (month_temp != 9) && (month_temp != 11))
          {                                                             //take into account 30 day months
            _LCD_PRINTF ( "%c",key_char);
            lsb_day = button;
            BIT_SET(date_reg, 3);
            i = 8;
            j = 5;
          }
          else if((msb_day > 2) && (button == 0))                    //take into account 31 day months
          {
            _LCD_PRINTF ( "%c",key_char);
            lsb_day = button;
            BIT_SET(date_reg, 3);
            i = 8;
            j = 5;
          }
          else if((msb_day == 0) && (button > 0))         // day cannot be 00
          {
            _LCD_PRINTF ( "%c",key_char);
            lsb_day = button;
            BIT_SET(date_reg, 3);
            i = 8;
            j = 5;
          }
          break;
           
          case 5:
          _LCD_PRINTF ( "%c",key_char);
          msb_yr = button;
          BIT_SET(date_reg,4);
          i = 9;
          j = 6;
          break;
          
          case 6:
          _LCD_PRINTF ( "%c",key_char);
          lsb_yr = button;
          BIT_SET(date_reg,5);
          i = 10; 
          j = 7;
          break;
        }
      }
      else if(button == YES)                    // Yes key was pressed
      { 
        if(date_reg == 63)                            // All spaces filled
        {
          CURSOR_OFF;
          month_temp = msb_month*10 + lsb_month;
          day_temp = msb_day*10 + lsb_day;
          if(date_check(month_temp,day_temp)==1)
          {             
            escape = TRUE;
            CURSOR_OFF;
                  
            break;
          }
          break;
        }
      }
      else if(button == CE)                                       // NO/CE key was pressed, clear last digit entered   
      {
        if(j != 1)                               
        { 
          if(j == 2)
          {
            --i;
            --j;
          } 
          else if (j == 3)
          {
            i -= 2;
            --j;
          }
          else if (j == 4)
          {
            --i;
            --j;
          } 
          else if (j == 5 )
          {
            i -= 4;
            --j;
          }
          else if (j == 6)
          { 
            --j;
            --i;
          }  
          else if (j == 7)
          { 
            --j;
            --i;
          } 
          LCD_position(LINE2 + (i+6));
        }
      }
      else  //button == ESC                                                   
      {
        escape = TRUE;
        CURSOR_OFF;
        break;
      }          
  }  // end of loop for entering each  digit
  
    if (button != ESC)                      
    {
      date.iday   = (msb_day *10) + lsb_day;
      date.imonth = (msb_month *10) + lsb_month;
      date.iyear    = (msb_yr *10) + lsb_yr + 2000;      
      date.iminute = 0;  
      date.ihour  = 12;
      date.isecond = 0;
      //write the date/time to memory
      NV_MEMBER_STORE(Constants.CAL_DATE, date);
    }
    break;
 } // end while(1) loop 
  
  if (button == ESC)   //MENU or STOP was pressed, exit to MENU
  {
     escape = TRUE;           
  }   
    
  
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
void enter_cal_stand (void)
{
  uint16_t stand;
  char number_ptr[11] = "\0\0\0\0\0\0";  // enter a code here when ready to test
  
  enter_cal_den1_std_text();    //TEXT// display "Enter Density 1 Std,Count: " LINE1 and LINE2
  YES_to_Accept(LINE3);        //TEXT// display "YES to Accept"  
  ESC_to_Exit(LINE4);          //TEXT// display "ESC to Exit"      
  
 
  NV_EE_RD( Constants.DENS_1_CAL_STD, &stand );
  
  if ( stand > 65535 )
  {
   stand = 9999 ;
  }
 
  sprintf( number_ptr, "%u", stand );
  stand = (uint16)enter_number(number_ptr,LINE2+10,5,0); 

  
  if( getLastKey() != ESC)
  {
    NV_MEMBER_STORE ( Constants.DENS_1_CAL_STD, stand);
  }  
     
  // get sys 2 calibration standard count
  enter_cal_den2_std_text();    //TEXT// display "Enter Density 2 Std Count: " LINE1 and LINE2
  YES_to_Accept(LINE3);        //TEXT// display "YES to Accept"  
  ESC_to_Exit(LINE4);          //TEXT// display "ESC to Exit"      
  
 
  NV_EE_RD( Constants.DENS_2_CAL_STD, &stand );
  
  if ( stand > 65535 )
  {
   stand = 9999 ;
  }
 
  sprintf( number_ptr, "%u", stand );
  stand = (uint16)enter_number(number_ptr,LINE2+10,5,0); 

  
  if( getLastKey() != ESC)
  {
    NV_MEMBER_STORE ( Constants.DENS_2_CAL_STD, stand);
  }  

 
}
