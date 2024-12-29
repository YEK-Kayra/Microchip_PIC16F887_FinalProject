#include <16f887.h>

//--> FUSES
#fuses HS,NOWDT,NOPUT,NOLVP,NOCPD,NOPROTECT,NODEBUG,NOBROWNOUT,NOWRT

//--> USEs
#use delay(clock=4M)


//--> INCLUDES
#include <lcd.c>   

unsigned long data;

void main(void) 
{
      set_tris_b(0x00);
      set_tris_e(0x01);   


      lcd_init(); // LCD ekranýmýzý baþlatacak

      
      printf(lcd_putc, "\fADC : %lu",data );   
      printf(lcd_putc, "\f");


         
 while(1)
 {
     
 }
}

