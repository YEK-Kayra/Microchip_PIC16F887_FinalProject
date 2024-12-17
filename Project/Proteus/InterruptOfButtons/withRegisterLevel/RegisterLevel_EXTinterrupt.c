#include <16f887.h>

#use delay(clock=4M)

#use fast_io(b) // B portlarý 3 display için de ortak kullanýlacak


#fuses HS,NOWDT,NOPUT,NOLVP,NOCPD,NOPROTECT,NODEBUG,NOBROWNOUT,NOWRT

#byte my_INTCON_REG      = 0x0B
#byte my_OPTION_REG      = 0x81 

#define pin_led pin_B1



typedef struct{
   
    unsigned mPS0    : 1;   // Prescaler Rate Select bit 0 (bit 0)
    unsigned mPS1    : 1;   // Prescaler Rate Select bit 1 (bit 1)
    unsigned mPS2    : 1;   // Prescaler Rate Select bit 2 (bit 2)
    unsigned mPSA    : 1;   // Prescaler Assignment bit   (bit 3)
    unsigned mT0SE   : 1;   // TMR0 Source Edge Select bit (bit 4)
    unsigned mT0CS   : 1;   // TMR0 Clock Source Select bit (bit 5)
    unsigned mINTEDG : 1;   // Interrupt Edge Select bit (bit 6)
    unsigned mRBPU   : 1;   // Weak Pull-up Enable bit (bit 7)

}OPTIONreg_bit;

typedef struct{

    unsigned mRBIF : 1;   // bit 0, RB Port Change Interrupt Flag bit 
    unsigned mINTF : 1;   // bit 1, INT External Interrupt Flag bit 
    unsigned mT0IF : 1;   // bit 2, TMR0 Overflow Interrupt Flag bit 
    unsigned mRBIE : 1;   // bit 3, RB Port Change Interrupt Enable bit
    unsigned mINTE : 1;   // bit 4, INT External Interrupt Enable bit 
    unsigned mT0IE : 1;   // bit 5, TMR0 Overflow Interrupt Enable bit
    unsigned mPEIE : 1;   // bit 6, Peripheral Interrupt Enable bit 
    unsigned mGIE  : 1;   // bit 7, Global Interrupt Enable bit 
    
}INTCONreg_bit;

volatile INTCONreg_bit INTCONbits;  
volatile OPTIONreg_bit OPTIONbits; 



#int_EXT
void system_OK_Button_isr(){
   
   output_toggle(pin_led);
   delay_ms(500);
   INTCONbits.mINTF   = 0; //Clear flag
   
}


void main(void) 
{

  
   //b0_pin giriþ olarak ayarlandý
   set_tris_b(0x01);     
   
   OPTIONbits.mINTEDG = 1; //Interrupt at rising edge
   INTCONbits.mINTF   = 0; //Clear flag
   INTCONbits.mINTE   = 1; //External Interrupt EnableS
   INTCONbits.mGIE    = 1; //Global Interrupt Enable bit 
   
   my_INTCON_REG |= 
//!   ext_int_edge(L_TO_H); //Harici kesme Lojik 0'dan 1'e geçerken
//!   enable_interrupts(INT_EXT); //Harici kesme aktif
//!   enable_interrupts(GLOBAL); //Aktif kesmeler için genel kesme yetkisi ver 
   
 while(1) //Sonsuz döngü baþlangýcý
 {
 

        
  
 }
} 
