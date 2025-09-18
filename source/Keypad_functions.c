/******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  Keypad_functions.c
 *  Originating Author:  Chris Churavy
 *       Creation Date:  10/21/2010 
 *
 *     5-1-2022  Added remote key function for Plus baseboard
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
#include "DataStructs.h"
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "prompts.h"
#include "Utilities.h"
#include "Batteries.h"


#define REMOTE_ENTER 0x04
#define REMOTE_ESC   0x02

/*----------------------------------------------------------------------------*/
/*------------------------[   Module Global Variables   ]---------------------*/
/*----------------------------------------------------------------------------*/
uint8_t key_char;

enum buttons button;


/*----------------------------------------------------------------------------*/
/*-------------------------[   Global Functions   ]---------------------------*/
/*----------------------------------------------------------------------------*/

extern void pulseBuzzer ( void );
extern uint8 getPlusRemoteKey ( void );  // Remote Key for Plus BB.

/*******************************************************************************
 * 
 *  FUNCTION:  readBothColumns()
 *
 *  PARAMETERS:  None
 *
 *  DESCRIPTION: Reads the 11 columns and the YES and NO keys. 
 *
 * 
 *  RETURNS: enum
 *
 ******************************************************************************/
void  scan_keys ( void )
{
  // function scans keypad, if button is pressed, button variable is set and remains set until another button is pressed
  static BYTE off_counter;
  
  uint16 column = 0, column1 = 0;
  uint8  row = 0;
  uint8 remote = 0;
  
  // disable both rows to check for No or YES pressed
  ROW_1_Write ( 1 );
  ROW_2_Write ( 1 );
 
  // check if key pressed
  column  =    Status_KEY_COLUMN_0_Read ();
  column1 =    Status_KEY_COLUMN_1_Read() ;
  column |=  ( column1  << 8 );
  column &= 0x1FFF;
  
  // get the row
  if ( column != 0x1FFF )
  {
    row = 3;
  } 
  else
  {
    // enable row 2
    ROW_1_Write ( 1 );
    ROW_2_Write ( 0 );
    delay_ms  ( 1 );
   
    // check if key pressed
    column  =    Status_KEY_COLUMN_0_Read ();
    column1 =    Status_KEY_COLUMN_1_Read() ;
    column |=  ( column1  << 8 );
    column &= 0x1FFF;
    
   // get the column
   if ( column != 0x1FFF )
   {
     row = 1;
   }
  
   else
   {
     // enable row 1
     ROW_1_Write ( 0 );
     ROW_2_Write ( 1 );
     delay_ms  ( 1 );
     
     // check if key pressed
     column  =   Status_KEY_COLUMN_0_Read  ();
     column |=  ( Status_KEY_COLUMN_1_Read  () << 8 );
     column &= 0x01FFF;
     if ( column != 0x1FFF )
     {
      row = 2;
     }
    }
  }
 
   
  if ( g_remote_start_flag == TRUE )
  {
   g_remote_start_flag = FALSE;
   remote = REMOTE_ENTER;
  }
  else if ( g_remote_recall_flag == TRUE )  
  {
    g_remote_recall_flag = FALSE;
    remote = REMOTE_ENTER;
    g_recall_flag = TRUE;
  }
  else
  {
    remote = 0;
  }
   

  // If row = 1 or 2, a key is being pressed. Find the key, and assign it to button variable
  if (( row != 0) || ( remote != 0 ) ) 
  {
#if 0   
    //Flags.button_pressed = TRUE;
    if ( bit_test ( column, 0 ) == 0 )
    {
      if ( row == 2 )
      {
        button = CE;
      }
      else if ( row == 1 )
      {
        button = TIME;
      }
    }
    else if  ( bit_test ( column, 1 ) == 0 )
    {
      if ( row == 2 )
      {
        button = UP;
      }
      else if ( row == 1 )
      {
        button = DOWN;
      }
    }
     else if  ( bit_test ( column, 2 ) == 0 )
    {
      if ( row == 2 )
      {
        button = STAND;
      }
      else if ( row == 1 )
      {
        button = MA;
      }
    }
    else if ( (bit_test ( column, 3 ) == 0) ||  ( remote & REMOTE_ENTER ) ) 
    {
      if ( row == 2 )
      {
        button = MENU;
      }
      else if ( (row == 1) ||  ( remote & REMOTE_ENTER ) ) 
      {
        button = ENTER;
      }
    }
    else if  ( bit_test ( column, 4 ) == 0  ||  ( remote & REMOTE_ESC )) 
    {
      if(( row == 2 ) ||  ( remote & REMOTE_ESC )) 
      {
        button = ESC;
      }
      else if ( row == 1 )
      {
        button = STORE;
      }
    }
    else if  ( bit_test ( column, 5 ) == 0 )
    {
      if ( row == 2 )
      {
        button = ONE;
        key_char = '1';
      }
      else if ( row == 1 )
      {
        button = PROJ;
      }
    }
    else if  ( bit_test ( column, 6 ) == 0 )
    {
      if ( row == 2 )
      {
        button = THREE;
        key_char = '3';
      }
      else if ( row == 1 )
      {
        button = EIGHT;
        key_char = '8';
      }
      else if ( row == 2 )
      {
       button = NO;
                
      }  
    }
    else if  ( bit_test ( column, 7 ) == 0 )
    {
      if ( row == 2 )
      {
        button = FOUR;
        key_char = '4';
      }
      else if ( row == 1 )
      {
        button = NINE;
        key_char = '9';
      }
    }
    else if  ( bit_test ( column, 8 ) == 0 )
    {
      if ( row == 2 )
      {
        button = FIVE;
        key_char = '5';
      }
      else if ( row == 1 )
      {
        button = ZERO;
        key_char = '0';
      }
    }
    else if  ( bit_test ( column, 9 ) == 0 )
    {
      if ( row == 2 )
      {
        button = SIX;
        key_char = '6';
      }
      else if ( row == 1 )
      {
        button = THICKNESS;
      }
    }
    else if  ( bit_test ( column, 10 ) == 0 )
    {
      if ( row == 3 )
      {
        button = NO;
      }  
    }
    else if  ( bit_test ( column, 11 ) == 0 )
    {
      if ( row == 3 )
      {
        button = YES;
      }  
    }
    else if ( bit_test ( column, 12 ) == 0 )
    {
      if ( row == 2 )
      {
        button = TWO;
        key_char = '2';
      }
      else if ( row == 1 )
      {
        button = SEVEN;
        key_char = '7';
      }
     }
    Flags.button_pressed = TRUE;
  #else
     //Flags.button_pressed = TRUE;
    if ( bit_test ( column, 0 ) == 0 )
    {
      if ( row == 2 )
      {
        button = CE;
      }
      else if ( row == 1 )
      {
        button = MA;
      }
    }
    else if  ( bit_test ( column, 1 ) == 0 )
    {
      if ( row == 2 )
      {
        button = ESC;
      }
      else if ( row == 1 )
      {
        button = THICKNESS;
      }
    }
     else if  ( bit_test ( column, 2 ) == 0 )
    {
      if ( row == 2 )
      {
        button = UP;
      }
      else if ( row == 1 )
      {
        button = DOWN;
      }
    }
    else if ( (bit_test ( column, 3 ) == 0) ||  ( remote & REMOTE_ENTER ) ) 
    {
      if ( row == 2 )
      {
        button = MENU;
      }
      else if ( (row == 1) ||  ( remote & REMOTE_ENTER ) ) 
      {
        button = ENTER;
      }
    }
    else if  ( bit_test ( column, 4 ) == 0  ||  ( remote & REMOTE_ESC )) 
    {
      if(( row == 2 ) ||  ( remote & REMOTE_ESC )) 
      {
        button = STAND;
      }
      else if ( row == 1 )
      {
        button = TIME;
      }
    }
    else if  ( bit_test ( column, 5 ) == 0 )
    {
      if ( row == 2 )
      {
        button = ONE;
        key_char = '1';
      }
      else if ( row == 1 )
      {
        button = PROJ;
      }
    }
    else if  ( bit_test ( column, 6 ) == 0 )
    {
      if ( row == 2 )
      {
        button = THREE;
        key_char = '3';
      }
      else if ( row == 1 )
      {
        button = EIGHT;
        key_char = '8';
      }
      else if ( row == 2 )
      {
       button = NO;
                
      }  
    }
    else if  ( bit_test ( column, 7 ) == 0 )
    {
      if ( row == 2 )
      {
        button = FOUR;
        key_char = '4';
      }
      else if ( row == 1 )
      {
        button = NINE;
        key_char = '9';
      }
    }
    else if  ( bit_test ( column, 8 ) == 0 )
    {
      if ( row == 2 )
      {
        button = FIVE;
        key_char = '5';
      }
      else if ( row == 1 )
      {
        button = ZERO;
        key_char = '0';
      }
    }
    else if  ( bit_test ( column, 9 ) == 0 )
    {
      if ( row == 2 )
      {
        button = SIX;
        key_char = '6';
      }
      else if ( row == 1 )
      {
        button = STORE;
      }
    }
    else if  ( bit_test ( column, 10 ) == 0 )
    {
      if ( row == 3 )
      {
        button = NO;
      }  
    }
    else if  ( bit_test ( column, 11 ) == 0 )
    {
      if ( row == 3 )
      {
        button = YES;
      }  
    }
    else if ( bit_test ( column, 12 ) == 0 )
    {
      if ( row == 2 )
      {
        button = TWO;
        key_char = '2';
      }
      else if ( row == 1 )
      {
        button = SEVEN;
        key_char = '7';
      }
     }
    Flags.button_pressed = TRUE; 
  #endif  
  }
  else
  {
    Flags.button_pressed = FALSE;       
  }
  
  // key was pressed
  if(Flags.button_pressed)
  {
    global_special_key_flag = TRUE;   // This global is cleared by whatever routine waits for a key press

    shutdown_timer = 0;                       // reset  shutdown timer if keypad is pressed (this is the auto shutdown)
    if( Controls.shut_dwn && !IS_CHARGER_DETECTED() && (button == NO) )  
    {
      if(off_counter >= 10 )   // 5 is pretty fast.
      {

        shut_down_text();
      }
	    else
      {
          off_counter +=1;
      }    
    }
    else // don't turn off if charger is connected.
	  {
      off_counter = 0;
	  }
   
    if ( Controls.buzz_enable && Features.sound_on && !Spec_flags.self_test )//
    { 
      Controls.buzz_enable = FALSE;
      pulseBuzzer ( );

     }   
    
  }   
  else 
  {
    off_counter = 0;
  }  
  
  
  ROW_1_Write ( 1 );
  ROW_2_Write ( 1 );
}


