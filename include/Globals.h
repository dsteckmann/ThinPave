/******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  Globals.h
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
 *---------------------------------------------------------------------------*/

/*  If we haven't included this file already.... */
#ifndef GLOBALS_H
#define GLOBALS_H

#include <project.h>
#include "elite.h"
#include "DataTypes.h"  
#include <stdio.h>
#include <stddef.h>
#include "stdbool.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
 
/*----------------------------------------------------------------------------*/
/*-------------------------[   Global Constants   ]---------------------------*/
/*----------------------------------------------------------------------------*/
  
// VERSION

#define VERSION 1.05

#define DEL_SIG 1
   
#define MAX_PROJECTS    10
#define MAX_STATIONS    100
#define PROJ_NAME_LENGTH 16 

#define PASS  1
#define FAIL  0

#define OFF   0
#define ON    1

#define FALSE 0
#define TRUE  1
  
#define ENABLED	  1
#define DISABLED	0

#define INACTIVE  0
#define ACTIVE    1

#define NONE     0

#define DISPLAY_LENGTH  21


#define UNUSED      0
#define USED        1 
  
#define NO_ERROR 0
#define ERROR    1

#define     MANUAL 0                   // definitions for shutdown mode
#define     AUTO 1
#define     NICAD 0                    // definitions for battery selection    
#define     ALK   1
#define     ACCESS_CODE     4659       //num to access entering cal values
#define     ACCESS_CODE_2   2025       //num to access entering cal values  
#define     XON  0x11
#define     XOFF 0x13


#define TEST_MODE 1
#define AUTO_TEST 0


#define MIN_THICKNESS 12.70
#define MAX_THICKNESS 102  
  
#define PCF     0
#define KG_M3   1
#define GM_CC   2

#define KG_UNITS   0
#define GCC_UNITS  1

#define KG_PCF_CONV  0.06242796
#define GCC_TO_PCF_CONV		 62.42796
#define EXPLORER_2_MODE    0
#define GCC_TO_KG  .001


// The number of interrupts needed for shutdown timer
// The shutdown timer is incremented once every 100ms
#define THIRTY_MINUTES 18000
#define SIXTY_MINUTES  36000  

#define NORMAL_DENSITY_MODE  1  
#define METAL_DENSITY_MODE   2    
#define PROFILE_DENSITY_MODE 3      
 
#define RATIO_UPPER  0.465
#define RATIO_LOWER  0.225
  
#define DENSITY_OFFSET_BIT     1<<0
#define INSTR_GAUGE_BIT        0

#define DEFAULT_DENSITY_STD   3600

#define null NULL

#define PULSETIMERCLK 1000 // one shot timer clock rate in Hz  

#define CHARGER_ON         0
#define CHARGER_OFF        1

#define DUMMY_COUNT_2      2000
#define DUMMY_COUNT_1      6000
  
#define  BS_POSITION       0
#define  SAFE_POSITION     1
#define  NO_POSITION       2

#define EPSILON .0001  
  
#define PROFILE_DENSITIES  15
#define PROFILE_DEN_START  .50
#define PROFILE_DEN_END    4.00  
#define PROFILE_DEN_INCREMENT    .25  
  
extern BOOL g_remote_start_flag;
extern BOOL g_remote_recall_flag;
extern BOOL g_recall_flag;  
  
/* This structure is used for defining 'screens' for the LCD */
typedef struct 
{
	const	uint8 lineloc;                      // Location of initial character on line
	char * linestring;                        // LCDSTRING const * const s_calWaitingToFoamC [4]Pointer to string
}  LCDSTRING;
  
  
/*----------------------------------------------------------------------------*/
/*-------------------------[   Global Variables   ]---------------------------*/
/*----------------------------------------------------------------------------*/
extern char gps_fix;
  
  
// These are the actual global definitions of the flags, coudn't define individual bits as extern

