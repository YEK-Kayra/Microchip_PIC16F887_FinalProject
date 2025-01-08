//Ister bisiklet ol ister tir, ne olursan ol yine gel

#include <16f887.h>

/*******************************************************************************************************************/  
/*                                                  SYSTEM DEVICES                                                */  
/*******************************************************************************************************************/  

#device ADC = 10


/*******************************************************************************************************************/  
/*                                                  SYSTEM USES                                                   */  
/*******************************************************************************************************************/  

#use delay(clock=4M) 
#use rs232(baud=9600,parity=N,uart1,bits=8)
#use fast_io(b) //Includes : The operation cancel button & The pins of the 7-segment display
#use fast_io(e) //Includes : 7-segment display pins for scanning



#define LCD_ENABLE_PIN  PIN_D0                                    
#define LCD_RS_PIN      PIN_D1                                    
#define LCD_RW_PIN      PIN_D2                                   
#define LCD_DATA4       PIN_D4                                    
#define LCD_DATA5       PIN_D5                                    
#define LCD_DATA6       PIN_D6                                    
#define LCD_DATA7       PIN_D7

/*******************************************************************************************************************/  
/*                                                SYSTEM INCLUDES                                                  */  
/*******************************************************************************************************************/  
#include <lcd.c>    
#include <math.h>       
                                
/*******************************************************************************************************************/  
/*                                                  SYSTEM FUSES                                                  */  
/*******************************************************************************************************************/  
                                      
#fuses HS,NOWDT,NOPUT,NOLVP,NOCPD,NOPROTECT,NODEBUG,NOBROWNOUT,NOWRT


/*******************************************************************************************************************/  
/*                                                 SYSTEM REGISTER                                                */  
/*******************************************************************************************************************/  

#byte my_TIM0_OPTION_REG = 0x81  //Config register
#byte my_TIM0_MODULE_REG = 0x01  //8 bit timer value storage register
#byte my_INTCON_REG      = 0x0B  //General purpose interrupt register


/*******************************************************************************************************************/  
/*                                                 SYSTEM STRUCTS                                                 */  
/*******************************************************************************************************************/  

typedef struct{

unsigned long int time_Foaming;         //Process number-1
unsigned long int time_Washing;         //Process number-2
unsigned long int time_Ventilation;     //Process number-3
unsigned long int mililitre_Polishing;  //Process number-4 

}MikroleumClient_HandleTypeDef;



/*******************************************************************************************************************/  
/*                                               SYSTEM VARIABLES                                                */  
/*******************************************************************************************************************/  


//------------ VAR --> CRITICAL PART <-- VAR ------------//
char readed_ID;            //Coming data will be When came from slave PIC
int8 systemLock;           //If the ID is valid, system open. Otherwise it will remain locked
int8 OPS_Status;           //55 means ops will be canceled, otherwise ops will be going on
char SelectionState;       //Put 'X' if button_Select is HIGH, put empty character if button_RemoveSelect is HIGH
int8 LockingMechanism = 1; //Will protect the system against repetitive or incorrect operations

//------------ VAR --> Keeps Clients <-- VAR ------------//
MikroleumClient_HandleTypeDef MikroClient[2]; //Each customer keeps their own records
                                              //MikroClient[0] Mr. Selcuk //ClientNumber=0
                                              //MikroClient[1] Mr. Emre   //ClientNumber=1
int8 ClientNumber;                            // Will be used as given on the right --> MikroClient[ClientNumber]                                        

//------------ VAR --> POTENTIOMETERS <-- VAR ------------//
int Index_OptionMenu;
int Index_TimeMoneyPreference;
int Index_PolishMililitre;
int Index_PastProcesses;
unsigned long int val_ADC_Pot_Surf;
unsigned long int val_ADC_Pot_Timer;
unsigned long int val_ADC_Pot_Polish;
unsigned long int val_ADC_Pot_PastProcesses;

//------------ VAR --> TIMER & ARRAYS <-- VAR ------------//
unsigned long int time_arr[12] = {20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240};//Seconds
unsigned long int timer0_isr_counter=0;
unsigned long int desired_value=0.0;
unsigned long int mililitrePolish[5] = {1,2,3,4,5}; // 1x100ml , 2x100ml, ... 5x100ml

//------------ VAR --> 7-SEGMENT DISPLAY NUMBERS <-- VAR ------------//
int8 segmentTable[16] = { 
    0x7E, 0x0C, 0xB6, 0x9E, 0xCC, //==> 0,1,2,3,4
    0xDA, 0xFA, 0x0E, 0xFE, 0xDE  //==> 5,6,7,8,9
};

//Split the current time value into digits.
int8 number_unitDigit=0;
int8 number_tensDigit=0;
int8 number_hundredDigit =0;

unsigned long int CurrentTime;     // Current countdown value

int8 counter_StartOpsButtonTick=0;  /*
                                     * If the value is 1, start the foaming process.
                                     * If the value is 2, start the washing process.
                                     * If the value is 3, start the ventilation process.
                                     * If the value is 4, start the polishing process.
                                     */
unsigned long SelectedTime;         //The SelectedTime variable will be sent to the macro to configure the desired_value variable
unsigned long currentPolishAmount;
int8 allProcessComplate_flag=0;    /** 
                                     * allProcessComplete_flag tracks the completion status of all processes, including polishing.
                                     * 0: Processes are still ongoing.
                                     * 1: All processes are complete.
                                     */
                                     
//------------ VAR --> INTERNAL EEPROM <-- VAR ------------//
//EEPROM variables to store the values selected by the user
// Variables for storing "written" values
unsigned int8 eeprom_foaming_written_cost     = 0;
unsigned int8 eeprom_washing_written_cost     = 0;
unsigned int8 eeprom_ventilating_written_cost = 0;
unsigned int8 eeprom_polishing_written_cost   = 0;