/*******************************************************************************
 *
 *  wait_for_key_release
 *
 *  PARAMETERS:None
 *
 *  DESCRIPTION: If a zero is pressed and this function is called, it will wait
 *               about 0.5 seconds before scanning the keys again. If zero is
 *               still pressed it will set the button to period.
 *
 *  RETURNS:None
 *
 ******************************************************************************/
uint8 wait_for_key_zero_period ( void )
{ 
  int16_t button_timer = 0;
 
  // wait until button not pressed or 500ms elapses.
  while ( Flags.button_pressed && (++button_timer < 50) )
  { 
   delay_ms(10);  
  }
  
  if ( Flags.button_pressed == FALSE )
  {
    key_char = '0';
    return ZERO;
  }
 
  key_char = '.';
  Controls.buzz_enable = TRUE;
  return PERIOD;  
   
}



/*******************************************************************************
 *
 *  wait_for_key_release
 *
 *  PARAMETERS:None
 *
 *  DESCRIPTION: If a key is pressed and this function is called, it will wait
 *               about 0.5 seconds before scanning the keys again.  If the key
 *               is released, the button status is set to DFLT
 *
 *  RETURNS:None
 *
 ******************************************************************************/
void wait_for_key_release ( void )
{ 
 // int32_t button_timer = 0;
 
 // while ( Flags.button_pressed && (++button_timer < 50) )
//  { 
 //  delay_ms(10);  
 // }
  
 while ( Flags.button_pressed  )
 { 
  delay_ms(10);  
 }
  
  Controls.buzz_enable = TRUE;
  
//  if ( button_timer >= 50 )
 // {
 //   if(button == NO)
 //   {
 //       Controls.buzz_enable = FALSE;//}    
  //  scan_keys();
//  else
 // {
    button = DFLT;
 // }    
}


