#include <16f887.h>

//--> FUSES
#fuses HS,NOWDT,NOPUT,NOLVP,NOCPD,NOPROTECT,NODEBUG,NOBROWNOUT,NOWRT

//--> USEs
#use delay(clock=4M)


//--> INCLUDES
#include <lcd.c>   


typedef struct{

unsigned long int time_Foaming;         //Process number-1
unsigned long int time_Washing;         //Process number-2
unsigned long int time_Ventilation;     //Process number-3
unsigned long int mililitre_Polishing;       //Process number-4 

}MikroleumClient_HandleTypeDef;


MikroleumClient_HandleTypeDef MikroClient[2]; //Each customer keeps their own records
                                              //MikroClient[0] Mr. Selcuk //ClientNumber=0
                                              //MikroClient[1] Mr. Emre   //ClientNumber=1
int8 ClientNumber;                            // Will be used as given on the right --> MikroClient[ClientNumber]  


//Will be implemented//

//------------ VAR --> INTERNAL EEPROM <-- VAR ------------//
//EEPROM variables to store the values selected by the user
// Variables for storing "written" values
unsigned int8 eeprom_foaming_written     = 0;
unsigned int8 eeprom_washing_written     = 0;
unsigned int8 eeprom_ventilating_written = 0;
unsigned int8 eeprom_polishing_written   = 0;

// Variables for storing "read" values
unsigned int8 eeprom_foaming_read     = 0;
unsigned int8 eeprom_washing_read     = 0;
unsigned int8 eeprom_ventilating_read = 0;
unsigned int8 eeprom_polishing_read   = 0;



//Keeps track of the last written address in EEPROM for each customer
unsigned int8 eeprom_Client0_last_written_address; //For Sel�uk Bey's account
unsigned int8 eeprom_Client1_last_written_address; //For Emre Bey's account

//Will be implemented into //
/* ======== LCD SCREEN FUNCTIONS PROTOTYPES  ======== */
void displayEEPROMDataOnLCD(void);

void main(void) 
{

      lcd_init(); // LCD ekran�m�z� ba�latacak

      //Retrieve records and inform the customer
      displayEEPROMDataOnLCD();
      
      //
      
      
      
      
      
      

      
      
      
      
            
 while(1)
 {
     
 }
}

//Will be implemented//
void displayEEPROMDataOnLCD(){

      lcd_gotoxy(2,1);
      printf(lcd_putc, "Operasyon Sonu");   
      delay_ms(2000);
      printf(lcd_putc, "\f");
      lcd_gotoxy(3,1);
      printf(lcd_putc, "Islemleriniz");  
      lcd_gotoxy(3,2);
      printf(lcd_putc, "Kaydediliyor");
      
      //Fiyatlar yaz�lacak sadece
      eeprom_foaming_written     = MikroClient[ClientNumber].time_Foaming;
      eeprom_washing_written     = MikroClient[ClientNumber].time_Washing;
      eeprom_ventilating_read    = MikroClient[ClientNumber].time_Ventilation;
      eeprom_polishing_written   = MikroClient[ClientNumber].mililitre_Polishing;
      
      delay_ms(1000);
      
      printf(lcd_putc, "\f");
      lcd_gotoxy(5,1);
      printf(lcd_putc, "Kayitlar");  
      lcd_gotoxy(4,2);
      printf(lcd_putc, "Tamamlandi");
      
      delay_ms(1000);
      
      printf(lcd_putc, "\f");
      lcd_gotoxy(4,1);
      printf(lcd_putc, "Ana Menuye");  
      lcd_gotoxy(2,2);
      printf(lcd_putc, "Yonlendiriliyor");
      
      delay_ms(1000);
      printf(lcd_putc, "\f");
      
}
