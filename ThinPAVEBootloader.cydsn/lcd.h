/* -------------------------------------------------------------------
 *
 * File:	Lcd.h
 *
 * Desc:	Declarations of all functions in Lcd.c which are needed
 *			outside of that file.
 *
 * ------------------------------------------------------------------- */

#ifndef LCDH
#define LCDH
#include <device.h> 
  
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



void LCD_NChar_Init(void) ;
void LCD_NChar_Enable(void) ;
void LCD_NChar_Start(void) ;
void LCD_NChar_Stop(void) ;
void LCD_NChar_WriteControl(uint8 cByte) ;
void LCD_NChar_WriteData(uint8 dByte, uint8 wright) ;
void LCD_NChar_PrintString(char8 const string[]) ;
void LCD_NChar_Position(uint8 row, uint8 column) ;
void LCD_NChar_PutChar(char8 character) ;
void LCD_NChar_IsReady(void) ;
void LCD_NChar_SaveConfig(void) ;
void LCD_NChar_RestoreConfig(void) ;



/* Clear Macro */
#define LCD_NChar_ClearDisplay() LCD_NChar_WriteControl(LCD_NChar_CLEAR_DISPLAY)

/* Off Macro */
#define LCD_NChar_DisplayOff() LCD_NChar_WriteControl(LCD_NChar_DISPLAY_CURSOR_OFF)

/* On Macro */
#define LCD_NChar_DisplayOn() LCD_NChar_WriteControl(LCD_NChar_DISPLAY_ON_CURSOR_OFF)



typedef char *STRING;


  
  
/* initializes the LCD */
void init_lcd( void );


void gotoloc(int LineNum, int x);
void clearlcd ( void );
void printOnLCDLineLocString ( uint8_t line, uint8_t loc, char * string );
void printStringOnLCD ( char * string );
void printLCDInteger ( uint16_t number );
void printCharOnLCD ( char cdata );

uint8_t centerStart( uint8_t stringLength );

void cursor_on_blink(void);
void cursor_off(void);
void cursor_on(void);
uint8 LCD_ReadDDRAM(char *buf, int32 num,uint8 address);
void printAtPosLCD(int8 pos, char *str);
void DisplayStrCentered(uint8 line, char *str);
void PutCharPosLCD(int8 pos, uint8 ch);

#define CURSOR_OFF  cursor_off()
#define CURSOR_ON   cursor_on()
#define CLEAR_DISP  clearlcd();



#endif