// Variables for storing "read" values
unsigned int8 eeprom_foaming_read_cost     = 0;
unsigned int8 eeprom_washing_read_cost     = 0;
unsigned int8 eeprom_ventilating_read_cost = 0;
unsigned int8 eeprom_polishing_read_cost   = 0;

//Keeps track of the last written address in EEPROM for each customer
unsigned int8 Client0_last_address=0; //For Selçuk Bey's account
unsigned int8 Client1_last_address=0; //For Emre Bey's account

unsigned int8 IndexOfReadMemory=2; /**
                                     * The operation number corresponds to the specific operation we are focusing on, 
                                     * e.g., operation-1, operation-2 ==> 1 and 2 are their operation numbers, 
                                     * with operation-1 being the minimum.
                                     */




/*******************************************************************************************************************/  
/*                                               SYSTEM DEFINITIONS                                              */  
/*******************************************************************************************************************/  

/* ======== SYSTEM INPUT & OUTPUT ======== */
//-IO--INPUT---->LCD Screen Control Section 
#define button_NEXT           pin_A3   //Proceed to the next operation.
#define button_BACK           pin_A4   //Go back to the previous operation.
#define button_Select         pin_A5   //Select the desired option.
#define button_RemoveSelect   pin_C0   //Remove the option I canceled.

#define button_OPS_START      pin_C1   /* button_OPS_START : 
                                        * This button performs the following functions:
                                        * - Starts the foaming, washing, ventilation, and polishing processes.
                                        * - Triggers the countdown for foaming, washing, and ventilation.
                                        * - Loads percentage progress into the system for polishing.
                                        */
                                        
#define button_OPS_CANCEL      pin_B0   //Cancel all operations and close the system for the user.
#define button_DrainPolishing  pin_C2   //It adds polish to the container while the button is pressed


//-IO--OUTPUT---->7-Segment Display Scanning Section
#define pin_HundredDigit_switch pin_E2
#define pin_TensDigit_switch    pin_E0 
#define pin_UnitDigit_switch    pin_E1 

//-MEMORY------> Internal EEPROM memory client base addresses
//They will share the EEPROM memory evenly
#define eeprom_Client0_systemStartAddress 0
#define eeprom_Client1_systemStartAddress 130



/*******************************************************************************************************************/  
/*                                               FUNCTIONS PROTOTYPES                                            */  
/*******************************************************************************************************************/  
/* ======== SYSTEM CONFIGURATION FUNCTIONS PROTOTYPES  ======== */
void SubSystem_Init(void);
void trisSetting_Init(void);
void displaySetting_Init(void);
void interruptSetting_Init(void);
void adcSetting_Init(void);
void SystemInit_RESET_Critical_Variable(void);

/* ======== LCD SCREEN FUNCTIONS PROTOTYPES  ======== */
void SubSystem_lcd_IdleStatus(void);
void NavigateOperationMenu(void);
void NavigateTimeMoneyPreferenceMenu(void);
void NavigatePolishingMenu(void);
void DisplayRecordsSequentiallyOnLCD(void);
void NavigatePastProcessesMenu(void);

/* ======== UART SERIAL COM. FUNCTIONS PROTOTYPES  ======== */
void SubSystem_uart_CheckTheMessage(void);

/* ======== 7-SEGMENT DISPLAY FUNCTIONS PROTOTYPES  ======== */
void sequentialDisplayScan(void);
void loadZeroValue_2_DisplaySegment(void);

/* ======== EEPROM WRITE/READ FUNCTIONS PROTOTYPES  ======== */

void Read_EEPROM_OpsVal_from_EEPROMregs(void);
void display_Selected_EEPROM_OpsVal_OnLCD(void);
void showSelectedPastProcessesOps(void);

void loadCostValuesToEEPROMVariables(void);
void Write_EEPROM_OpsVal_to_EEPROMregs(void);
void displayProcessCompletionStatus(void);

/*******************************************************************************************************************/  
/*                                                  SYSTEM MACROS                                                 */  
/*******************************************************************************************************************/  

/* ======== -BEGIN- SYSTEM POTENTIOMETER INDEXER -BEGIN- ======== */
#define ADC_TO_INDEXofOPTION_MENU(val_ADC_Pot_Surf)        \
   do{                                        \
          Index_OptionMenu = ((val_ADC_Pot_Surf*5)/1020);  \
          if(Index_OptionMenu >= 5){                       \
            Index_OptionMenu = 4;                          \
          }                                                \
   }while(0)
   
#define ADC_TO_INDEXofTIME_MONEY_MENU(val_ADC_Pot_Timer)              \
   do{                                                                \
          Index_TimeMoneyPreference = ((val_ADC_Pot_Timer*12)/1020);  \
          if(Index_TimeMoneyPreference >= 12){                        \
             Index_TimeMoneyPreference = 11;                          \
          }                                                           \
   }while(0)
   
#define ADC_TO_INDEXofPolish(val_ADC_Pot_Polish)                  \
   do{                                                            \
          Index_PolishMililitre = ((val_ADC_Pot_Polish*5)/1020);  \
          if(Index_PolishMililitre >= 5){                         \
            Index_PolishMililitre = 4;                            \
          }                                                       \
   }while(0)
   
#define ADC_TO_INDEXofPastProcesses(val_ADC_Pot_PastProcesses)         \
   do{                                                                 \
          Index_PastProcesses = ((val_ADC_Pot_PastProcesses*5)/1020);  \
          if(Index_PastProcesses >= 5){                                \
            Index_PastProcesses = 4;                                   \
          }                                                            \
   }while(0)
   
/* ======== -END- SYSTEM POTENTIOMETER INDEXER -END- ======== */   