struct flag_struct
{
   uint16_t button_pressed        : 1; // Flag is high when "key" is pressed
   uint16_t stat_flag             : 1; // set during stat test   
   uint16_t stand_flag            : 1; // set during stand test
   uint16_t in_spec_cal           : 1; // set during special cal routine
   uint16_t drift_flag            : 1; // set during drift test
   uint16_t auto_number_stations  : 1; // set when stations are to be automatically numbered
   uint16_t bat_volt              : 1; // when FALSE, battery is automatically monitored by timer1_isr()
   uint16_t diag                  : 1; // set during diagnostic self test  
   uint16_t usb_stopped           : 1; // set when usb in stop mode
   uint16_t Abort                 : 1; // Set when ON pressed for 3 seconds.
  
} ;

 union 
{
  uint16_t * flag_value;
  struct flag_struct * flag_bitfield;
}  Flag_U;

 struct control_struct
{
   uint16_t buzz_enable           : 1; // set to enable buzzer when sound_on is enabled
   uint16_t shut_dwn              : 1; // set when "auto" shut off is enabled, some tests temporarily turn this off.
   uint16_t LCD_light             : 1; // enables LCD backlight if set
   uint16_t bat_sel               : 1; // selects NICAD or ALK (via scaler board connector only).  EBB pins should not be used.
   uint16_t reset_count           : 1; // resets count when enter is pressed during test
   uint16_t update_time           : 1; // controls when the display will update countdown clock
} ;

 union 
{
  uint16_t * controls_value;
  struct control_struct * controls_bitfield;
}  Controls_U;

 struct features_struct
{
	uint16_t avg_std_mode          : 1; // 1 flag for average standard mode, set when enabled
	uint16_t SI_units              : 1; // 2 set when units are in kg/m3
	uint16_t auto_store_on         : 1; // 3 set when configured in auto store mode
	uint16_t sound_on              : 1; // 4 set to enable buzzer 
	uint16_t kg_gcc_units          : 1; // 5 Cleared when kg_m3 enable. Set when GCC enabled
	uint16_t auto_scroll           : 1; // 6 automatically scrolls data if set
	uint16_t language_f            : 1; // 7 Select English "bit 1" or Spanish "bit 0"
	uint16_t dummy_mode            : 1; // 9 0: Normal, 1:DUMMY
  uint16_t gps_on                : 1; // 10 0: GPS disabled, 1 GPS enabled
	uint16_t auto_depth            : 1; // 0 flag for auto depth setting, set high when enabled  
  uint16_t day_month_year_flag   : 1; // 11 
  uint16_t flag_24HourMode       : 1; // 12
  uint16_t temp_15               : 1; // 13
} ;

 union 
{
  uint16_t * features_value;
  struct features_struct * features_bitfield;
}  Features_U;



 struct offsets_struct
{
   uint16_t den_offset_pos        : 1; // Density offset flag, set when "ON"
} ;

 union 
{
  uint16_t * offsets_value;
  struct offsets_struct * offsets_bitfield;
}  Offsets_U;

 struct spec_flag_struct
{
   uint16_t auto_turn_off         : 1; // set when enabled to time out and turn off automatically                                              
   uint16_t recall_flag           : 1; // set when recall feature is activated.
   uint16_t self_test             : 1; // self test flag
   uint16_t RDA_CR                : 1; // '\r' has been read into uart1 buffer
} ;

 union 
{
  uint16_t * spec_flags_value;
  struct spec_flag_struct * spec_flags_bitfield;
}  Spec_flags_U;



#define FLAGS              ( *(Flag_U.flag_value) )
#define CONTROL            ( *(Controls_U.controls_value) )
#define FEATURES           ( *(Features_U.features_value) )
#define OFFSETS            ( *(Offsets_U.offsets_value) )
#define SPEC_FLAG         ( *(Spec_flags_U.spec_flags_value) )

#define GET_BITFIELD_ADDRESSES()  do { \
                                  Features_U.features_bitfield      = &Features ;  \
                                  Flag_U.flag_bitfield              = &Flags;      \
                                  Controls_U.controls_bitfield      = &Controls;   \
                                  Offsets_U.offsets_bitfield        = &Offsets;    \
                                  Spec_flags_U.spec_flags_bitfield  = &Spec_flags; \
                                  } while ( 0 )


// time and date structure for information from RTC
#pragma pack(1) 
typedef struct 
{
  uint8_t   iday;
  uint8_t   imonth;
  uint16_t  iyear;
  uint8_t   ihour;
  uint8_t   iminute;
  uint8_t   isecond;

} date_time_t;

#pragma pack(1)  
typedef struct
{
   float latitude;
   float longitude;
   int16 altitude;
   uint16 sats;
   uint8  fix;
} GPSDATA;

extern struct spec_flag_struct Spec_flags;
extern  struct offsets_struct Offsets;
extern struct features_struct Features;
extern struct flag_struct Flags;
extern struct control_struct Controls;
extern uint8_t usb_start;
extern uint8_t global_special_key_flag;
extern volatile uint32 msTimer;
extern uint16_t g_half_secs;

extern uint8_t cnt_time;
extern uint8_t second;
extern uint16_t  g_stop_time_secs;
extern uint8_t minute;
extern uint8_t tst_depth_g;
extern uint8_t ok_2_print;
extern uint8_t gp_disp; 

extern uint8_t test_number;
extern uint8_t in_menu;
extern uint8_t next_in2; 
extern uint8_t next_out2;
extern uint16_t temp;				

extern uint8_t	splashScreen;  
extern uint32_t shutdown_timer;
extern uint32_t g_turn_off_cnts;
extern uint16_t valid_depth;

extern uint16_t total_seconds;
extern uint16_t depth_setting;

extern uint32_t date_unformatted; 

extern uint32_t day_counter_g;
extern char s1[4];

extern char buffer2[32];

extern char lcdstr[80];
extern date_time_t date_time_g;

