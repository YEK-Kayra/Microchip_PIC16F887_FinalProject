#include <16f887.h>

//--> DEVICES

//--> FUSES
#fuses HS,NOWDT,NOPUT,NOLVP,NOCPD,NOPROTECT,NODEBUG,NOBROWNOUT,NOWRT

//--> USEs
#use delay(clock=4M)
#use fast_io(b)
#use fast_io(e)

//--> PIN DEFINATIONS
#define led_pin pin_B1


#int_EXT
void system_OK_Button_isr(){
   
   output_toggle(led_pin);
   delay_ms(500);

}




void main(void) 
{
   
   set_tris_b(0x01);

   
   output_low(led_pin);
   
 ext_int_edge(L_TO_H); //Harici kesme Lojik 0'dan 1'e ge�erken
 enable_interrupts(INT_EXT); //Harici kesme aktif
 enable_interrupts(GLOBAL); //Aktif kesmeler i�in genel kesme yetkisi ver 
    
      while(1){
      
      }
      
}




