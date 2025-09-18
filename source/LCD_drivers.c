/******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  LCD_drivers.c
 *  Originating Author:  Chris Churavy
 *       Creation Date:  10/21/2010 
 *
 ******************************************************************************/
 
 /*---------------------------------------------------------------------------*/
/*---------------------------[  Revision History  ]--------------------------*/ 
/*---------------------------------------------------------------------------*/ 
/*
 *  when?       who?    what?
 *  ----------- ------- ------------------------------------------------------ 
 *  10/21/10    CC      Original 
 *
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/ 
/*----------------------------[  Include Files  ]----------------------------*/ 
/*---------------------------------------------------------------------------*/

#include "LCD_drivers.h"
#include "prompts.h"
#include "Keypad_functions.h"
#include "Interrupts.h"

/*---------------------------------------------------------------------------*/
/*-----------------------[  Module Global Variables  ]-----------------------*/
/*---------------------------------------------------------------------------*/

unsigned int LCD_timeout,LCD_timer=0;

/*---------------------------------------------------------------------------*/
/*----------------------------[  define macros ]-----------------------------*/
/*---------------------------------------------------------------------------*/


/* Stores state of component. Indicates whether component is or not
* in enable state.
*/
uint8 LCD_Char_enableState = 0u;

uint8 LCD_Char_initVar = 0u;

// The schematic component must be called LCD_Char
// The original LCD_Char functions have been moved to LCD-c and renames LCD_NChar functions.
// All calls to an LCD_Char function from a project function now use LCD_NChar function calls.
// This allows customized low level LCD functions. For example, the delay times are increased.
//


/* useful LCD control words. */
#define INIT_LCD          0x38
#define CLEAR_LCD         0x01
#define AUTO_CURSOR_RIGHT 0x06
#define NOCURSOR_NOBLINK  0x0C
#define CURSOR_NOBLINK    0x0E
#define CURSOR_BLINK      0x0F
#define CURSOR_LEFT       0x80
#define DISP_OFF          0x08
#define DISP_ON           0x04

// HD44780 LCD controller command set (do not modify these)
// writing:
#define LCD_CLR              0     // DB0: clear display
#define LCD_HOME             1     // DB1: return to home position
#define LCD_ENTRY_MODE       2     // DB2: set entry mode
#define LCD_ENTRY_INC        1     // DB1: increment
#define LCD_ENTRY_SHIFT      0     // DB2: shift
#define LCD_ON_CTRL          3     // DB3: turn lcd/cursor on
#define LCD_ON_DISPLAY       2     // DB2: turn display on
#define LCD_ON_CURSOR        1     // DB1: turn cursor on
#define LCD_ON_BLINK         0     // DB0: blinking cursor
#define LCD_MOVE             4     // DB4: move cursor/display
#define LCD_MOVE_DISP        3     // DB3: move display (0-> move cursor)
#define LCD_MOVE_RIGHT       2     // DB2: move right (0-> left)
#define LCD_FUNCTION         5     // DB5: function set
#define LCD_FUNCTION_8BIT    4     // DB4: set 8BIT mode (0->4BIT mode)
#define LCD_FUNCTION_2LINES  3     // DB3: two lines (0->one line)
#define LCD_FUNCTION_10DOTS  2     // DB2: 5x10 font (0->5x7 font)
#define LCD_CGRAM            6     // DB6: set CG RAM address
#define LCD_DDRAM            7     // DB7: set DD RAM address
#define LCD_BUSY             7     // DB7: LCD is busy

// Default LCD setup
// this default setup is loaded on LCD initialization
#define LCD_FDEF_1           (1<<LCD_FUNCTION_8BIT)
#define LCD_FDEF_2           (1<<LCD_FUNCTION_2LINES)
#define LCD_FUNCTION_DEFAULT ((1<<LCD_FUNCTION) | LCD_FDEF_1 | LCD_FDEF_2)
#define LCD_MODE_DEFAULT     ((1<<LCD NTRY_MODE) | (1<<LCD NTRY_INC))


/* Point to Character Generator Ram 0 */
#define LCD_Char_CGRAM_0                  (0x40u)

/* Point to Display Data Ram 0 */
#define LCD_Char_DDRAM_0                  (0x80u)

#define LCD_Char_CUSTOM_CHAR_SET_LEN      (0x40u)
//xSemaphoreHandle SemaLCD;

/* LCD line starting locations for 4 lines X 20 char/line.*/
const char LCD_Lines[] = { 0x00, 0x40, 0x14, 0x54 };

/* Blanks for clear line routine. */
const char blanks[] = { "                    " };


char shadow[NUMBER_LINES][NUMBER_CHARS];

static void LCD_NChar_PrintU32Number(uint32 value) ;
static void LCD_NChar_WrDatNib(uint8 nibble) ;
static void LCD_NChar_WrCntrlNib(uint8 nibble) ;

