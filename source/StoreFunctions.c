/**************************   ***************************************************************** ***************************/
//
// Title:       Elite
// Company:     Instrotek
//
// Document No.:
// Issue:
// Description:
//
// Filename:
// Author:
//
//
//
// History: date, version, personal initials with comments about any changes
//
/**********************************************************************************************************************/
/*********************************************  INCLUDE FILES  ***************************************************/
#include "project.h"
#include "Globals.h"
#include "DataStructs.h"
#include "StoreFunctions.h"
#include "Utilities.h"
#include "conversions.h"
#include "Alfat.h"
#include "prompts.h"
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "ProjectData.h"
#include "SDcard.h"
#include "UARTS.h"
/************************************* EXTERNAL FUNCTION DECLARATIONS  *************************************/
extern float convertKgM3DensityToUnitDensity ( float value_in_kg, uint8_t units );
extern  uint8_t getCalibrationDepth ( uint8_t depth_inches );
extern uint32 getSerialNumber ( void );
extern char select_measurement_mode(void);

/************************************* EXTERNAL VARIABLE AND BUFFER DECLARATIONS  *************************************/
project_info_t project_info;
/************************************************  LOCAL DEFINITIONS  *************************************************/
/*****************************************  VARIABLE AND BUFFER DECLARATIONS  *****************************************/
char temp_str[512];;
/******************************************************************************
 *  Name: updateProjectInfo ( void )
 *****************************************************************************/
 uint8 updateProjectInfo ( void )
 {
    char current_p_name[PROJ_NAME_LENGTH] ;
  //  FS_FILE *p_file = null;
      // read total number of projects stored on SD card
    if ( getProjectNumber() == 0 )
    {
      resetProjectStorage();
    }
    getActiveProjectEE ( current_p_name ); // get the current project name from EEPROM;
    // read total number of projects stored on SD card
    project_info.number_of_projects = getProjectNumber();
    if ( SD_CheckIfProjExists ( current_p_name ) != false )
    {
      if ( project_info.number_of_projects > 0 )
      {
        // get the project's measurement mode. This is needed because the project will need to be changed if the modes change.
        project_info.station_measurement_mode = checkStationMeasureMode (current_p_name );
        //read total number of stations within current project
        project_info.station_index     = getStationNumber ( current_p_name );
        // get project name
        strcpy( project_info.current_project,current_p_name );
      //set auto name option for current project
      Flags.auto_number_stations          = checkStationAutoNumber( current_p_name );    //set auto name option for current project
      project_info.station_auto_en = Flags.auto_number_stations;
      if ( project_info.station_auto_en == AUTO_NUMBER_ON )
      {
        project_info.station_start = getAutoStationStart ( current_p_name );
      }
     }
    }
    else
    {
     Features.auto_store_on = FALSE;
     return false;
    }
    return true;
 }
/******************************************************************************
 *
 *  Name: start_new_project ( void )
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *
 *
 *  RETURNS:
 *
 *****************************************************************************/
/////STORAGE FUNCTIONS//////////////////////////////////////////////////////////
void start_new_project ( void )  // leads user through setup for new project
{
  Bool auto_temp = 0, escape = 0;
  uint8_t  go_to_screen = 0, lcd_line ;
  FS_FILE *nfile;
  uint16_t station_start = 0;
  enum buttons button;
  //array that enter_name stores the name
  char project[PROJ_NAME_LENGTH]  = NULL_NAME_STRING;
 //  char station_name[PROJ_NAME_LENGTH] = NULL_NAME_STRING;
  char num_temp[5] = "1";
  char mode = NORMAL_DENSITY_MODE; 
  
  SD_Wake();
  // loop until break
  while(!escape)
  {
    CLEAR_DISP;
    switch ( go_to_screen )
    {
      case 0:
          enter_project_name_text ( project );  //TEXT// display "Enter Project \nName: %s",project LINE1,2
          YES_to_Accept(LINE3);              //TEXT// display "YES to Accept"
          ESC_to_Exit(LINE4);                //TEXT// display "ESC to Exit"
          lcd_line = (Features.language_f) ? LINE2+6 : LINE2+10;
          enter_name ( project,lcd_line) ;
          button = getLastKey();
          if(button == ESC)
          {
            escape = TRUE;
          }
          else if (button == YES)        // YES was pressed to accept entered project name
          {
            if ( project[0] != '\0' )   // make sure something was entered for the project name
            {
              if ( true  == SD_CheckIfProjExists ( project ) )
              {
               CLEAR_DISP;
               LCD_PrintAtPosition ("Project Exists!",LINE2);
               ESC_to_Exit(LINE4);                //TEXT// display "ESC to Exit"
               CyDelay ( 1500 );
               go_to_screen = 0;
              }
              else
              {
               // create file
               nfile = SD_CreateProjectSimpleFile ( project );
               // RESET STATION
               clearStationNumber ( project );
               setStationAutoNumber ( project, AUTO_NUMBER_ON ); // make station auto number
               if ( nfile == null )
               {
                escape = TRUE;
                continue;
               }
              else
               {
                go_to_screen = 1;        // if not, stay on enter name screen.
               }
              }
            }
          }
          break;
      case 1:
            project_name_text(project);  //TEXT// display "    Project Name\n    %s",project LINE2,3
            hold_buzzer();
            delay_ms(2000);
            go_to_screen = 6; // Get measurement type (Normal,Metal,Profile )
            break;
     case 2:
            station_mode_text();    //TEXT// display "Station Name Mode\n1. Auto (sequential)\n2. Manual Entry" LINE1,2,3
            up_down_select_text(0); //TEXT// display "Select #, ESC to Exit" LINE4
            while(1)
            {
              button = getKey( TIME_DELAY_MAX );
              if(button == 1 || button == 2 || button == ESC)
              {
                break;
              }
            }
            if(button == ESC)
            {
              escape = TRUE;
            }
            else if ( button == 1 )
            {
              go_to_screen = 3;
            }
            else if(button == 2)
            {
              go_to_screen = 4;
            }
            break;
      case 3:
            // Auto Number Enabled
            station_start_text(num_temp);  //TEXT// display "Starting Station\nNumber: %s",num_temp
            YES_to_Accept(LINE3);          //TEXT// display "YES to Accpet"
            ESC_to_Exit(LINE4);            //TEXT// display "ESC to Exit"
            auto_temp = TRUE;
            station_start = (uint16)enter_number_std ( num_temp, LINE2 + 8, 4, 0 );
            button = getLastKey();
            if(button == ESC)
            {
              escape = TRUE;
            }
            else if(button == YES)
            {
              go_to_screen = 4;
            }
            break;
      case 4:
              if ( auto_temp )
              {
                station_increment_text ( station_start );  //TEXT// display "Station # %u\nIncrements by 1\nFor Each Reading",station_start
                //save first station name
                sprintf ( project_info.current_station_name,"%u",station_start );
                //write name to first station name location in project
                writeAutoStationStart ( project, station_start );
                station_start = getAutoStationStart ( project );
                hold_buzzer();
                delay_ms(2000);
              }
              else
              {
                station_name_text();  //TEXT// display "Enter Station Name\nBefore Each Reading"
                hold_buzzer();
                delay_ms(2000);
              }
              // RESET STATION
              clearStationNumber ( project );
              // AUTO NUMBER MODE
              if (auto_temp)
              {
                setStationAutoNumber ( project, AUTO_NUMBER_ON );
              }
             else
              {
                setStationAutoNumber ( project, AUTO_NUMBER_OFF );
              }
              //enable AUTO STORE
              if( !Features.auto_store_on )  //auto store is not enabled, ask if they want to enable it
              {
                button = enable_disable_features('O');
              }
              escape = TRUE;
               //set auto name option for current project
              Flags.auto_number_stations = checkStationAutoNumber( project );    //set auto name option for current project
              setActiveProjectEE ( project ); // Put the active project name into EEPROM
              break;
      case 5:
            project_limit_text();  //TEXT// display "Project Limit\nExceeded, Delete\nOne or More Project" LINE1,2,3
            hold_buzzer();
            delay_ms(2000);
            escape = TRUE;
            break;
     case 6:
          // Measurement Type by default is Normal
          mode = select_measurement_mode();
          setStationMeasureMode ( project, mode );  
          go_to_screen = 2;        // if not, stay on enter name screen.
          break;            
    }
  }
}
/******************************************************************************
 *
 *  Name: getProjectIndex (  )
 *
 *  PARAMETERS:  Name for project selected
 *
 *  DESCRIPTION: Allows user to select a project for review.normal_data
 *
 *
 *  RETURNS: 00 if selected or error (17)
 *
 *****************************************************************************/
