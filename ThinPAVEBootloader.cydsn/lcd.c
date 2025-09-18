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

#include "project.h"


/*---------------------------------------------------------------------------*/
/*-----------------------[  Module Global Variables  ]-----------------------*/
/*---------------------------------------------------------------------------*/

unsigned int LCD_timeout,LCD_timer=0;

uint8 LCD_Char_enableState = 0u;

uint8 LCD_Char_initVar = 0u;

/*---------------------------------------------------------------------------*/
/*----------------------------[  define macros ]-----------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------[  Global Macros   ]----------------------------*/
/*---------------------------------------------------------------------------*/
#define     CLEAR_DISP   clearlcd()       //
#define     CURSOR_OFF   cursor_off()     //  Cursor off no blink
#define     CURSOR_ON    cursor_on()      //  Cursor on and blinking


/*---------------------------------------------------------------------------*/
/*----------------------[  Global Function Prototypes  ]---------------------*/
/*---------------------------------------------------------------------------*/
  
#define USE_4_LINE_DISPLAY  

/* Custom Character References */
#define LCD_Char_CUSTOM_0                 (0x00u)
#define LCD_Char_CUSTOM_1                 (0x01u)
#define LCD_Char_CUSTOM_2                 (0x02u)
#define LCD_Char_CUSTOM_3                 (0x03u)
#define LCD_Char_CUSTOM_4                 (0x04u)
#define LCD_Char_CUSTOM_5                 (0x05u)
#define LCD_Char_CUSTOM_6                 (0x06u)
#define LCD_Char_CUSTOM_7                 (0x07u)
  
#define LCD_UP    LCD_Char_CUSTOM_0                
#define LCD_DOWN  LCD_Char_CUSTOM_1                
#define LCD_LEFT  LCD_Char_CUSTOM_2                
#define LCD_RIGHT LCD_Char_CUSTOM_3               
  
#ifdef USE_4_LINE_DISPLAY
	#define NUMBER_CHARS  20
	#define NUMBER_LINES  4
	#define LINE1         0
  #define LINE2 				20
	#define LINE3					40
	#define LINE4					60

  #define LINE_LENGTH 20
  #define NUMBER_CHARS1    	NUMBER_CHARS+1 
#endif
  

/***************************************
*           API Constants
***************************************/

/* Full Byte Commands Sent as Two Nibbles */
#define LCD_NChar_DISPLAY_8_BIT_INIT       (0x03u)
#define LCD_NChar_DISPLAY_4_BIT_INIT       (0x02u)
#define LCD_NChar_DISPLAY_CURSOR_OFF       (0x08u)
#define LCD_NChar_CLEAR_DISPLAY            (0x01u)
#define LCD_NChar_CURSOR_AUTO_INCR_ON      (0x06u)
#define LCD_NChar_DISPLAY_CURSOR_ON        (0x0Eu)
#define LCD_NChar_DISPLAY_2_LINES_5x10     (0x2Cu)
#define LCD_NChar_DISPLAY_ON_CURSOR_OFF    (0x0Cu)

#define LCD_NChar_RESET_CURSOR_POSITION    (0x03u)
#define LCD_NChar_CURSOR_WINK              (0x0Du)
#define LCD_NChar_CURSOR_BLINK             (0x0Fu)
#define LCD_NChar_CURSOR_SH_LEFT           (0x10u)
#define LCD_NChar_CURSOR_SH_RIGHT          (0x14u)
#define LCD_NChar_DISPLAY_SCRL_LEFT        (0x18u)
#define LCD_NChar_DISPLAY_SCRL_RIGHT       (0x1Eu)
#define LCD_NChar_CURSOR_HOME              (0x02u)
#define LCD_NChar_CURSOR_LEFT              (0x04u)
#define LCD_NChar_CURSOR_RIGHT             (0x06u)

/* Point to Character Generator Ram 0 */
#define LCD_NChar_CGRAM_0                  (0x40u)