/******************************************************************************
 *
 *  Name: getKey ( void )
 *
 *  PARAMETERS: waits for key release, then gets the next key hit
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 
 enum buttons  getKey ( uint32_t time_delay_ms )
 {
  wait_for_key_release();
      
  while ( ( !Flags.button_pressed ) && ( time_delay_ms-- > 0 ))
  {
    delay_ms(1);
     
  }
   return button;
 
 }


/******************************************************************************
 *
 *  Name: getNewKey ( void )
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 
 enum buttons  getNewKey ( uint32_t time_delay_ms )
 {
  
  button = DFLT;
  
  wait_for_key_release();
      
  while ( ( !Flags.button_pressed ) && ( time_delay_ms-- > 0 ))
  {
    delay_ms(1);
     
  }
   return button;
 
 }

/******************************************************************************
 *
 *  Name: getLastKey ( void )
 *
 *  PARAMETERS: gets the last key press
 *
 *  DESCRIPTION: 
 *               
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

 enum buttons getLastKey( void )
 {
   return button;
 }

/*******************************************************************************
 *
 *  enter_number
 *
 *  PARAMTERS:String for number,LCD position,length of number, signed/unsigned
 *
 *  DESCRIPTION:Accepts and displays user entered number at specified position
 *
 *  RETURNS:Entered number as float
 *
 ******************************************************************************/