uint16_t getProjectIndex ( char * project )
{
  uint16_t i = 0, display_index = 1, project_count;
  enum buttons button;
  for ( i = 0; i < PROJ_NAME_LENGTH; i++ )
  {
    project[i] = '\0';
  }
  SDstart(); // enable power the SD card
  project_count = getProjectNumber() ;
  CURSOR_OFF;
  if ( project_count == 0 )
  {
    no_stored_projects_text();  //TEXT// display "    No Projects\n      Stored" LINE2,3
    hold_buzzer();
    SDstop(null );
    delay_ms(1000);
    return ( 17);
  }
   //TEXT// display "   Select Project\n    From List" LINE2,3
  select_from_list_text(0);
  hold_buzzer();
  delay_ms(1000);
  while(1)
  {
  LCD_position(LINE1);
  _LCD_PRINT ("                    ");
  LCD_position(LINE1);
  // Get project name
  SD_FindFile  (  display_index, "\\Project\\", project, false );
  sprintf ( lcdstr, "%u. %s",display_index, project );
  LCD_print (lcdstr);
  up_down_ENTER_select_text();
  while(1)
  {
    button = getKey ( TIME_DELAY_MAX );
    if((button <= 11) || (button == ESC))
    {
      break;
    }
  }
  if ( button == ESC )
  {
    SDstop(null );
    return (17);
  }
  if ( button == UP )
  {
    --display_index;
    if ( display_index == 0 )
    {
      display_index = project_count ;
    }
  }
  else if(button == DOWN)
  {
    if(++display_index > project_count)
    {
      display_index = 1;
    }
  }
  else if(button == ENTER )
  {
   // Project XXXX Selected
   CLEAR_DISP;
   LCD_position(LINE1);
   sprintf ( lcdstr, "%s ", project );
   LCD_print (lcdstr);
   LCD_position(LINE2);
   LCD_print ( "    Selected");
   CyDelay ( 2000 );
   SDstop(null );
   return 0; // file selected
  }
 }// end while(1)
  SDstop(null );
 return (17);
}
/******************************************************************************
 *
 *  Name: getStation (  )
 *
 *  PARAMETERS: Project
 *
 *  DESCRIPTION: Allows user to select a station for review.normal_data
 *
 *
 *  RETURNS: station index or error (17)
 *
 *****************************************************************************/
uint32_t getStation ( char * project )
{
  uint16 station_count;
  uint16_t display_index;
  char station[PROJ_NAME_LENGTH] = NULL_NAME_STRING;
  enum buttons button;
  FS_FILE* nfile = null;
  if ( SD_CheckIfProjExists ( project ) == false )
  {
    return 17;
  }
  // open the file for read,write
  nfile = SDProjOpen ( project );
  if ( nfile == null )
  {
    return 17;
  }
  //read number of stations from eeprom
  station_count = getStationNumber ( project );
  if ( station_count == 0 )
  {
    no_data_stored_text();  //TEXT// display "   No Data Stored"  LINE2
    hold_buzzer();
    delay_ms(1000);
    return(17);
  }
  display_index = 0;
  select_from_list_text(1); //TEXT// display "   Select Station\n     From List"  LINE2,3
  hold_buzzer();
  delay_ms ( 1000 );
  while(1)
  {
    LCD_position(LINE1);
    _LCD_PRINT ("                    ");
    LCD_position(LINE1);
    // Get station name and display it
    getStationName (  project, station , display_index );
    sprintf (lcdstr, "%s", station );
    LCD_print ( lcdstr );
    up_down_ENTER_select_text();
    while(1)
    {
      button = getKey(TIME_DELAY_MAX);
      if((button <= 11) || (button == ESC) || (button == ENTER ) )
      {
        break;
      }
    }
    if(button == ESC)
    {
      return(17);
    }
    if(button == UP)
    {
      if(display_index == 0)
      {
        display_index = station_count-1;
      }
      else
      {
        display_index--;
      }
    }
    else if(button == DOWN)
    {
      if( display_index >= ( station_count - 1 ))
      {
        display_index = 0;
      }
      else
      {
       display_index++;
      }
    }
   else if(button == ENTER )
   {
     // Display the station name ans return the station number
     CLEAR_DISP;
     _LCD_PRINTF ( "Station : %s", station );
     delay_ms(1500);
     return ( display_index );
   }
 }
}
/******************************************************************************
 *
 *  Name: select_stored_project ( uint8_t function )
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *  //select proj. (0=resume,1=delete,2=display)
 *
 *  RETURNS: selected station number of selected project, and proj name, or 17 is esc
 *
 *****************************************************************************/
