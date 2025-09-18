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
#include "project.h"
#include "Globals.h"
#include "ProjectData.h"
#include "StoreFunctions.h"
#include "prompts.h"
#include "SDcard.h"
#include "DataStructs.h"
#include "LCD_drivers.h"
#include <stddef.h> /* for offsetof */
/************************************************************************
//  Functions Name: readStation ()
//  Description:  Given a project and station number, a station is copied from
//                NV Memory to RAM
//  Parameters:   Project number, Station Number, destination Ram
***************************************************************************/
int32 readStation ( char* project, uint16_t index_station, station_data_t * station   )
{
 int32 error;
 FS_FILE* pFile = null;
 int32 offset = offsetof(project_data_t,station[index_station]);
 pFile = SDProjOpen ( project );
 FS_FSeek ( pFile, offset, FS_SEEK_SET );
 error = SDreadBuffer ( pFile, (char*)station, sizeof(station_data_t) );
 FS_FClose( pFile );
 return error;
}
/************************************************************************/
//  Functions Name: writeStation ()
//  Description:  Given a project and station number, a station is copied from
//                RAM to  NV Memory
//  Parameters:   Project number, Station Number, Source address in RAM
/***************************************************************************/
void writeStation ( char* project, uint16_t index_station, station_data_t * station_n )
{
 FS_FILE *pFile = null;
  uint32_t offset = offsetof(project_data_t,station[index_station]);
 pFile = SDProjOpen ( project );
 FS_FSeek ( pFile, offset, FS_SEEK_SET );
 SD_WriteBuffer( pFile, (char*)station_n, (uint32_t)sizeof(station_data_t) ) ;
 FS_FClose( pFile );
}
/************************************************************************/
//  Functions Name: writeStationName ( )
//  Description:  Given a project and station number, a station name is copied
//                from RAM to NV Memory
//  Parameters:   open Project file pointer, Station Name, station index
//  Returns:   0=fail, 1 = success
/***************************************************************************/
uint8 writeStationName (char* project, char * name, uint16_t index_station )
{
 uint8 open;
 FS_FILE *pFile = null;
 int32_t size = strlen(name) + 1;   // length of name string plus NULL
 // find the offset of the station name of project station
 int32_t offset = offsetof(project_data_t, station[index_station].name[0]);
 pFile = SDProjOpen ( project );
 if ( pFile == null )
 {
  return 0 ;
 }
 open = FS_FSeek ( pFile, offset, FS_SEEK_SET );
 if ( open  )
 {
  return 0 ;
 }
open =  SD_WriteBuffer( pFile, name, size ) ;
open = FS_FClose( pFile );
 return 1;
}
/************************************************************************/
//  Functions Name: getStationName ( )
//  Description:  Given a project and station number, a station name is copied
//                from NV Memory to RAM
//  Parameters:   Project number, Station Number, Destination address in RAM
//  Returns:   0 if error, 1 if success
/***************************************************************************/
int32 getStationName ( char* project, char * name, uint16_t index_station )
{
 int32 error;
 uint8 i;
 FS_FILE *pFile = null;
 char s_name[ PROJ_NAME_LENGTH ];
 // find the offset of the station name of project station
 uint32_t offset = offsetof(project_data_t, station[index_station].name[0]);
 pFile = SDProjOpen ( project );
 FS_FSeek ( pFile, offset, FS_SEEK_SET );
 error = 1;
 for ( i=0; i < PROJ_NAME_LENGTH; i++ )
 {
   error = SDreadBuffer ( pFile, &s_name[i], 1 );
   if ( error == -1 )
   {
    error = 0;
    break;
   }
  if ( s_name[i] == '\0' )
  {
    error = 1;
    break;
  }
 }
 memcpy( name, s_name, PROJ_NAME_LENGTH );
 FS_FClose( pFile );
 return error;
}
/************************************************************************/
//  Functions Name: clearStationNumber ()
//  Description:  clears the number of stations stored to zero
//  Parameters:   Open Project file
//  Returns:
/***************************************************************************/
void clearStationNumber ( char* project   )
{
  uint16_t st_num = 0 ;
 FS_FILE *pFile = null;
 // offset of station number for project in NV Memory
 uint32_t offset = offsetof(project_data_t,station_number );
  // store the number
 pFile = SDProjOpen ( project );
 FS_FSeek ( pFile, offset, FS_SEEK_SET );
 SD_WriteBuffer( pFile, (char*)&st_num, 2 ) ;
 FS_FClose( pFile );
}
/************************************************************************/
//  Functions Name: incrementStationNumber ()
//  Description:  Given a project number, the number of stations stored in it is incremented
//  Parameters:   Project number
//  Returns:      number of stations after increment
/***************************************************************************/
uint16_t incrementStationNumber ( char* project   )
{
  uint16_t st_num;
 FS_FILE *pFile = null;
 // offset of station number for project in NV Memory
 uint32_t offset = offsetof(project_data_t,station_number );
 pFile = SDProjOpen ( project );
 FS_FSeek ( pFile, offset, FS_SEEK_SET );
 // read the current number of stored station in the project
 SDreadBuffer ( pFile, (char*)&st_num, 2 );
 // increment stations by one
 st_num++ ;
 // store the number
 FS_FSeek ( pFile, offset, FS_SEEK_SET );
 SD_WriteBuffer( pFile, (char*)&st_num, 2 ) ;
 FS_FClose( pFile );
 return st_num;
}
/************************************************************************/
//  Functions Name: getStationNumber ()
//  Description:  Given a project number, get the number of stations stored in it
//  Parameters:   Project number
//  Returns:      number of stations in project
/***************************************************************************/
uint16_t  getStationNumber ( char* project  )
{
  uint16_t st_num;
 FS_FILE *pFile = null;
 // offset of station number for project in NV Memory
 uint32_t offset = offsetof(project_data_t,station_number );
 pFile = SDProjOpen ( project );
 FS_FSeek ( pFile, offset, FS_SEEK_SET );
 // read the current number of stored station in the project
 SDreadBuffer ( pFile, (char*)&st_num, 2 );
 FS_FClose( pFile );
 return st_num;
}
/************************************************************************/
//  Functions Name: setStationAutoNumber( )
//
//  Description:  Enables or disables auto number for the project
//
//
//  Parameters:  project number, flag ( 1 = ON, 0 = OFF )
//
//  Returns:
//
/***************************************************************************/
void setStationAutoNumber ( char* project, char flag )
{
 FS_FILE *pFile = null;
// calculate the offset into the NV Memory to start the write
 uint32_t offset = offsetof ( project_data_t, station_auto );
 // store the number
 pFile = SDProjOpen ( project );
 FS_FSeek ( pFile, offset, FS_SEEK_SET );
 SD_WriteBuffer (  pFile, &flag , 1);
 FS_FClose( pFile );
}
/************************************************************************/
//  Functions Name: checkStationAutoNumberE( )
//
//  Description:  gets the auto number status
//
//
//  Parameters:  project number
//
//  Returns:    flag ( 1 = ON, 0 = OFF )
//
/***************************************************************************/
uint8_t checkStationAutoNumber ( char* project )
{
 char st_auto;
 FS_FILE *pFile = null;
 pFile = SDProjOpen ( project );
 // calculate the offset into the NV Memory to start the write
 uint32_t offset = offsetof ( project_data_t, station_auto );
 FS_FSeek ( pFile, offset, FS_SEEK_SET );
 // read the current number of stored station in the project
 SDreadBuffer ( pFile,  &st_auto , 1 );
 FS_FClose( pFile );
 return st_auto;
}
/**************************************************************************/
//  Functions Name: setActiveProjectEE ( char* proj )
//
//  Description: Puts the name of the active project into EEPROM
//
//  Parameters:
//
//  Returns:
//
/***************************************************************************/
void  setActiveProjectEE ( char * proj )
{
  EEpromWriteArray(  (uint8 *)proj, PROJ_NAME_LENGTH, offsetof( EEPROM_DATA_t,active_project_name ) );
}
/**************************************************************************/
//  Functions Name: getActiveProjectEE ( char* proj )
//
//  Description: Puts the name of the active project into EEPROM
//
//  Parameters:
//
//  Returns:
//
/***************************************************************************/
void  getActiveProjectEE ( char * proj )
{
  EEpromReadArray(  (uint8 *)proj, PROJ_NAME_LENGTH, offsetof( EEPROM_DATA_t,active_project_name ) );
}
/**************************************************************************/
//  Functions Name: getProjectNumber ()
//
//  Description: Finds total number of projects stored in NV Memory
//
//  Parameters:
//
//  Returns:      number of projects
//
/***************************************************************************/
uint16_t  getProjectNumber ( void  )
{
 uint16_t proj_num;
 SD_Wake();
 proj_num = SD_FindTotalFiles (  "Project\\", false );
 return proj_num;
}
/************************************************************************/
//  Functions Name: writeAutoStationStart( )
//
//  Description:  Set the starting station number for auto station mode
//
//
//  Parameters:  project name, layer type
//
//  Returns:
//
/***************************************************************************/
void writeAutoStationStart ( char* project , uint16 start )
{
 // calculate the offset into the NV Memory to start the write
 FS_FILE *file = null;
 SD_Wake();
 file = SDProjOpen ( project );
 uint32_t offset = offsetof ( project_data_t, station_auto_start );
 FS_FSeek ( file, offset, FS_SEEK_SET );
 SD_WriteBuffer( file, (char*)&start, 2 ) ;
 FS_FClose( file );
}
/************************************************************************/
//  Functions Name: getAutoStationStart( )
//
//  Description:  get the starting station number for auto station mode
//
//
//  Parameters:  project name, layer type
//
//  Returns:
//
/***************************************************************************/
uint16 getAutoStationStart ( char* project  )
{
 // calculate the offset into the NV Memory to start the write
 uint16 start;
 FS_FILE *file = null;
 SD_Wake();
 file = SDProjOpen ( project );
 uint32_t offset = offsetof ( project_data_t, station_auto_start );
 FS_FSeek ( file, offset, FS_SEEK_SET );
 SDreadBuffer( file, (char*)&start, 2 ) ;
 FS_FClose( file );
 return start;
}