/* ======== -BEGIN- SYSTEM TIMER TIME CONFIG -BEGIN- ======== */
//--> Timer Desired Value Calculator 
#define TimerScalingFactor ((1000.0) / 64.0)
#define SECOND_TO_ISR_COUNT(SelectedTime)                     \
   do{                                                        \
         desired_value = (SelectedTime * TimerScalingFactor); \
   }while(0)

/* ======== -END- SYSTEM TIMER TIME CONFIG -END- ======== */



/*******************************************************************************************************************/  
/*                                               SYSTEM INTERRUPTS                                               */  
/*******************************************************************************************************************/  

/* ======== -BEGIN- EXTERNAL INTERRUPT -BEGIN- ======== */
#int_EXT
void system_isr_OperationCANCEL(){
  OPS_Status = 55;       //That's a password to break do-while's loops
  my_INTCON_REG &= 0XFD; //Clear External Interrupt Flag bit 
}
/* ======== -END- EXTERNAL INTERRUPT -END- ======== */

/* ======== -BEGIN- TIMER_0 INTERRUPT -BEGIN- ======== */
#INT_TIMER0
void systemTimer0_isr(){

   timer0_isr_counter++;
   
/*
 * e.g., 20 seconds is equal to 20000 milliseconds.
 * We divide by 64 because Timer 0 generates an interrupt every 64 milliseconds.
 * (20000 milliseconds / 64 milliseconds) = 312.5
 * This gives the desired value for the timing calculation.
 */
   if(timer0_isr_counter == desired_value){
   
      //CurrentTime=0;
      timer0_isr_counter=0;  
      //Disable Timer0 Interrupt
      my_INTCON_REG &= 0xDF;
      
   }
   
   
   //If 1 second has passed
   if((timer0_isr_counter%16) == 0){
   
      //Decrease the current time value by one.
      CurrentTime-=1;
          
         if(CurrentTime==0){     
            //Disable Timer0 Interrupt
            my_INTCON_REG &= 0xDF;     
         }
   }
   
   // Clear timer0 overflow interrupt flag
   my_INTCON_REG &= 0xFB; 
   
}
/* ======== -END- TIMER_0 INTERRUPT -END- ======== */



