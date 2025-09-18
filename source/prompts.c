
#include "Globals.h"
#include "DataStructs.h"
#include "LCD_drivers.h"
#include "prompts.h"

LCDSTRING const PressStart        = {  0, "Press <START>" };       // Press <START>
LCDSTRING const mYesToChange      = {  0, "  <YES> to Change  " }; 
LCDSTRING const EnterToAccept     = { 10, "<ENTER> to Accept" };   // <ENTER> to Accept
LCDSTRING const EnterValueFor     = {  0, "Enter Value for " };   
LCDSTRING const YesToAccept       = { 10, "<YES> to Accept" };   
LCDSTRING const EscToExit         = { 10, "<ESC> to Exit" };       // <ESC> to Exit
LCDSTRING const PressEnter        = { 10, "Press <ENTER>" };       // Press <ENTER>

LCDSTRING const PressStart_sp     = {  0, "Pulse <START>" };       // Press <START>
LCDSTRING const mYesToChange_sp   = {  0, "  <YES> Cambiar  " }; 
LCDSTRING const EnterToAccept_sp  = {  0, "<ENTER> para aceptar" };   // <ENTER> to Accept
LCDSTRING const EnterValueFor_sp  = {  0, "Entre el Valor de" };   
LCDSTRING const YesToAccept_sp    = { 10, "<YES> para Aceptar" };   
LCDSTRING const EscToExit_sp      = { 10, "<ESC> para Salir" };       // <ESC> to Exit
LCDSTRING const PressEnter_sp     = { 10, "Pulse <ENTER>" };       // Press <ENTER>

void Press_Start_to_Test_text ( BYTE position )
{
  LCD_position(position); 
  if(Features.language_f)
  {  _LCD_PRINT("Press START to Test "); }
   else
  {  _LCD_PRINT("    Pulse <START>   "); } 
}

void Recharge_Battery_text ( BYTE position )
{
  LCD_position(position); 
  if(Features.language_f)
  {  _LCD_PRINT("  RECHARGE BATTERY  "); }
   else
  {  _LCD_PRINT("Recargar la Bateria "); } 
}


void Press_YES_or_NO (BYTE position)
{ 
  LCD_position(position); 
  if(Features.language_f)
  {  _LCD_PRINT("Press YES or NO"); }
   else
  {  _LCD_PRINT("Pulse YES o NO"); } 
}

void ESC_to_Exit(BYTE position)
{
  LCD_position(position);
  if(Features.language_f)
  {  _LCD_PRINT(" <ESC> to Exit     "); }
   else
  { _LCD_PRINT("  <ESC> para Salir  ");}
}

void Enter_to_Accept(BYTE position)
{ 
  LCD_position(position);
  if(Features.language_f)
    _LCD_PRINT("ENTER to Accept");
   else
   _LCD_PRINT("<ENTER> para Aceptar");  

}


void YES_to_Accept(BYTE position)
{ 
  LCD_position(position);
  if(Features.language_f)
    _LCD_PRINT("YES to Accept");
   else
   _LCD_PRINT("<YES> para Aceptar");  

}

void up_down_select_text(int both)
{  
  if(both)
  {
    LCD_position(LINE3);
    if(Features.language_f)
     _LCD_PRINT("  UP/DOWN for Next  ");
     else 
     _LCD_PRINT("    Arriba/Abajo    ");       
  }
  LCD_position(LINE4);
  if(Features.language_f)
    _LCD_PRINT("Select #, ESC Exit  "); 
   else
   _LCD_PRINT("Sel #,ESC para Salir");  
   
}


void up_down_ENTER_select_text()
{  

  LCD_position(LINE2);
  if(Features.language_f)
    _LCD_PRINT(" <ENTER> to Select  "); 
  else
   _LCD_PRINT(" <ENTER> a Escoger  ");
  
  LCD_position(LINE3);
  if(Features.language_f)
    _LCD_PRINT("  UP/DOWN for Next  ");
  else 
    _LCD_PRINT("   Arriba/Abajo     ");       
  
  LCD_position(LINE4);
  if(Features.language_f)
    _LCD_PRINT("     <ESC> Exit     "); 
  else
    _LCD_PRINT("     <ESC> Salir    ");  
   
}

void display_version(void) { 

  struct features_struct  Feat        =  NV_RAM_MEMBER_RD(FEATURE_SETTINGS); 
  CLEAR_DISP;
  CtrStrC("ThinPAVE",0);
  CtrStrC("Version 1.05",LINE2);
  
  if (Feat.dummy_mode == TRUE )
  { 
    CtrStrNC("DEMO", LINE3); //LCD_PrintAtPosition("       DEMO         ",LINE3);
  }  
}

void calibration_due()
{   //213240  213128 saves 112 bytes, could free a lot of space if all are fixed
  if(Features.language_f)
  {
    CtrStrNC("Calibration Due!",LINE1);
    CtrStrNC("Last Calibration On", LINE2);
  }
  else
  {
    CtrStrNC("Require Calibracion",LINE1);
    CtrStrNC("Ultima cal. hecha en", LINE2);
  }

}

void eeprom_msg(void)
{ 
  CLEAR_DISP;    
  LCD_position (LINE2+9);
  _LCD_PRINT("EE");  
  CyDelay ( 1000 );
}


void initializing(void)
{ 
  CLEAR_DISP;    
  LCD_position (LINE2+5);
  _LCD_PRINT("Initializing");  // no spanish needed now.  Should only see on first programming
}

void main_menu_display(uint8_t menu_trk)   // display for the main menu
{  
  LCD_position(LINE1);  
  if(Features.language_f)
  {
    switch (menu_trk)
    {
      
      case 1: 
          _LCD_PRINT(" 1. Recall          ");
          LCD_position(LINE2);
          _LCD_PRINT(" 2. Offset          ");   
          break;    
      
      case 2: 
          _LCD_PRINT(" 3. Auto Scroll     ");   
          LCD_position(LINE2);
          _LCD_PRINT(" 4. Backlight       ");
          break;    
        
      case 3:    
          _LCD_PRINT(" 5. Stat Test       ");   
           LCD_position(LINE2);               
          _LCD_PRINT(" 6. Drift Test      ");  
          break;
  
      case 4:    
           _LCD_PRINT(" 7. GPS             ");
           LCD_position(LINE2);
          _LCD_PRINT(" 8. Diagnostic Test ");  
           break; 
      
      case 5:    
          _LCD_PRINT(" 9. Review STD cnts.");
          LCD_position(LINE2); 
          _LCD_PRINT("10. Select Language ");            
          break;    
          
      case 6:   
          _LCD_PRINT("11. Set Units       ");
           LCD_position(LINE2);
          _LCD_PRINT("12. Standard Mode   ");      
          break;    
     
      case 7:   
          _LCD_PRINT("13. Serial Number   ");
          LCD_position(LINE2);      
          _LCD_PRINT("14. Date/Time       ");      
          break;   
     
          
      case 8:
          _LCD_PRINT("15. Buzzer/Alarm    ");
           LCD_position(LINE2);
          _LCD_PRINT("16. Cal. Constants  ");
           break;    
          
      case 0:
          _LCD_PRINT("17. Auto Depth    ");
           LCD_position(LINE2);
          _LCD_PRINT("18. Measurement Mode");
           break;    
          
          
   }
  }
   else
   {
     switch (menu_trk)
     {
        case 1: 
             _LCD_PRINT("1. Recordar         ");     //RECALL
             LCD_position(LINE2);
             _LCD_PRINT("2. Compensacion     ");  
             break;    
       
       case 2 :
             _LCD_PRINT("3. Voluta Auto      ");         // AUTO SCROLL        
             LCD_position(LINE2);
             _LCD_PRINT("4. Iluninacion LCD  ");         // LCD LIGHT 
             break;    
       case 3: 
          
            _LCD_PRINT("5. Prueba estadist. ");      // STAT TEST
            LCD_position(LINE2);
            _LCD_PRINT("6. Prueba de deriva ");      // DRIFT TESTT   
            break;
        
      case 4:    
            _LCD_PRINT("7. GPS             ");        // GPS
            LCD_position(LINE2);
            _LCD_PRINT("8. Auto Diagnostico");       // Diagnostics            
            break;    
      case 5:    
             _LCD_PRINT("9. Revisar EST Cnts");        // review standard counts 
            LCD_position(LINE2);
            _LCD_PRINT("10. Lengua           ");       // language   
            break;        
        case 6:   
            _LCD_PRINT("11. Unidades        ");         // SET UNITS
            LCD_position(LINE2);            
            _LCD_PRINT("12. Modo AVG del EST  ");        // AVG STANDARD COUNT MODE
            break;    
        case 7:   
            _LCD_PRINT("13. Numero de Serie  ");           // Serial Number
            LCD_position(LINE2);
            _LCD_PRINT("14. Fecha y Hora     ");        // DATE TIME 
            break;   
        case 8:
            _LCD_PRINT("15. Alarma En. / Ap.");         // BUZZER ON/OFF
            LCD_position(LINE2);
            _LCD_PRINT("16. Const. de Calib.    ");   //  Cal constants
            break;  
      case 0:
          _LCD_PRINT("17. Modo profundidad");
           LCD_position(LINE2);
          _LCD_PRINT("18. Modo de Medicion");
           break;    
                      

     }
   }
  if(in_menu)
  {  up_down_select_text(1);  }
}

void project_menu_display(uint8_t menu_trk)   // display for the project menu
{
  if(Features.language_f)
  {
     switch (menu_trk)
    {
      case 1:
      LCD_position(LINE1);
      _LCD_PRINT("1. Auto Store       ");
      LCD_position(LINE2);
      _LCD_PRINT("2. Start New Project");
      break;
      
      case 2:
      LCD_position(LINE1);
      _LCD_PRINT("3. Sel. Stored Proj.");
      LCD_position(LINE2);
      _LCD_PRINT("4. Review Data      ");
      break;
      
      case 0:
      LCD_position(LINE1);
      _LCD_PRINT("5. Delete Data      ");
      LCD_position(LINE2);
      _LCD_PRINT("6. Send Data to USB ");      
      break;   
     }
   }
    else
    {
      switch (menu_trk)
      {
        case 1:
        LCD_position(LINE1);
        _LCD_PRINT("1.Auto Memoria      ");
        LCD_position(LINE2);
        _LCD_PRINT("2.Nuevo Proyecto    ");
        break;
      
        case 2:
        LCD_position(LINE1);
        _LCD_PRINT("3.Proyect Memorizado");
        LCD_position(LINE2);
        _LCD_PRINT("4.Revisar la Info.  ");
        break;
       
        case 0 :
        LCD_position(LINE1);
        _LCD_PRINT("5. Borrar la Info.   ");       
        LCD_position(LINE2);
        _LCD_PRINT("6. Trans. Info al USB");      
      }
   }
  if(in_menu)
  {  up_down_select_text(1);  }
}

void print_menu_display( void )   // display for the print menu
{
  CLEAR_DISP; 
  if(Features.language_f)
  {
    LCD_position(LINE1);
    _LCD_PRINT("1. Send Data to USB ");  
    LCD_position(LINE2);
    _LCD_PRINT("Select #, ESC Exit  "); 

  }
  else
  {
    LCD_position(LINE1);
    _LCD_PRINT("1.Trans. Info al USB");     
     LCD_position(LINE2);
     _LCD_PRINT("Sel #,ESC para Salir"); 
  }
   
}

void mode_menu_display(unsigned int menu_trk)
{
  LCD_position(LINE1);    
  switch (menu_trk)
  {
    case 1: _LCD_PRINT("1. NORMAL MODE       ");     
            LCD_position(LINE2);      
            _LCD_PRINT("2. DEMO MODE        ");
            break;
  }
  if(in_menu)
    up_down_select_text(1);  
}


