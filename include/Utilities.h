/******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  Utilities.h
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
 *---------------------------------------------------------------------------*/

/*  If we haven't included this file already.... */
#ifndef UTILITIES_H
#define UTILITIES_H

#include "Globals.h"
#include  "ProjectData.h" 

    
#define NICAD_ADC_CHAN             0
#define TEMPERATURE_ADC_CHAN       1
#define ALKALINE_ADC_CHAN          2
#define DEPTH_SENS_ADC_CHAN        3  
  
#define ADC_CHANNELS               4

extern void shut_down_test (void ) ;
extern void light_test (void ) ;
extern void raw_count_test (void ) ;
extern void key_pad_test (void );
extern void USB_store_test (void );
extern void SleepDelay128ms( void );
extern void set_adc_channel ( uint16_t chann );
extern float readADCVolts ( uint8_t channel );

extern void initSystem ( void );
extern void printTimeDate ( date_time_t date );
extern void getTimeDateStr ( date_time_t date, char * d_t_str );
extern void SendBLEData ( station_data_t * ble_data, bool isRecall );
extern void SendBLEDataCC ( ) ;
extern void SendBleSn();
extern void SendBleReset();
extern void reset_ble_menu();
extern void RequestBleVersion();
extern void ParseBleVersion(uint8* b);
extern void InitBleCharacteristics();
extern void setSerialNumber ( uint32 serial_number );
extern void ParseCalibrationConstants(uint8* b);
extern void SendBleCcAck();
extern void RTC_WriteTime ( date_time_t* d_time );  //write time and date from RTC
#endif