int32 enter_number(char *temp_str, int16_t position, int16_t length, int16_t sign)
{ 
  int i, c= 1, x;
  signed int j;
  float num_temp;
  
  c= strlen(temp_str);  
  i = c;
  for(;i<11;i++)                   // fill everyting after the first '\0' with '\0'
  {
    temp_str[i] = '\0';
  }
  i = 0; 
  LCD_position(position + (1-c));
  _LCD_PRINTF("%s",temp_str); 
  num_temp = atof(temp_str);
       
  while(1)
  {
    LCD_position(position);
    if((i < length) || ((i == length) && (temp_str[0] == '-')) )
    {  CURSOR_ON; }
    else 
    {  CURSOR_OFF; }
   
    wait_for_key_release();
   
    while(!Flags.button_pressed)                            // wait for button press
    {
      delay_ms(10);
    }  
  
    CURSOR_OFF;
   
    if ( button == ZERO  )
    {
      button = wait_for_key_zero_period();
    }
    
     if(button <= NINE )         // number button pressed
     {      
      
      if(i == 0)
      {        
        LCD_position(position + (1-c));
        for(j=0;j<c;j++)
        {
          _LCD_PRINT(" ");       
          temp_str[j] = 0;
        }
        c = 1; 
        temp_str[i] = key_char;
        if(button != ZERO)
          i++;       
      }         
      else if((i > 0) && ((c < length) || ((c == length) && (temp_str[0] == '-'))))
      {
        temp_str[i] = key_char;
        x = 0;
        for(j=0;j<=c;j++)
        {
          if(temp_str[j] == '.')
          x++;
        }
        if(x > 1)
        {
          temp_str[i] = 0;            
          continue; 
        }        
        c++;
        i++;        
      }
      LCD_position(position + (1-c));
      _LCD_PRINTF("%s",temp_str);
    }
    else if(button == YES)                          // ENTER pressed, save number           
    {      
      num_temp = atof(temp_str);          
      return(num_temp);
    } 
    else if(button == UP && sign)
    {      
      if(temp_str[0] != '-')                            // number is positive, toggle minus sign and slide number to right
      {  
        for(j=c;j>0;j--)
          temp_str[j] = temp_str[j-1];
        temp_str[0] = '-';  
        if(((temp_str[1] == '0') && (c <=1))|| (temp_str[1]== ' ') || (temp_str[1] == 0))  // if first digit is cleared or a zero
        {      
          c = 1;                                                           // reset i and k values to match   
          temp_str[1] = 0;
        }
        else
          c++;
        i = c;              
        LCD_position(position - c + 1);// strlen(temp_str));          
        for(j=0;j<c; j++)
          _LCD_PRINT(" ");
      }
      else                                            // number is already negative
      {
        for(j=0;j<c;j++)               // toggle minus sign and slide number to the left
          temp_str[j] = temp_str[j+1]; 
          temp_str[j+1] = 0;                       // clear repeated digit at end of number
        c = (temp_str[0]==0) ? 1 : c-1;            // the number was already blank 
        i = (i!=1) ? c : i-1;                         // this is so i never rolls past zero     
        LCD_position(position - c);// strlen(temp_str));          
        for(j=0;j<=c; j++)
          _LCD_PRINT(" ");
      }      
      LCD_position(position + (1 - c));
      _LCD_PRINTF("%s",temp_str);   
    }
    else if(button == CE)                             // CE pressed, backspace                      
    {      
      if(c <= 1)                                      // there is only one displayed symbol, backspace will make it '0'
      {
        i = 0;
        temp_str[0] = '0';       
      }
      else if(c > 1)                                  // number is first displayed and backspace is pressed
      {
        i = c - 1;                                    // reset i so whole number is not erased
        temp_str[i] = 0;
        c--;
      }        
      LCD_position(position - strlen(temp_str)); 
      for(j = 0;j < (int)strlen(temp_str); j++)
        _LCD_PRINT(" ");
      LCD_position(position + (1-c));        
      _LCD_PRINTF("%s",temp_str);
    }
    else if(button == DOWN)             //erase
    {
      LCD_position(position + (1-c));
      for(j=0;j<c;j++)
      {
        _LCD_PRINT(" ");       
        temp_str[j] = 0;
      }     
      temp_str[0] = '0';
      LCD_position(position);
      _LCD_PRINT("0");
      c = 1;
      i = 0;
    }        
    else if((button == MENU) || (button == ESC))             // MENU or STOP was pressed                          
      return(num_temp);         
  }
} 

