/******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2025                  
 *  1 Triangle Dr.
 *  Durham,NC 27709
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  
 *  Originating Author:  
 *       Creation Date:  
 *
 ******************************************************************************/
 
 
/*---------------------------------------------------------------------------*/
/*---------------------------[  Revision History  ]--------------------------*/
/*---------------------------------------------------------------------------*/
/*
 *  when?       who?    what?
 *  ----------- ------- ------------------------------------------------------
 *  10/21/2010  CC      Original
 *
 *---------------------------------------------------------------------------*/

#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

#include "Globals.h"

extern void SavePartialEepromData(uint8* array,int32 len,int32 offset);  
extern void getNVFromEEProm (void );
extern void EEpromReadArray(uint8 *array, uint32 len,uint32 eepromOffset);
//*************************************************************************************************************************

// Limites to project storage 

#define NULL_NAME_STRING {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

#define PROJECT_NOT_VALID 0
#define AUTO_NUMBER_ON     1
#define AUTO_NUMBER_OFF    0

#define THINPAVE_CONST_NUM  38
#define THINPAVE_CONST_NUM_DOUBLES  34
#pragma pack(1) 
typedef struct constants_s
{
  double           A1;             //
  double           B1;             //
  double           C1;             //
  double           A11;             //
  double           A12;             //
  double           A13;             //
  double           A2;             //
  double           B2;             //
  double           C2;             //
  double           A21;             //
  double           A22;             //
  double           A23;             //
  double           S11;
  double           S12;
  double           S13;
  double           S21;
  double           S22;
  double           S23;
  double           L125;             //
  double           L225;             //
  double           L150;             //
  double           L250;             //
  double           L175;             // 
  double           L275;             //
  double           L110;             //
  double           L210;             //
  double           H125;             //  
  double           H225;             //  
  double           H150;             //  
  double           H250;             //  
  double           H175;             //    
  double           H275 ;            //  
  double           H110;             //  
  double           H210;             //   
  uint32_t         SERIAL_NUM;
  uint16_t     DENS_1_CAL_STD     ;        // saves sys 1 standard count at time of calibration
  uint16_t     DENS_2_CAL_STD     ;        // saves sys 2standard count at time of calibration
  date_time_t  CAL_DATE           ;           
 } constants_t;


////////////////////////Memory locations for avg standard test values///////////
#pragma pack(1) 
typedef struct avg_stand_test_s
{
  uint16_t     dense_count1_1    ;          
  uint16_t     dense_count1_2    ;          
  uint16_t     dense_count1_3    ;          
  uint16_t     dense_count2_1    ;         
  uint16_t     dense_count2_2    ;         
  uint16_t     dense_count2_3    ;         
  uint16_t     std_number       ;          
  uint16_t    dense_count1_x[30] ;         // save the last 30 sys 1 std counts 
  uint16_t    dense_count2_x[30] ;          // save the last 30 sys 2 std counts 
  date_time_t date_count_x[30]  ;          // save the last 30 std count dates 
  uint16_t    std_index         ;          // next place to save standard value 
  uint16_t    std_counts        ;          // save number of counts
} avg_stand_test_t;

#pragma pack(1) 
typedef struct eepromData_s
{
  uint16_t      DEN1_STAND    ;
  uint16_t      DEN2_STAND    ;
  constants_t   Constants     ;
  avg_stand_test_t  stand_test   ;
  uint32_t     D1_CNT_AVG      ;           // average density count sys 1
  uint32_t     D2_CNT_AVG      ;           // average density count sys 2
  uint16_t     D1_CNT_STD      ;           // statisical density count sys 1
  uint16_t     D2_CNT_STD      ;           // statisical density count  sys 2
  // Other Storage
  uint16_t     COUNT_TIME     ;          // 1BYTES test time for counting pulses 
  float        D_OFFSET       ;          
  float        MARSHALL       ;          

  ///////////////////////STORAGE FOR RECALL///////////////////////////////////////
  date_time_t LAST_TEST_TIME      ;      
  float    MAT_THICK           ;    // Material thickness 
  uint16_t    OFF_MODE            ;      // Monitors when gauge is turned off or shuts down after 1 hr.
  struct offsets_struct    OFFSET_SETTINGS     ;     // stores offset settings for auto shutdown
  struct flag_struct       FLAG_SETTINGS       ;     // stores flag settings for auto shutdown
  struct control_struct    CONTROL_SETTINGS    ;     // stores Control settings for auto shutdown
  struct features_struct   FEATURE_SETTINGS    ;     
  uint16_t     MANUAL_STORE       ;         
  uint16_t     GP_DISPLAY         ;         // index of the last displayed screen in general purpose test
  uint32_t     d_temp             ;         // 4 BYTES gauge temp before drift test
  char         active_project_name[PROJ_NAME_LENGTH];
  GPSDATA      LAST_GPS_READING   ;
  uint16_t     D_CNT_STD[6]      ;
  uint16_t     dr_dense_avg[6]    ;
  uint8_t      measure_mode       ;
  uint8        temp[50]           ;         // future use
  uint8_t      intialized;

 } EEPROM_DATA_t; ;

typedef struct 
{
  char * name;
  uint8_t value;
} t_const_key_value;  

extern t_const_key_value const_name_value[37];

extern EEPROM_DATA_t  eepromData;  

void EEpromWriteArray(uint8 *array, uint32 len,uint32 eepromOffset) ;
uint8 ReadEepromData(void);
//************************* Macros ***************************************

#define NV_AVG_STD_TEST    eepromData.stand_test
#define NV_CONSTANTS(M)    eepromData.Constants.M

// Gets the data from RAM copy
#define NV_RAM_MEMBER_RD(M)     eepromData.M

// update both RAM and EEPROM copies  
#define NV_MEMBER_STORE(M,V)  do{\
                                 eepromData.M = V;\
                                 SavePartialEepromData((uint8*)&eepromData.M, sizeof(eepromData.M), offsetof( EEPROM_DATA_t,M)) ;\
								                }while(0)    
   
#define READ_EEPROM(M) 		EEpromReadArray((uint8*)&eepromData.M,sizeof(eepromData.M),offsetof(EEPROM_DATA_t,M))

#define NV_EE_RD(M,D)     do {\
							              	READ_EEPROM(M);\
              								*D = NV_RAM_MEMBER_RD(M);\
              							  }while(0)	


#endif 