/********************************************************************************************************************/
/*                                MAIN CODE & SEQUENTIAL STEPS OF THE OPERATION                                     */
/********************************************************************************************************************/
void main(void) 
{
  
   //System parameters and peripherals will be initialized
   SubSystem_Init();
  
while(1){//--------------------- BEGIN---MAIN WHILE(1)---BEGIN----------------------------//
  
  
   readed_ID  = '.';   //Clear Id symbol
   systemLock = 0;
   OPS_Status = 56;    //Disable 
   counter_StartOpsButtonTick=0;  //En son manalý deðiþiklik
   allProcessComplate_flag = 0;
   
   //To prevent old values from being written to the system EEPROM.
   eeprom_foaming_written_cost     = 0;
   eeprom_washing_written_cost     = 0;
   eeprom_ventilating_written_cost = 0;
   eeprom_polishing_written_cost   = 0;
  
   //Wait until unlock the system
//!   do{
//!           SubSystem_lcd_IdleStatus();   //Greeting the customer
//!           
//!           if(kbhit())
//!           {
//!              readed_ID = getc();
//!              SubSystem_uart_CheckTheMessage();
//!           }      
//!        
//!    }while(systemLock!=1);
 
    readed_ID = '+';
    delay_ms(1000);
    
    
    
    
  
/**
 * @brief Program navigates through option menus using a potentiometer and buttons.
 * 
 * Reads potentiometer values to select options or adjust parameters like time and cost.
 * The "NEXT" button moves forward or confirms selection, while the "BACK" button allows returning or resetting.
 * Specific sub-menus handle time/money preferences and polishing settings, with double-clicks for confirmation 
 * and single clicks for resetting choices.
 */


/*-----------------------------------------------------------------------------------------------------------------*/  
/*                       >>> BEGIN - SEQUENTIAL STEPS OF THE OPERATION - BEGIN <<<                                 */  
/*-----------------------------------------------------------------------------------------------------------------*/  
  

      do{
             //Start Of Conversation for ADC_Surf
             set_adc_channel(0);
             //Provide sufficient acquisition time.
             delay_us(20);
             
             //Get Surf_Pot ADC value(0-1024)
             val_ADC_Pot_Surf = read_adc();  
             
             //Convert Surf_Pot ADC value into the option menu index
             ADC_TO_INDEXofOPTION_MENU(val_ADC_Pot_Surf);
            
             //Use index value to show selected option and other one
             NavigateOperationMenu();
             
             //Clear selection
             SelectionState = ' ';
             
             //Reset the flag, becauese there will be a new process
             allProcessComplate_flag = 0;
             
                //The client wants to see the next section based on the selected option
                //Selections will be made for foaming, washing, and ventilation processes in this section
                if((input(button_NEXT) == 1) && (Index_OptionMenu <= 2))
                {
      
                        do{
                              //Start Of Conversation for ADC_Timer
                              set_adc_channel(1);
                              //Provide sufficient acquisition time.
                              delay_us(20);
                              
                              //Get Timer_Pot ADC value(0-1024)
                              val_ADC_Pot_Timer = read_adc(); 
                              
                              //Convert Timer_Pot ADC value into the option menu index
                              ADC_TO_INDEXofTIME_MONEY_MENU(val_ADC_Pot_Timer);
                              
                              //Use index value to show time & money & preference
                              NavigateTimeMoneyPreferenceMenu();
                              
                              //Break the while condation
                              if(input(button_BACK) == 1){
                                 break;
                              }
                        
                        }while(OPS_Status!=55); 
                }
                
                
                //In this section, only milliliter selection will be made for the polishing process
                if((input(button_NEXT) == 1) && (Index_OptionMenu == 3)){
   
                        do{
                              //Start Of Conversation for ADC_Timer
                              set_adc_channel(1);
                              
                              //Get Timer_Pot ADC value(0-1024)
                              val_ADC_Pot_Polish = read_adc(); 
                              
                              //Convert Timer_Pot ADC value into the option menu index
                              ADC_TO_INDEXofPolish(val_ADC_Pot_Polish);
                              
                              //Use index value to show time & money & preference
                              NavigatePolishingMenu();
                              
                              //Break the while condation
                              if(input(button_BACK) == 1){
                                 break;
                              }
                        
                        }while(OPS_Status!=55); 
                
                }
                
                //In this section, the client can see all of his past processes
                if((input(button_NEXT) == 1) && (Index_OptionMenu == 4)){
                
                        do{
                              //Start Of Conversation for ADC_Timer
                              set_adc_channel(1);
                              
                              //Get Timer_Pot ADC value(0-1024)
                              val_ADC_Pot_PastProcesses = read_adc(); 
                              
                              //Convert Timer_Pot ADC value into the past processes index
                              ADC_TO_INDEXofPastProcesses(val_ADC_Pot_PastProcesses);
                              
                              //Use past processes index value to show selected past processes
                               NavigatePastProcessesMenu();
                               
                               delay_ms(250);
                               
                                     //Enter the selected past-procesess
                                     if((input(button_NEXT) == 1)){      
                                        printf(lcd_putc, "\f");
                                        
                                       do{
                                       
                                          //This way, the location to be read in the EEPROM is determined.
                                          IndexOfReadMemory = (Index_PastProcesses + 1);
                                          
                                          //Display the selected past processes ops. on the LCD in an organized manner
                                          showSelectedPastProcessesOps();
                                          
                                            
                                       }while(input(button_BACK) != 1); //Wait until press back button
                                     
                                     delay_ms(250);
                                     }
                               
                               
                              //Break the while condation
                              if(input(button_BACK) == 1){
                                 break;
                              }
                              
                         }while(OPS_Status!=55); 
                }
                
      /****************************************************************************************************/  
      /*         BEGIN- Foam, Washing, Ventilation, and Polishing Operations will be executed -BEGIN      */  
      /****************************************************************************************************/
 
                
                /*
                 * In this section, a countdown will start for the selected times.
                 * Each time the button_OPS_START is pressed, a new countdown for the process will begin.
                */
                
                if(input(button_OPS_START) == 1){
                
                        printf(lcd_putc,"\f");
                        lcd_gotoxy(4,1);
                        printf(lcd_putc,"Operasyon");
                        lcd_gotoxy(4,2);
                        printf(lcd_putc,"Baslatildi");
                        
                        //Wait 100 milliseconds to allow for button interference
                        delay_ms(100); 
                        
                        //Increase the value by one, if clicked during each recheck
                        counter_StartOpsButtonTick+=1;
                        LockingMechanism = 1;
                        
                        do{
                              
                              //Loads the foaming time
                              if((counter_StartOpsButtonTick == 1) && (LockingMechanism == 1)){       
                                   
                                    //The SelectedTime variable will be sent to the macro to configure the desired_value variable
                                    SelectedTime = MikroClient[ClientNumber].time_Foaming;
                                    
                                    //The CurrentTime variable will hold the current number  to be displayed
                                    CurrentTime = MikroClient[ClientNumber].time_Foaming;
                                    
                                    //Determine the value that the desired_value variable will take.
                                    SECOND_TO_ISR_COUNT(SelectedTime);  
                                    
                                    // Enable interrupts by setting GIE (Global Interrupt Enable)
                                    // and TOIE (Timer0 Overflow Interrupt Enable) bits in INTCON register
                                    my_INTCON_REG |= 0xA0;
                                                                     
                                    //The lock mechanism was broken to prevent re-evaluation
                                    LockingMechanism = 0;
                                                                                
                              }
                              
                               //Loads the washing time
                              if((counter_StartOpsButtonTick == 2) && (LockingMechanism == 1)){       
                                   
                                    //The SelectedTime variable will be sent to the macro to configure the desired_value variable
                                    SelectedTime = MikroClient[ClientNumber].time_Washing;
                                    
                                    //The CurrentTime variable will hold the current number  to be displayed
                                    CurrentTime = MikroClient[ClientNumber].time_Washing;
                                    
                                    //Determine the value that the desired_value variable will take.
                                    SECOND_TO_ISR_COUNT(SelectedTime);  
                                    
                                    // Enable interrupts by setting GIE (Global Interrupt Enable)
                                    // and TOIE (Timer0 Overflow Interrupt Enable) bits in INTCON register
                                    my_INTCON_REG |= 0xA0;
                                                                     
                                    //The lock mechanism was broken to prevent re-evaluation
                                    LockingMechanism = 0;
                                                                                
                              }
                                                        
                              //Loads the Ventilation time
                              if((counter_StartOpsButtonTick == 3) && (LockingMechanism == 1)){       
                                   
                                    //The SelectedTime variable will be sent to the macro to configure the desired_value variable
                                    SelectedTime = MikroClient[ClientNumber].time_Ventilation;
                                    
                                    //The CurrentTime variable will hold the current number  to be displayed
                                    CurrentTime = MikroClient[ClientNumber].time_Ventilation;
                                    
                                    //Determine the value that the desired_value variable will take.
                                    SECOND_TO_ISR_COUNT(SelectedTime);  
                                    
                                    // Enable interrupts by setting GIE (Global Interrupt Enable)
                                    // and TOIE (Timer0 Overflow Interrupt Enable) bits in INTCON register
                                    my_INTCON_REG |= 0xA0;
                                                                     
                                    //The lock mechanism was broken to prevent re-evaluation
                                    LockingMechanism = 0;
                                                                                
                              }
                              
                              //Load the selected amount of polish
                              if((counter_StartOpsButtonTick == 4) && (LockingMechanism == 1)){
                              
                                    //The loop will exit when the polish is finished or if the process is canceled
                                    do{
                                    
                                       if(input(button_DrainPolishing) == 1){
                                          currentPolishAmount-=4;
                                          delay_ms(100);
                                          }
                                          
                                          if(currentPolishAmount<0){
                                          currentPolishAmount=0;
                                          }
                                       //Display/scroll the CurrentTime value on the displays
                                       sequentialDisplayScan();
                                       
                                    }while(currentPolishAmount != 0);
                                       
                                    LockingMechanism=0;
                                    allProcessComplate_flag = 1;
                              }
                                                    
                             //Display/scroll the CurrentTime value on the displays
                             sequentialDisplayScan();
                             
                             
                             /** 
                               * These conditions are used to break out of the loop for various reasons, 
                               * and will redirect the program back to the main menu.
                               * 
                               * - If allProcessComplete_flag is 1, indicating all processes are finished, the loop will exit.
                               * - If CurrentTime equals 1 and SelectedTime equals 60, the loop will also exit.
                               */
                                if(allProcessComplate_flag == 1){
                                    break;    
                                }                                    
                                if((CurrentTime==1) && (SelectedTime==60)){
                                  break;
                                }
            
                        }while( (CurrentTime!=0) );
                        
                        
                }
                           
      /****************************************************************************************************/  
      /*         END- Foam, Washing, Ventilation, and Polishing Operations will be executed -END      */  
      /****************************************************************************************************/  
      
      
               delay_ms(100);
               
            //Load the initial value(0) into the segments.
            loadZeroValue_2_DisplaySegment();
         
         
       //If the cancel operation button hasn't been pressed, it will return to the main menu again
      }while(OPS_Status!=55);
      
  displayProcessCompletionStatus();//buraya koyarsan ana menüye gitmez ki
  
      
      
}//--------------------- END---MAIN WHILE(1)---END----------------------------//


  
      
/*-----------------------------------------------------------------------------------------------------------------*/  
/*                             >>> END - SEQUENTIAL STEPS OF THE OPERATION - END <<<                               */  
/*-----------------------------------------------------------------------------------------------------------------*/  
   
 
}


