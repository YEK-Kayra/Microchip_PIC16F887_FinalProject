//Ister bisiklet ol ister tir, ne olursan ol yine gel

#include <16f887.h>

/********************************************************/
/*               SYSTEM DEVICES                         */
/********************************************************/
#device ADC = 10


/********************************************************/
/*               SYSTEM USES                            */
/********************************************************/
#use delay(clock=4M) 
#use rs232(baud=9600,parity=N,uart1,bits=8)
#use fast_io(b)

/********************************************************/
/*               SYSTEM INCLUDES                        */
/********************************************************/
#include <lcd.c>    
#include <math.h>       
                                       
/********************************************************/
/*               SYSTEM FUSES                           */
/********************************************************/                                         
#fuses HS,NOWDT,NOPUT,NOLVP,NOCPD,NOPROTECT,NODEBUG,NOBROWNOUT,NOWRT


/********************************************************/
/*               SYSTEM REGISTER                        */
/********************************************************/
//!#byte my_TIM0_OPTION_REG = 0x81 
//!#byte my_TIM0_MODULE_REG = 0x01 
//!#byte my_INTCON_REG      = 0x0B


/********************************************************/
/*               SYSTEM VARIABLES                       */
/********************************************************/
char readed_ID;   //Coming data will be When came from slave PIC
int8 systemLock;  //If the ID is valid, system open. Otherwise it will remain locked
int8 OPS_Status;  //55 means ops will be canceled, otherwise ops will be going on


/********************************************************/
/*               SYSTEM STRUCTS                         */
/********************************************************/


/********************************************************/
/*               SYSTEM DEFINATION                      */
/********************************************************/
/* ======== SYSTEM INPUT & OUTPUT ======== */
#define button_NEXT           pin_A3
#define button_BACK           pin_A4
#define button_Select         pin_A5
#define button_RemoveSelect   pin_C0
#define button_OPS_START      pin_C1
#define button_OPS_CANCEL     pin_B0


/********************************************************/
/*               FUNCTIONS PROTOTYPES                   */
/********************************************************/
/* ======== SYSTEM CONFIGURATION FUNCTIONS PROTOTYPES  ======== */
void SubSystem_Init(void);
void trisSetting_Init(void);
void interruptSetting_Init(void);
void adcSetting_Init(void);

/* ======== LCD SCREEN FUNCTIONS PROTOTYPES  ======== */
void SubSystem_lcd_IdleStatus(void);
void NavigateOperationMenu(void);
void NavigateTimeMoneyPreferenceMenu(void);

/* ======== UART SERIAL COM. FUNCTIONS PROTOTYPES  ======== */
void SubSystem_uart_CheckTheMessage(void);


/********************************************************/
/*               SYSTEM MACROS                          */
/********************************************************/



/********************************************************/
/*               SYSTEM INTERRUPTS                      */
/********************************************************/
#int_EXT
void system_isr_OperationCANCEL(){
  OPS_Status = 55;
}


void main(void) 
{
  
   //-->System parameters will be initialized
   SubSystem_Init();
      
   //-->Wait until unlock the system
   do{
        SubSystem_lcd_IdleStatus();   //Greeting the customer
        
        if(kbhit())
        {
           readed_ID = getc();
           SubSystem_uart_CheckTheMessage();
        }      
        
    }while(systemLock!=1);
 
  
      //Potansiyometreden de�erler okuyacak, opsiyonlar� gezmek i�in
      //ileri butonuna bas�l�nca pot hangi opsiyonda ise o opsiyona gidilecek
      //geri butonuna bas�l�nca pot ile tekrardan yukardan a�a�� gezilebilecek
      //diyelim ki bir opsiyonun i�ine girdi, pot kullanarak zamanlama ayarlar� ile oynuyor, 
      //�ift t�k ile ilgili zaman ve �creti se�ebilecek, tek t�k ile de bozabilecek
      do{
      }while(OPS_Status!=55);
          
    while(1)
    {
    
   
   
    }
 
 
}


/* ======== -BEGIN- SYSTEM CONFIGURATION FUNCTIONS -BEGIN- ======== */
//Function-1
void SubSystem_Init(void){
 lcd_init();
 trisSetting_Init();
 interruptSetting_Init();
 adcSetting_Init();
 
}

//Function-2
void trisSetting_Init(){
   set_tris_b(0x01);
}

//Function-3
void interruptSetting_Init(){
//-CONFIG--> OPERATION CANCEL BUTTON 
 ext_int_edge(L_TO_H); //Harici kesme Lojik 0'dan 1'e ge�erken
 enable_interrupts(INT_EXT); //Harici kesme aktif
 enable_interrupts(GLOBAL); //Aktif kesmeler i�in genel kesme yetkisi ver
}

//Function-4
void adcSetting_Init(){
  //-CONFIG--> Surfing Potentiometer
  setup_adc(adc_clock_div_32); 
  setup_adc_ports(sAN0);
  set_adc_channel(0); 
  delay_us(200);
  //-CONFIG--> Timer Potentiometer
  //-CONFIG--> Process Intensity Potentiometer
}
/* ======== -END- SYSTEM CONFIGURATION FUNCTIONS -END- ======== */




/* ======== -BEGIN- LCD SCREEN FUNCTIONS -BEGIN- ======== */
/**
 *@brief :It sequentially displays a welcome message and 
 *        prompts the user to present their ID card.
 */
void SubSystem_lcd_IdleStatus(void){
      lcd_gotoxy(4,1);
      printf(lcd_putc,"Mikroleum'a");
      lcd_gotoxy(4,2);
      printf(lcd_putc,"Hosgeldiniz");
      delay_ms(500);
      printf(lcd_putc,"\f");
      
      lcd_gotoxy(4,1);
      printf(lcd_putc,"Lutfen ID Karti");
      lcd_gotoxy(4,2);
      printf(lcd_putc,"Gosteriniz");
      delay_ms(500);
      printf(lcd_putc,"\f");
}

//Function-2
void NavigateOperationMenu(void){

}

//Function-3
void NavigateTimeMoneyPreferenceMenu(void){
}

/* ======== -END- LCD SCREEN FUNCTIONS -END- ======== */




/* ======== -BEGIN- UART PROTOCOL FUNCTIONS -BEGIN- ======== */
/**
 *@brief : This function checks the `readed_ID` variable to determine 
 *         the action to take based on predefined identifiers. Depending on the identifier,
 *         it displays a welcome message for specific users or sets the system to an idle state.
 */
void SubSystem_uart_CheckTheMessage(void){
   
   if(readed_ID == '+'){      //Mr. Sel�uk's ID
   
      lcd_gotoxy(4,1);
      printf(lcd_putc,"Hosgeldiniz");
      lcd_gotoxy(4,2);
      printf(lcd_putc,"Selcuk Bey");
      systemLock = 1;
   }
   else if(readed_ID == '*'){ //Mr. Emre's ID
   
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