uint16_t select_stored_project ( uint8_t function, char* prj_name )
{
  uint16_t  project_count, selection, display_index, station_count ;
  char station[PROJ_NAME_LENGTH] = NULL_NAME_STRING;
  enum buttons button;
  project_count = getProjectNumber() ;
  CURSOR_OFF;
  if ( project_count == 0 )
  {
    no_stored_projects_text();  //TEXT// display "    No Projects\n      Stored" LINE2,3
    hold_buzzer();
    SDstop(null);
    delay_ms(1000);
    return ( 17);
  }
  //TEXT// display "   Select Project\n    From List" LINE2,3
  select_from_list_text(0);
  hold_buzzer();
  delay_ms(1000);
  display_index = 1;
  while(1)
  {
      LCD_position(LINE1);
      _LCD_PRINT ("                    ");
      LCD_position(LINE1);
      // Get project name
      SD_FindFile (  display_index, "\\Project\\", prj_name, false );
      sprintf ( lcdstr, "%s", prj_name );
      station_count = getStationNumber ( prj_name );
      SD_FindFile (  display_index, "\\Project\\", prj_name, false );
      sprintf ( lcdstr, "%s", prj_name );
      station_count = getStationNumber ( prj_name );
      LCD_print (lcdstr);
      up_down_ENTER_select_text();
      while(1)
      {
        button = getKey ( TIME_DELAY_MAX );
        if ( (button == ESC) || (button == ENTER) || ( button == UP ) || ( button == DOWN ) )
        {
          break;
        }
      }
      if ( button == ESC )
      {
        return (17);
      }
      if ( button == UP )
      {
        display_index --;
        if ( display_index == 0 )
        {
          display_index = project_count;
        }
      }
      else if(button == DOWN)
      {
        if(++display_index >  project_count )
        {
          display_index = 1;
        }
      }
      else if ( button == ENTER )
      {
////////////////////////////////////////////////////////////////////////////////
        if ( function == 0)  //resume old project
        {
          hold_buzzer ( );
          delay_ms ( 1000 );
          if( !Features.auto_store_on && (function == 0) )  //auto store is not enabled, ask if they want to enable it
          {
            button = enable_disable_features('O');
          }
          setActiveProjectEE ( prj_name ); // Put the active project name into EEPROM
          return(0);
        }
        //////////////////////////////////////////////////////////////////////////
        else if ( function == 1 ) //delete selected project or select project for USB write
        {
          return ( true );
        }
        /////////////////////////////////////////////////////////////////////////////
        else if ( function == 2 )  // select project for display
        {          //read number of stations
          station_count = getStationNumber ( prj_name );
          if ( station_count == 0 )
          {
            no_data_stored_text();  //TEXT// display "   No Data Stored"  LINE2
            hold_buzzer();
            delay_ms(1000);
            return(17);
          }
          display_index = 0;
          select_from_list_text(1); //TEXT// display "   Select Station\n     From List"  LINE2,3
          hold_buzzer();
          delay_ms ( 1000 );
          while(1)
          {
            LCD_position(LINE1);
            _LCD_PRINT ("                    ");
            LCD_position(LINE1);
            // Get station name and display it
            getStationName ( prj_name, station , display_index++ );
            sprintf (lcdstr, "%s", station );
            LCD_print ( lcdstr );
            up_down_ENTER_select_text();
            while(1)
            {
              button = getKey ( TIME_DELAY_MAX );
              if ( (button == ESC) || (button == ENTER) || ( button == UP ) || ( button == DOWN ) )
              {
                break;
              }
            }
            if ( button == ESC )
            {
               return(17);
            }
            if(button == UP)
            {
              if(display_index == 0)
              {
                display_index = station_count-1;
              }
            }
            else if(button == DOWN)
            {
              if(++display_index >= station_count)
              {
                display_index = 0;
              }
            }
           else if ( button == ENTER ) // maximum of 50 stations
           {
             selection = display_index;
             // Display the station number
             CLEAR_DISP;
             getStationName ( prj_name, station , selection );
             _LCD_PRINTF ( "Station : %s", station );
             delay_ms(500);
             //return offset into EEPROM of the selected station
             // station index starts at 0 in EEPROM, so decrement by 1
             return( selection );  // pass station number
           }
         }
        }
      }
    }
 return(0);
}


/******************************************************************************
 *
 *  Name: select_stored_project ( uint8_t function )
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *  //select proj. (0=resume,1=delete,2=display)
 *
 *  RETURNS: selected station number of selected project, and proj name, or 17 is esc
 *
 *****************************************************************************/
uint16_t select_stored_project_for_review ( uint8_t function, char* prj_name )
{
  uint16_t  project_count, selection, display_index, station_count ;
  char station[PROJ_NAME_LENGTH] = NULL_NAME_STRING;
  enum buttons button;
  project_count = getProjectNumber() ;
  CURSOR_OFF;
  if ( project_count == 0 )
  {
    no_stored_projects_text();  //TEXT// display "    No Projects\n      Stored" LINE2,3
    hold_buzzer();
    SDstop(null);
    delay_ms(1000);
    return ( 17);
  }
  //TEXT// display "   Select Project\n    From List" LINE2,3
  select_from_list_text(0);
  hold_buzzer();
  delay_ms(1000);
  display_index = 1;
  while(1)
  {
      LCD_position(LINE1);
      _LCD_PRINT ("                    ");
      LCD_position(LINE1);
      // Get project name
      SD_FindFile (  display_index, "\\Project\\", prj_name, false );
      sprintf ( lcdstr, "%s", prj_name );
      station_count = getStationNumber ( prj_name );
      SD_FindFile (  display_index, "\\Project\\", prj_name, false );
      sprintf ( lcdstr, "%s", prj_name );
      station_count = getStationNumber ( prj_name );
      LCD_print (lcdstr);
      up_down_ENTER_select_text();
      while(1)
      {
        button = getKey ( TIME_DELAY_MAX );
        if ( (button == ESC) || (button == ENTER) || ( button == UP ) || ( button == DOWN ) )
        {
          break;
        }
      }
      if ( button == ESC )
      {
        return (17);
      }
      if ( button == UP )
      {
        display_index --;
        if ( display_index == 0 )
        {
          display_index = project_count;
        }
      }
      else if(button == DOWN)
      {
        if(++display_index >  project_count )
        {
          display_index = 1;
        }
      }
      else if ( button == ENTER )
      {
////////////////////////////////////////////////////////////////////////////////
        if ( function == 0)  //resume old project
        {
          hold_buzzer ( );
          delay_ms ( 1000 );
          setActiveProjectEE ( prj_name ); // Put the active project name into EEPROM
          return(0);
        }
        //////////////////////////////////////////////////////////////////////////
        else if ( function == 1 ) //delete selected project or select project for USB write
        {
          return ( true );
        }
        /////////////////////////////////////////////////////////////////////////////
        else if ( function == 2 )  // select project for display
        {          //read number of stations
          station_count = getStationNumber ( prj_name );
          if ( station_count == 0 )
          {
            no_data_stored_text();  //TEXT// display "   No Data Stored"  LINE2
            hold_buzzer();
            delay_ms(1000);
            return(17);
          }
          display_index = 0;
          select_from_list_text(1); //TEXT// display "   Select Station\n     From List"  LINE2,3
          hold_buzzer();
          delay_ms ( 1000 );
          while(1)
          {
            LCD_position(LINE1);
            _LCD_PRINT ("                    ");
            LCD_position(LINE1);
            // Get station name and display it
            getStationName ( prj_name, station , display_index++ );
            sprintf (lcdstr, "%s", station );
            LCD_print ( lcdstr );
            up_down_ENTER_select_text();
            while(1)
            {
              button = getKey ( TIME_DELAY_MAX );
              if ( (button == ESC) || (button == ENTER) || ( button == UP ) || ( button == DOWN ) )
              {
                break;
              }
            }
            if ( button == ESC )
            {
               return(17);
            }
            if(button == UP)
            {
              if(display_index == 0)
              {
                display_index = station_count-1;
              }
            }
            else if(button == DOWN)
            {
              if(++display_index >= station_count)
              {
                display_index = 0;
              }
            }
           else if ( button == ENTER ) // maximum of 50 stations
           {
             selection = display_index;
             // Display the station number
             CLEAR_DISP;
             getStationName ( prj_name, station , selection );
             _LCD_PRINTF ( "Station : %s", station );
             delay_ms(500);
             //return offset into EEPROM of the selected station
             // station index starts at 0 in EEPROM, so decrement by 1
             return( selection );  // pass station number
           }
         }
        }
      }
    }
 return(0);
}