/*******************************************************************************
 *
 *  NAME: enter_number_std()
 *
 *  PARAMETERS: String for number,LCD position,length of number,whether signed
 *              or unsigned
 *
 *  DESCRIPTION:  Accepts and displays user entered number.(The difference between 
 *                this function and enter_number is that this function will accept 
 *                numbers from left to right, the cursor advancing to the next 
 *                space, whereas enter_number has a stationary cursor with 
 *                entered numbers shifting to the left.
 *
 *  RETURNS:  Entered number as int32
 *
 ******************************************************************************/
int32 enter_number_std(char *temp_str, int position, int length, int16_t sign)
{
  int i = 0, x = 0, j = 0;
  int32 num_temp;
  
  LCD_position(position);
  
  _LCD_PRINTF ( "%s", temp_str ); 
  
  num_temp = atoi ( temp_str ); 
  
  i=0;
  
  while(1)
  {
    LCD_position(position + i);
    
    if (i < length )
    {
        CURSOR_ON;
    }    
    else 
    {  
      CURSOR_OFF;
    }  
    
    wait_for_key_release();
    
    while(!Flags.button_pressed)
    {
        delay_ms(10);
    }    
    
    CURSOR_OFF;  
    
    if( (button <= NINE)  &&  (i < length) ) // digit was entered   
    {
      if (i == 0)
      {
        temp_str[i] = key_char;
        LCD_position(position + i);
        _LCD_PRINTF("%c",temp_str[i]);
        i++;       
      } 
      else
      {
        temp_str[i] = key_char;
        x = 0;
        for ( j=0; j < (int)strlen(temp_str); j++ )
        {
          if(temp_str[j] == '.')
          {
            x++;
          }  
        }
        if(x > 1)
        {
          temp_str[i] = 0;          
          continue;
        }        
        _LCD_PRINTF("%c",temp_str[i]); 
        i++;        
      }
    }
    else if(((button == UP) || (button == DOWN)) && sign)           // uncomment to enable negative numbers
    {                                            // you must assign button numbers   
      
      if  ( i == 0 )
      { 
        if ( button == UP )
        { 
         temp_str[i] = '+';
        }
        else 
        {
         temp_str[i] = '-';
        }
        
        LCD_position(position + i);
        _LCD_PRINTF("%c",temp_str[i]);
        i++;    
      }     
        
    }
    else if(button == YES)
    {      
      num_temp = atoi(temp_str);
      return(num_temp);      
    }
    else if(button == ESC)
    {
        return(num_temp);        
    }    
    else if(button == CE)                          // CE key was pressed, clear last digit entered   
    {
      if ( i >= 1 )                               
      {
          i--;
      }    
    
      LCD_position(position + i);
      _LCD_PRINT(" ");                           // blank out last digit entered
      temp_str[i] = 0;                             // remove last digit from string                       
    }    
  }
}
/*******************************************************************************
 *
 *  NAME: enter_name
 *
 *  PARAMETERS:Pointer to string, LCD_position
 *
 *  DESCRIPTION:Accepts and displays user entered name, and writes it to memory
 *              pointed to by *file
 *
 *  RETURNS:None
 *
 ******************************************************************************/
