/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#ifndef PROJECTDATA_H
#define PROJECTDATA_H

#include "Globals.h"
#include <FS.h>

  
#define PROJECT_NOT_VALID 0  
  
////////////////////////Memory locations for project storage////////////////////
#pragma pack(1)  
typedef struct station_data_s
{
  uint16_t   density1_count  ;     // density sys 1 count at station         
  uint16_t   density2_count  ;     //  density sys 2 count at station         
  uint16_t   DT[PROFILE_DENSITIES]     ;     //   Profile mode has 15 densities        
  uint16_t   density                   ;
  uint16_t   density1_stand  ;     // d1_stand when station count was done    
  uint16_t   density2_stand ;     // d2_stand when station count was done       
  float      MA             ;     // MA count when station count was done     
  float      den_off        ;     //                                          
  float      thickness      ;     //                                          
  float      battery_voltage[2];
  uint8      mode           ;     // Metal, Normal, Profile  
  uint8_t    units          ;     //                                          
  char       name[PROJ_NAME_LENGTH]  ;     // name of station                 
  date_time_t date          ;     //                                           
  uint32_t   ser_number     ;
  float      version        ;
  GPSDATA    gps_read       ;     //  
 } station_data_t;                                                      


#pragma pack(1)  
typedef struct 
{
  uint16_t    station_auto_start     ;     // if auto numbering, start here
  uint8_t     station_auto           ;     // auto number enabled 
  uint16_t    station_number         ;     // total number of stations within project, will also tell station index
  uint8       station_measurement_mode;
  station_data_t  station[MAX_STATIONS] ;    // station data
} project_data_t;


/*---------------------------------------------------------------------------*/
/*----------------------[  Global Function Prototypes  ]---------------------*/
/*---------------------------------------------------------------------------*/


uint16_t  getProjectNumber ( void );
void      clearStationNumber ( char* project  );
void      setProjectPointer ( uint16_t proj_ptr  );
uint8     writeStationName (  char* project, char * name, uint16_t index_station );
uint8_t   checkStationAutoNumber ( char* project );
uint16_t  getStationNumber ( char* project );
void      setStationAutoNumber ( char* project, char flag );
uint16    resetProjectStorage ( void );
int32     readStation (char* project, uint16_t index_station, station_data_t * station   );
uint16_t  incrementStationNumber ( char* project   );
void      writeStation ( char* project, uint16_t index_station, station_data_t * station_n );

uint16_t  clearProject  ( char * project_name  );
void      setActiveProjectEE ( char * proj );
void      getActiveProjectEE ( char * proj );
int32     getStationName (  char* project, char * name, uint16_t index_station );
void      writeAutoStationStart ( char* project,  uint16_t auto_start_num );
uint16    getAutoStationStart ( char* project  );

void setStationMeasureMode ( char* project, char m_mode );
uint8_t checkStationMeasureMode ( char* project );

#endif 

/* [] END OF FILE */