void diag_menu_display( uint8_t menu_trk)
{
  LCD_position(LINE1);    
  if(Features.language_f)
  {
    switch (menu_trk)
    {
        case 1:    
            _LCD_PRINT("1. Battery Voltage  ");     
            LCD_position(LINE2);      
            _LCD_PRINT("2. Extended Counts  ");
            break;    
        case 2:    
            _LCD_PRINT("3. Check Temperature");  
            LCD_position(LINE2);          
            _LCD_PRINT("4. Memory Reset    ");
            break;   
        case 3:
            _LCD_PRINT("5. Source Pos.      ");
            LCD_position(LINE2);
            _LCD_PRINT("6. KEY PAD Test     ");      
            break;           
        case 4:
            _LCD_PRINT("7. USB Store Test   ");
            LCD_position(LINE2);
           _LCD_PRINT("8. Count Test       ");      
             break;                   
        case 5:
            _LCD_PRINT("9.  Shut Down Test  ");
            LCD_position(LINE2);
            _LCD_PRINT("10. Light Test      ");      
            break;                   
        
        case 6:
            _LCD_PRINT("11.  GPS Test       ");
            LCD_position(LINE2);
            _LCD_PRINT("12.  Update Firmware");      
            break;      
      
        case 0:
            _LCD_PRINT("13.  SD Card Test   ");
            LCD_position(LINE2);
            _LCD_PRINT("                    ");
            break;      
          
      default: break;  
    }        
  }
  else
  {
    switch (menu_trk)
    {
        case 1:    
            _LCD_PRINT("1.Volt. de la bater.");     
            LCD_position(LINE2);      
            _LCD_PRINT("2.Conteos extendidos");
            break;    
        case 2:    
            _LCD_PRINT("3.Temperatura       ");  
            LCD_position(LINE2);          
            _LCD_PRINT("4.Restablecer memor.");
            break;   
        case 3:
            _LCD_PRINT("5.Posic. de la var. ");
            LCD_position(LINE2);
            _LCD_PRINT("6.Prueba del teclado");      
            break;           
        case 4:
            _LCD_PRINT("7.Prueba USB        ");
             LCD_position(LINE2);
            _LCD_PRINT("8.Prueba de conteo  ");      
             break;                   
        case 5:
            _LCD_PRINT("9.Prueba de apagado");
            LCD_position(LINE2);
            _LCD_PRINT("10.Prueba de LED   ");      
            break;                   
        
        case 6:
            _LCD_PRINT("11.Prueba de GPS   ");
            LCD_position(LINE2);
            _LCD_PRINT("12.Actual. firmware");      
            break;      
      
        case 0:
            _LCD_PRINT("13.Prueba tarj. SD");
            LCD_position(LINE2);
            _LCD_PRINT("                    ");
            break;      
          
      default: break; 
    }
  }  
  if(in_menu)
    up_down_select_text(1);  
}




void sd_menu_display(uint8_t menu_trk)   // display for the project menu
{
  if(Features.language_f == 0)
  {
    switch (menu_trk)
    {
      case 1:
      LCD_position(LINE1);
      _LCD_PRINT("1.Prueba de tarj. SD");
      LCD_position(LINE2);
      _LCD_PRINT("2.Formatear tarj. SD");
      break;
      
      case 2:
      LCD_position(LINE1);
      _LCD_PRINT("3.Eliminar director.");
      LCD_position(LINE2);
      _LCD_PRINT("4.Crear directorio  ");
      break;
      
      case 0:
      LCD_position(LINE1);
      _LCD_PRINT("5.Anadar/elim. arch.");
      LCD_position(LINE2);
      _LCD_PRINT("                    ");
      break;
    }
  }
  else
    switch (menu_trk)
    {
      case 1:
      LCD_position(LINE1);
      _LCD_PRINT("1. SD Card Test     ");
      LCD_position(LINE2);
      _LCD_PRINT("2. Format SD Card   ");
      break;
      
      case 2:
      LCD_position(LINE1);
      _LCD_PRINT("3. Delete Proj. Dir.");
      LCD_position(LINE2);
      _LCD_PRINT("4. Create Proj. Dir.");
      break;
      
      case 0:
      LCD_position(LINE1);
      _LCD_PRINT("5. Add/Delete Files ");
      LCD_position(LINE2);
      _LCD_PRINT("                    ");
      break;
    }
  
  if (in_menu)
  {  up_down_select_text(1);  }
}


// *******************************************************************************************************************************


void ready (   )
{
  CLEAR_DISP;

  if(Features.language_f)
  {
     LCD_PrintAtPositionCentered("GAUGE READY",LINE1+10); 
  }
  else
  {
     LCD_PrintAtPositionCentered("PREPARADO",LINE1+10); 
  }

  Press_Start_to_Test_text( LINE4 ) ;
}


void display_count_time(BYTE value_temp, uint8_t position )
{
  LCD_position  (position );
  if(Features.language_f)
    {
      _LCD_PRINT("Count Time: ");
    }
    else
    {
      _LCD_PRINT("   PERIODO:");
    }
      
  if(value_temp <= 30)
  {
    if(Features.language_f)
      {
        _LCD_PRINTF( "%u sec. ",value_temp );
      
      }
      else
      {
        _LCD_PRINTF( "%u seg.",value_temp ); 
      }  
  }
  else
  {
      _LCD_PRINTF(" %u min. ",value_temp/60); 
  }
}
void select_test_or_spec_cal(int inspec)
{  
  CLEAR_DISP;  
  LCD_position(LINE1);
  if(Features.language_f)
  {
    if(!inspec)
    {  _LCD_PRINT("Select Test");  }
    else 
     { _LCD_PRINT("Select Spec. Cal"); }
  }
    else
    {
      if(!inspec)
        _LCD_PRINT("Sel. la Prueba");  
      else 
        _LCD_PRINT("Sel. Calib. Especial");  
    }
}
void new_std_count()
{  
  LCD_position (LINE3);
  if(Features.language_f)
    { _LCD_PRINT("Take New Std Count? "); }
    else
    {  _LCD_PRINT("Nueva medida?"); }
  
}


void press_start(BYTE which_test)
{
  CLEAR_DISP; 
  LCD_position (LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT ( "  Press START for   ");
    LCD_position (LINE3);
    if(which_test==0) //  stat test
    { 
      _LCD_PRINT("  20 one min counts ");  
      LCD_position ( LINE1 );
      _LCD_PRINT("      Stat Test     ");  
    }
    else if(which_test==1) //  drift test
    {
      _LCD_PRINT(" 5 four min counts  ");  
      LCD_position ( LINE1 );
      _LCD_PRINT("     Drift Test     ");  
    }
    else if (which_test==2)  // standard
    { 
       _LCD_PRINT("  Standard Count."); 
    }
    else if (which_test==3 )
    {
      _LCD_PRINT("  Count to Begin");  
    }
  }
    else
    {
      _LCD_PRINT(" Pulse Iniciar Para ");
      LCD_position (LINE3);
      if(which_test==0) //  stat test
      {  _LCD_PRINT("20m Prb estadistica"); }    
      else if(which_test==1)//   drift test
      {  _LCD_PRINT("20m Prb Desviacion"); }
      else if(which_test==2)//   standard
      { _LCD_PRINT(" Cuenta Standard"); }
      else if(which_test==3)
      {  _LCD_PRINT(" Cuenta a Iniciarse"); }
    }
}



void std_count_press_start(void)
{
  
  char temp_str[10];
  CLEAR_DISP; 
  
  
  if (  Features.SI_units ==  TRUE  )
  {
   sprintf (temp_str,"50mm");
  }
  else
  {
   sprintf(temp_str, "2in");
  }
  

  if(Features.language_f)
  {
    LCD_PrintAtPosition("Place Gauge on Poly ",LINE1);
    LCD_PrintAtPosition("Std. Block in SAFE  ",LINE2);
    LCD_PrintAtPositionCentered("Position",LINE3+10);
  	LCD_PrintAtPositionCentered("Press Start", LINE4+10 );
  }
  else
  {
    LCD_PrintAtPosition("Coloque Medidor en",LINE1);
    LCD_PrintAtPosition("bloque & barra",LINE2);
    LCD_PrintAtPosition("la posicion sequra",LINE3);
  	LCD_PrintAtPosition(" Pulse <Start>", LINE4 );
  }
    
}

void use_new_std_count()
{  
  LCD_position (LINE3);
  if(Features.language_f)
  { _LCD_PRINT("Use New STD Count?");   }
  else
  { _LCD_PRINT("Utilice nueva ESTD?"); }
    
}

void std_count_text()
{  
  LCD_position (LINE2);
  if(Features.language_f)
  { _LCD_PRINT("   Standard Count   ");   }
  else
  { _LCD_PRINT("   Conteo Estandar  "); }
    
}

void current_project_text ( char *temp_str )
{
   if(Features.language_f)
    _LCD_PRINTF("Project: %s",temp_str);
   else
   _LCD_PRINTF("Projecto: %s",temp_str); 
}

void max_stations_text(char *temp_str)
{  
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Max # of Stations");
    LCD_position(LINE2);
    _LCD_PRINTF("For %s has",temp_str);
    LCD_position(LINE3);
    _LCD_PRINT("Been Exceeded.");
    LCD_position(LINE4);
    _LCD_PRINT("Start New Project");  
  }
    else
    {
      _LCD_PRINT("Max # de Estaciones");  
      LCD_position(LINE2);
      _LCD_PRINTF("Para %s se ha",temp_str);
      LCD_position(LINE3);
      _LCD_PRINT("Execido. Inicie un");
      LCD_position(LINE4);
      _LCD_PRINT("Nuevo Proyecto.");   
    }
}
void no_project_selected ( void )
{ 
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  { 
    _LCD_PRINT("No Project Has Been");
    LCD_position(LINE2);
    _LCD_PRINT("Selected. Please");
    LCD_position(LINE3);
    _LCD_PRINT("Create or Select");
    LCD_position(LINE4);
    _LCD_PRINT("Project.");  
  }
    else
    { 
      _LCD_PRINT("Ningun Projecto se a");  
      LCD_position(LINE2);
      _LCD_PRINT("Seleccionado.       ");
      LCD_position(LINE3);
      _LCD_PRINT("Elabore o Seleccione");
      LCD_position(LINE4);
      _LCD_PRINT("un Projecto.");   
    }  
}




void calibration_date_txt ( void )
{  
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Cal Date:           ");
    
  }
    else
    {
    _LCD_PRINT("Cal Date:           ");
    }
}

void invalid_std_or_const(void)
{
  CLEAR_DISP;
  LCD_position (LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("   Invalid STD or");
    LCD_position (LINE3);
    _LCD_PRINT("   Cal. Constants");
  }
  else
  {
    _LCD_PRINT("Conteo EST const. de");  
    LCD_position (LINE3);
    _LCD_PRINT("  Calib. Invalidas");  
  }
}

void invalid_den_std_text(void)
{
  CLEAR_DISP;
  LCD_position (LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("Invalid Density Std.");
  }
  else
  {
    _LCD_PRINT("estandar dens. malo");
  }
}


void mat_thickness_error ( void )
{

  CLEAR_DISP;
  LCD_position (LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Thickness is between");
    LCD_position (LINE2);
    if ( Features.SI_units == FALSE )
    {
      _LCD_PRINT (".75 and 4 inches      ");
    }
    else
    {
     _LCD_PRINT ( "19 and 100 mm       ");
    }  
  }
  else
  {
    _LCD_PRINT("    Grueso entre    ");
    LCD_position (LINE2);
    if ( Features.SI_units == FALSE )
    {
      _LCD_PRINT (".75 y 4 inches      ");
    }
    else
    {
     _LCD_PRINT ( "195 y 100 mm       ");
    } 
  }

}



void reading(void)
{
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
   { _LCD_PRINT("Reading # "); }
    else
      _LCD_PRINT("Lectura # ");
}

void self_test_text(BYTE which_test)
{  
  LCD_position(LINE2);
  if(Features.language_f)
  {
    if(which_test < 3)  
    {
      if(which_test==0)
      {      
        _LCD_PRINT(" High Voltage Test");  
        LCD_position(LINE3);
        _LCD_PRINT("    In Progress");
      }
      else if(which_test==1)
        _LCD_PRINT("    Keypad Test   ");
      else if(which_test==2)
        _LCD_PRINT("     Tube Test    ");    
    
    }
    else 
    {     
      _LCD_PRINT("     Self Test");
      LCD_position(LINE3);
      _LCD_PRINT("      Passed");
    }
  }
    else
    {
      if(which_test < 3)  
      {
        if(which_test==0)
        {      
          _LCD_PRINT("Prueba de Alto Volt.");
          LCD_position(LINE3);
          _LCD_PRINT("    En Progreso");
        }
        else if(which_test==1)
          _LCD_PRINT(" Prueba del Teclado");  
        else if(which_test==2)
          _LCD_PRINT("  Prueba de Tubos");    
    
      }
      else 
      {     
        _LCD_PRINT("    Auto Prueba");
        LCD_position(LINE3);
        _LCD_PRINT("      Aprobada");  
      }
    }  
}
void voltage_out_of_range(float volts_temp)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINTF("   Voltage = %3.0f",(double)volts_temp);
    LCD_position(LINE2);
    _LCD_PRINT("    Out of Range");
    LCD_position(LINE4);
    _LCD_PRINT("    Test Aborted!");
  }
    else 
    {
      _LCD_PRINTF("   Voltaje = %3.0f",volts_temp);  
      LCD_position(LINE2);
      _LCD_PRINT("   Fuera de Rango");
      LCD_position(LINE4);
      _LCD_PRINT("  Pruebo Abortada!"); 
    }
}