/* Point to Display Data Ram 0 */
#define LCD_NChar_DDRAM_0                  (0x80u)

/* LCD Characteristics */
#define LCD_NChar_CHARACTER_WIDTH          (0x05u)
#define LCD_NChar_CHARACTER_HEIGHT         (0x08u)


    #define LCD_NChar_NUMBER_OF_REMAINDERS_U32 (0x0Au)
    #define LCD_NChar_TEN                      (0x0Au)
    #define LCD_NChar_8_BIT_SHIFT              (8u)
    #define LCD_NChar_32_BIT_SHIFT             (32u)
    #define LCD_NChar_ZERO_CHAR_ASCII          (48u)


/* Nibble Offset and Mask */
#define LCD_NChar_NIBBLE_SHIFT             (0x04u)
#define LCD_NChar_NIBBLE_MASK              (0x0Fu)

/* LCD Module Address Constants */
#define LCD_NChar_ROW_0_START              (0x80u)
#define LCD_NChar_ROW_1_START              (0xC0u)
#define LCD_NChar_ROW_2_START              (0x94u)
#define LCD_NChar_ROW_3_START              (0xD4u)

/* Custom Character References */
#define LCD_NChar_CUSTOM_0                 (0x00u)
#define LCD_NChar_CUSTOM_1                 (0x01u)
#define LCD_NChar_CUSTOM_2                 (0x02u)
#define LCD_NChar_CUSTOM_3                 (0x03u)
#define LCD_NChar_CUSTOM_4                 (0x04u)
#define LCD_NChar_CUSTOM_5                 (0x05u)
#define LCD_NChar_CUSTOM_6                 (0x06u)
#define LCD_NChar_CUSTOM_7                 (0x07u)

/* Other constants */
#define LCD_NChar_BYTE_UPPER_NIBBLE_SHIFT  (0x04u)
#define LCD_NChar_BYTE_LOWER_NIBBLE_MASK   (0x0Fu)
#define LCD_NChar_U16_UPPER_BYTE_SHIFT     (0x08u)
#define LCD_NChar_U16_LOWER_BYTE_MASK      (0xFFu)
#define LCD_NChar_CUSTOM_CHAR_SET_LEN      (0x40u)

#define LCD_NChar_LONGEST_CMD_US           (0x651u)
#define LCD_NChar_WAIT_CYCLE               (0x10u)
#define LCD_NChar_READY_DELAY              ((LCD_Char_LONGEST_CMD_US * 4u)/(LCD_Char_WAIT_CYCLE))

/* Clear Macro */
#define LCD_NChar_ClearDisplay() LCD_NChar_WriteControl(LCD_NChar_CLEAR_DISPLAY)

/* Off Macro */
#define LCD_NChar_DisplayOff() LCD_NChar_WriteControl(LCD_NChar_DISPLAY_CURSOR_OFF)

/* On Macro */
#define LCD_NChar_DisplayOn() LCD_NChar_WriteControl(LCD_NChar_DISPLAY_ON_CURSOR_OFF)

static void LCD_NChar_WrCntrlNib(uint8 nibble) ;
void LCD_NChar_WriteControl(uint8 cByte) ;
void LCD_NChar_WriteData(uint8 dByte) ;
static void LCD_NChar_WrDatNib(uint8 nibble) ;

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
   LCD_NChar_Position ( line,loc);
   printStringOnLCD ( string);

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
	
	LCD_NChar_Position(line,pos);
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
	
  LCD_NChar_Position(line,pos);
	
  printStringOnLCD(str);

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
void LCD_position ( char cX )
{
 uint8 row, col;
  
  row = cX / 20;
  col = cX - (20 * row);
  
  LCD_NChar_Position( row,col);
  //LCD_NChar_WriteControl(cX + 0x80);  //set DDRAM address
  
  LCD_NChar_Position( row,col);
  //LCD_NChar_WriteControl(cX + 0x80);  //set DDRAM address

}