#define LCD_NChar_PrintNumber(value) LCD_NChar_PrintU32Number((uint16) (value))

uint8 const CYCODE LCD_Char_customFonts[] = \
{
    /* Character LCD_Char_CUSTOM_0   */
        0x04u,    0x0Eu,    0x1Fu,    0x04u,    0x04u,    0x04u,    0x04u,    0x04u, \
    /* Character LCD_Char_CUSTOM_1   */
        0x04u,    0x04u,    0x04u,    0x04u,    0x04u,    0x1Fu,    0x0Eu,    0x04u, \
    /* Character LCD_Char_CUSTOM_2   */
        0x00u,    0x04u,    0x0Cu,    0x1Fu,    0x0Cu,    0x04u,    0x00u,    0x00u, \
    /* Character LCD_Char_CUSTOM_3   */
        0x00u,    0x04u,    0x06u,    0x1Fu,    0x06u,    0x04u,    0x00u,    0x00u, \
    /* Character LCD_Char_CUSTOM_4   */
        0x00u,    0x0Eu,    0x08u,    0x0Cu,    0x08u,    0x0Eu,    0x00u,    0x00u, \
    /* Character LCD_Char_CUSTOM_5   */
        0x00u,    0x0Eu,    0x08u,    0x0Eu,    0x02u,    0x0Eu,    0x00u,    0x00u, \
    /* Character LCD_Char_CUSTOM_6   */
        0x00u,    0x0Eu,    0x08u,    0x0Eu,    0x02u,    0x0Eu,    0x00u,    0x00u, \
    /* Character LCD_Char_CUSTOM_7   */
        0x00u,    0x04u,    0x04u,    0x04u,    0x00u,    0x04u,    0x00u,    0x00u \
 };


/*******************************************************************************
* Function Name: LCD_Char_LoadCustomFonts
********************************************************************************
*
* Summary:
*  Loads 8 custom font characters into the LCD Module for use.  Cannot use
*  characters from two different font sets at once, but font sets can be
*  switched out during runtime.
*
* Parameters:
*  customData:  pointer to a constant array of 64 bytes representing 8 custom
*               font characters.
* Return:
*  None.
*
* Theory:
*  Prior to using this function user need to import the pointer to custom
*  font array to your project by writting the following in the source code file
*  where custom font will be used:
*       extern uint8 const CYCODE LCD_Char_customFonts[];
*  This function is not automatically called by the Start() routine and must be
*  called manually by the user.
*
*******************************************************************************/
void LCD_NChar_LoadCustomFonts(uint8 const customData[]) 
{
    uint8 indexU8;

    /* Set starting address in the LCD Module.
    * Optionally: Read the current address to restore at a later time.
    */
    LCD_NChar_WriteControl(LCD_Char_CGRAM_0);

    /* Load in the 64 bytes of CustomChar Data */
    for(indexU8 = 0u; indexU8 < LCD_Char_CUSTOM_CHAR_SET_LEN; indexU8++)
    {
        LCD_NChar_WriteData(customData[indexU8]);
    }

  
    LCD_NChar_WriteControl(LCD_Char_DDRAM_0);
}

/*******************************************************************************
* Function Name: LCD_NChar_Init
********************************************************************************
*
* Summary:
*  Performs initialization required for the components normal work.
*  This function initializes the LCD hardware module as follows:
*        Enables a 4-bit interface
*        Clears the display
*        Enables the auto cursor increment
*        Resets the cursor to start position
*  Also, it loads a custom character set to the LCD if it was defined in the customizer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LCD_NChar_Init(void) 
{
    /* INIT CODE */
    CyDelay(80u);                                                        /* Delay 40 ms */
    LCD_NChar_WrCntrlNib(LCD_NChar_DISPLAY_8_BIT_INIT);    /* Selects 8-bit mode */
    CyDelay(10u);                                                         /* Delay 5 ms */
    LCD_NChar_WrCntrlNib(LCD_NChar_DISPLAY_8_BIT_INIT);    /* Selects 8-bit mode */
    CyDelay(25u);                                                        /* Delay 15 ms */
    LCD_NChar_WrCntrlNib(LCD_NChar_DISPLAY_8_BIT_INIT);    /* Selects 8-bit mode */
    CyDelay(2u);                                                         /* Delay 1 ms */
    LCD_NChar_WrCntrlNib(LCD_NChar_DISPLAY_4_BIT_INIT);    /* Selects 4-bit mode */
    CyDelay(10u);                                                         /* Delay 5 ms */

    LCD_NChar_WriteControl(LCD_NChar_CURSOR_AUTO_INCR_ON);    /* Incr Cursor After Writes */
    LCD_NChar_WriteControl(LCD_NChar_DISPLAY_CURSOR_ON);      /* Turn Display, Cursor ON */
    LCD_NChar_WriteControl(LCD_NChar_DISPLAY_2_LINES_5x10);   /* 2 Lines by 5x10 Characters */
    LCD_NChar_WriteControl(LCD_NChar_DISPLAY_CURSOR_OFF);     /* Turn Display, Cursor OFF */
    LCD_NChar_WriteControl(LCD_NChar_CLEAR_DISPLAY);          /* Clear LCD Screen */
    LCD_NChar_WriteControl(LCD_NChar_DISPLAY_ON_CURSOR_OFF);  /* Turn Display ON, Cursor OFF */
    LCD_NChar_WriteControl(LCD_NChar_RESET_CURSOR_POSITION);  /* Set Cursor to 0,0 */
    CyDelay(10u);
   
    LCD_NChar_LoadCustomFonts(LCD_Char_customFonts);
   
}