/* ======== -BEGIN- SYSTEM CONFIGURATION FUNCTIONS -BEGIN- ======== */
//Function-1
void SubSystem_Init(void){
 lcd_init();
 trisSetting_Init();
 displaySetting_Init();
 interruptSetting_Init();
 adcSetting_Init();
}

//Function-2
void trisSetting_Init(){
   set_tris_b(0x01); //B0       ==> Cancel all operations and close the system
                     //B1 to B7 ==> 7-Segment Display 
                     
   //Assign the Display Scanning switches as outputs
   output_drive(pin_HundredDigit_switch); 
   output_drive(pin_TensDigit_switch);
   output_drive(pin_UnitDigit_switch);
     
}

//Function-3
void interruptSetting_Init(){
//---> OPERATION CANCEL BUTTON CONFIGURATION  <---//

        //Interrupt on rising edge of INT pin 
        my_TIM0_OPTION_REG |= 0x40;
        //External Interrupt Enable &  Global Interrupt Enable 
        my_INTCON_REG |= 0x90; 
     
//---> TIMER & INTERRUPT CONFIGURATION  <---//
     
        // T(overflow) = InstructionTime*PSC*(256-TIM0_val)
        //        64mS = 1uS * 256 * (256-56)
        my_TIM0_OPTION_REG &= 0xC7 ;
        my_TIM0_MODULE_REG = 56;

}

//Function-4
void adcSetting_Init(){
     //-CONFIG--> Surfing Potentiometer & Timer Potentiometer
        setup_adc_ports(sAN0|sAN1);
        setup_adc(adc_clock_div_32);
        delay_us(20);
       
     //-CONFIG--> Process Intensity Potentiometer
}

//Function-5
void displaySetting_Init(){

   //Apply voltage to display the digits
   output_high(pin_UnitDigit_switch);    
   output_high(pin_TensDigit_switch);     
   output_high(pin_HundredDigit_switch); 
   
   //Load the initial value into the segments.
   output_b(segmentTable[0]); 
}

/* ======== -END- SYSTEM CONFIGURATION FUNCTIONS -END- ======== */



/* ======== -BEGIN- LCD SCREEN FUNCTIONS -BEGIN- ======== */
/**
 *@brief :It sequentially displays a welcome message and 
 *        prompts the user to present their ID card.
 */
void SubSystem_lcd_IdleStatus(void){
      printf(lcd_putc,"\f");
      lcd_gotoxy(4,1);
      printf(lcd_putc,"Mikroleum'a");
      lcd_gotoxy(4,2);
      printf(lcd_putc,"Hosgeldiniz");
      delay_ms(800);
      printf(lcd_putc,"\f");
      
      lcd_gotoxy(1,1);
      printf(lcd_putc,"Lutfen Sifrenizi");
      lcd_gotoxy(6,2);
      printf(lcd_putc,"Giriniz");
      delay_ms(800);
      printf(lcd_putc,"\f");
}