void diagnostics_text(BYTE which_prompt)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    if(which_prompt==1)
    {
      _LCD_PRINT("Place Gauge on");
      LCD_position(LINE2);
      _LCD_PRINT("Standard Block");
      LCD_position(LINE3);
      _LCD_PRINT("in SAFE Position");
      LCD_position(LINE4);
      _LCD_PRINT("and Press START");
    }
    else if(which_prompt==2)
    {         
      LCD_position(LINE2);
      _LCD_PRINT("    Please Wait");
      LCD_position(LINE3);
      _LCD_PRINT("  Time Left 60:00");   
    }     
  }
    else
    {
      if(which_prompt==1)
      {
        _LCD_PRINT("Coloque el Equipo en");  
        LCD_position(LINE2);
        _LCD_PRINT("el Bloque Standard");
        LCD_position(LINE3);
        _LCD_PRINT("en Posicion se Segur");
        LCD_position(LINE4);
        _LCD_PRINT("Presione Inciar");
      }
      else if(which_prompt==2)
      {   
        LCD_position(LINE2);
        _LCD_PRINT("  Por Favor Espere");  
        LCD_position(LINE3);
        _LCD_PRINT("     Tiempo 60:00");   
      }     
    }
}

void USB_text(BYTE which_prompt)
{
  CLEAR_DISP; 
  LCD_position(LINE1);
  if(Features.language_f)
  {
    if(which_prompt==1)
    {   
      _LCD_PRINT("    Writing Data");
      LCD_position(LINE2);
      _LCD_PRINT("    to USB Drive");
    }
    else if(which_prompt==2)
    {   
      _LCD_PRINT("   No USB Device");  
      LCD_position (LINE3);
      _LCD_PRINT("    Detected.");
    }
    else if(which_prompt==3)
    {    
      _LCD_PRINT("   Data Download");
      LCD_position(LINE2);
      _LCD_PRINT("     Complete");    
    }
    else if(which_prompt==0)
    {
      LCD_position(LINE1);
      _LCD_PRINT("  Insert External   ");
      LCD_position(LINE2);
      _LCD_PRINT(" Drive in USB Port  ");      
      LCD_position(LINE4);
      _LCD_PRINT("    Press ENTER     ");  
    }
  }
    else
    {
      if(which_prompt==1)
      {   
        _LCD_PRINT("   Escribiendo la"); 
        LCD_position(LINE2);
        _LCD_PRINT(" Informacion al USB");  
      }
      else if(which_prompt==2)
      {   
        _LCD_PRINT("   No se Detecta"); 
        LCD_position (LINE2);
        _LCD_PRINT("        USB");  
      }
      else if(which_prompt==3)
      {    
        _LCD_PRINT("     Descarga de"); 
        LCD_position(LINE2);
        _LCD_PRINT("Informacion Completa");     
      }
      else if(which_prompt==0)
      {
        LCD_position(LINE1);
        _LCD_PRINT("Conecte el Disposit-");  // Conecte el dispositivo de memoria externa al USB Presione iniciar
        LCD_position(LINE2);
        _LCD_PRINT("ivo de Memoria");      
        LCD_position(LINE3);
        _LCD_PRINT("Externa al USB.");
        LCD_position(LINE4);
        _LCD_PRINT("Presione Inciar.");   
      }    
    }
}


void printer_text(BYTE which_prompt)
{
  CLEAR_DISP; 
  LCD_position(LINE2);
  if(Features.language_f)
  {
    if(which_prompt==0)
    {   
      LCD_position(LINE2);
      _LCD_PRINT("   Printing Data");
      LCD_position(LINE3);
      _LCD_PRINT("    Please Wait");
    }
    else if(which_prompt==1)
    {   
      LCD_position (LINE2);
      _LCD_PRINT("     No Printer");  
      LCD_position (LINE3);
      _LCD_PRINT("      Detected!");
    }
    else if(which_prompt==2)
    {   
      LCD_position(LINE1);
      _LCD_PRINT("  Connect Printer");
      LCD_position(LINE2);
      _LCD_PRINT("     To Gauge");      
      LCD_position(LINE4);
      _LCD_PRINT("    Press ENTER");  
    } 
  }
    else
     {
      if(which_prompt==0)
      {   
        LCD_position(LINE1);
        _LCD_PRINT("   Imprimiendo la");  
        LCD_position(LINE2);
        _LCD_PRINT("    Informacion");
        LCD_position(LINE3);
        _LCD_PRINT("  Por Favor Espere.");   
      }
      else if(which_prompt==1)
      {   
        LCD_position (LINE2);
        _LCD_PRINT("   No se Detecta    ");  
        LCD_position (LINE3);
        _LCD_PRINT("    Impresora!");  
      }
      else if(which_prompt==2)
      {   
        LCD_position(LINE1);
        _LCD_PRINT("Conecte la Impresora"); 
        LCD_position(LINE2);
        _LCD_PRINT("    al Equipo.");      
        LCD_position(LINE4);
        _LCD_PRINT("  Presione Iniciar");   
      } 
    }
}
void display_offset ( struct offsets_struct  which_offsets)
{
   
 LCD_position (LINE3+17);
 //_LCD_PRINT("Offset:");
     
 if ( which_offsets.den_offset_pos )
 {
  _LCD_PRINT("D");
 }
 
}

void display_offset_on ( struct offsets_struct  which_offsets)
{
   
  LCD_position (LINE2 + 12);
  if ( which_offsets.den_offset_pos )
  { 
   _LCD_PRINT("OFFSET:Y");  
  }
  else
  { 
   _LCD_PRINT("OFFSET:N");  
  } 	

}

void display_time(BYTE cnt_temp,uint8_t line)
{
  LCD_position(line);
  if(Features.language_f)
  {
    _LCD_PRINT( "Count Time:");
    _LCD_PRINTF("%3u sec. ",cnt_temp);
  }
  else
  {
    _LCD_PRINT( "    Tiempo:");
    _LCD_PRINTF("%3u seg. ",cnt_temp);
  }
 
}
void display_station_name(char *temp_str)
{
  if(Features.language_f)
   { _LCD_PRINT("Station: "); }
    else 
   {  _LCD_PRINT("Estacion: "); } 
  _LCD_PRINTF("%s",temp_str);
}


void std_text(void)
{
  if(Features.language_f)
   { _LCD_PRINT("   Standard Count   "); }
    else
   {  _LCD_PRINT("  Conteo Estandar ");}
}

void stat_text(void)
{
  if(Features.language_f)
     { _LCD_PRINT("Stat Test"); }
    else 
     { _LCD_PRINT("Prueba Estadistica"); }
}




void not_in_safe_text ( void )
{
 CLEAR_DISP;
 LCD_position (LINE1);
 if(Features.language_f)
 {
  _LCD_PRINT("Depth not at Safe    ");
  LCD_position (LINE2);
  _LCD_PRINT("Position             ");
 } 
 else 
 {
  _LCD_PRINT("Profundidad no en la");
  LCD_position (LINE2);
  _LCD_PRINT("Posicion de Segura  ");
 } 

}

void depth_not_in_bs_text ( void )
{
 CLEAR_DISP;
 LCD_position (LINE1);
 if(Features.language_f)
 {
  _LCD_PRINT("Depth not at BS     ");
  LCD_position (LINE2);
  _LCD_PRINT("Position            ");
 } 
 else 
 {
  _LCD_PRINT("profundidad no en la");
  LCD_position (LINE2);
  _LCD_PRINT("pos. de retrodisper.");
 }  

}

void drift_text(void)
{
  if(Features.language_f)
    { _LCD_PRINT("Drift Test"); }
    else
    {  _LCD_PRINT("Prueba de Desviacion");  }
}


void enable_disable_text(char *temp_str,char c)
{
  CLEAR_DISP;
  LCD_position (LINE1);
  if(Features.language_f)
  {
    switch(c)
    {
      case 'L':
      _LCD_PRINTF("%s LCD/Keypad",temp_str);
      LCD_position(LINE2);
      _LCD_PRINT("Backlight?");   
      break;
      
      case 'S':
      _LCD_PRINTF("%s Auto",temp_str);
      LCD_position(LINE2);
      _LCD_PRINT("Scroll?");  
      break;
      
      case 'D':
      _LCD_PRINTF("%s Auto",temp_str);
      LCD_position(LINE2);
      _LCD_PRINT("Depth?");   
      break;
      
      case 'A':
      _LCD_PRINTF("%s AVG STD",temp_str);
      LCD_position(LINE2);
      _LCD_PRINT("Mode?");      
      break;
      
      case 'O':
      _LCD_PRINTF("%s Auto",temp_str);
      LCD_position(LINE2);
      _LCD_PRINT("Store?");
      break;
      
      case 'B':
      _LCD_PRINTF("%s Buzzer?",temp_str);
      break;
      
      case 'G':
      _LCD_PRINTF("%s GPS?",temp_str);
      break;
      
    }    
  }
    else
    {    
      switch(c)
      {
        case 'L':
        _LCD_PRINTF("%s ",temp_str); // Encender / Apagar Iluminacion LCD
        LCD_position(LINE2);
        _LCD_PRINT("Iluminacion LCD?");   
        break;
      
        case 'S':
        _LCD_PRINTF("%s Cambia Auto",temp_str); // Habilitar / Deshabilitar Cambio Autom�tico de Pantalla
        LCD_position(LINE2);
        _LCD_PRINT("de Pantalla?");  
        break;
      
        case 'D':
        _LCD_PRINTF("%s",temp_str); // Habilitar / Deshabilitar  Profundidad Automatica
        LCD_position(LINE2);
        _LCD_PRINT("Profundidad Auto?");   
        break;
      
        case 'A':
        _LCD_PRINTF("%s Modo",temp_str); // Habilitar / Deshabilitar Modo AVG STD
        LCD_position(LINE2);
        _LCD_PRINT("AVG STD?");      
        break;
      
        case 'O':
        _LCD_PRINTF("%s Auto",temp_str);  // Habilitar / Deshabilitar Auto Memoria
        LCD_position(LINE2);
        _LCD_PRINT("Memoria?");
        break;
      
        case 'B':
        _LCD_PRINTF("%s Alarma?",temp_str);  // Habilitar / Deshabilitar Alarma
        break;
      
        case 'G':
        _LCD_PRINTF("%s GPS?",temp_str);  // Habilitar / Deshabilitar  Memoria GPS
        break;
      }    
    }
}
void reset_avg_count(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
   { _LCD_PRINT("Reset AVG Count?"); }
    else
      _LCD_PRINT("Borrar Cuentas AVG?");   //  Borrar cuentas AVG?
}

void no_data_to_store_text ( void )
{
 CLEAR_DISP;
 LCD_position (LINE1);
 if(Features.language_f)
 {
  _LCD_PRINT("  No Data To Store  ");
  LCD_position (LINE2);
  _LCD_PRINT("        Start       ");
  LCD_position (LINE3);
  _LCD_PRINT("   New Measurement  ");
 } 
 else 
 {
  _LCD_PRINT("No hay datos que almacenar");
  LCD_position (LINE2);
  _LCD_PRINT("almacenar. Iniciar");
  LCD_position (LINE3);
  _LCD_PRINT("   nueva medicion   ");
 }  

}

void stand_mode_settings_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("1. Average Mode     ");
    LCD_position(LINE2);
    _LCD_PRINT("2. Decay Mode       ");
  }
  else 
  {
    _LCD_PRINT("1. Media Modo     ");
    LCD_position(LINE2);
    _LCD_PRINT("2. Desinteg. Modo");
  }
}


void avg_stand_mode_enabled_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    LCD_position(LINE2);
    _LCD_PRINT("Avg Std Mode Enabled");
  }
  else 
  {
   LCD_position(LINE2);
   _LCD_PRINT("Media Modo Activar");
  }
}


void decay_stand_mode_enabled_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    LCD_position(LINE2);
    _LCD_PRINT("Decay Mode Enabled  ");
  }
  else 
  {
   LCD_position(LINE2);
   _LCD_PRINT("Desint. Modo Activar");
  }
}


