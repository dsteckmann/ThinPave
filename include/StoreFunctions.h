/******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  StoreFunctions.h
 *  Originating Author:  DMS
 *       Creation Date:  9/2012
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
#ifndef STOREFUNCTIONS_H
#define STOREFUNCTIONS_H

#include "Globals.h"  
#include "DataStructs.h"
#include "Alfat.h"
#include  "ProjectData.h"
  
  
/*----------------------------------------------------------------------------*/
/*-------------------------[   Global Constants   ]---------------------------*/
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*-------------------------[   Global Variables   ]---------------------------*/
/*----------------------------------------------------------------------------*/

typedef struct project_s
{
 
 uint8    station_auto_en;
 uint16   station_start;
 uint16_t number_of_projects;
 uint16_t station_index;
 uint8    station_measurement_mode; 
 char current_project[PROJ_NAME_LENGTH];
 char current_station_name[PROJ_NAME_LENGTH];
} project_info_t;

extern project_info_t project_info;


/*----------------------------------------------------------------------------*/
/*--------------------[   Global Function Prototypes   ]----------------------*/
/*----------------------------------------------------------------------------*/

uint8    updateProjectInfo ( void );
void     enter_project_name (void);
void     start_new_project  (void);
uint16_t select_stored_project ( uint8_t function, char* prj_name );
void     manual_storage(uint32_t address);
void     print_data (  char * project );
void     write_data_to_printer(void);
void     write_data_to_USB(void);
void     review_data(void);
void    delete_projects(void) ;
void    storeStationData ( char * project, station_data_t station  )  ;


#endif 