/******************************************************************************
 *  Name:
 *  PARAMETERS:
 *  DESCRIPTION:
 *****************************************************************************/
void delete_projects(void)  // allows user to delete one or all project(s)
{
  Bool escape = 0;
  uint8_t go_to_screen = 0;
  char selected_project_name [ PROJ_NAME_LENGTH] ;
  enum buttons button;
  uint8 esc_key;
 // char project [] = "TEMP";
  //unsigned int32 selected_project;
 // char temp_str[PROJ_NAME_LENGTH] = NULL_NAME_STRING;
  SD_Wake();
  updateProjectInfo();
  if ( project_info.number_of_projects == 0 )
  {
   no_stored_projects_text();  //TEXT// display "    No Projects\n     Stored" LINE2,3
   hold_buzzer();
    delay_ms(1500);
    return;
  }
  while ( !escape )
  {
    CLEAR_DISP;
    switch ( go_to_screen )
    {
      case 0:
            delete_data_text();     //TEXT// display "Delete Data\n1. Delete All Data\n2. Delete One Proj." LINE1,2,3
            up_down_select_text(0); //TEXT// display "Select #, ESC Exit"
            while(1)
            {
              button = getKey ( TIME_DELAY_MAX );
              if((button == 1) || (button == 2) || (button == ESC))
              {
                break;
              }
            }
            if(button == ESC)
            {
              escape = TRUE;
            }
            else if ( button == 1 )
            {
              go_to_screen = 1;  //go to delete all data screen set
            }
            else
            {
              go_to_screen = 4;  //go to select_stored_project(delete)
            }
            break;
      case 1: // ASk to Delete all data from project storage
            enter_to_delete_text();  //TEXT// display "  Press ENTER to\n  Delete All Data"  LINE2,3
            while(1)
            {
              button = getKey ( TIME_DELAY_MAX );
              if((button == ESC) || (button == ENTER))
              {
                break;
              }
            }
            if(button == ESC)
            {
              escape = TRUE;
            }
            else
            {
              go_to_screen = 2;
            }
            break;
      case 2: // Ask to Delete All Data
            erase_data_text();  //TEXT// display "Erase All Data\nFrom Memory?\nYES to Continue"
            ESC_to_Exit(LINE4); //TEXT// display "ESC to Exit"
            while(1)
            {
              button = getKey ( TIME_DELAY_MAX );
              if((button == YES) || (button == ESC))
              {
                break;
              }
            }
            if ( button == ESC )
            {
              escape = TRUE;
            }
            else
            {
              go_to_screen = 3;
            }
            break;
      case 3: // Delete Data after asking twice
              if ( resetProjectStorage ( ) == 0 )
              {
                all_data_erased_text();  //TEXT// display "      All Data\n       Erased" LINE2,3
                hold_buzzer();
              }
              updateProjectInfo();
              Features.auto_store_on = FALSE;
              NV_MEMBER_STORE( FEATURE_SETTINGS, Features ); // makes user reselect a new proj.
              delay_ms(2000);
              escape = TRUE;
              break;
      case 4: //select single project to delete
              esc_key = select_stored_project( 1, selected_project_name );  //arg 1 returns index of selected project
              if ( esc_key == 17 )  //esc was pressed, 17 is not a valid vector
              {
                return;
              }
              delete_project_text(selected_project_name);  //TEXT// display "Press ENTER to\nDelete Data In\nProject %s",temp_str LINE1,2,3
              ESC_to_Exit(LINE4);             //TEXT// display "ESC to Exit"
              while(1)
              {
                button = getKey ( TIME_DELAY_MAX );
                if((button == ESC) || (button == ENTER))
                {
                  break;
                }
              }
              if(button == ESC)
              {
                escape = TRUE;
              }
              else
              {
                go_to_screen = 5;   // go delete the project
              }
              break;
      case 5: // Ask again to delete the project
            erase_project_data_text();  //TEXT// display "Erase Project Data?\nYES to Continue" LINE1,3
            ESC_to_Exit(LINE4);
            while(1)
            {
              button = getKey ( TIME_DELAY_MAX );
              if((button == YES) || (button == ESC))
              {
                break;
              }
            }
            if(button == ESC)
            {
              escape = TRUE;
            }
            else
            {
              clearProject ( selected_project_name );
              updateProjectInfo();
              Features.auto_store_on = FALSE;
              escape = TRUE;
              NV_MEMBER_STORE( FEATURE_SETTINGS, Features );// makes user reselect a new proj.
            }
            break;
    }
  }
}
/******************************************************************************
 *
 *  Name:
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *
 *
 *  RETURNS:
 *
 *****************************************************************************/