/************************************************************************/
//  Functions Name: setStationMeasureMode( )
//
//  Description:  sets the Measurement Mode (normal, metal or Profile )
//
//
//  Parameters:  measurement mode normal,metal, profile
//
//  Returns:
//
/***************************************************************************/
void setStationMeasureMode ( char* project, char m_mode )
{
 FS_FILE *pFile = null;
// calculate the offset into the NV Memory to start the write
 uint32_t offset = offsetof ( project_data_t, station_measurement_mode );
 // store the number
 pFile = SDProjOpen ( project );
 FS_FSeek ( pFile, offset, FS_SEEK_SET );
 SD_WriteBuffer (  pFile, &m_mode , 1);
 FS_FClose( pFile );
}

/************************************************************************/
//  Functions Name: checkStationMeasureMode( )
//
//  Description:  gets the Measurement Mode (normal, metal or Profile )
//
//
//  Parameters:  Measurement Mode
//
//  Returns:    
//
/***************************************************************************/
uint8_t checkStationMeasureMode ( char* project )
{
 char st_mode;
 FS_FILE *pFile = null;
 pFile = SDProjOpen ( project );
 // calculate the offset into the NV Memory to start the write
 uint32_t offset = offsetof ( project_data_t, station_measurement_mode );
 FS_FSeek ( pFile, offset, FS_SEEK_SET );
 // read the current number of stored station in the project
 SDreadBuffer ( pFile,  &st_mode , 1 );
 FS_FClose( pFile );
 return st_mode;
}