extern  uint16_t alfat_errors;
extern uint8_t alfat_error;

extern GPSDATA gdata;
extern uint8 PCB_REV;
extern uint8 charger_state;

/*---------------------------------------------------------------------------*/
/*------------------------------[  Global Macros  ]--------------------------*/
/*---------------------------------------------------------------------------*/


#define     CLEAR_BUFFER2 (next_in2=next_out2)
                               
        
                
#define  _LCD_PRINTF(str,param)  do { sprintf( lcdstr, (str),(param) ) ;  \
                                      LCD_print ( lcdstr ) ; }            \
                                  while (0)    

#define  _LCD_PRINTF2(str,param1,param2)  do { sprintf( lcdstr, (str),(param1,param2) ) ;  \
                                      LCD_print ( lcdstr ) ; }            \
                                  while (0) 
                          

#define  _UART2_PRINT(str)  do { sprintf( lcdstr, (str) ) ;\
                                 UART2_PutString ( (char *)lcdstr ) ;}\
                             while (0)          
                
#define  _UART2_PRINTF(str,param)  do { sprintf( lcdstr, (str),(param) ) ;\
                                        UART2_PutString ( (char *)lcdstr ) ;}\
                                   while (0)   

/*----------------------------------------------------------------------------*/
/*---------------------[   Global Function Prototypes   ]---------------------*/
/*----------------------------------------------------------------------------*/

void _LCD_PRINT ( char *lcdstr) ;
extern void nullString ( char * string, uint16_t length );
void print_string_backward(char *temp_str, uint8_t position);  //prints string from right to left starting at position

extern uint8_t stat_test(void);
extern void drift_test(void);
extern void get_const_computer(void);
extern DOUBLE_FLOAT get_constant(char ch, uint8_t depth);
extern void wait_for_serial(void);
extern void general_purpose_test(void);
extern void stand_test(void);
extern void set_count_time(void);
extern void recall(void);
extern void special_cal(void);
extern void offset(void);
extern void set_units(void);
extern void ma(void);
extern void enter_cal_const(void);
extern void serial_number( void );
extern void monitor_batt(void);
extern void memory_reset(void);
extern void auto_initialization(void);
extern void shut_down_text(void);
extern void nomograph(void);
extern Bool check_temp(Bool display);
extern void selftest(void);
extern void retrieve_offsets(void);
extern void batt_volt(void);
extern void check_depth (void ) ;
extern enum buttons enable_disable_features(char c);       // enable or disable LCD BKlight, auto scroll and auto depth
extern void main_menu(void);
extern void main_menu_display(uint8_t menu_trk);
extern void project_menu(void);
extern void project_menu_display(uint8_t menu_trk);
extern void diagnostic_menu(void);
extern void diagnostic_menu_display(uint8_t menu_trk);
extern void settings_menu(void);
extern void settings_menu_display(uint8_t menu_trk);


extern void LCD_light_timer(uint16_t timeout_seconds);

extern void enter_system_time(void);
extern Bool date_check(uint8_t month_temp, uint8_t day_temp);
extern Bool time_check(uint8_t hr);
extern void read_RTC( date_time_t* );
extern uint32_t decode_date ( date_time_t  date);
extern void check_std_counts(void);
extern void gets_comp(char* s);


extern void puts_printer(char *s);

extern void review_std_counts(void);
extern void diag_self_test(void);
extern void diag_self_test_review(Bool skip_prompt);
extern void diag_self_test_print(uint8_t);
extern Bool enter_access_code(void);
extern void select_language(void);
extern void hold_buzzer(void);
extern void diag_menu(void);

extern void initUART96 ( void );
extern void initUART ( void );

extern void append_char(char* s, char c) ;
extern float displayValueWithUnits ( float value_in_kg, uint8_t position, char * dstr );
extern float displayValueWithUnitsBW ( float value_in_kg, uint8_t position, char * dstr );
extern uint8_t measureDensity(void) ;
extern void checkFloatLimits ( float * );
extern void display_invalid_depth( BYTE depth_temp_inches);
extern void select_mode(void);

extern void  scan_keys ( void );
extern uint8 checkCountDone ( void );
extern void clearGPSData ( );
extern void parseGPSString();
extern void FirmwareMenu ( );
extern void  convertRTCtoAlfatTime ( date_time_t date );
extern uint8_t get_depth ( void );
extern void measurePulses6Tubes (  )  ;
extern void getGMPulseCounts ( uint32 count_array[] );
extern void displayCounts6 ( uint16 div );
extern void auto_depth_enable(void);
extern void measurePulsesForDensity ( uint32_t * density_2_count, uint32_t * density_1_count ) ;
void changeMonthDayOrder ( void );
void changeAMPM_24Hour ( void );
void enterTimeDate ( void );

#endif /* endif !GLOBALS_H for "if we haven't included this file already..."    */