//Function-2
void NavigateOperationMenu(void){
   printf(lcd_putc, "\f"); // LCD'yi temizle

    // 1. Seçenek
    if (Index_OptionMenu == 0) {
        lcd_gotoxy(1, 1);
        printf(lcd_putc, "1-Kopuk Islem <-");
        lcd_gotoxy(1, 2);
        printf(lcd_putc, "2-Su Islem");   
    } 

    // 2. Seçenek
    if (Index_OptionMenu == 1) {
        lcd_gotoxy(1, 1);
        printf(lcd_putc, "2-Su Islem <-");
        lcd_gotoxy(1, 2);
        printf(lcd_putc, "3-Hava Islem");       
    }
    
    // 3. Seçenek
        if (Index_OptionMenu == 2) {
        lcd_gotoxy(1, 1);
        printf(lcd_putc, "3-Hava Islem <-");
        lcd_gotoxy(1, 2);
        printf(lcd_putc, "4-Cila Islem");   
    } 

    // 4. Seçenek
    if (Index_OptionMenu == 3) {
        lcd_gotoxy(1, 1);
        printf(lcd_putc, "4-Cila Islem <-");
        lcd_gotoxy(1, 2);
        printf(lcd_putc, "5-Kayit Islem");    
    }
    
    // 5. Seçenek
    if (Index_OptionMenu == 4) {
        lcd_gotoxy(1, 1);
        printf(lcd_putc, "5-Kayit Islem <-"); 
    }


}

//Function-3
void NavigateTimeMoneyPreferenceMenu(void){
   
   printf(lcd_putc, "\f"); 
   lcd_gotoxy(1,1);
   printf(lcd_putc, "Ucret:%d tl",(unsigned int)((time_arr[Index_TimeMoneyPreference]/10)*2));
   lcd_gotoxy(1,2);
   printf(lcd_putc, "Sure:%ld",time_arr[Index_TimeMoneyPreference]);
   lcd_gotoxy(10,2);
   printf(lcd_putc, "Sec:");
   lcd_gotoxy(14,2);
   printf(lcd_putc, "%c",SelectionState);
   
   
   if(input(button_Select) == 1){
      SelectionState = 'X';
      
      //-->The values will be saved into foaming, washing, ventilation register according to the Index_OptionMenu
      if(Index_OptionMenu == 0){
         MikroClient[ClientNumber].time_Foaming = time_arr[Index_TimeMoneyPreference];
      }
      else if(Index_OptionMenu == 1){
         MikroClient[ClientNumber].time_Washing = time_arr[Index_TimeMoneyPreference];
      }
      else if(Index_OptionMenu == 2){
         MikroClient[ClientNumber].time_Ventilation = time_arr[Index_TimeMoneyPreference];
      }
   }
   
   if(input(button_RemoveSelect) == 1){
      SelectionState = ' ';
   }
   
   delay_ms(80);
}

//Function-4
void NavigatePolishingMenu(){
   printf(lcd_putc, "\f"); 
   lcd_gotoxy(1,1);
   printf(lcd_putc, "Ucret:%ld tl",(15*mililitrePolish[Index_PolishMililitre]));
   lcd_gotoxy(1,2);
   printf(lcd_putc, "Ml:%ld",(unsigned long int)(100*mililitrePolish[Index_PolishMililitre]));
   lcd_gotoxy(10,2);
   printf(lcd_putc, "Sec:");
   lcd_gotoxy(14,2);
   printf(lcd_putc, "%c",SelectionState);
   
   
   if(input(button_Select) == 1){
      SelectionState = 'X';
      
      //-->The values will be saved into foaming, washing, ventilation register according to the Index_OptionMenu
      if(Index_OptionMenu == 3){
         MikroClient[ClientNumber].mililitre_Polishing = (100*mililitrePolish[Index_PolishMililitre]); 
         currentPolishAmount = MikroClient[ClientNumber].mililitre_Polishing;
      }
      
   }
   
   if(input(button_RemoveSelect) == 1){
      SelectionState = ' ';
   }
   
   delay_ms(80);
}


void NavigatePastProcessesMenu(){
   printf(lcd_putc, "\f");
   
   // 1-Islem
   if (Index_PastProcesses == 0) {
        lcd_gotoxy(1, 1);
        printf(lcd_putc, "Islem - 1 <-");
        lcd_gotoxy(1, 2);
        printf(lcd_putc, "Islem - 2");   
    } 

    // 2-Islem
   if (Index_PastProcesses == 1) {
        lcd_gotoxy(1, 1);
        printf(lcd_putc, "Islem - 2 <-");
        lcd_gotoxy(1, 2);
        printf(lcd_putc, "Islem - 3");   
    } 
       // 3-Islem
   if (Index_PastProcesses == 2) {
        lcd_gotoxy(1, 1);
        printf(lcd_putc, "Islem - 3 <-");
        lcd_gotoxy(1, 2);
        printf(lcd_putc, "Islem - 4");   
    } 

    // 4-Islem
   if (Index_PastProcesses == 3) {
        lcd_gotoxy(1, 1);
        printf(lcd_putc, "Islem - 4 <-");
        lcd_gotoxy(1, 2);
        printf(lcd_putc, "Islem - 5");  
    } 
    
   // 5-Islem
   if (Index_PastProcesses == 4) {
        lcd_gotoxy(1, 1);
        printf(lcd_putc, "Islem - 5 <-");
    } 
  
   delay_ms(80);

}

//Function-5
void DisplayRecordsSequentiallyOnLCD(){
   printf(lcd_putc,"\f");
   lcd_gotoxy(2,1);
   printf(lcd_putc,"CLOSING");
   delay_ms(1000);
   printf(lcd_putc,"\f");  
   lcd_gotoxy(1,1);
   printf(lcd_putc, "Kopuk:%ld", MikroClient[ClientNumber].time_Foaming);
   lcd_gotoxy(1,2);
   printf(lcd_putc, "Su::%ld",MikroClient[ClientNumber].time_Washing);
   delay_ms(2000);
   printf(lcd_putc,"\f");  
   lcd_gotoxy(1,1);
   printf(lcd_putc, "Hava:%ld", MikroClient[ClientNumber].time_Ventilation);
   lcd_gotoxy(1,2);
    printf(lcd_putc, "Cila:%ld", MikroClient[ClientNumber].mililitre_Polishing);
   delay_ms(2000);
}