/*******************************************************************************
* Function Name: LCD_Char_Enable
********************************************************************************
*
* Summary:
*  Turns on the display.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
* Theory:
*  This function has no effect when it is called the first time as
*  LCD_Char_Init() turns on the LCD.
*
*******************************************************************************/
void LCD_NChar_Enable(void) 
{
    LCD_NChar_DisplayOn();
    LCD_Char_enableState = 1u;
}


/*******************************************************************************
* Function Name: LCD_NChar_Start
********************************************************************************
*
* Summary:
*  Performs initialization required for the components normal work.
*  This function initializes the LCD hardware module as follows:
*        Enables 4-bit interface
*        Clears the display
*        Enables auto cursor increment
*        Resets the cursor to start position
*  Also, it loads a custom character set to the LCD if it was defined in the customizer.
*  If it was not the first call in this project, then it just turns on the
*  display
*
*
* Parameters:
*  LCD_Char_initVar - global variable.
*
* Return:
*  LCD_Char_initVar - global variable.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LCD_NChar_Start(void) 
{
    /* If not initialized, perform initialization */
    if(LCD_Char_initVar == 0u)
    {
        LCD_NChar_Init();
        LCD_Char_initVar = 1u;
    }

    /* Turn on the LCD */
    LCD_NChar_Enable();
}


/*******************************************************************************
* Function Name: LCD_Char_Stop
********************************************************************************
*
* Summary:
*  Turns off the display of the LCD screen.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LCD_NChar_Stop(void) 
{
    /* Calls LCD Off Macro */
    LCD_NChar_DisplayOff();
    LCD_Char_enableState = 0u;
}


/*******************************************************************************
*  Function Name: LCD_NChar_Position
********************************************************************************
*
* Summary:
*  Moves the active cursor location to a point specified by the input arguments
*
* Parameters:
*  row:    Specific row of LCD module to be written
*  column: Column of LCD module to be written
*
* Return:
*  None.
*
* Note:
*  This only applies for LCD displays that use the 2X40 address mode.
*  In this case Row 2 starts with a 0x28 offset from Row 1.
*  When there are more than 2 rows, each row must be fewer than 20 characters.
*
*******************************************************************************/
void LCD_NChar_Position(uint8 row, uint8 column) 
{
    switch (row)
    {
        case (uint8)0:
            LCD_NChar_WriteControl(LCD_NChar_ROW_0_START + column);
            break;
        case (uint8) 1:
            LCD_NChar_WriteControl(LCD_NChar_ROW_1_START + column);
            break;
        case (uint8) 2:
            LCD_NChar_WriteControl(LCD_NChar_ROW_2_START + column);
            break;
        case (uint8) 3:
            LCD_NChar_WriteControl(LCD_NChar_ROW_3_START + column);
            break;
        default:
            /* if default case is hit, invalid row argument was passed.*/
            break;
    }
}


/*******************************************************************************
* Function Name: LCD_NChar_PrintString
********************************************************************************
*
* Summary:
*  Writes a zero terminated string to the LCD.
*
* Parameters:
*  string: Pointer to head of char8 array to be written to the LCD module
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_NChar_PrintString(char8 const string[]) 
{
    uint8 indexU8 = 1u;
    char8 current = *string;

    /* Until null is reached, print next character */
    while((char8) '\0' != current)
    {
        LCD_NChar_WriteData((uint8)current);
        current = string[indexU8];
        indexU8++;
    }
}