void gauge_in_safe(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Place Gauge in");
    LCD_position(LINE2);
    _LCD_PRINT("SAFE Position");
    LCD_position(LINE3);
    _LCD_PRINT("and Press ENTER"); 
  }
    else
    {    
      _LCD_PRINT("Coloque el Equipo");  // Coloque el equipo en posicion de seguridad.  Presione Iniciar
      LCD_position(LINE2);
      _LCD_PRINT("en Posicion de Segur");
      LCD_position(LINE3);
      _LCD_PRINT("Presione <ENTER>");
    }
}



void gauge_in_bs_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Place Gauge in      ");
    LCD_position(LINE2);
    _LCD_PRINT("BS Position         ");
    LCD_position(LINE3);
    _LCD_PRINT("and Press ENTER     "); 
  }
    else
    {    
      _LCD_PRINT("Coloque el Equipo");  // Coloque el equipo en posicion de seguridad.  Presione Iniciar
      LCD_position(LINE2);
      _LCD_PRINT("en Posicion de RD");
      LCD_position(LINE3);
      _LCD_PRINT("Presione <ENTER>");
    }
}

void set_units_text(void)
{
  CLEAR_DISP;
 
  LCD_PrintAtPosition ( "1.   PCF", LINE1 );
  LCD_PrintAtPosition ( "2.  kgm3", LINE2 );
  LCD_PrintAtPosition ( "3.   GCC", LINE3 );
  
}

void offset_on_off_text(char *temp_str, int enabled)
{
  CLEAR_DISP;
  if(Features.language_f)
  {
    if(enabled)                              
    {        
      LCD_position (LINE1);
      _LCD_PRINTF("%s Offset ON",temp_str);        
      LCD_position (LINE2);
      _LCD_PRINT("Want to Disable?");
    }
    else
    {
      LCD_position (LINE1);
      _LCD_PRINTF("%s Offset OFF",temp_str);             
      LCD_position (LINE2);
      _LCD_PRINT("Want to Enable?");
    }      
  }
    else
    {
      if(enabled)                             //    Densidad / Humedad / Zanja  Compensacion Habilitado Desea Deshabilitarlo?   
      {        
        LCD_position (LINE1);
        _LCD_PRINTF("%s Compensacion",temp_str);        
        LCD_position (LINE2);
        _LCD_PRINT("Habilitado Desea");
        LCD_position(LINE3); 
        _LCD_PRINT("Deshabilitarlo?");
      }
      else
      {
        LCD_position (LINE1);
        _LCD_PRINTF("%s Compensacion",temp_str);  // Densidad / Humedad / Zanja - Compensacion Deshabilitado desea Habilitarlo?               
        LCD_position (LINE2);
        _LCD_PRINT("Deshabilitado Desea");
        LCD_position(LINE3);
        _LCD_PRINT("Habilitarlo?");
      }      
    }  
}
void enter_offset_text(char *temp_str)
{
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINTF("Enter %s",temp_str);  
    LCD_position(LINE2);
    _LCD_PRINT("Offset:");
  }
    else
    {
      _LCD_PRINT("Ingresar Compensac'n");
      LCD_position(LINE2);
      _LCD_PRINTF("%s",temp_str);
    }
}

void offset_enable_disable_text(char *temp_str, int enabled)
{  
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
  {
    if(!enabled)
    {    
      _LCD_PRINTF(" %s Offset",temp_str);
      LCD_position(LINE3);
      _LCD_PRINT("   Disabled");           
    }
    else
    {
      _LCD_PRINTF(" %s Offset",temp_str);
      LCD_position(LINE3);
      _LCD_PRINT("   Enabled");
    }
  }
    else
    {  
      if(!enabled)
      {    
        _LCD_PRINTF(" %s Compensacion",temp_str);
        LCD_position(LINE3);
        _LCD_PRINT("   Deshabilitado");           
      }
      else
      {
        _LCD_PRINTF(" %s Offset",temp_str);
        LCD_position(LINE3);
        _LCD_PRINT("   Habilitado"); 
      }
    }
}


void enter_access_number_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Access"); 
    LCD_position(LINE2);
    _LCD_PRINT("Number: ");
  }
    else
    {
      _LCD_PRINT("Ingrese el Codego de");    //Ingrese el C�digo de Acceso.
      LCD_position(LINE2);
      _LCD_PRINT("Acceso: ");
    }
}

void enter_cal_den1_std_text (void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Density 1 Std "); 
    LCD_position(LINE2);
    _LCD_PRINT("Count:              ");
  }
    else
    {
     _LCD_PRINT("Enter Density 1 Std "); 
     LCD_position(LINE2);
     _LCD_PRINT("Count:              ");
    }
}

void enter_cal_den2_std_text (void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Density 2 Std "); 
    LCD_position(LINE2);
    _LCD_PRINT("Count:              ");
  }
    else
    {
     _LCD_PRINT("Enter Density 2 Std "); 
     LCD_position(LINE2);
     _LCD_PRINT("Count:              ");
    }
}

void access_number_incorrect_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("    Access Code");
    LCD_position(LINE3);
    _LCD_PRINT("     Incorrect"); 
  }
    else
    {
      _LCD_PRINT("  Codego de Acceso");  // Codigo de Acceso Incorrecto
      LCD_position(LINE3);
      _LCD_PRINT("     Incorrecto"); 
    }  
}

void cal_menu_text(BYTE display_set)
{
  if(Features.language_f)
  {
    switch(display_set)
    {
      case 1:
            LCD_position(LINE1);
            _LCD_PRINT("0. A1 B1 C1         ");
            LCD_position(LINE2);
            _LCD_PRINT("1. A11 A12 A13      ");
            break;
      case 2:
            LCD_position(LINE1);
            _LCD_PRINT("2. A2 B2 C2          ");
            LCD_position(LINE2);
            _LCD_PRINT("3. A21 A22 A23       ");
            break;
  
            
      case 0:
            LCD_position(LINE1);
            _LCD_PRINT("4. Calib Std Counts");
            LCD_position(LINE2);
            _LCD_PRINT("5. Calibration Date");
            break;
     
    }       
  }
    else
    {
      switch(display_set) //  0. Costantes de Humedad. 1.Retro-dispercion B. 2. Profundidad
      {
        case 1:
          LCD_position(LINE1);
          _LCD_PRINT("0.  A1 B1 C1        ");
          LCD_position(LINE2);
          _LCD_PRINT("1. A11 A12 A13      ");
        break;
          
        case 2:
          LCD_position(LINE1);
          _LCD_PRINT("2. A2 B2 C2        ");
          LCD_position(LINE2);
          _LCD_PRINT("3. A21 A22 A23     ");
        break;
      
         case 0:
            LCD_position(LINE1);
            _LCD_PRINT("4. Cal Std Counts  ");
            LCD_position(LINE2);
            _LCD_PRINT("                    ");
            break;

      }       
    }
}

void read_write_Constants_text ( )
{

  // Press <1> to write
  // constants to USB
  // Press <2> to read
  // constants to USB

  CLEAR_DISP;
  if(Features.language_f)
  {
    LCD_position(LINE1);
    _LCD_PRINT("Press <1> to write");
    LCD_position(LINE2);
    _LCD_PRINT("Constants to USB");
    LCD_position(LINE3);
    _LCD_PRINT("Press <2> to read");
    LCD_position(LINE4);
    _LCD_PRINT("Constants from USB");
   }
  else
  {
     LCD_position(LINE1);
    _LCD_PRINT("PULSE <1> Escribir");
    LCD_position(LINE2);
    _LCD_PRINT("Constantes en USB");
    LCD_position(LINE3);
    _LCD_PRINT("Press <2> lectura de");
    LCD_position(LINE4);
    _LCD_PRINT("Constants desde USB");
  }
}


void cal_menu_hidden_text(BYTE display_set)
{
  if(Features.language_f)
  {
    switch(display_set)
    {
      case 1:
            LCD_position(LINE1);
            _LCD_PRINT("0. L125 L225 L150   ");
            LCD_position(LINE2);
            _LCD_PRINT("1. L250 L175 L275   ");
            break;
      case 2:
            LCD_position(LINE1);
            _LCD_PRINT("2. L110 L210 H125  ");
            LCD_position(LINE2);
            _LCD_PRINT("3. H225 H150 H250   ");
            break;
            
      case 3:
            LCD_position(LINE1);
            _LCD_PRINT("4. H175 H275  H110  ");
            LCD_position(LINE2);
            _LCD_PRINT("5. H210              ");
            break;            
     
      case 4:
            LCD_position(LINE1);
            _LCD_PRINT("6. S11  S12 S13     ");
            LCD_position(LINE2);
            _LCD_PRINT("7. S21  S22  S23    ");
            break;             
    }       
  }
    else
    {
      switch(display_set) //  0. Costantes de Humedad. 1.Retro-dispercion B. 2. Profundidad
      {
      case 1:
            LCD_position(LINE1);
            _LCD_PRINT("0. L125 L225 L150   ");
            LCD_position(LINE2);
            _LCD_PRINT("1. L250 L175 L275   ");
            break;
      case 2:
            LCD_position(LINE1);
            _LCD_PRINT("2. L110 L2110 H125  ");
            LCD_position(LINE2);
            _LCD_PRINT("3. H225 H150 H250   ");
            break;
            
      case 3:
            LCD_position(LINE1);
            _LCD_PRINT("4. H175 H275  H110");
            LCD_position(LINE2);
            _LCD_PRINT("5. H2110   ");
            break;            
      }       
    }
}


void reset_cal_date_text(BYTE which_prompt)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    if(which_prompt==0)
    {
      _LCD_PRINT("Reset Calibration   ");
      LCD_position(LINE2);
      _LCD_PRINT("Date?");
    }
    else
    {   
      _LCD_PRINT("  Calibration Date  ");
      LCD_position(LINE2);
      _LCD_PRINT("       Reset        ");
    }
  }
    else
    {
      if(which_prompt==0)
      {
        _LCD_PRINT("Borrar la Fecha de  ");  // Borrar la fecha de Calibracion?
        LCD_position(LINE2);
        _LCD_PRINT("Calibracion?");
      }
      else
      {   
        _LCD_PRINT("Fecha de Calibracion");  // Fecha de Calibracion Borrada
        LCD_position(LINE2);
        _LCD_PRINT("      Borrada       ");
      }
    }  
}


void memory_reset_text(void)
{
  CLEAR_DISP;
  LCD_position (LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Reset Memory");
    LCD_position(LINE2);
    _LCD_PRINT("to Default?");
  }
    else
    {
      _LCD_PRINT("Borrar Memoria");  // Borrar memoria a Inicio?
      LCD_position(LINE2);
      _LCD_PRINT("a Inicio?");
    }
}

void enter_mat_thickness_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Material");
    LCD_position (LINE2);
    _LCD_PRINT("Thickness:");
  }
    else
    {
      _LCD_PRINT("introducir material"); 
      LCD_position (LINE2);
      _LCD_PRINT("Grueso:");
    }
}

void enter_time_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Present");
    LCD_position(LINE2);      
    _LCD_PRINT("Time: hh:mm"); 
  }
    else
    {
      _LCD_PRINT("Ingrese la Hora"); //  Ingrese la Hora Actual   hh : mm 
      LCD_position(LINE2);      
      _LCD_PRINT("Actl: hh:mm"); 
    }
}
void am_pm_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
    { _LCD_PRINT("Select"); }
    else
      _LCD_PRINT("Seleccione");   //Seleccione 1. AM - PM     
  LCD_position(LINE2);  
  _LCD_PRINT("1. AM");       
  LCD_position(LINE3);
  _LCD_PRINT("2. PM");
}

void change_val_text(void)
{
  LCD_position(LINE2);
  if(Features.language_f)
     {_LCD_PRINT("Change Value?");}  // Cambiar Valor? 
    else
      _LCD_PRINT("Cambiar Valor?");
}

void get_new_offset_text(void)
{
  LCD_position(LINE2);
  if(Features.language_f)
     {_LCD_PRINT("Get New Offset?     ");}  
    else
     {_LCD_PRINT("obt. un nuevo despl?");}  
}


void use_val_text(void)
{
  LCD_position(LINE2);
  if(Features.language_f)
     {_LCD_PRINT("Use Value?");}  // Cambiar Valor? 
    else
     { _LCD_PRINT("valor de uso?");}
}