/************************************************************************/
//  Functions Name: clearProjectValid ()
//
//  Description: deletes the project
//
//  Parameters:  The project number
//
//  Returns:     0, if successfully deleted
//
/***************************************************************************/
uint16_t clearProject  ( char * project_name  )
{
  int error,i;
  char buffer[30];
  SD_Wake();
  sprintf( buffer, "\\Project\\%s", project_name );
  for ( i=0; i<3; i++ )
  {
    error =  FS_Remove ( buffer ) ;
    if ( error == 0 ) break;
    SDstop(NULL);
    CyDelay(200);
    SD_Wake();
  }
  if ( error != 0 )
  {
      CLEAR_DISP ;
      DisplayStrCentered(LINE1,"Error Deleting");
      sprintf(buffer,"%s",project_name );
      DisplayStrCentered(LINE2,buffer);
      DisplayStrCentered(LINE3,(char * )FS_ErrorNo2Text(error));
  }
  else
  {
    CLEAR_DISP ;
    DisplayStrCentered(LINE1,"Deleted");
    sprintf(buffer,"%s",project_name );
    DisplayStrCentered(LINE2,buffer);
  }
  setActiveProjectEE ( "none_selected" ); // Put the active project name into EEPROM
  CyDelay(2000);
  return error;
}
/************************************************************************/
//  Functions Name: resetProjectStorage ( void )
//
//  Description:  Delete all files from project storage
//
//
//  Parameters:
//
//  Returns:
//
/*************************************************************************/
uint16 resetProjectStorage ( void )
{
  int Err;
  char project [] = "TEMP";
  SD_Wake();
  DeleteProjectDirectory();
  if( 0 == CreateDir("Project") )
  {
    /* Display failure message */
  	CLEAR_DISP;
    DisplayStrCentered(LINE1,"Failed to create");
    DisplayStrCentered(LINE2,"Project directory");
    CyDelay(2000);
  }
  Err =  getProjectNumber (  );
  if ( Err != 0 )
  {
    /* Display failure message */
  	CLEAR_DISP;
    DisplayStrCentered(LINE1,"Failed to Delete");
    DisplayStrCentered(LINE2,"Project directory");
    CyDelay(2000);
  }
  else
  {
   // create file
   FS_FILE *file = SD_CreateProjectSimpleFile ( project );
   if ( file != null )  
   {
     // RESET STATION
     clearStationNumber ( project );
     setStationAutoNumber ( project, AUTO_NUMBER_ON ); // make station auto number
     //write name to first station name location in project
     writeAutoStationStart ( project, 1 );
     setStationMeasureMode ( project, NORMAL_DENSITY_MODE );
     setActiveProjectEE ( project ); // Put the active project name into EEPROM
    } 
  }
  return ( Err );
}
/* [] END OF FILE */