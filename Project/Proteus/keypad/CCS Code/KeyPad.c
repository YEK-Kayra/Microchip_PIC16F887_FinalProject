#include <16f887.h>  

#include <string.h>

#use delay(clock=4M)
#use fast_io(d)  
#use rs232(baud=9600, parity=N, uart1, bits=8) 


#fuses HS,NOWDT,NOPUT,NOLVP,NOCPD,NOPROTECT,NODEBUG,NOBROWNOUT,NOWRT


char related_ID;              // Stores the character to be sent via UART
char password[5];             // Collects the entered number characters, 5 characters including the null terminator '\0'
char selcukID[5] = "1234";    // Selcuk's password
char emreID[5]   = "4321";    // Emre's password
int8 password_counter = 0;    // Counter for entered characters, query statements are triggered when it reaches 4



// 4 rows and 3 columns representing the keypad layout
char keypad[4][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
};

void main(void) 
{
   set_tris_d(0xF0); 
   
    while(1)
    {
      
         //Scan all rows(lines) sequentially
         for(int8 row = 0; row < 4; row++)
         {
           
               output_d(0x00);                            //Clear all D port pins     
               output_bit(PIN_D0 + row, 1);               //Set the current line to "HIGH"      
               delay_ms(10);                              //Short delay for debounce to stabilize the button press              
      
               //Scan all columns sequentially
               for(int8 col = 0; col < 3; col++)
               {
                     //If found, try to find the column that is HIGH
                     if(input(PIN_D4 + col)) 
                     {    
                        //Wait until the button is released
                        while(input(PIN_D4 + col));   
                        
                        password[password_counter]=keypad[row][col];       //Save the selected row and column value to the array
                        password_counter++;
                        
                     }
              
               }
      
         }
         
         
         if(password_counter==4)
         {
         
               if(strcmp(password, selcukID) == 0){
                  related_ID = '+'; 
                  putc(related_ID);
               }
               
               else if(strcmp(password, emreID) == 0){
                  related_ID = '*'; 
                  putc(related_ID);
               }
               
               else{
                  related_ID = '.'; 
                  putc(related_ID);
               }
               
               password_counter=0;
         }

      delay_ms(50); //50ms bekle.
   }
}