void review_data(void)  // computes and displays stored data
{
  uint8_t  display_set = 1, LCD_offset = 15;
  uint16_t index;
  int32_t location;
  char proj [PROJ_NAME_LENGTH];
  float per_MA,  unit_density;
  char younit[7] = " PCF";
  char temp_str[20];
  station_data_t review;
  enum buttons button;
  #define REVIEW_SCREENS 3
  
  SD_Wake();
  // Ask the user to select a project
  index = select_stored_project_for_review ( 0, proj );
  if ( index == 17)  // no project selected
  {
     return;
  }
  
  while ( 1 )
  {
    // get a station from the project
    location = getStation ( proj );
    if ( location == 17)  // no project selected
    {
     return;
    }
    // This takes a pointer to a station stuct in EEPROM and copies it into RAM
    readStation ( proj, location, &review );

    if ( review.units == KG_M3 )
    {
      sprintf(younit," kg/m3");
      LCD_offset = 13;
    } else if ( review.units == PCF)
    {
      sprintf(younit," PCF");
      LCD_offset = 15;
    } else
    {
      sprintf(younit," GCC");
      LCD_offset = 15;
    }
////////////////////////Display data//////////////////////////////////////////////////////
    while(1)                     //display data until button is pressed to exit
    {
      CLEAR_DISP;
      switch(display_set)
      {
        case 1:
            LCD_position (LINE1);
            sprintf(temp_str,"%lu",(unsigned long) review.density1_count);
            count_text(0);  //TEXT// display "D1 Count";
            print_string_backward(temp_str,LINE1+LCD_offset);
            LCD_position (LINE2);
            sprintf(temp_str,"%lu",(unsigned long) review.density2_count);
            count_text(1);  //TEXT// display "D2 Count";
            print_string_backward(temp_str,LINE2+LCD_offset);
           
            LCD_position (LINE3);
            sprintf(temp_str,"%.2f", (float)review.density1_count / (float)review.density1_stand);
            count_text(2);  //TEXT// display "    DCR1:"
            print_string_backward(temp_str,LINE3+LCD_offset);
            
            LCD_position (LINE4);
            sprintf(temp_str,"%.2f", (float)review.density2_count / (float)review.density2_stand);
            count_text(3);  //TEXT// display "    DCR2:"
            print_string_backward(temp_str,LINE4+LCD_offset);
            break;
        case 2:
            unit_density = convertKgM3DensityToUnitDensity ( review.density, review.units );
            if ( review.units != GM_CC )
            {
              sprintf(temp_str,"%3.1f", unit_density);
            }
            else
            {
              sprintf(temp_str,"%.3f", unit_density);
            }
            strcat(temp_str,younit);
            count_text(4);  //TEXT// display "   DT:"
            print_string_backward(temp_str,LINE1+19);
            if ( review.MA != 0.0 )
            {
              per_MA = (review.density/review.MA)*100;
            }
            else
            {
              per_MA = 0.0;
            }
            
            checkFloatLimits( & per_MA );
            LCD_position (LINE2);
            sprintf(temp_str,"%.1f",(float)per_MA);
            count_text(5);  //TEXT// display "   %%MA:"
            print_string_backward(temp_str,LINE2+LCD_offset);
            
            LCD_position (LINE3);
            sprintf(temp_str,"%.1f",100-(float)per_MA);
            count_text(6);  //TEXT// display ""Voids:"
            print_string_backward(temp_str,LINE3+LCD_offset);
            
            LCD_position (LINE4);
            
           if ( review.units == PCF)
           {
            sprintf(temp_str,"%.2f in.",(float)review.thickness/25.4);
           }
           else
           {
             sprintf(temp_str,"%.0f ",(float)review.thickness);
           }
            count_text(7);  //TEXT// display "Thickness:"
            print_string_backward(temp_str,LINE4+19);          
           break;

       case 0:
            LCD_position(LINE1);
            printTimeDate(review.date );
            snprintf( temp_str,20,"LAT: %9.6f",review.gps_read.latitude); //gdata.latitude);
            LCD_PrintAtPosition ( temp_str, LINE2 );
            snprintf( temp_str,20,"LNG:%9.6f", review.gps_read.longitude); //gdata.longitude);
            LCD_PrintAtPosition ( temp_str, LINE3 );
            snprintf( temp_str,20,"ALT:%u SAT:%u ",review.gps_read.altitude,review.gps_read.sats); // gdata.altitude, gdata.sats);
            LCD_PrintAtPosition ( temp_str, LINE4 );
            break;
      } 
      
     while(1)
     {
       button = getKey ( 50 );
       if((button == ESC) || (button == UP) || (button == DOWN) )
       {
         break;
       }
     }
    
     if( button == DOWN )  // down button
     {
       display_set = (display_set + 1) % REVIEW_SCREENS;
     }
     else if(button == UP)                       // up button
     {
       display_set = ( display_set + REVIEW_SCREENS - 1 ) % REVIEW_SCREENS;
     }
     else  //button == ESC exit function
     {
       break;
     }
    }
  }
}
/******************************************************************************
 *
 *  Name:
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *
 *
 *  RETURNS:
 *
 *****************************************************************************/