void battery_shutdwn_text(void)
{
  CLEAR_DISP;
  LCD_position (LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("    Battery Low");
    LCD_position (LINE3);
    _LCD_PRINT("  Shutdown: 5 sec.");
  }
    else
    {
      _LCD_PRINT("    Bateria Baja");   // Bater�a Baja       Interrupci�n en XX segundos
      LCD_position (LINE3);
      _LCD_PRINT("Interrupcion: 5 seg.");
    }
}
void display_alk_warning(void)
{
  CLEAR_DISP;
  LCD_position (LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("  *** WARNING ***");
    LCD_position (LINE2);
    _LCD_PRINT("  Alkaline Battery  ");
    LCD_position (LINE3);
    _LCD_PRINT("      is  Low       ");
  }
    else
    {
      _LCD_PRINT(" *** ADVERTENCIA ***");  // *** ADVERTENCIA ***    Calibrador Demasiado Caliente    �Voltaje Bajo?
      LCD_position (LINE2);
      _LCD_PRINT("  Bateria Alcalina  ");    //Bater�a Baja       Interrupci�n en XX segundos
      LCD_position (LINE3);
      _LCD_PRINT("      es Baja       ");
    }
}

void warning_text(BYTE which_source)
{
  CLEAR_DISP;
  LCD_position (LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("  *** WARNING ***");
    LCD_position (LINE3);
    if(which_source==0)
      { _LCD_PRINT("   Gauge Too Hot");   }
    else 
      { _LCD_PRINT("    Low Voltage ");   }
  }
    else
    {
      _LCD_PRINT(" *** ADVERTENCIA ***");   //*** ADVERTENCIA ***    Calibrador Demasiado Caliente    �Voltaje Bajo?
      LCD_position (LINE3);
      if(which_source==0)
      {
        _LCD_PRINT("Calibrador Demasiado");   
        LCD_position(LINE4);
        _LCD_PRINT("     Caliente!");
      }
      else
        _LCD_PRINT("    Voltaje Bajo");   
    }  
}
void gauge_temp_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
    { _LCD_PRINT("Gauge Temp ="); } // Temperatura del Densimetro 
    else
      _LCD_PRINT("Temperatura=");
}

   
void press_key_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  
  if(Features.language_f)
  { 
   _LCD_PRINT("   Press Any Key    "); 
   LCD_position(LINE2);
   _LCD_PRINT("    ESC to Exit    "); 
  } 
  else
  { 
    _LCD_PRINT("pres. cualq. tecla"); 
   LCD_position(LINE2);
   _LCD_PRINT(" <ESC> para salir  "); 
  }  
   
  
}


void high_voltage_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
   { _LCD_PRINT("High Voltage =");  }
    else
      _LCD_PRINT("Voltaje Alto =");  //Voltaje Alto
}
void shut_down_text_text(void)
{
  CURSOR_OFF;
  CLEAR_DISP;
  LCD_position (LINE2 + 6);
  if(Features.language_f)
    { _LCD_PRINT("Shutdown"); }
    else
      _LCD_PRINT(" Apagar");  // Apagar
}
void shutdown_inactivity_text_text(void)
{
  //LCD_init();
  CURSOR_OFF;
  CLEAR_DISP;
  LCD_position (LINE2 + 1);
  if(Features.language_f)
    { _LCD_PRINT("Idle Shutdown"); }
    else
      _LCD_PRINT(" Inactivo Apagar");  // Apagar
}

void enter_valid_text(BYTE which_source)
{
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("    Please Enter");
    LCD_position(LINE3);  
    _LCD_PRINT("     Valid ");
    if(which_source==0)
      { _LCD_PRINT("Date"); }
    else
      { _LCD_PRINT("Time"); }
  }
    else
    {
      _LCD_PRINT("   Favor Ingresar");   //Favor ingresar fecha y hora validos
      LCD_position(LINE3);  
      if(which_source==0)
        _LCD_PRINT(" Fecha y Validos");
      else 
        _LCD_PRINT(" Hora y Validos");
    }
  delay_ms(1000);
}
void enter_project_name_text(char *temp_str)
{
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Project ");
    LCD_position(LINE2);
    _LCD_PRINT("Name: ");
  }
    else
    {
      _LCD_PRINT("Ingrese Nombre del ");   // Ingrese nombre del proyecto
      LCD_position(LINE2);
      _LCD_PRINT("Proyecto: ");
    }
  _LCD_PRINTF("%s",temp_str);
}

void project_name_text(char *temp_str)
{
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
    { _LCD_PRINT("    Project Name");   }
    else
      _LCD_PRINT("Nombre del Projecto");  // Nombre del Projecto
  LCD_position(LINE3);
  _LCD_PRINTF(  "    %s",temp_str);   
}
void station_mode_text()
{
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Station Name Mode");
    LCD_position(LINE2);
    _LCD_PRINT("1. Auto (sequential)");
    LCD_position(LINE3);
    _LCD_PRINT("2. Manual Entry"); 
  }
    else
    {
      _LCD_PRINT("Opc. Nombre Estacion");    // Opciones Nombre Estacion  1. Automatico (Secuencial)  2. Ingreso manual
      LCD_position(LINE2);
      _LCD_PRINT("1. Automatico");
      LCD_position(LINE3);
      _LCD_PRINT("2. Ingreso Manual"); 
    }
}
void station_start_text(char *temp_str)
{ 
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Starting Station");
    LCD_position(LINE2);
    _LCD_PRINTF("Number: %s",temp_str); 
  }
    else
    {
      _LCD_PRINT("Iniciacion del");   //  Iniciacion del  Numero de Estacion
      LCD_position(LINE2);
      _LCD_PRINTF("Numero de Estacion: %s",temp_str); 
    }
}
void station_increment_text(unsigned int inc)
{ 
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINTF("Station # %u",inc);
    LCD_position(LINE2);
    _LCD_PRINT("Increments by 1");
    LCD_position(LINE3);
    _LCD_PRINT("For Each Reading");
  }
    else
    {
      _LCD_PRINTF("Estacion# %u",inc);   //Estacion #  Incrementar en 1 (uno) para cada lectura 
      LCD_position(LINE2);
      _LCD_PRINT("Encrementar en 1");
      LCD_position(LINE3);
      _LCD_PRINT("Para Cada Lectura");
    }
}
void station_name_text()
{  
  LCD_position(LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Station Name");
    LCD_position(LINE3);
    _LCD_PRINT("Before Each Reading");
  }
    else
    {
      _LCD_PRINT("Ingrese el Nombre de");   //Ingrese el Nombre de la Estaci�n antes de Tomar cada lectura
      LCD_position(LINE3);
      _LCD_PRINT("la Estacion Antes de");
      LCD_position(LINE4);
      _LCD_PRINT("Tomar Cada Lectura");
    }
}
void project_limit_text()
{
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Project Limit");
    LCD_position(LINE2);
    _LCD_PRINT("Exceeded, Delete");
    LCD_position(LINE3);
    _LCD_PRINT("One or More Projects");    
  }
    else
    {
      _LCD_PRINT("Proyecto Limites");    //Proyecto Limites Exceso de proyectos, Borrar uno o mas proyectos. 
      LCD_position(LINE2);
      _LCD_PRINT("Exceso de Proyectos,");
      LCD_position(LINE3);
      _LCD_PRINT("Borrar Uno o Mas");
      LCD_position(LINE4);
      _LCD_PRINT("Proyectos.");
    }
}


void no_stored_projects_text()
{ 
  CLEAR_DISP;
  LCD_position(LINE2);  
  if(Features.language_f)
  {
    _LCD_PRINT("    No Projects");
    LCD_position(LINE3);
    _LCD_PRINT("      Stored"); 
  }
    else
    {
      _LCD_PRINT("  No Hay Proyectos");   //No hay proyectos en la memoria
      LCD_position(LINE3);
      _LCD_PRINT("   En la Memoria"); 
    }
}
void select_from_list_text(BYTE from_where)
{
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
  {
    if(from_where==0)
    {  _LCD_PRINT("   Select Project"); }
    else if(from_where==1)
    {  _LCD_PRINT("   Select Station"); }
   
  }
  else
  {
      if(from_where==0)
      {  _LCD_PRINT("  Selec. Proyecto"); }
      else if(from_where==1)
      {  _LCD_PRINT("   Selec. Estacion"); }
  }
}

void no_data_stored_text()
{ 
  CLEAR_DISP;
  LCD_position(LINE2);

  _LCD_PRINT("   No Data Stored");   
}

void delete_data_text()
{ 
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Delete Data");
    LCD_position(LINE2);
    _LCD_PRINT("1. Delete All Data");
    LCD_position(LINE3); 
    _LCD_PRINT("2. Delete One Proj."); 
  }
    else
    {
      _LCD_PRINT("Borrar la Info.");
      LCD_position(LINE2);
      _LCD_PRINT("1. Toda Informacion");  // Borrar la Informaci�n; 1. Borrar toda Informacion  2.Borrar un projecto
      LCD_position(LINE3);  
      _LCD_PRINT("2. Un Projecto"); 
    }  
}
void enter_to_delete_text()
{  
  LCD_position(LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("  Press ENTER to");
    LCD_position(LINE3);
    _LCD_PRINT("  Delete All Data"); 
  }
    else
    {
      _LCD_PRINT("Oprima Entrada para");  // Oprima Entrada para Borrar Toda la Informaci�n
      LCD_position(LINE3);
      _LCD_PRINT("Borrar Toda la"); 
      LCD_position(LINE4);
      _LCD_PRINT("Informacion.");    
    }  
}
void erase_data_text()
{ 
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Erase All Data");
    LCD_position(LINE2);
    _LCD_PRINT("From Memory?");
    LCD_position(LINE3);
    _LCD_PRINT("YES to Continue"); 
  }
    else
    {
      _LCD_PRINT("Borrar Toda la Info.");  // Borrar toda la Informaci�n de la Almacenada en la Memoria SI para continuar
      LCD_position(LINE2);
      _LCD_PRINT("en la Memoria?");
      LCD_position(LINE3);
      _LCD_PRINT("<YES> Para Continuar"); 
    }
}
void all_data_erased_text()
{  
  LCD_position(LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("      All Data");
    LCD_position(LINE3);
    _LCD_PRINT("       Erased"); 
  }
    else
    {
      _LCD_PRINT("Toda la Informacion");  // Toda la informaci�n ha sido borrada
      LCD_position(LINE3);
      _LCD_PRINT("  Ha Sido Borrada"); 
    }
}
void delete_project_text(char *temp_str)
{  
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Press ENTER to"); 
    LCD_position(LINE2);
    _LCD_PRINT("Delete Data In");
    LCD_position(LINE3);
    _LCD_PRINT("Project: "); 
  }
    else
    {
      _LCD_PRINT("Press <ENTER> Para");   //Presione Entrada para Borrar Toda la Informaci�n del Proyecto
      LCD_position(LINE2);
      _LCD_PRINT("Borrar Toda la Info.");
      LCD_position(LINE3);
      _LCD_PRINT("Proyecto: "); 
    }
  _LCD_PRINTF("%s",temp_str);   //print project name
}
void erase_project_data_text()
{ 
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Erase Project Data?");
    LCD_position(LINE3);
    _LCD_PRINT("YES to Continue");
  }
    else
    {
      _LCD_PRINT("Borrar Informacion?");  // Borrar Informacion del Projecto?
      LCD_position(LINE3);
      _LCD_PRINT("<YES> para continuar");
    }
  
}
void write_USB_text()
{  
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT(" Write Data to USB"); 
    LCD_position(LINE2);
    _LCD_PRINT("1. Write All Data");
    LCD_position(LINE3);
    _LCD_PRINT("2. Write One Project"); 
  }
    else
    {
      _LCD_PRINT("Escriba Datos a USB");  //Escriba Datos a USB  1. Escriba toda la Informacion  2. Escriba un Proyecto
      LCD_position(LINE2);
      _LCD_PRINT("1. Toda la Info.");
      LCD_position(LINE3);
      _LCD_PRINT("2. Un Proyecto"); 
    }
}
void batt_volt_text()
{  
  CLEAR_DISP;
  LCD_position(LINE1);
  _LCD_PRINT("NiCd Volt =");  
  LCD_position(LINE2);
  _LCD_PRINT(" Alk Volt =");  
}

void print_data_text()
{  
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Print Data");
    LCD_position(LINE2);
    _LCD_PRINT("1. Print All Data");
    LCD_position(LINE3);
    _LCD_PRINT("2. Print One Project");  
  }
    else
    {
      _LCD_PRINT("Imprima");    // Imprima 1. Imprima toda la Informacion  2. Imprima un Proyecto
      LCD_position(LINE2);
      _LCD_PRINT("1. Toda la Info.");
      LCD_position(LINE3);
      _LCD_PRINT("2. Un Proyecto");  
    }
}