/*******************************************************************************
*  Function Name: LCD_NChar_PutChar
********************************************************************************
*
* Summary:
*  Writes a single character to the current cursor position of the LCD module.
*  Custom character names (_CUSTOM_0 through
*  _CUSTOM_7) are acceptable as inputs.
*
* Parameters:
*  character: Character to be written to LCD
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_NChar_PutChar(char8 character) 
{
    LCD_NChar_WriteData((uint8)character);
}


/*******************************************************************************
*  Function Name: LCD_NChar_WriteData
********************************************************************************
*
* Summary:
*  Writes a data byte to the LCD module's Data Display RAM.
*
* Parameters:
*  dByte: Byte to be written to the LCD module
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_NChar_WriteData(uint8 dByte) 
{
    uint8 nibble;
  
    nibble = dByte >> LCD_NChar_NIBBLE_SHIFT;

    /* Write high nibble */
    LCD_NChar_WrDatNib(nibble);

    nibble = dByte & LCD_NChar_NIBBLE_MASK;
    /* Write low nibble */
    LCD_NChar_WrDatNib(nibble);
}


/*******************************************************************************
*  Function Name: LCD_NChar_WriteControl
********************************************************************************
*
* Summary:
*  Writes a command byte to the LCD module.
*
* Parameters:
*  cByte:  The byte to be written to theLCD module
* 
* Return:
*  None.
*
*******************************************************************************/
void LCD_NChar_WriteControl(uint8 cByte) 
{
    uint8 nibble;

    nibble = cByte >> LCD_NChar_NIBBLE_SHIFT;
    
    /* WrCntrlNib(High Nibble) */
    LCD_NChar_WrCntrlNib(nibble);
    nibble = cByte & LCD_NChar_NIBBLE_MASK;

    /* WrCntrlNib(Low Nibble) */
    LCD_NChar_WrCntrlNib(nibble);
}



/*******************************************************************************
*  Function Name: LCD_NChar_WrDatNib
********************************************************************************
*
* Summary:
*  Writes a data nibble to the LCD module.
*
* Parameters:
*  nibble:  Byte containing nibble in the least significant nibble to be
*           written to the LCD module.
*
* Return:
*  None.
*
*******************************************************************************/
static void LCD_NChar_WrDatNib(uint8 nibble) 
{
 
  /* RS should be high to select data register */
  LCDRS_Write ( 1 );

 /* Guaranteed delay between Setting RS and RW and setting E bits */
 CyDelayUs(20);
    
 // set data pins, and enable pins as outputs
 LCD_Reg_Write ( 0x10 | nibble );
 CyDelayUs(10u);
 
  CyGlobalIntDisable;
  /* Write data, bring E high */
  LCDEN_Write ( 1 ); 
  
  CyDelayUs(100u);

  /* bring E low */
  LCDEN_Write ( 0 ); 
  CyGlobalIntEnable;
  CyDelayUs(500);
  
  LCDRS_Write ( 0 );
  LCD_Reg_Write ( 0x00  );
  CyDelayUs(20u);
}