Bool USB_write_file (  char * project , FILE_PARAMETERS * file)  // writes project info at vector to file on USB
{
  uint8_t  units_rev, i;
  uint16_t station_count;
  uint32_t d1_count_rev, d1_stand_rev, d2_count_rev, d2_stand_rev;;
  float  density_offset_rev;
  float dense_rev = 0.0;
  float unit_density;
  station_data_t review;
  uint32_t serial_number;
  char younit[8] = "\0";
  char HT[3] = ",";
  char name_rev[PROJ_NAME_LENGTH];
  char temp_val[256];
  char date_time_str[50];
  Bool pass = TRUE;
  float per_MA, MA_rev;
  
  double A1 = (double)NV_CONSTANTS ( A1 );
  double B1 = (double)NV_CONSTANTS ( B1 );
  double C1 = (double)NV_CONSTANTS ( C1 );
  
  double A2 = (double)NV_CONSTANTS ( A2 );
  double B2 = (double)NV_CONSTANTS ( B2 );
  double C2 = (double)NV_CONSTANTS ( C2 );
  
  double A11 = (double)NV_CONSTANTS ( A11 );
  double A12 = (double)NV_CONSTANTS ( A12 );
  double A13 = (double)NV_CONSTANTS ( A13 );
  
  double A21 = (double)NV_CONSTANTS ( A21 );
  double A22 = (double)NV_CONSTANTS ( A22 );
  double A23 = (double)NV_CONSTANTS ( A23 );
  
  isrTIMER_1_Disable();
  
  // get the project mode.
  readStation ( project, 0, &review );
  
  // Store the Header
  nullString( temp_str, sizeof(temp_str) );
  
 // check to see if project mode is metal,normal or profile 
 if ( review.mode != PROFILE_DENSITY_MODE )
 {
    sprintf(temp_str,"Date, Serial Number, Station, DT,Density Offset, %%MA, MA, %%Voids,D1 Count,D2 Count, DCR1, DCR2, Density1 Std Cnt, DENS2 Std Cnt, Thickness,");
    sprintf(temp_val,"Const A1, Const B1, Const C1, Const A11, Const A12, Const A13, Const A2, Const B2, Const C2, Const A21, Const A22, Const A23,LAT,LNG,ALT\r\n");
    strcat(temp_str,temp_val);
    AlfatWriteStr( file,temp_str);   //write string to USB

    // Get the number of stations to store
    station_count = getStationNumber( project );
    // Get the serial number
    serial_number = getSerialNumber ();
    // store each station
    for( i=0; i < station_count; i++ )
    {
      if ( readStation ( project, i, & review ) == -1 )
      {
       break;
      }
      //read station name into local variable
      if ( strlen(review.name) < PROJ_NAME_LENGTH  )
      {
       strcpy ( name_rev, review.name );
      }
      
      // print station number on line 4.
      LCD_PrintAtPosition ("Stat:",LINE4);
      LCD_PrintAtPosition(review.name,LINE4+6 );
      
      d1_count_rev =   review.density1_count;
      d2_count_rev =   review.density2_count;
      dense_rev    =   (float)review.density;
      d1_stand_rev =   review.density1_stand;
      d2_stand_rev =   review.density2_stand;
      units_rev    =   review.units;
      MA_rev       =   review.MA;

      density_offset_rev =  review.den_off;
      
      if (units_rev == KG_M3 )  // SI units, printf kg/m3
      {
        sprintf(younit," kg/m3,");
      }
      else if ( units_rev == PCF )
      {
        sprintf(younit," PCF,");
      }
      else
      {
        sprintf(younit," GCC,");
      }
      
      // Date
      // get the time/date and make a string
      getTimeDateStr ( review.date, date_time_str );
      nullString( temp_str, sizeof(temp_str) );
      strcpy ( temp_str,date_time_str );
      strcat ( temp_str,HT );
     
      // Serial Number
      sprintf( temp_val,"%lu",(unsigned long)serial_number );
      strcat(temp_str,temp_val);
      strcat(temp_str,HT);
      
      // STATION
      strcat(temp_str,name_rev);
      strcat(temp_str,HT);
   
      // DENSITY
      checkFloatLimits ( & dense_rev );
      unit_density = convertKgM3DensityToUnitDensity ( dense_rev, units_rev );
      if ( units_rev != GM_CC )
      {
       sprintf(temp_val,"%3.1f", unit_density);
      }
      else
      {
        sprintf(temp_val,"%.3f", unit_density);
      }
      strcat ( temp_str, temp_val );
      strcat ( temp_str, younit );
      
      // send the offsets
      // Density Offset to proper units
      unit_density = convertKgM3DensityToUnitDensity ( density_offset_rev, units_rev );
      checkFloatLimits ( & unit_density );
      if ( units_rev != GM_CC )
      {
       sprintf(temp_val,"%3.1f", unit_density);
      }
      else
      {
        sprintf(temp_val,"%.3f", unit_density);
      }
      strcat ( temp_str, temp_val );
      strcat ( temp_str, younit );
 
      
      // %MA
      per_MA = (dense_rev / MA_rev) * 100;
      checkFloatLimits ( & per_MA );
      sprintf(temp_val,"%.2f %%,", per_MA);
      strcat(temp_str,temp_val);
      
      // MA
      checkFloatLimits ( & MA_rev );
      unit_density = convertKgM3DensityToUnitDensity ( MA_rev, units_rev );
      if ( units_rev != GM_CC )
      {
        sprintf(temp_val,"%3.1f", unit_density);
      }
      else
      {
        sprintf(temp_val,"%.3f", unit_density);
      }
      strcat ( temp_str, temp_val );
      strcat ( temp_str, younit );
      
      // %VOIDS
      sprintf(temp_val,"%.2f,",100 - per_MA);
      strcat(temp_str,temp_val);
      
      // DENSITY COUNT
      sprintf(temp_val,"%u,",(unsigned int)d1_count_rev);
      strcat(temp_str,temp_val);

      sprintf(temp_val,"%u,",(unsigned int)d2_count_rev);
      strcat(temp_str,temp_val);
      
       // DCR
       sprintf(temp_val,"%.4f,", (float)d1_count_rev / (float)d1_stand_rev);
       strcat(temp_str,temp_val);
       sprintf(temp_val,"%.4f,", (float)d2_count_rev / (float)d2_stand_rev);
       strcat(temp_str,temp_val);

      // DENSITY STD COUNT
      sprintf(temp_val,"%.0f,", (float)d1_stand_rev );
  		strcat(temp_str,temp_val);
      sprintf(temp_val,"%.0f,", (float)d2_stand_rev );
  		strcat(temp_str,temp_val);
      
      // Thickness
      if ( review.units == PCF)
      {
        sprintf(temp_val,"%.2f in.,",(float)review.thickness/25.4);
      }
      else
      {
        sprintf(temp_val,"%.0f,",(float)review.thickness);
      }
      strcat(temp_str,temp_val);
    
      // store the row of data
      AlfatWriteStr( file,temp_str);   //write string to USB

      // DENSITY A1 CONST
      sprintf(temp_str,"%.5f,", A1 );
      // DENSITY B1 CONST
      sprintf(temp_val,"%.5f,", B1 );
  		strcat(temp_str,temp_val);
      // DENSITY C1 CONST
      sprintf(temp_val,"%.5f,", C1 );
      strcat(temp_str,temp_val);
      AlfatWriteStr( file,temp_str);   //write string to USB
      
      // DENSITY A11 CONST
      sprintf(temp_str,"%.5f,", A11 );
     
      // DENSITY A12 CONST
      sprintf(temp_val,"%.5f,", A12 );
      strcat(temp_str,temp_val);    
      
     // DENSITY A13 CONST
      sprintf(temp_val,"%.5f,", A13 );
      strcat(temp_str,temp_val);    
      AlfatWriteStr( file,temp_str);   //write string to USB

     // DENSITY A2 CONST
      sprintf(temp_str,"%.5f,", A2 );
      // DENSITY B2 CONST
      sprintf(temp_val,"%.5f,", B2 );
  		strcat(temp_str,temp_val);
      // DENSITY C3 CONST
      sprintf(temp_val,"%.5f,", C2 );
      strcat(temp_str,temp_val);    
      AlfatWriteStr( file,temp_str);   //write string to USB
      
      // DENSITY A21 CONST
      sprintf(temp_str,"%.5f,", A21 );
     
      // DENSITY A22 CONST
      sprintf(temp_val,"%.5f,", A22 );
      strcat(temp_str,temp_val);    
      
      // DENSITY A23 CONST
      sprintf(temp_val,"%.5f,", A23 );
      strcat(temp_str,temp_val);    
      // store the row of data
      AlfatWriteStr( file,temp_str);   //write string to USB
     
      // Write GPS Data
      sprintf( temp_str,"%9.6f,",  review.gps_read.latitude);
      AlfatWriteStr( file,temp_str);   //write string to USB
      sprintf( temp_str,"%9.6f,",  review.gps_read.longitude );
      AlfatWriteStr( file,temp_str);   //write string to USB
      sprintf( temp_str,"%u.00",  review.gps_read.altitude );
      strcat ( temp_str, "\r\n");
      // store the row of data
      AlfatWriteStr( file,temp_str);   //write string to USB
    }
  }
  else
  {
   sprintf(temp_str,"Date, Serial Number, Station, DT[.5],DT[.75],DT[1.00],DT[1.25],DT[1.50],DT[1.75],DT[2.00],DT[2.25],DT[2.50],DT[2.75],DT[3.00],DT[3.25],DT[3.50],DT[3.75],DT[4.00],");               
   sprintf(temp_val, "Density Offset,D1 Count,D2 Count, DCR1, DCR2, Density1 Std Cnt, DENS2 Std Cnt\r\n");
   strcat(temp_str,temp_val);    
   AlfatWriteStr( file,temp_str);
  
    // Get the number of stations to store
    station_count = getStationNumber( project );
    // Get the serial number
    serial_number = getSerialNumber ();
    // store each station
    for( i=0; i < station_count; i++ )
    {
      if ( readStation ( project, i, & review ) == -1 )
      {
       break;
      }
      //read station name into local variable
      if ( strlen(review.name) < PROJ_NAME_LENGTH  )
      {
       strcpy ( name_rev, review.name );
      }
      
      // print station number on line 4.
      LCD_PrintAtPosition ("Stat:",LINE4);
      LCD_PrintAtPosition(review.name,LINE4+6 );
      
      d1_count_rev =   review.density1_count;
      d2_count_rev =   review.density2_count;
     // dense_rev    =   review.density;
      
      d1_stand_rev =   review.density1_stand;
      d2_stand_rev =   review.density2_stand;
      units_rev    =   review.units;
      MA_rev       =   review.MA;

      density_offset_rev =  review.den_off;
      
      if (units_rev == KG_M3 )  // SI units, printf kg/m3
      {
        sprintf(younit," kg/m3,");
      }
      else if ( units_rev == PCF )
      {
        sprintf(younit," PCF,");
      }
      else
      {
        sprintf(younit," GCC,");
      }
      
      // Date
      // get the time/date and make a string
      getTimeDateStr ( review.date, date_time_str );
      nullString( temp_str, sizeof(temp_str) );
      strcpy ( temp_str,date_time_str );
      strcat ( temp_str,HT );
     
      // Serial Number
      sprintf( temp_val,"%lu",(unsigned long)serial_number );
      strcat(temp_str,temp_val);
      strcat(temp_str,HT);
      
      // STATION
      strcat(temp_str,name_rev);
      strcat(temp_str,HT);
      AlfatWriteStr( file,temp_str);
      
      // PROFILE DENSITIES
      for ( uint8 profile = 0; profile < PROFILE_DENSITIES; profile++ )
      {
     
        unit_density = convertKgM3DensityToUnitDensity ( (float)review.DT[profile], units_rev );
        if ( units_rev != GM_CC )
        {
         sprintf(temp_str,"%3.1f", unit_density);
        }
        else
        {
          sprintf(temp_str,"%.3f", unit_density);
        }
        strcat ( temp_str, younit );
        AlfatWriteStr( file,temp_str);
      }
      
      // send the offsets
      // Density Offset to proper units
      unit_density = convertKgM3DensityToUnitDensity ( density_offset_rev, units_rev );
      if ( units_rev != GM_CC )
      {
       sprintf(temp_str,"%3.1f", unit_density);
      }
      else
      {
        sprintf(temp_str,"%.3f", unit_density);
      }
       strcat ( temp_str, younit );
  
      
      // DENSITY COUNT
      sprintf(temp_val,"%u,",(unsigned int)d1_count_rev);
      strcat(temp_str,temp_val);

      sprintf(temp_val,"%u,",(unsigned int)d2_count_rev);
      strcat(temp_str,temp_val);
      
       // DCR
       sprintf(temp_val,"%.4f,", (float)d1_count_rev / (float)d1_stand_rev);
       strcat(temp_str,temp_val);
       sprintf(temp_val,"%.4f,", (float)d2_count_rev / (float)d2_stand_rev);
       strcat(temp_str,temp_val);

      // DENSITY STD COUNT
      sprintf(temp_val,"%.0f,", (float)d1_stand_rev );
  		strcat(temp_str,temp_val);
      sprintf(temp_val,"%.0f\r\n", (float)d2_stand_rev );
  		strcat(temp_str,temp_val);
      // store the row of data
      AlfatWriteStr( file,temp_str);   //write string to USB      
  /*    
      // Write GPS Data
      sprintf( temp_str,"%9.6f,",  review.gps_read.latitude);
      AlfatWriteStr( file,temp_str);   //write string to USB
      sprintf( temp_str,"%9.6f,",  review.gps_read.longitude );
      AlfatWriteStr( file,temp_str);   //write string to USB
      sprintf( temp_str,"%u.00",  review.gps_read.altitude );
      strcat ( temp_str, "\r\n");
      // store the row of data
      AlfatWriteStr( file,temp_str);   //write string to USB
*/      
    }  
  } 
  isrTIMER_1_Enable();
  return pass;
}
/******************************************************************************
 *
 *  Name:
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *
 *
 *  RETURNS:
 *
 *****************************************************************************/