void print_complete_text()
{  
  LCD_position(LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("    Print Data");
    LCD_position(LINE3);
    _LCD_PRINT("     Complete");
  }
    else
    {
      _LCD_PRINT("    Impresion");   
      LCD_position(LINE3);
      _LCD_PRINT("     Completa");
    }
}
void enter_station_name_text()
{  
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Station");
    LCD_position(LINE2);
    _LCD_PRINT("Name:");
  }
    else 
    {
      _LCD_PRINT("Entre Nombre de la");    // Entre nombre  de la estaci�n.
      LCD_position(LINE2);
      _LCD_PRINT("Estacion:");
    }
}
void serial_port_text()
{ 
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("    Serial Port");
    LCD_position(LINE3);
    _LCD_PRINT("     Activated");
  }
    else
    {
      _LCD_PRINT("    Conexion de");  // Conexi�n de Transferencia Activada
      LCD_position(LINE3);
      _LCD_PRINT("   Transferencia");
      LCD_position(LINE4);
      _LCD_PRINT("      Activada");
    }  
}
void select_language_text()
{                                                        
  CLEAR_DISP;
  LCD_position(LINE1); 
  if(Features.language_f)
  {
    _LCD_PRINT("1. English");
    LCD_position(LINE2);
    _LCD_PRINT("2. Spanish");
  }
    else
    {
      _LCD_PRINT("1. Ingles");  // Seleccione el Idioma: 1. Ingles 2. Espa�ol
      LCD_position(LINE2);
      _LCD_PRINT("2. Espanol");
    }
}

void select_measurement_mode_text()
{                                                        
  CLEAR_DISP;
  LCD_position(LINE1); 
  if(Features.language_f)
  {
    _LCD_PRINT("1. Normal Density");
    LCD_position(LINE2);
    _LCD_PRINT("2. Metal Density");
    LCD_position(LINE3);
    _LCD_PRINT("3. Density Profile");
    
  }
    else
    {
    _LCD_PRINT("1. Normal Densidad");
    LCD_position(LINE2);
    _LCD_PRINT("2. Metal Densidad");
    LCD_position(LINE3);
    _LCD_PRINT("3. Densidad Perfil");
    }
}


void count_text(BYTE source)
{ 
  if(Features.language_f)
  {
    switch(source)
    {
      case  0: _LCD_PRINT("D1 Count:"); break;
      case  1: _LCD_PRINT("D2 Count:"); break;
      case  2: _LCD_PRINT("DCR1    :"); break;
      case  3: _LCD_PRINT("DCR2    :"); break;
      case  4: _LCD_PRINT("    DT:");  break;
      case  5: _LCD_PRINT("  %MAX:"); break;
      case  6: _LCD_PRINT("%Voids:"); break;
      case  7: _LCD_PRINT("THICKNESS:"); break; 
      case  8: _LCD_PRINT("MODE: NORMAL"); break; 
      case  9: _LCD_PRINT("MODE: METAL"); break;  
      case 10: _LCD_PRINT("MODE: PROFILE"); break;  
   }
  }
    else
    {
      switch(source)
      {
        case  0: _LCD_PRINT("D1 Cuenta:"); break;
        case  1: _LCD_PRINT("D2 Cuenta:"); break;
        case  2: _LCD_PRINT("DCR1    :"); break;
        case  3: _LCD_PRINT("DCR2    :"); break;
        case  4: _LCD_PRINT("    DT:");  break;
        case  5: _LCD_PRINT("   %HA:"); break;
        case  6: _LCD_PRINT("%Voids:"); break;
        case  7: _LCD_PRINT("Grueso:"); break; 
        case  8: _LCD_PRINT("MODO: NORMAL"); break; 
        case  9: _LCD_PRINT("MODO: METAL"); break;          
        case 10: _LCD_PRINT("MODO: PERFIL"); break;  
      }
    }
}

void stat_drift_count_text(BYTE source)
{
  if(Features.language_f)
  {
    switch(source)
    {     
      case  0: _LCD_PRINT("D1%%  Drift:  "); break;
      case  1: _LCD_PRINT("Dens. 1 AVG: ");  break;
      case  2: _LCD_PRINT("D2%%  Drift:  "); break;
      case  3: _LCD_PRINT("Dens 2. AVG: ");  break;      
      case  4: _LCD_PRINT("Dens. 1 R:   ");  break;
      case  5: _LCD_PRINT("Dens. 2 R:   ");  break;
      case  6: _LCD_PRINT("D3%%  Drift:  "); break;
      case  7: _LCD_PRINT("Dens. 3 AVG: ");  break;
      case  8: _LCD_PRINT("D4%%  Drift:  "); break;
      case  9: _LCD_PRINT("Dens. 4 AVG: ");  break;
      case 10: _LCD_PRINT("D5%%  Drift:  "); break;
      case 11: _LCD_PRINT("Dens. 5 AVG: ");  break;          
      case 12: _LCD_PRINT("D6%%  Drift:  "); break;
      case 13: _LCD_PRINT("Dens. 6 AVG: ");  break;          
      
    }
  }
    else
    {
      switch(source)
      {
      case  0: _LCD_PRINT("D1%%  Drift:  "); break;
      case  1: _LCD_PRINT("Dens. 1 AVG: ");  break;
      case  2: _LCD_PRINT("D2%%  Drift:  "); break;
      case  3: _LCD_PRINT("Dens 2. AVG: ");  break;      
      case  4: _LCD_PRINT("Dens. 1 R:   ");  break;
      case  5: _LCD_PRINT("Dens. 2 R:   ");  break;
      case  6: _LCD_PRINT("D3%%  Drift:  "); break;
      case  7: _LCD_PRINT("Dens. 3 AVG: ");  break;
      case  8: _LCD_PRINT("D4%%  Drift:  "); break;
      case  9: _LCD_PRINT("Dens. 4 AVG: ");  break;
      case 10: _LCD_PRINT("D5%%  Drift:  "); break;
      case 11: _LCD_PRINT("Dens. 5 AVG: ");  break;          
      case 12: _LCD_PRINT("D6%%  Drift:  "); break;
      case 13: _LCD_PRINT("Dens. 6 AVG: ");  break;   
      }
    }
}

void self_test_pf_text(BYTE source, int test_failed)
{ 
  char source_text[16][14] =    {{"Teclado:     "},{"Keypad:      "},
                                 {"Tubos GM:    "},{"GM Tubes:    "},
                                 {"Tubo 1 GM:   "},{"GM Tube 1:   "},                        
                                 {"Tubo 2 GM:   "},{"GM Tube 2:   "},
                                 {"Temp:        "},{"Temp:        "},                                
                                 {"FALLADO"},{"FAILED"},
                                 {"PASADO"},{"PASSED"}};
 
  _LCD_PRINTF("%s",source_text[source + Features.language_f]);
  
  if(test_failed)
   { _LCD_PRINTF("%s",source_text[12+Features.language_f]); }
  else
   { _LCD_PRINTF("%s",source_text[14+Features.language_f]);   }
}

void auto_cal_text(BYTE display_set)
{
  if(Features.language_f)
  {
    switch(display_set)
    {
      case 0:
      LCD_position(LINE1);
      _LCD_PRINT("1. Calibrate");        
      LCD_position(LINE2);
      _LCD_PRINT("2. Recount  ");
      break; 
    }       
  }
    else
    {
      switch(display_set)  // 0. Costantes de Humedad. 1.Retro-dispercion B. 2. Profundidad
      {
        case 0:
        LCD_position(LINE1);
        _LCD_PRINT("1. Calibracion");        
        LCD_position(LINE2);
        _LCD_PRINT("2. Recuento   ");   //Borrar la Fecha
        break; 
      }       
    }
}

void place_gauge_txt()
{
  if(Features.language_f)
  {   
      LCD_position(LINE1);
      _LCD_PRINT("Place the Gauge on   ");        
  }
    else
    {   
        LCD_position(LINE1);
        _LCD_PRINT("Coloque el medidor en");                  
    }

}

void yes_when_ready_txt ( void )
{
  if(Features.language_f)
  {   
      LCD_position(LINE4);
      _LCD_PRINT("Press Yes when READY");        
  }
    else
    {   
        LCD_position(LINE4);
        _LCD_PRINT("<YES>, cuando PREPARADO");                  
    }
}

void shut_down_txt( void )
{
  CLEAR_DISP;
  if(Features.language_f)
  {   
      LCD_position(LINE1);
      _LCD_PRINT("  Hold OFF Key Down ");        
      LCD_position(LINE2);
      _LCD_PRINT("    for 25secs      ");        
      LCD_position(LINE4);
      _LCD_PRINT("  Press ESC to Exit ");        
  }
  else
  {   
       LCD_position(LINE1);
      _LCD_PRINT("  Hold OFF Key Down ");        
      LCD_position(LINE2);
      _LCD_PRINT("    for 25secs      ");        
      LCD_position(LINE4);
      _LCD_PRINT("  <ESC> para Salir ");        
  }                 
  
}


void press_off_txt ( void )
{
  CLEAR_DISP;
  if(Features.language_f)
  {   
      LCD_position(LINE1);
      _LCD_PRINT("  TEST IN PROCESS   ");        
      LCD_position(LINE2);
      _LCD_PRINT(" Keep Pressing OFF  ");        
      LCD_position(LINE4);
      _LCD_PRINT("  Press ESC to Exit ");        
  }
  else
  {   
       LCD_position(LINE1);
      _LCD_PRINT("  TEST IN PROCESS   ");        
      LCD_position(LINE2);
      _LCD_PRINT(" Keep Pressing OFF  ");        
      LCD_position(LINE4);
      _LCD_PRINT(" <ESC> para Salir   ");         
  }                 
  
}


void light_test_txt( void )
{
  CLEAR_DISP;
  if(Features.language_f)
  {   
      LCD_position(LINE1);
      _LCD_PRINT("     Press START   ");        
      LCD_position(LINE2);
      _LCD_PRINT(" to Turn on Lights ");        
      LCD_position(LINE3);
      _LCD_PRINT("    for 5 secs     ");        
      LCD_position(LINE4);
      _LCD_PRINT("  Press ESC to Exit ");        
  }
  else
  {   
      LCD_position(LINE1);
      _LCD_PRINT("     Press START    ");        
      LCD_position(LINE2);
      _LCD_PRINT(" to Turn on Lights  ");        
      LCD_position(LINE3);
      _LCD_PRINT("    for 5 secs      ");        
      LCD_position(LINE4);
      _LCD_PRINT("  <ESC> para Salir  ");    
  }                 
  
}

void lights_are_on_txt( void )
{
  CLEAR_DISP;
  if(Features.language_f)
  {   
      
      LCD_position(LINE2);
      _LCD_PRINT("    Lights are on   ");        
      
  }
  else
  {   
    LCD_position(LINE2);
    _LCD_PRINT("    Lights are on   ");        
  }                 
  
}


void count_test_txt( void )
{
  CLEAR_DISP;
  if(Features.language_f)
  {   
      LCD_position(LINE1);
      _LCD_PRINT("  Press START for   ");        
      LCD_position(LINE2);
      _LCD_PRINT("  30 s. Tube Test   ");        
      LCD_position(LINE4);
      _LCD_PRINT("  Press ESC to Exit ");        
  }
  else
  {   
      LCD_position(LINE1);
      _LCD_PRINT("  Press START for   ");        
      LCD_position(LINE2);
      _LCD_PRINT("  30 s. Tube Test   ");        
      LCD_position(LINE4);
      _LCD_PRINT("  <ESC> para Salir  ");    
  }                 
  
}


void display_count_text ( uint32 dens2, uint32 dens1 )
{ 

  CLEAR_DISP;
  
  if(Features.language_f)
  {
    LCD_position (LINE1);
    _LCD_PRINTF("Den Sys1 Count:%lu", dens1); 
    LCD_position (LINE2);
    _LCD_PRINTF("Den Sys2 Count:%lu", dens2); 
    LCD_position(LINE4);
    _LCD_PRINT("  Press ESC to Exit ");  
  }
  else
  {
    LCD_position (LINE1);
    _LCD_PRINTF("Den Sys1 Count:%lu", dens1); 
    LCD_position (LINE2);
    _LCD_PRINTF("Den Sys2 Count:%lu", dens2); 
    LCD_position(LINE4);
    _LCD_PRINT("  <ESC> para Salir  ");     
  }
}



void review_consts(){ 
  if(Features.language_f)
  {
    LCD_position(LINE2); 
    _LCD_PRINT(" Review Constants?  ");
  	LCD_position(LINE3); 
    _LCD_PRINT("  Press YES or NO   "); 	
	}

 
}