/*******************************************************************************
*  Function Name: LCD_NChar_WrCntrlNib
********************************************************************************
*
* Summary:
*  Writes a control nibble to the LCD module.
*
* Parameters:
*  nibble: The byte containing a nibble in the four least significant bits.????
*
* Return:
*  None.
*
*******************************************************************************/
static void LCD_NChar_WrCntrlNib(uint8 nibble) 
{

  /* RS should be high to select control register */
  LCDRS_Write ( 0 );

 /* Guaranteed delay between Setting RS and RW and setting E bits */
 CyDelayUs(10u);
    
 // set data pins, and enable pins as outputs
 LCD_Reg_Write ( 0x10 | nibble );
 CyDelayUs(10);
  
 CyGlobalIntDisable;
  /* Write data, bring E high */
  LCDEN_Write ( 1 ); 
  
  CyDelayUs(100u);

  /* bring E low */
  LCDEN_Write ( 0 ); 
  CyGlobalIntEnable; 
  CyDelayUs ( 2000 );
  
  LCDRS_Write ( 0 );
  LCD_Reg_Write ( 0x00 );
  CyDelayUs ( 10 );
}


    /*******************************************************************************
    *  Function Name: LCD_Char_PrintInt8
    ********************************************************************************
    *
    * Summary:
    *  Print a byte as two ASCII characters.
    *
    * Parameters:
    *  value: The byte to be printed out as ASCII characters.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void LCD_NChar_PrintInt8(uint8 value) 
    {
        static char8 const CYCODE LCD_Char_hex[16u] = "0123456789ABCDEF";
        
        LCD_NChar_PutChar((char8) LCD_Char_hex[value >> LCD_NChar_BYTE_UPPER_NIBBLE_SHIFT]);
        LCD_NChar_PutChar((char8) LCD_Char_hex[value & LCD_NChar_BYTE_LOWER_NIBBLE_MASK]);
    }


    /*******************************************************************************
    *  Function Name: LCD_NChar_PrintInt16
    ********************************************************************************
    *
    * Summary:
    *  Print a uint16 as four ASCII characters.
    *
    * Parameters:
    *  value: The uint16 to be printed out as ASCII characters.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void LCD_NChar_PrintInt16(uint16 value) 
    {
        LCD_NChar_PrintInt8((uint8)(value >> LCD_NChar_U16_UPPER_BYTE_SHIFT));
        LCD_NChar_PrintInt8((uint8)(value & LCD_NChar_U16_LOWER_BYTE_MASK));
    }

    
    /*******************************************************************************
    *  Function Name: LCD_NChar_PrintInt32
    ********************************************************************************
    *
    * Summary:
    *  Print a uint32 hexadecimal number as eight ASCII characters.
    *
    * Parameters:
    *  value: The uint32 to be printed out as ASCII characters.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void LCD_NChar_PrintInt32(uint32 value) 
    {
        uint8 shift = LCD_NChar_32_BIT_SHIFT;
        
        while (shift != 0u)
        {
            /* "shift" var is to be subtracted by 8 prior shifting. This implements 
            * shifting by 24, 16, 8 and 0u. 
            */
            shift -= LCD_NChar_8_BIT_SHIFT;
            
            /* Print 8 bits of uint32 hex number */
            LCD_NChar_PrintInt8((uint8) ((uint32) (value >> shift)));
        }
    }
    
    
    /*******************************************************************************
    *  Function Name: LCD_NChar_PrintNumber
    ********************************************************************************
    *
    * Summary:
    *  Print an uint16 value as a left-justified decimal value.
    *
    * Parameters:
    *  value: A 16-bit value to be printed in ASCII characters as a decimal number
    *
    * Return:
    *  None.
    *
    * Note:
    *  This function is implemented as a macro.
    *
    *******************************************************************************/

    
    /*******************************************************************************
    *  Function Name: LCD_NChar_PrintU32Number
    ********************************************************************************
    *
    * Summary:
    *  Print an uint32 value as a left-justified decimal value.
    *
    * Parameters:
    *  value: A 32-bit value to be printed in ASCII characters as a decimal number
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void LCD_NChar_PrintU32Number(uint32 value) 
    {
        uint8 tmpDigit;
        char8 number[LCD_NChar_NUMBER_OF_REMAINDERS_U32 + 1u];
        uint8 digIndex = LCD_NChar_NUMBER_OF_REMAINDERS_U32;
        
        /* This API will output a decimal number as a string and that string will be 
        * filled from end to start. Set Null termination character first.
        */
        number[digIndex] = (char8) '\0';
        digIndex--;
        
        /* Load these in reverse order */
        while(value >= LCD_NChar_TEN)
        {
            /* Extract decimal digit, indexed by 'digIndex', from 'value' and
            * convert it to ASCII character.
            */
            tmpDigit = (uint8) (((uint8) (value % LCD_NChar_TEN)) + (uint8) LCD_NChar_ZERO_CHAR_ASCII);
            
            /* Temporary variable 'tmpDigit' is used to avoid Violation of MISRA rule 
            * #10.3.
            */
            number[digIndex] = (char8) tmpDigit;
            value /= LCD_NChar_TEN;
            digIndex--;
        }
        
        /* Extract last decimal digit 'digIndex', from the 'value' and convert it
        * to ASCII character.
        */
        tmpDigit = (uint8) (((uint8)(value % LCD_NChar_TEN)) + (uint8) LCD_NChar_ZERO_CHAR_ASCII);
        number[digIndex] = (char8) tmpDigit;

        /* Print out number */
        LCD_NChar_PrintString(&number[digIndex]);
    }
    