void write_data_to_USB ( void )  // leads user though process to write project(s) to USB
{
  Bool escape = 0, scope =0;
  uint16_t go_to_screen = 0, i = 0, j;
  uint16_t location = 0;
  char name_temp[PROJ_NAME_LENGTH];
  char proj[PROJ_NAME_LENGTH];
  enum buttons button;
  Bool pass = TRUE;
  FILE_PARAMETERS  file;
  uint8 station_count;

  updateProjectInfo();
  if ( project_info.number_of_projects == 0 )
  {
    no_stored_projects_text();  //TEXT// display "    No Projects\n      Stored" LINE2,3
    hold_buzzer();
    delay_ms(2000);
    return;
  }
 // AlfatStart();    // wake up the ALFAT device to full power
  AlfatStart();
  while(!escape)
  {
    CLEAR_DISP;
    switch(go_to_screen)
    {
      case 0:
            write_USB_text();  //TEXT// display " Write Data to USB\n1. Write All Data\n2. Write One Project" LINE1,2,3
            ESC_to_Exit(LINE4);
            while(1)
            {
             button = getKey ( TIME_DELAY_MAX );
              if((button == 1) || (button == 2) || (button == ESC))
              {
                break;
              }
            }
            if(button == ESC)
            {
              escape = TRUE;
            }
            else if(button == 1)
            {
              scope = 1;
              go_to_screen = 1;
            }
            else //button == 2
            {
              scope = 0;
              go_to_screen = 2;
            }
            break;
      case 1:
            USB_text(0); // display "  Insert External\n Drive in USB Port\n     Press ENTER" on LINE1, LINE2 and LINE4
            while(1)
            {
              button = getKey ( TIME_DELAY_MAX );
              if((button == ENTER) || (button == ESC))
              {
                break;
              }
            }
            if(button == ESC)
            {
              escape = TRUE;
            }
            else //ENTER was pressed
            {
              go_to_screen = 3;
            }
            break;
      case 2:
            location = select_stored_project (1, proj);   //get memory location of project to write to USB
            if(location == 17)  // no project selected
            {
              go_to_screen = 0;
              break;
            }
            go_to_screen = 1;
            break;
      case 3:
              j = 0;
              while ( !check_for_USB() && j < 5 )
              {
               USB_text(2);  // display " No USB Device "
               delay_ms ( 1000 );
               j++;
              };
              if ( j >= 5 )
              {
                escape = TRUE;
                break;
              }
              USB_text(1);  // display "    Writing Data\n    To USB Drive" on LINE1 and LINE2
              hold_buzzer();
              // Mount the USB drive
              if ( !initialize_USB( TRUE ) )  //try to initialize USB.  if it cant initialize, escape
              {
                escape = TRUE;
                break;
              }
              if(scope == 1)  //write all data to USB
              {
                for( i=1; i <= project_info.number_of_projects; i++ )
                {
                  SD_FindFile (  i, "\\Project\\", proj, false );
                  strcpy ( name_temp, proj );
                  LCD_PrintBlanksAtPosition ( 20, LINE3 );
                  LCD_PrintAtPositionCentered ( name_temp , LINE3 + 10 );
                  //read number of stations
                  station_count = getStationNumber ( proj );
                  if ( station_count == 0 )
                  {
                    no_data_stored_text();  //TEXT// display "   No Data Stored"  LINE2
                    hold_buzzer();
                    delay_ms(2000);
                  }
                  else
                  {
                    // send project
                    pass = USB_open_file ( name_temp, & file);
                    if ( pass == TRUE )
                    {
                       pass = USB_write_file(name_temp, &file);   //write project data to the USB drive
                       if ( pass == TRUE )
                       {
                        // close file
                        AlfatFlushData( file.fileHandle );
                        AlfatCloseFile( file.fileHandle );
                       }
                    }
                  } 
                 // If wrting to the USB fails, break out of the for loop
                 if ( pass == FALSE )
                 {
                  break;
                 }
                }
              }
              else
              {
                FILE_PARAMETERS fp;
                //read number of stations
                station_count = getStationNumber ( proj );
                if ( station_count == 0 )
                {
                  no_data_stored_text();  //TEXT// display "   No Data Stored"  LINE2
                  hold_buzzer();
                  delay_ms(2000);
                  escape = TRUE;
                }
                else
                {
                  //read project name to variable
                  pass = USB_open_file  ( proj, &fp );
                  if ( pass == TRUE )
                  {
                    LCD_PrintBlanksAtPosition ( 20, LINE3 );
                    LCD_PrintAtPositionCentered ( proj , LINE3 + 10 );
                    pass = USB_write_file ( proj, &fp );   //write project data to the USB drive
                    if ( pass == TRUE )
                    {
                     // close file
                     AlfatFlushData(fp.fileHandle);
                     AlfatCloseFile(fp.fileHandle);   //close file
                    }
                  }
                } 
              }
             if ( pass == FALSE )
             {
              escape = TRUE;
             }
             delay_ms(1000);
             go_to_screen = 4;
             break;
      case 4:
              USB_text(3);  // display "   Data Download\n     Complete" on LINE2 and LINE3
              delay_ms(2000);
              escape = TRUE;
              break;
     }
  }
  // Put ALFAT in reduced power mode
  //AlfatStop();
  AlfatStop();
}