/* ======== -END- LCD SCREEN FUNCTIONS -END- ======== */




/* ======== -BEGIN- UART PROTOCOL FUNCTIONS -BEGIN- ======== */
/**
 *@brief : This function checks the `readed_ID` variable to determine 
 *         the action to take based on predefined identifiers. Depending on the identifier,
 *         it displays a welcome message for specific users or sets the system to an idle state.
 */
void SubSystem_uart_CheckTheMessage(void){
   
   if(readed_ID == '+'){      //Mr. Selçuk's ID
      ClientNumber = 0;       //System works for Mr. Selcuk
      lcd_gotoxy(4,1);
      printf(lcd_putc,"Hosgeldiniz");
      lcd_gotoxy(4,2);
      printf(lcd_putc,"Selcuk Bey");
      systemLock = 1;
   }
   else if(readed_ID == '*'){ //Mr. Emre's ID
      ClientNumber = 1;       //System works for Mr. Emre
      lcd_gotoxy(4,1);
      printf(lcd_putc,"Hosgeldiniz");
      lcd_gotoxy(4,2);
      printf(lcd_putc,"Emre Bey");
      systemLock = 1;
   }
   else if(readed_ID == '.'){ //Slave pic is idle status
       systemLock = 0;
   }

}
/* ======== -END- UART PROTOCOL FUNCTIONS -END- ======== */


/* ======== -BEGIN- 7-SEGMENT DISPLAY -BEGIN- ======== */
void sequentialDisplayScan(){

         //It indicates that the polishing process has now started
         if((counter_StartOpsButtonTick == 4)){
                  
            number_unitDigit    = (currentPolishAmount%10);
            number_tensDigit    = ((currentPolishAmount/10)%10);
            
         }
         //CurrentTime will be taken from the timer.
         else{
            
            number_unitDigit    = (CurrentTime%10);  
            number_tensDigit    = ((CurrentTime/10)%10); 
            number_hundredDigit = ((CurrentTime/100)%10);
         }
         
   
         
        
         /*
          * Perform digit changes using the transistor.
          * Display the corresponding value for the relevant digit.
          * Wait for a millisecond to allow the human eye to perceive the change.
          */
          output_high(pin_UnitDigit_switch);     
          output_low(pin_TensDigit_switch);      
          output_low(pin_HundredDigit_switch);   
          output_b(segmentTable[number_unitDigit]); 
          delay_ms(10);                            
          
          output_low(pin_UnitDigit_switch);      
          output_high(pin_TensDigit_switch);     
          output_low(pin_HundredDigit_switch);          
          output_b(segmentTable[number_tensDigit]); 
          delay_ms(10);  
          
          output_low(pin_UnitDigit_switch);       
          output_low(pin_TensDigit_switch);       
          output_high(pin_HundredDigit_switch);   
          output_b(segmentTable[number_hundredDigit]); 
          delay_ms(10); 
   
}

void loadZeroValue_2_DisplaySegment(){

   //Apply voltage to display the digits
   output_high(pin_UnitDigit_switch);    
   output_high(pin_TensDigit_switch);     
   output_high(pin_HundredDigit_switch); 
   
   //Load the initial value into the segments.
   output_b(segmentTable[0]); 
}

/* ======== -END- 7-SEGMENT DISPLAY -END- ======== */


/* ===================== -BEGIN- EEPROM WRITE/READ -BEGIN- ===================== */

void showSelectedPastProcessesOps(void){

   Read_EEPROM_OpsVal_from_EEPROMregs();
   
   display_Selected_EEPROM_OpsVal_OnLCD();

}

void display_Selected_EEPROM_OpsVal_OnLCD(){
          
      printf(lcd_putc, "\f");
      
      lcd_gotoxy(1,1);
      printf(lcd_putc, "F: %d TL",eeprom_foaming_read_cost); 
      lcd_gotoxy(8,1);
      printf(lcd_putc, " W: %d TL",eeprom_washing_read_cost); 
      lcd_gotoxy(1,2);
      printf(lcd_putc, "V: %d TL",eeprom_ventilating_read_cost); 
      lcd_gotoxy(8,8);
      printf(lcd_putc, " P: %d TL",eeprom_polishing_read_cost); 
      delay_ms(100);
       
}

void Read_EEPROM_OpsVal_from_EEPROMregs(){
    
    // NE ZAMAN NEXT BUTONUNA BASILIR O ZAMAN SEÇÝM YAPILIR
    // IndexOfReadMemory butonun üzerinde bulunduðu yer(next demeden önce)
    
        if(ClientNumber == 0)
        {
                  
            eeprom_foaming_read_cost     = read_eeprom( eeprom_Client0_systemStartAddress + (((IndexOfReadMemory - 1)*4 )+4) );  
            eeprom_washing_read_cost     = read_eeprom( eeprom_Client0_systemStartAddress + (((IndexOfReadMemory - 1)*4 )+5) );
            eeprom_ventilating_read_cost = read_eeprom( eeprom_Client0_systemStartAddress + (((IndexOfReadMemory - 1)*4 )+6) );
            eeprom_polishing_read_cost   = read_eeprom( eeprom_Client0_systemStartAddress + (((IndexOfReadMemory - 1)*4 )+7) );
     
        }
        
        if(ClientNumber == 1){
        
            eeprom_foaming_read_cost     = read_eeprom( eeprom_Client1_systemStartAddress + (((IndexOfReadMemory - 1)*4 )+4) );  
            eeprom_washing_read_cost     = read_eeprom( eeprom_Client1_systemStartAddress + (((IndexOfReadMemory - 1)*4 )+5) );
            eeprom_ventilating_read_cost = read_eeprom( eeprom_Client1_systemStartAddress + (((IndexOfReadMemory - 1)*4 )+6) );
            eeprom_polishing_read_cost   = read_eeprom( eeprom_Client1_systemStartAddress + (((IndexOfReadMemory - 1)*4 )+7) );
        
        }

}