void store_consts(){ 
  if(Features.language_f)
  {
    LCD_position(LINE2); 
    _LCD_PRINT(" Store Constants?  ");
    LCD_position(LINE3); 
    _LCD_PRINT("  Press YES or NO   "); 	
	}

  
}


void press_yes_txt(){

if(Features.language_f){
LCD_position(LINE4); 
_LCD_PRINT("      Press YES     ");
}
else{
LCD_position(LINE4); 
_LCD_PRINT("     PULSE INTRO    "); 
}
}

void press_enter_txt(){

  if(Features.language_f){
  LCD_position(LINE4); 
  _LCD_PRINT("     Press ENTER    ");
  }
  else{
  LCD_position(LINE4); 
  _LCD_PRINT("    PULSE INTRO    "); 
  }
}



void enter_value_text( void )
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Value For");
    LCD_position(LINE2);
     _LCD_PRINT("MAX DENS:");
  }
  else
  {    
    _LCD_PRINT("Entre el Valor de"); //  Entre el Valor de Densidad:
    LCD_position(LINE2);
    _LCD_PRINT("MAX DENS:");  // Ingresar valor Proctor
     
  }
}


void clock_set_error_text ( void )
{
 CLEAR_DISP;
 LCD_position (LINE1);
 if(Features.language_f)
 {
  _LCD_PRINT("    Error setting   ");
  LCD_position (LINE2);
  _LCD_PRINT("    Internal Clock  ");
  LCD_position (LINE3);
  _LCD_PRINT("   Restart Gauge    ");
  LCD_position (LINE4);
  _LCD_PRINT("   Press <ENTER>    ");

 } 
 else 
 { 
  _LCD_PRINT("    Error setting   ");
  LCD_position (LINE2);
  _LCD_PRINT("    Internal Clock  ");
  LCD_position (LINE3);
  _LCD_PRINT("   Restart Gauge    ");
  LCD_position (LINE4);
  _LCD_PRINT("   Press <ENTER>    ");
 }  
}


void depth_in_safe_text ( void )
{
 CLEAR_DISP;
 LCD_position (LINE1);
 if(Features.language_f)
 {
  _LCD_PRINT("Depth in Safe       ");
  LCD_position (LINE2);
  _LCD_PRINT("Position            ");
 } 
 else 
 {
  _LCD_PRINT("Profundidad en caja ");
  LCD_position (LINE2);
  _LCD_PRINT("segura posicion    ");
 } 

}

void depth_in_no_text ( void )
{
 CLEAR_DISP;
 LCD_position (LINE1);
 if(Features.language_f)
 {
  _LCD_PRINT("Depth not in Safe");
  LCD_position (LINE2);
  _LCD_PRINT("or BS Position   ");
 } 
 else 
 {
  _LCD_PRINT("Profundidad no en  ");
  LCD_position (LINE2);
  _LCD_PRINT("pos. Seguara o RD  ");
 } 

}


void auto_depth_on_off_text( )
{
  CLEAR_DISP;
  if(Features.auto_depth == TRUE)                              
  {        
    LCD_position (LINE1);
    _LCD_PRINT("AutoDepth ON");        
    LCD_position (LINE2);
    _LCD_PRINT("Want to Disable?");
  }
  else
  {
    LCD_position (LINE1);
    _LCD_PRINT("AutoDepth OFF");             
    LCD_position (LINE2);
    _LCD_PRINT("Want to Enable?");
   }
   
}

void depth_in_bs_text ( void )
{
 CLEAR_DISP;
 LCD_position (LINE1);
 if(Features.language_f)
 {
  _LCD_PRINT("Depth in Backscatter");
  LCD_position (LINE2);
  _LCD_PRINT("Position            ");
 } 
 else 
 {
  _LCD_PRINT("Profundidad en caja ");
  LCD_position (LINE2);
  _LCD_PRINT("Retrodispersion Pos.");
 } 

}



void thickness_out_of_range_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    LCD_PrintAtPositionCentered ( "Thickness out", LINE1+10);
    LCD_PrintAtPositionCentered ( "of Range", LINE2+10);
    if ( Features.SI_units == FALSE ) 
    {
      LCD_PrintAtPositionCentered ( ".50in to 4.00in", LINE3+10);
    }
    else
    {
      LCD_PrintAtPositionCentered ( "21.70mm to 102mm", LINE3+10);
    }
  }
  else
  {
    LCD_PrintAtPositionCentered ( "espesor fuera", LINE1+10);
    LCD_PrintAtPositionCentered ( "de rango", LINE2+10);
    if ( Features.SI_units == FALSE ) 
    {
      LCD_PrintAtPositionCentered ( ".50 pulg. a 4 pulg.", LINE3+10);
    }
    else
    {
      LCD_PrintAtPositionCentered ( "6.35mm. a 102mm", LINE3+10);
    }
  }
}

LCDSTRING const m_yesorNo    = { 10, "<YES> or <NO>" };        // <YES> or <NO>
LCDSTRING const Blank        = { 0,   ""                 };
// Read Calibration
// Constants From USB?
// <YES> or <NO>
LCDSTRING const m_ReadCal     = {0, "Read Calibration"};
LCDSTRING const m_ConFromUSB  = {0, "Constants From USB?"};
LCDSTRING const * const s_readCalibUSB[4]  = { &m_ReadCal,&m_ConFromUSB, &Blank, &m_yesorNo };

// Write Calibration
// Constants To USB?
// 
// <YES> or <NO>
LCDSTRING const m_WriteCal  = {0, " Write Calibration"};
LCDSTRING const m_ConToUSB  = {0, " Constants To USB?"};
LCDSTRING const * const s_writeCalibUSB[4]  = { &m_WriteCal,&m_ConToUSB, &Blank, &m_yesorNo };

// 1. Change Time/Date
// 2. Change M/D Order
// 3. AM-PM or 24 Hour
// <ESC> to Exit
void sSelectTimeItem( void)
{
  CLEAR_DISP;
  if(Features.language_f)
  {
    LCD_position(LINE1);
    _LCD_PRINT("1. Change Time/Date");        
    LCD_position(LINE2);
    _LCD_PRINT("2. Change M/D Order");
    LCD_position(LINE3);
    _LCD_PRINT("3. 24 Hour Mode");    
    ESC_to_Exit ( LINE4 );  
  }
  else
  {
    LCD_position(LINE1);
    _LCD_PRINT("1.Cambiar hora/fecha");        
    LCD_position(LINE2);
    _LCD_PRINT("2.Cambiar M/D Orden");
    LCD_position(LINE3);
    _LCD_PRINT("3.24 Hora Modo");    
    displine ( 3 ,  &EscToExit_sp, 1 );  
  }
}



// Enter Value for
// Month:
// <Enter> to Accept
// <ESC> to Exit
LCDSTRING const m_Month        = { 0, "Month:" };
LCDSTRING const m_Month_sp        = { 0, "  Mes:" };
void s_EnterValueMonth( void)
{
  CLEAR_DISP;
  if(Features.language_f)
  {
    displine ( 0 ,  &EnterValueFor, 1 );       
    displine ( 1 ,  &m_Month, 1 );  
    displine ( 2 ,  &EnterToAccept, 1 );  
    displine ( 3 ,  &EscToExit, 1 );      
  }
  else
  {
    displine ( 0 ,  &EnterValueFor_sp, 1 );       
    displine ( 1 ,  &m_Month_sp, 1 );  
    displine ( 2 ,  &EnterToAccept_sp, 1 );  
    displine ( 3 ,  &EscToExit_sp, 1 ); 
  }
}



// Enter Value for
// DAY:
// <Enter> to Accept
// <ESC> to Exit
LCDSTRING const m_Day        = { 0, "Day:" };
LCDSTRING const m_Day_sp     = { 0, "Dia:" };
void s_EnterValueDay ( void )
{
  CLEAR_DISP;
  if(Features.language_f)
  {
    displine ( 0 ,  &EnterValueFor, 1 );       
    displine ( 1 ,  &m_Day, 1 );  
    displine ( 2 ,  &EnterToAccept, 1 );  
    displine ( 3 ,  &EscToExit, 1 );      
  }
  else
  {
    displine ( 0 ,  &EnterValueFor_sp, 1 );       
    displine ( 1 ,  &m_Day_sp, 1 );  
    displine ( 2 ,  &EnterToAccept_sp, 1 );  
    displine ( 3 ,  &EscToExit_sp, 1 ); 
  }

}

// Enter Value for
// Year:
// <Enter> to Accept
// <ESC> to Exit
LCDSTRING const m_Year       = { 0, "Year:" };
LCDSTRING const m_Year_sp    = { 0, " Ano:" };


void s_EnterValueYear ( void ) 
{
  CLEAR_DISP;
  if(Features.language_f)
  {
    displine ( 0 ,  &EnterValueFor, 1 );       
    displine ( 1 ,  &m_Year, 1 );  
    displine ( 2 ,  &EnterToAccept, 1 );  
    displine ( 3 ,  &EscToExit, 1 );      
  }
  else
  {
    displine ( 0 ,  &EnterValueFor_sp, 1 );       
    displine ( 1 ,  &m_Year_sp, 1 );  
    displine ( 2 ,  &EnterToAccept_sp, 1 );  
    displine ( 3 ,  &EscToExit_sp, 1 ); 
  }

}  


// Enter Value for
// Hour:
// <Enter> to Accept
// <ESC> to Exit
LCDSTRING const m_Hour       = { 0, "Hour:" };
LCDSTRING const m_Hour_sp    = { 0, "Hora:" };
void  s_EnterValueHour ( void ) 
{
  CLEAR_DISP;
  if(Features.language_f)
  {
    displine ( 0 ,  &EnterValueFor, 1 );       
    displine ( 1 ,  &m_Hour, 1 );  
    displine ( 2 ,  &EnterToAccept, 1 );  
    displine ( 3 ,  &EscToExit, 1 );      
  }
  else
  {
    displine ( 0 ,  &EnterValueFor_sp, 1 );       
    displine ( 1 ,  &m_Hour_sp, 1 );  
    displine ( 2 ,  &EnterToAccept_sp, 1 );  
    displine ( 3 ,  &EscToExit_sp, 1 ); 
  }

}  


// Enter Value for
// Minutes:
// <Enter> to Accept
// <ESC> to Exit
LCDSTRING const mMinutes       = { 0, "Minutes:" };
LCDSTRING const mMinutes_sp    = { 0, "Minutos:" };
void  s_EnterValueMinutes ( void )
{

  CLEAR_DISP;
  if(Features.language_f)
  {
    displine ( 0 ,  &EnterValueFor, 1 );       
    displine ( 1 ,  &mMinutes, 1 );  
    displine ( 2 ,  &EnterToAccept, 1 );  
    displine ( 3 ,  &EscToExit, 1 );      
  }
  else
  {
    displine ( 0 ,  &EnterValueFor_sp, 1 );       
    displine ( 1 ,  &mMinutes_sp, 1 );  
    displine ( 2 ,  &EnterToAccept_sp, 1 );  
    displine ( 3 ,  &EscToExit_sp, 1 ); 
  }

}

// Enter Current 
// DATE: MM/DD/10YY
// Yes to Accept
// ESC to Exit
LCDSTRING const mEnterCurrent    = { 10, "Enter Current" };
LCDSTRING const mDate            = { 0, "Date: MM/DD/20YY" };
LCDSTRING const mEnterCurrent_sp = { 0, "Ingrese la corriente" };
LCDSTRING const mDate_sp         = { 0, "Fecha: MM/DD/20YY" };
void  sEnterTime ( void )
{
  CLEAR_DISP;
  if(Features.language_f)
  {
    displine ( 0,  &mEnterCurrent, 1 );       
    displine ( 1 ,  &mDate, 1 );  
    displine ( 2 ,  &YesToAccept, 1 );  
    displine ( 3 ,  &EscToExit, 1 );      
  }
  else
  {
    displine ( 0 ,  &mEnterCurrent_sp, 1 );       
    displine ( 1 ,  &mDate_sp, 1 );  
    displine ( 2 ,  &YesToAccept_sp, 1 );  
    displine ( 3 ,  &EscToExit_sp, 1 ); 
  }
}