/******************************************************************************
 *
 *  Name: storeStationData ( station_data_t station_d )
 *
 *  PARAMETERS: station struct.
 *
 *
 *  DESCRIPTION: Takes station data and stores it in the current project and station
 *
 *  RETURNS:
 *
 *****************************************************************************/
void storeStationData ( char * project, station_data_t station  )
{
  uint8_t lcd_line;
  FS_FILE *  SDfile = null;
  uint16 station_num;
  strcpy ( station.name, "              ");
  SD_Wake();
  // Open the project file.
  SDfile = SDProjOpen ( project );
  if ( SDfile == null )
  {
    SDstop ( null );
    return ;
  }
  // get the current station number from the open project file
  station_num = getStationNumber ( project );
  // Check to see if all station positions are full
  if ( station_num >= MAX_STATIONS )
  {
   // display "Max # of Stations\nFor %s Has\nBeen Exceeded.\nStart New Project",current_project
   max_stations_text( project );
   SDstop ( SDfile );       // stop SD card and close the opened file
   delay_ms(1500);
   return ;
  }
  CLEAR_DISP;
  LCD_position(LINE2);
  current_project_text( project );
  delay_ms(1000);
  enter_station_name_text();  //TEXT// display "Enter Station\nName:"  LINE1,2
  YES_to_Accept(LINE3);       //TEXT// display "YES to Accept"
  ESC_to_Exit(LINE4);         //TEXT// display "ESC to Exit"
  lcd_line = (Features.language_f) ? LINE2+6 : LINE2+10;
  //write entered name of station
  enter_name ( station.name, lcd_line );
  if ( getLastKey() == ESC )
  {
    SDstop ( SDfile );       // stop SD card and close the opened file
    return;
  }
  writeStation ( project, station_num, &station );
  //increment number of stations within project
  incrementStationNumber ( project );
  project_info.station_index++ ;
}