void enter_name(char *file, int position)
{
//allows user to enter file name, position is LCD position, file is what you want to save
  int i = 0, f  = 0, last_key = 99;
  uint8 len = strlen(file);
  char temp_str[PROJ_NAME_LENGTH] = NULL_NAME_STRING ;     
 
  enum buttons button;
 
  LCD_position(position);
  _LCD_PRINTF("%s",file);
  LCD_position(position);
  
  for ( i=0; i < (int)strlen(file); i++)
  {
    temp_str[i] = file[i];
  }  
  
  i = 0;
  f = 0;
  
  while(1)
  {
    LCD_position(position + i);
   
    if(i < 10)
    {
      CURSOR_ON;
    }   
    else 
    { 
      CURSOR_OFF;
    }  
    
    wait_for_key_release();
    
    while(!Flags.button_pressed)                            // wait for button press
    {
      delay_ms(10);    
    }  
    
    button = getLastKey( );
    
    //if ( ((button <= NINE) || (button == PERIOD)) && (i < 10) )
    if ( (button <= NINE)  && (i < 10) )
    {           
      f++;
      if((button != last_key) && (last_key !=DFLT) && (i != 9))
      {          
        i++;
        f = 0;
      }
      last_key = button;                     //store current button value      
      if(button == ZERO)
      {
        if(f != 0)
        {
          temp_str[i] = '.';
          f = -1;
        }  
        else
        {
          temp_str[i] = '0';
          f = 1;
        } 
        
      }
    
      else if(button == ONE)
      {
        if(f == 1)
        { 
          temp_str[i] = '1';
        }  
        else if(f == 2)
        {
          temp_str[i] = 'A';
        }  
        else if(f == 3)
        {
          temp_str[i] = 'B';
        }  
        else if(f == 4)
        { 
          temp_str[i] = 'C';
        }  
        else
        {
          temp_str[i] = '1';
          f = 1;
        }
      }
      else if(button == TWO)
      {
        if(f == 1)
        { 
          temp_str[i] = '2';
        }  
        else if(f == 2)
        { 
          temp_str[i] = 'D';
        }  
        else if(f == 3)
        { 
          temp_str[i] = 'E';
        }  
        else if(f == 4)
        { 
          temp_str[i] = 'F';
        }  
        else
        {
          temp_str[i] = '2';
          f = 1;
        }
      }
      else if(button == THREE)
      {
        if(f == 1)
        {
          temp_str[i] = '3';
        }  
        else if(f == 2)
        { 
          temp_str[i] = 'G';
        }  
        else if(f == 3)
        { 
          temp_str[i] = 'H';
        }  
        else if(f == 4)
        { 
          temp_str[i] = 'I';
        }  
        else
        {
          temp_str[i] = '3';
          f = 1;
        }
      }
      else if(button == FOUR)
      {
        if(f == 1)
        { 
          temp_str[i] = '4';
        }  
        else if(f == 2)
        { 
          temp_str[i] = 'J';
        }  
        else if(f == 3)
        { 
          temp_str[i] = 'K';
        }  
        else if(f == 4)
        { 
          temp_str[i] = 'L';
        }  
        else
        {
          temp_str[i] = '4';
          f = 1;
        }
      }
      else if(button == FIVE)
      {
        if(f == 1)
        { 
          temp_str[i] = '5';
        }  
        else if(f == 2)
        {
          temp_str[i] = 'M';
        }  
        else if(f == 3)
        { 
          temp_str[i] = 'N';
        }  
        else if(f == 4)
        { 
          temp_str[i] = 'O';
        }  
        else
        {
          temp_str[i] = '5';
          f = 1;
        }
      }
      else if(button == SIX)
      {
        if(f == 1)
        { 
          temp_str[i] = '6';
        }  
        else if(f == 2)
        { 
          temp_str[i] = 'P';
        }  
        else if(f == 3)
        { 
          temp_str[i] = 'Q';
        }  
        else if(f == 4)
        {
          temp_str[i] = 'R';
        }  
        else
        {
          temp_str[i] = '6';
          f = 1;
        }
      }
      else if(button == SEVEN)
      {
        if(f == 1)
        { 
          temp_str[i] = '7';
        }  
        else if(f == 2)
        { 
          temp_str[i] = 'S';
        }  
        else if(f == 3)
        {
           temp_str[i] = 'T';
        }   
        else if(f == 4)
        { 
          temp_str[i] = 'U';
        }  
        else
        {
          temp_str[i] = '7';
          f = 1;
        }
      }
      else if(button == EIGHT)
      {
        if(f == 1)
        {
          temp_str[i] = '8';
        }   
        else if(f == 2)
        {  
          temp_str[i] = 'V';
        }  
        else if(f == 3)
        { 
          temp_str[i] = 'W';
        }  
        else if(f == 4)
        { 
          temp_str[i] = 'X';
        }  
        else
        {
          temp_str[i] = '8';
          f = 1;
        }
      }
      else if(button == NINE)
      {
        if(f == 1)
        {
           temp_str[i] = '9';
        }   
        else if(f == 2)
        {  
          temp_str[i] = 'Y';
        }  
        else if(f == 3)
        { 
          temp_str[i] = 'Z';
        }  
        else
        {
          temp_str[i] = '9';
          f = 1;
        }
      }        
      CURSOR_OFF;
      LCD_position(position+i);
      _LCD_PRINTF("%c",temp_str[i]);
    }
    else if(button == CE)        // NO was pressed clear digit
    {             
      uint8 n;
      CURSOR_OFF;
      len = strlen(temp_str);
      for (n= i; n < len; n++ )
      {
        LCD_position(position + n);
       _LCD_PRINT(" ");
       temp_str[n] = ' ';
      } 
      if ( i > 0 ) 
      {
        i--;
      } 
      f = 0;      
      last_key = DFLT;        
    }   
    else if(button == YES)                   // YES pressed, Save file name 
    {   
      CURSOR_OFF;        
      for(i=0;i<10;i++)
      {
        *(file + i) = temp_str[i];
      }  
      break;
    }
    else if(button == ESC)
    {
      CURSOR_OFF;
      break;
    }
    else if(  (button == ENTER) || (button == UP) || (button == DOWN) )
    {
      last_key = DFLT;        
      f = 0;
      if( ((button == ENTER) || (button == DOWN))  && i != 9 )
      {
        if(temp_str[i+1] == 0)
        {
          temp_str[i+1] = ' ';
        }  
        i++;
      }
      else if((button == UP) && (i >0))
        i--;
    }
  }  
}
/*******************************************************************************
 *
 ** discover_secret_characters
 *
 *  PARAMETERS:None
 *
 *  DESCRIPTION:Unused function that can be used to determine the extended ascii
 *              characters of the LCD display driver, as they do not match the
 *              standard extended characters.
 *
 *  RETURNS:None
 *
 ******************************************************************************/ 