struct tableLoc
{
 	int limitLoc[10];
 	int percent[10];

} TABLE_LOC = { { 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
 				  { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 }
              };

struct tableLoc  *tableLoc_ptr;

			
/*
 *  FUNCTION: lcdDelay
 *
 *  PARAMETERS: int delay
 *
 *  DESCRIPTION:
 *
 *  RETURNS: None
 *
 */
void lcdDelay(int delay)
{
	CyDelay (delay);
}


/*
 *  FUNCTION: gotoloc
 *
 *  PARAMETERS: int LineNum, int x
 *
 *  DESCRIPTION: gotoloc(LineNum,x) sets the cursor position to the specified line number,
 *               x positions in (starting at 0). x is the offset from the starting address.
 *               0x80 is the code signifying that this is a data address.
 *
 *  RETURNS: None
 *
 */

void gotoloc(int LineNum, int x)
{

	LCD_NChar_Position(LineNum,x);

}

/*
 *  FUNCTION: clearlcd
 *
 *  PARAMETERS:	None
 *
 *  DESCRIPTION: Used to clear LCD by overwriting all four lines.
 *
 *  RETURNS: None
 *
 */

void clearlcd(void)
{

  LCD_NChar_WriteControl(LCD_NChar_CLEAR_DISPLAY);
}


/*
 *  FUNCTION: cursor_on
 *
 *  PARAMETERS:	None
 *
 *  DESCRIPTION:
 *
 *  RETURNS: None
 *
 */

void cursor_on(void)
{
  LCD_NChar_WriteControl(LCD_NChar_DISPLAY_CURSOR_ON);
}


/*
 *  FUNCTION: cursor_off
 *
 *  PARAMETERS:	None
 *
 *  DESCRIPTION:
 *
 *  RETURNS: None
 *
 */
void cursor_off(void)
{
  LCD_NChar_WriteControl(LCD_NChar_DISPLAY_ON_CURSOR_OFF);
}


/*
 *  FUNCTION: cursor_on_blink
 *
 *  PARAMETERS:	None
 *
 *  DESCRIPTION:
 *
 *  RETURNS: None
 *
 */

void cursor_on_blink(void)
{
  LCD_NChar_WriteControl(LCD_NChar_CURSOR_BLINK);
}


void printCharOnLCD ( char cdata )
{
    LCD_NChar_PutChar ( cdata );
}


void printStringOnLCD ( char * string )
{
		if ( strlen ( string ) < 21 )
    {
      LCD_NChar_PrintString ( string );
    }
    else
    { 
      string[20] = 0; 
      LCD_NChar_PrintString ( string );
    }
  
}

void printOnLCDLineLocString ( uint8_t line, uint8_t loc, char * string )
{
   gotoloc ( line,loc);
   printStringOnLCD ( string);

}

void printLCDInteger ( uint16_t number )
{

  LCD_NChar_PrintNumber ( number );

}

/*******************************************************************************
* Function Name: PutCharPos LCD
********************************************************************************
* Summary: Enter a number from the key pads (5 digit max) positive only
*          
* Parameters:  position to print at, string to print (pos=0 to 31, line1=0-15,line2=16-31)
* Return: Number entered
*******************************************************************************/
void PutCharPosLCD(int8 pos, uint8 ch)
{

	int8 line;
	line = pos / NUMBER_CHARS;
	pos = pos % NUMBER_CHARS;
	shadow[line][pos] = ch;
	gotoloc(line,pos);
	printCharOnLCD(ch);
	
}

/*******************************************************************************
* Function Name: printAtPos LCD
********************************************************************************
* 
*          
* Parameters:  position to print at, string to print (pos=0 to 31, line1=0-15,line2=16-31)
* Return: Number entered
*******************************************************************************/
void printAtPosLCD(int8 pos, char *str)
{
	int8 line;
	
  if ( strlen(str) > NUMBER_CHARS)
  {
    str[NUMBER_CHARS] = 0;
  }  
	line = pos / NUMBER_CHARS;
	pos = pos % NUMBER_CHARS;
	
  memcpy(&shadow[line + pos],str,strlen(str));
	
  gotoloc(line,pos);
	
  printStringOnLCD(str);

}

/*******************************************************************************
* Function Name: DisplayStr Centered
********************************************************************************
* Summary: Display given string centered on 16 char display leaves extra space at end
*          
* Parameters:  char *str, line to print on
* Return: None
*******************************************************************************/
void DisplayStrCentered(uint8 line, char *str)
{
	char buf[NUMBER_CHARS1];
	int32 len;

	memset(buf,' ',NUMBER_CHARS);
	line = line / 20;
	line = line * 20;
	if(line > LINE4) line = LINE1; // default to first line
	if(str != 0) // if no string display blank line)
	{
		len = strlen(str);
		if(len > 0) // if 0 or less display blank line
		{
			if(len > NUMBER_CHARS) { len = NUMBER_CHARS; }
			memcpy(&buf[(NUMBER_CHARS - len) / 2],str,len);
		}
	}
	buf[NUMBER_CHARS] = '\0';
	if(strlen(buf) > 20)
	{
		printAtPosLCD(line,"Error Center");
		while(1);
	}
	printAtPosLCD(line,buf);
}

/*
 *  FUNCTION: displine
 *
 *  PARAMETERS: int linenum, LCDSTRING const *dispstring, int clearline
 *
 *  DESCRIPTION: displine places the string from dispstring on the lcd at the location
 *               given by lineloc. The clearline flag can erase the line before putting
 *               the string up. This allows the routine to be used fur updating a
 *               whole line or just adding a short string.
 *
 *  Note: displine expects a pointer to an LCDSTRING, not the LCDSTRING
 *        structure itself.
 *
 *  RETURNS: None
 *
 */

void displine(int linenum, LCDSTRING const *dispstring, int clearline)
{
	char lineloc;

	// First, if clearline, write blanks.
	if (clearline)
	{
		gotoloc(linenum, 0);    // Start at beginning of line.
	
		// Grab the LCD Semaphore
 	  printStringOnLCD( "                    " );

	}

	// now, write the line
	if( 10 == dispstring->lineloc )
	{
		lineloc = (char)floor(((19 - strlen(dispstring->linestring)) / 2 + 0.5));
		gotoloc( linenum, lineloc );

	}
	else
		gotoloc(linenum,dispstring->lineloc);  // Go to start of text.

   printStringOnLCD( dispstring->linestring);   // Print the string.


}



/*
 *  FUNCTION: displine_var
 *
 *  PARAMETERS: int linenum, LCDSTRING *dispstring, int clearline
 *
 *  DESCRIPTION: displine places the string from dispstring on the lcd at the location
 *               given by lineloc. The clearline flag can erase the line before putting
 *               the string up. This allows the routine to be used for updating a
 *               whole line or just adding a short string.
 *
 *               Note: displine expects a pointer to an LCDSTRING, not the LCDSTRING
 *                     structure itself.
 *
 *  RETURNS: None
 *
 */

void displine_var (int linenum, LCDSTRING *dispstring, int clearline)
{
	// First, if clearline, write blanks.
	if (clearline)
	{
		gotoloc(linenum, 0);                // Start at beginning of line.

	   printStringOnLCD( "                    " );              // Put 20 blanks.
	}

	gotoloc(linenum,dispstring->lineloc);  // Go to start of text.
  printStringOnLCD (dispstring->linestring); // Print the string.
  

}



/*
 *  FUNCTION: displine_chars
 *
 *  PARAMETERS: int linenum, LCDSTRING const * dispstring, int clearline
 *
 *  DESCRIPTION: displine places the string from dispstring on the lcd at the location
 *               given by lineloc. The clearline flag can erase the line before putting
 *               the string up. This allows the routine to be used fur updating a
 *               whole line or just adding a short string.
 *
 *               Note: displine expects a pointer to an LCDSTRING, not the LCDSTRING
 *               structure itself.
 *
 *  RETURNS: None
 *
 */

void displine_chars ( int linenum, LCDSTRING const * dispstring, int clearline )
{

	unsigned char n;
	char lineloc;

	// First, if clearline, write blanks.
	if ( clearline )
	{
		gotoloc ( linenum, 0 );

		printStringOnLCD ( "                    " );              // Put 20 blanks.
  }
	// now, write the line
	if( 10 == dispstring->lineloc )
	{
		lineloc = (char)floor(((19 - strlen(dispstring->linestring)) / 2 + 0.5));
		gotoloc( linenum, lineloc );

	}
	else
		gotoloc ( linenum, dispstring->lineloc );
	
	for ( n = 0; n < 20; n++ )
	{
		if ( 0x00 == dispstring->linestring[n] || '\n' == dispstring->linestring[n] )
			break;
		else
       printCharOnLCD( dispstring->linestring[n] );
   }

}


/*
 *  FUNCTION: dispscrn
 *
 *  PARAMETERS: LCDSTRING const * const scrn[]
 *
 *  DESCRIPTION: dispscrn is used to overwrite all four lines on the lcd. It receives a pointer
 *               to an array of LCDSTRING. There should be four strings in this array. Displine
 *               is called four times, starting at each line, clearing first with clearline.
 *
 *  RETURNS: None
 *
 */

void dispscrn(LCDSTRING const * const scrn[] )
{
	int  i;

  for ( i=0;i<4;i++)
	{
		// Note displine is expecting a pointer to an LCDSTRING.
		displine(i,scrn[i],1);	
								
	}

}

											
/*
 *  FUNCTION: init_lcd
 *
 *  PARAMETERS: None
 *
 *  DESCRIPTION: initializes the LCD to a known, usable state
 *
 *  RETURNS: None
 *
 */

void  init_lcd(void)
{
	//vSemaphoreCreateBinary(SemaLCD);
   	
	// initialize hardware
  LCD_NChar_Start();
}


/*
 *  FUNCTION: lcdControlWrite
 *
 *  PARAMETERS: unsigned char data
 *
 *  DESCRIPTION:
 *
 *  RETURNS: None
 *
 */

void lcdControlWrite(unsigned char data)
{

  LCD_NChar_WriteControl( data );

}

/*
 *  FUNCTION: lcdHome
 *
 *  PARAMETERS: None
 *
 *  DESCRIPTION:
 *
 *  RETURNS: None
 *
 */

void lcdHome(void)
{
	// move cursor to home
	lcdControlWrite(1<<LCD_HOME);
}


/*
 *  FUNCTION: lcdClear
 *
 *  PARAMETERS: None
 *
 *  DESCRIPTION:
 *
 *  RETURNS: None
 *
 */

void lcdClear(void)
{
	// clear LCD
	LCD_NChar_WriteControl(LCD_NChar_CLEAR_DISPLAY);
}

/******************************************************************************
 *
 ** LCD_position
 *
 *  PARAMETERS:LCD position (BYTE value indicating position)
 *
 *  DESCRIPTION:Sets position for that next LCD write will start at.
 *
 *  RETURNS:None
 *
 *****************************************************************************/
void LCD_position ( BYTE cX )
{
 uint8_t scan = 0;
 uint8 row, col;
  // turn off keycan timer if enabled
  if ( CHECK_KEYSCAN_ENABLE() == 1 )
  {
    KEYSCAN_DISABLE();
    scan = 1;
  }
  else
  {
    scan = 0;
  }
  
  row = cX / 20;
  col = cX - (20 * row);
  
  LCD_NChar_Position( row,col);
  //LCD_NChar_WriteControl(cX + 0x80);  //set DDRAM address

  // re-enable keyscan
  if ( 1 == scan )
  {
    KEYSCAN_ENABLE();
  } 
  
  LCD_NChar_Position( row,col);
  //LCD_NChar_WriteControl(cX + 0x80);  //set DDRAM address

}


/******************************************************************************
 *
 ** LCD_char
 *
 *  PARAMETERS:Character to be printed to LCD screen
 *
 *  DESCRIPTION:Prints character to LCD screen.
 *
 *  RETURNS:None
 *
 *****************************************************************************/
void LCD_char (char cX)
{
  uint8_t scan = 0;
 
  if ( CHECK_KEYSCAN_ENABLE() == 1 )
  {
    KEYSCAN_DISABLE();
    scan = 1;
  }
  else
  {
    scan = 0;
  }
    
  LCD_NChar_PutChar(cX);  

  
  if ( 1 == scan )
  {
    KEYSCAN_ENABLE();
  }
}


/******************************************************************************
 *
 ** LCD_print
 *
 *  PARAMETERS:String to be printed to LCD screen
 *
 *  DESCRIPTION:Prints string to LCD screen.
 *
 *  RETURNS:None
 *
 *****************************************************************************/
void LCD_print (char *buffer)
{
 char n;
 
 while(*buffer)                  // Write data to LCD up to null
 {
  n = *buffer;
      
  LCD_char(n); // Write character to LCD
  buffer++;               // Increment buffer
 }
 
  
}

void LCD_light_timer( uint16 timeout_seconds)  // LCD will turn off in timeout_seconds
{
  unsigned int timeout_sec;
  timeout_sec = timeout_seconds * 10;              // convert seconds for use in timer1 interrupt function
  LCD_timeout = timeout_sec;
  LCD_timer = 0;                      // reset LCD_timer                  
}
void LCD_PrintAtPosition ( char * buffer, uint8_t position )
{
 // set the position
 LCD_position ( position );
 // print the string
 LCD_print ( buffer );
}
void CtrStrNC(char *buf, uint8 line) { // constant strings cannot be chaged, copy it to a local buffer
    int i;
    char tmp[LINE_LENGTH + 1];      //LINE_LENGTH = 20 CHARACTERS MAX [19:0], 
    memcpy(tmp,buf,LINE_LENGTH);    //copy only the amount we can hold
    tmp[LINE_LENGTH] = '\0';        //[20] = '\0'; terminate the end
    //if the string is shorter it will have its own null at the appropriate spot
    i = strlen(tmp);
    i = (LINE_LENGTH - i) / 2;
    LCD_PrintAtPosition (tmp, line + i);
}
void CtrStrC(char *buf, uint8 line) {
    char Line[LINE_LENGTH + 1];         // max length of string + null
    memset(Line,' ',LINE_LENGTH);       // fill it with blanks
    Line[LINE_LENGTH] = '\0';           // terminate the end, LCD_PrintAtPosition does not check
    LCD_PrintAtPosition (Line, line);   // print the blank line
    CtrStrNC(buf,line);                 // center the new string
}
void LCD_PrintAtPositionCentered ( char * buffer, uint8_t line_position )
{
 uint8_t length;

 
 length = strlen ( buffer )/2;
 
 
 LCD_PrintAtPosition (  buffer, line_position - length );
}


void LCD_PrintBlanksAtPosition ( uint8_t length, uint8_t position )
{
   uint8_t n;
   
   // set the position
   LCD_position ( position );
  
   // FILL IN THE BLANKS.   Don't fill in more than 20
   for ( n = 0; (n < length) && ( n < 20 ); n++ )
   {
     LCD_char ( ' ' );
   }
   
}


/*
*  FUNCTION: centerStart
*
*  PARAMETERS: string length
*
*  DESCRIPTION: Calculates where a string should begin to center it
*
*  RETURNS: start location to center a string
*
*/

uint8_t centerStart( uint8_t stringLength )
{
  uint8_t startLocation;
  
  startLocation = (uint8_t)floor(((19 - stringLength)) / 2 + 0.5);
  
  return startLocation;
  
} // char centerStart( char stringLength )



void _LCD_PRINT ( char *lcdstr) 
{
 LCD_print ( lcdstr ) ;         
                      
}                          