// Enter Current 
// DATE: DD/MM/20YY
// Yes to Accept
// ESC to Exit
LCDSTRING const mDate_day_month = { 0, "Date: DD/MM/20YY" };
LCDSTRING const mDate_day_month_sp = { 0, "Fecha: DD/MM/20YY" };
void  sEnterTimeDayMonth ( void )
{
  CLEAR_DISP;
  if(Features.language_f)
  {
    displine ( 0 ,  &mEnterCurrent, 1 );       
    displine ( 1 ,  &mDate_day_month, 1 );  
    displine ( 2 ,  &YesToAccept, 1 );  
    displine ( 3 ,  &EscToExit, 1 );      
  }
  else
  {
    displine ( 0 ,  &mEnterCurrent_sp, 1 );       
    displine ( 1 ,  &mDate_day_month_sp, 1 );  
    displine ( 2 ,  &YesToAccept_sp, 1 );  
    displine ( 3 ,  &EscToExit_sp, 1 ); 
  }
}



// Time Displayed in 
// AM-PM Mode
// <YES> to Change
// <ESC> to Exit
LCDSTRING const mTimeDisplayed         = { 10, "Time Displayed in" };
LCDSTRING const mAM_PM_Mode            = { 10, "AM-PM Mode" };
LCDSTRING const mTimeDisplayed_sp      = { 10, "Se muestra la hora" };
LCDSTRING const mAM_PM_Mode_sp         = { 10, "AM-PM Modo" };
void  sAmPmModeChange ( void ) 
{
  CLEAR_DISP;
  if(Features.language_f)
  {
    displine ( 0 ,  &mTimeDisplayed, 1 );       
    displine ( 1 ,  &mAM_PM_Mode, 1 );  
    displine ( 2 ,  &mYesToChange, 1 );  
    displine ( 3 ,  &EscToExit, 1 );      
  }
  else
  {
    displine ( 0 ,  &mTimeDisplayed_sp, 1 );       
    displine ( 1 ,  &mAM_PM_Mode_sp, 1 );  
    displine ( 2 ,  &mYesToChange_sp, 1 );  
    displine ( 3 ,  &EscToExit_sp, 1 );   
  }
}



// Time Displayed in 
// 24 Hour Mode
// <YES> to Change
// <ESC> to Exit
LCDSTRING const m24_Hour_Mode         = { 10, "24 Hour Mode" };
LCDSTRING const m24_Hour_Mode_sp      = { 10, "Modo 24 horas" };
void  s24HourModeChange ( void )
{
  CLEAR_DISP;
  if(Features.language_f)
  {
    displine ( 0 ,  &mTimeDisplayed, 1 );       
    displine ( 1 ,  &m24_Hour_Mode, 1 );  
    displine ( 2 ,  &mYesToChange, 1 );  
    displine ( 3 ,  &EscToExit, 1 );      
  }
  else
  {
    displine ( 0 ,  &mTimeDisplayed_sp, 1 );       
    displine ( 1 ,  &m24_Hour_Mode_sp, 1 );  
    displine ( 2 ,  &mYesToChange_sp, 1 );  
    displine ( 3 ,  &EscToExit_sp, 1 );  
  }
}

 
// Date Displayed in 
// Day/Month/Year order
// <YES> to Change
// <ESC> to Exit
LCDSTRING const dateDisplayed       = { 0, "Date Displayed in" };
LCDSTRING const day_month_YearOrder = { 0, "Day/Month/Year Order" };

LCDSTRING const dateDisplayed_sp       = { 0, "Fecha se muestra en" };
LCDSTRING const day_month_YearOrder_sp = { 0, "orden Dai/Mes/Ano" };

void  sDayMonthOrderChange ( void ) 
{
  CLEAR_DISP;
  if(Features.language_f)
  {
    displine ( 0 ,  &dateDisplayed, 1 );       
    displine ( 1 ,  &day_month_YearOrder, 1 );  
    displine ( 2 ,  &mYesToChange, 1 );  
    displine ( 3 ,  &EscToExit, 1 );      
  }
  else
  {
    displine ( 0 ,  &dateDisplayed_sp, 1 );       
    displine ( 1 ,  &day_month_YearOrder_sp, 1 );  
    displine ( 2 ,  &mYesToChange_sp, 1 );  
    displine ( 3 ,  &EscToExit_sp, 1 ); 
  }
}

// Date Displayed in 
// Month/Day/Year order 
// <YES> to Change
// <ESC> to Exit
LCDSTRING const month_day_YearOrder             = { 0, "Month/Day/Year Order" };
LCDSTRING const month_day_YearOrder_sp          = { 10, "orden Mes/Dia/Anor" };

void  sMonthDayOrderChange ( void ) 
{
  CLEAR_DISP;
  if(Features.language_f)
  {
    displine ( 0 ,  &dateDisplayed, 1 );       
    displine ( 1 ,  &month_day_YearOrder, 1 );  
    displine ( 2 ,  &mYesToChange, 1 );  
    displine ( 3 ,  &EscToExit, 1 );      
  }
  else
  {
    displine ( 0 ,  &dateDisplayed_sp, 1 );       
    displine ( 1 ,  &month_day_YearOrder_sp, 1 );  
    displine ( 2 ,  &mYesToChange_sp, 1 );  
    displine ( 3 ,  &EscToExit_sp, 1 );  
  }
}



//"    Error setting   "
//"    Internal Clock  "
//"   Restart Gauge    "
//"   Press <ENTER>"
LCDSTRING const mErrorSetting = { 0, "    Error setting   "};
LCDSTRING const mInternalClk  = { 0, "    Internal Clock  "};
LCDSTRING const mRestart      =  {0, "   Restart Gauge    "};

LCDSTRING const mErrorSetting_sp = { 10, "Error al configurar"};
LCDSTRING const mInternalClk_sp  = { 10, " el reloj interno"};
LCDSTRING const mRestart_sp      = {  0, "reiniciar el medidor"};

void  sClockSetError (  void )
{
  CLEAR_DISP;
  if(Features.language_f)
  {
    displine ( 0 ,  &mErrorSetting, 1 );       
    displine ( 1 ,  &mInternalClk, 1 );  
    displine ( 2 ,  &mRestart, 1 );  
    displine ( 3 ,  &PressEnter, 1 );      
  }
  else
  {
    displine ( 0 ,  &mErrorSetting_sp, 1 );       
    displine ( 1 ,  &mInternalClk_sp, 1 );  
    displine ( 2 ,  &mRestart_sp, 1 );  
    displine ( 3 ,  &PressEnter_sp, 1 );  
  }
}


void selectAM_PM ( void )
{
  CLEAR_DISP;
  if(Features.language_f)
  {
    gotoloc(0,0);
    _LCD_PRINT("Select"); 
    gotoloc(1,0);
    _LCD_PRINT("1. AM");       
    gotoloc(2,0);
    _LCD_PRINT("2. PM");          
  }
  else
  {
    gotoloc(0,0);
    _LCD_PRINT("Seleccionar"); 
    gotoloc(1,0);
    _LCD_PRINT("1. AM");       
    gotoloc(2,0);
    _LCD_PRINT("2. PM");   
  }
}

void change_val_yesno_text(void)
{
  LCD_position(LINE2);
  if(Features.language_f)
  {
    printAtPosLCD ( LINE2, "Change Value?");
    printAtPosLCD ( LINE3, "Press YES or NO"); 
    printAtPosLCD ( LINE4, "ESC to Exit"); 
  }
  else
  {
    printAtPosLCD ( LINE2,"Cambiar Valor?");
    printAtPosLCD ( LINE3, "Presione YES o NO" );
    printAtPosLCD ( LINE4, "<ESC> para Salir" );
    
  }  
}


void no_valid_std_count_text(void)
{
  LCD_position(LINE2);
  if(Features.language_f)
  {
    LCD_position(LINE1);
    LCD_print ( "No Valid Std. Counts" ) ;  
    LCD_position( LINE2 );
    LCD_print ( "Take a new STD Count" ) ;   
  }
  else
  {
    LCD_position(LINE1);
    LCD_print ( "Ningun STD valido" ) ;  
    LCD_position( LINE2 );
    _LCD_PRINT("Nueva Cuenta STD?");  ;   
    
  }  
}


void no_sd_card_detected_count_text(void)
{
  CLEAR_DISP;
  if(Features.language_f)
  {
   printAtPosLCD ( LINE2,"SD Card Not Detected" );
  }
  else
  {
   printAtPosLCD ( LINE2,"tarjeta SD no detect" );   
  }  
}

void writing_drift_results_text(void)
{
  CLEAR_DISP;
  if(Features.language_f)
  {
    LCD_PrintAtPosition( "Writing Drift Result", LINE2);
  }
  else
  {
    LCD_PrintAtPosition( "Escribir Deriva res.", LINE2);
  }  
}


void change_serial_number_text ( uint32 serial_number)
{
  CLEAR_DISP;
  if(Features.language_f)
  {  
    LCD_position (LINE1);
    _LCD_PRINT ( "Ser. Num:");
    LCD_position (LINE1 + 10);
    _LCD_PRINTF ( "%lu", serial_number);
    LCD_position (LINE2);
    _LCD_PRINT ( "Change Serial #?");
    Press_YES_or_NO(LINE3);
    ESC_to_Exit(LINE4);
  }
  else
  {
    LCD_position (LINE1);
    _LCD_PRINT ( "Ser. Num:");
    LCD_position (LINE1 + 10);
    _LCD_PRINTF ( "%lu", serial_number);
    LCD_position (LINE2);
    _LCD_PRINT ( "Cambiar Serial #?");
    Press_YES_or_NO(LINE3);
    ESC_to_Exit(LINE4);
  }
    
 }   

void enter_new_serial_number_text()
{
  CLEAR_DISP;
  if(Features.language_f)
  {
    LCD_position(LINE1);
    _LCD_PRINT ( "Enter New Serial");
    LCD_position(LINE2);
    _LCD_PRINT ( "Number:");
    YES_to_Accept(LINE3);   
    ESC_to_Exit(LINE4); 
  }
  else
  {
    LCD_position(LINE1);
    _LCD_PRINT ( "Introduce un nuevo ");
    LCD_position(LINE2);
    _LCD_PRINT ( "Numero:");
    YES_to_Accept(LINE3);   
    ESC_to_Exit(LINE4);
  }
}

void metal_mode_thickness_text()
{
  CLEAR_DISP;
  if(Features.language_f)
  {
    LCD_PrintAtPositionCentered("Metal Density Mode",LINE1+10);
    LCD_PrintAtPositionCentered("must have a Thickness",LINE2+10);
    if ( Features.SI_units == FALSE )
    {
      LCD_PrintAtPositionCentered("of .25\",.50,\".75\" ",LINE3+10);
      LCD_PrintAtPositionCentered("or 1.0\"",LINE3+10);
    }
    else
    {
      LCD_PrintAtPositionCentered("of 6.35mm,12.7mm,19mm",LINE3+10);
      LCD_PrintAtPositionCentered("or 25mm",LINE4+10);
    }
  }
  else
  {
    LCD_PrintAtPositionCentered("Modo de Dens. Metal ",LINE1+10);
    LCD_PrintAtPositionCentered("debe tener Grueso",LINE2+10);
    if ( Features.SI_units == FALSE )
    {
      LCD_PrintAtPositionCentered("de .25\",.50,\".75\" ",LINE3+10);
      LCD_PrintAtPositionCentered("o 1.0\"",LINE3+10);
    }
    else
    {
      LCD_PrintAtPositionCentered("de 6.35mm,12.7mm,19mm",LINE3+10);
      LCD_PrintAtPositionCentered("o 25mm",LINE4+10);
    }
  }
}        


void error_in_counting_text ( void )
{

  CLEAR_DISP;
  if(Features.language_f)
   LCD_PrintAtPositionCentered ( "Error in Counting",LINE2+10 );
  else
   LCD_PrintAtPositionCentered ( "Error al Contar",LINE2+10 );
}

void error_in_measurement_mode_text ( void )
{
  CLEAR_DISP;
  if(Features.language_f)
  {
    LCD_PrintAtPositionCentered ( "Project Storage",LINE1+10 );
    LCD_PrintAtPositionCentered ( "Measurement Mode",LINE2+10 );
    LCD_PrintAtPositionCentered ( "Does not Match",LINE3+10 );
    LCD_PrintAtPositionCentered ( "Current Mode Setting",LINE4+10 );
  }
  else
  {
    LCD_PrintAtPositionCentered ( "Project Storage",LINE1+10 );
    LCD_PrintAtPositionCentered ( "Measurement Mode",LINE2+10 );
    LCD_PrintAtPositionCentered ( "Does not Match",LINE3+10 );
    LCD_PrintAtPositionCentered ( "Current Mode Setting",LINE4+10 );
  }
  
}  