void displayProcessCompletionStatus(){

      printf(lcd_putc, "\f");
      
      lcd_gotoxy(2,1);
      printf(lcd_putc, "Operasyon Sonu");   
      delay_ms(2000);
      printf(lcd_putc, "\f");
      lcd_gotoxy(3,1);
      printf(lcd_putc, "Islemleriniz");  
      lcd_gotoxy(3,2);
      printf(lcd_putc, "Kaydediliyor");
      
      loadCostValuesToEEPROMVariables();
         
      delay_ms(1000);
      
      printf(lcd_putc, "\f");
      lcd_gotoxy(5,1);
      printf(lcd_putc, "Kayitlar");  
      lcd_gotoxy(4,2);
      printf(lcd_putc, "Tamamlandi");
      
      delay_ms(1000);
      
      printf(lcd_putc, "\f");
      lcd_gotoxy(4,1);
      printf(lcd_putc, "Saglicakla");  
      lcd_gotoxy(4,2);
      printf(lcd_putc, "Kaliniz");
      
      delay_ms(1000);
      printf(lcd_putc, "\f");
      
}




void write_EEPROMVariables_to_EEPROMregs(){

// Operations will be performed for the relevant customer
// A new operation will be defined each time the system is activated
// Example usage:
// Main Menu -> Registration Process ->NextButton -> Operation 1 -> LCD display view:
//                                                                [F=32TL    W=12TL] F: foaming     , W: Water
//                                                                [V=24TL    P=85TL] V: Ventilating , P: Polishing
//                                   ->NextButton -> Operation 2 -> LCD display view:
//                                                                [F=32TL    W=12TL] F: foaming     , W: Water
//                                                                [V=24TL    P=85TL] V: Ventilating , P: Polishing

   //Selcuk Bey's ID
   if(ClientNumber == 0){
      
      //Pull the last address to keep going on
      //The last cursor position address will always be written to the zero address, to ensure that new data does not overwrite previously written data
      Client0_last_address = read_eeprom(eeprom_Client0_systemStartAddress);
      
      write_eeprom( (eeprom_Client0_systemStartAddress + Client0_last_address + 5), (eeprom_foaming_written_cost)     ); Client0_last_address++; //Save foaming cost
      write_eeprom( (eeprom_Client0_systemStartAddress + Client0_last_address + 5), (eeprom_washing_written_cost)     ); Client0_last_address++; //Save washing cost
      write_eeprom( (eeprom_Client0_systemStartAddress + Client0_last_address + 5), (eeprom_ventilating_written_cost) ); Client0_last_address++; //Save Ventilating cost
      write_eeprom( (eeprom_Client0_systemStartAddress + Client0_last_address + 5), (eeprom_polishing_written_cost)   ); Client0_last_address++; //Save Polishing cost
      write_eeprom( (eeprom_Client0_systemStartAddress                           ), ( Client0_last_address)           );                         //Save cursor location in the client0 start address
   }
   
   if(ClientNumber == 1){
      
      //Pull the last address to keep going on
      //The last cursor position address will always be written to the zero address, to ensure that new data does not overwrite previously written data
      Client1_last_address = read_eeprom(eeprom_Client1_systemStartAddress);
      
      write_eeprom( (eeprom_Client1_systemStartAddress + Client1_last_address + 5), (eeprom_foaming_written_cost)     ); Client1_last_address++; //Save foaming cost
      write_eeprom( (eeprom_Client1_systemStartAddress + Client1_last_address + 5), (eeprom_washing_written_cost)     ); Client1_last_address++; //Save washing cost
      write_eeprom( (eeprom_Client1_systemStartAddress + Client1_last_address + 5), (eeprom_ventilating_written_cost) ); Client1_last_address++; //Save Ventilating cost
      write_eeprom( (eeprom_Client1_systemStartAddress + Client1_last_address + 5), (eeprom_polishing_written_cost)   ); Client1_last_address++; //Save Polishing cost
      write_eeprom( (eeprom_Client1_systemStartAddress                           ), ( Client1_last_address)           );                         //Save cursor location in the client0 start address
   
   }
   
}

//will be implemented
void loadCostValuesToEEPROMVariables(){

      if(counter_StartOpsButtonTick >= 1){
         //Cost values to be written
         eeprom_foaming_written_cost     = (unsigned int8)(((MikroClient[ClientNumber].time_Foaming)/10)*2);
         eeprom_washing_written_cost     = (unsigned int8)(((MikroClient[ClientNumber].time_Washing)/10)*2);
         eeprom_ventilating_written_cost = (unsigned int8)(((MikroClient[ClientNumber].time_Ventilation)/10)*2);
         eeprom_polishing_written_cost   = (unsigned int8)(((MikroClient[ClientNumber].mililitre_Polishing)/100)*15);
      }
      else{
      
         eeprom_foaming_written_cost     = 0;
         eeprom_washing_written_cost     = 0;
         eeprom_ventilating_written_cost = 0;
         eeprom_polishing_written_cost   = 0;
      
      }
      
     
     
      //Write data to the allocated space for the customer who is using it
      write_EEPROMVariables_to_EEPROMregs();
}




/* ===================== -END- EEPROM WRITE/READ -END- ===================== */


  
    


/********************************************************************************************************************/
/*                                MAIN CODE & SEQUENTIAL STEPS OF THE OPERATION                                     */
/********************************************************************************************************************/