void discover_secret_characters(void)
{
  char ch = 0xDF;
  
  CURSOR_OFF;
  
  CLEAR_DISP;
  
  while(1)
  {
    LCD_position(LINE2);    
    wait_for_key_release();
    
    while(!Flags.button_pressed)
    {
        delay_ms(10);
    }    
    
    if(button == UP)  
    {
      ch ++;
    }    
    else if(button == DOWN)
    {
      ch--;
    }  
   
    sprintf( lcdstr, "char. %c dec. %u",ch, ch ) ; 
    LCD_print ( lcdstr ) ; 
  }
}

 

 
/*******************************************************************************
 *
 *  NAME: enterNumber()
 *
 *  PARAMETERS: String for number,LCD position,length of number
 *
 *  DESCRIPTION:  Accepts and displays user entered number.(The difference between 
 *                this function and enter_number is that this function will accept 
 *                numbers from left to right, the cursor advancing to the next 
 *                space, whereas enter_number has a stationary cursor with 
 *                entered numbers shifting to the left.
 *
 *  RETURNS:  Entered number as float
 *
 ******************************************************************************/
double enterNumber ( char *temp_str, int position, int length )
{
  uint8_t blanks, decimals, index, char_to_add;
  double num_temp;
  CURSOR_OFF;  
  
  // Write the existing number to the LCD
  LCD_position ( position );
  _LCD_PRINTF ( "%s", temp_str ); 
  
  
  index = 0;      // no characters have been entered.
  decimals = 0;   // No decial points have been entered. Only 1 is allowed
  
  do
  {
    // Set the cursor to blink at first char position
    LCD_position ( position + index );
    CURSOR_ON;
    
    while ( 1)
    // get a key from the keypad
    {
      button = getKey( TIME_DELAY_MAX ); 
      if ( button == ESC || button <= NINE || button == CE || button == UP || button == DOWN || button == ENTER ||  button == YES)
      {
        // If the number entry is at length size, wait unit proper key is hit.
        if ( index >= length )
        {
          if ( button == ESC || button == ENTER || button == CE )
          {
           break;
          }
        }  
        else 
        {
          break;
        }  
      }
    } 
    // return error code instead of value  
    if ( button == ESC )
    {
      CURSOR_OFF;
      return 999999.0;
    }
    
    // Exit when enter hit
    if ( button == ENTER )
    {
     break;
    }        
    
    if(button == YES)                          // ENTER pressed, save number           
    {      
      CURSOR_OFF; 
      num_temp = atof(temp_str);          
      return(num_temp);
    } 
    
    // add blanks to LCD after number is displayed
    // this is needed if the displayed number of digits gets smaller 
    // because of a CE hit, or this is the first time through the loop.
    // The first time through the loop causes the original number to be erased.
    blanks = strlen ( temp_str );
    
    if ( blanks > index )
    { 
      temp_str[index] = 0;
    };  

    
    if ( button == ZERO && decimals == 0 )
    {
      button = wait_for_key_zero_period();
      
    }
    
    if( button <= NINE  ) // digit was entered   
    {
      
      // add character to the string, and increment the string index
      append_char( temp_str, key_char ) ;
      index++;
          
      // clear the number
      LCD_PrintBlanksAtPosition ( blanks, position );
          
      // display the string ( it was blanked earlier )
      LCD_PrintAtPosition ( temp_str, position );
      
      
    }
    else if (( button == PERIOD ) && ( decimals == 0 ) )
    {
    
      // add character to the string, and increment the string index
      append_char( temp_str, key_char ) ;
      index++;
          
      // clear the number
      LCD_PrintBlanksAtPosition ( blanks, position );
          
      // display the string ( it was blanked earlier )
      LCD_PrintAtPosition ( temp_str, position );
      
      decimals++;                  // a decimal point has been entered
    }
    else if( (button == UP) || (button == DOWN) ) 
    {                                            // you must assign button numbers   
      // Only allowed if this is the first character entered
      if  ( index == 0 )
      { 
        if ( button == UP )
        { 
         char_to_add = '+';
        }
        else 
        {
         char_to_add = '-';
        }
        
        // add character to the string, and increment the string index
       append_char( temp_str, char_to_add ) ;
       index++;
          
       // clear the number
       LCD_PrintBlanksAtPosition ( blanks, position );
          
       // display the string ( it was blanked earlier )
       LCD_PrintAtPosition ( temp_str, position );   
      }     
        
     }
     else if ( button == CE )                          
     {
      // CE key was pressed, clear last digit entered   
      if ( index >= 1 )                               
      {
          index--;
      }    
      
      // If '.' is being erased, then allow decimal points to be entered again
      if ( temp_str[index] == '.' )
      {
        decimals--;
      }
     
       // remove last digit from string
      temp_str[index] = 0;                                                   
     
       // clear the number
       LCD_PrintBlanksAtPosition ( blanks, position );
          
       // display the string ( it was blanked earlier )
       LCD_PrintAtPosition ( temp_str, position );  
     }    
   }  while ( 1 );

   CURSOR_OFF;  
   return atof ( temp_str);
}

