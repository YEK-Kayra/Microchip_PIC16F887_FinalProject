#include <16F690.h>
 
#define GP0 PIN_C0
#define GP1 PIN_C1
#define GP2 PIN_C2
#define GP3 PIN_C3
#define GP4 PIN_C4
#define GP5 PIN_C5
#define GP6 PIN_C6
#define GP7 PIN_C7
 
#fuses INTRC_IO, NOWDT, NOPROTECT, BROWNOUT                             /* Ext. oscilator, no watchdog, no code protect, no brownout (until voltage 
                                                                            rises on power up the PIC is held in reset = inactive) reset */
 
#use delay(clock=4M)                                                        /* Specify clock frequency, to use "delay_()" function */
#use rs232 (baud=9600, xmit=PIN_B7, rcv=PIN_B5, parity=N, bits=8, errors)
#use i2c (master, sda=PIN_B4, scl=PIN_B6, FAST, FORCE_HW)                                       /* Master mode, SDA pin = RB4, SCL pin = RB6, force hardware i2c functions */
 
#define SLAVE1_WRT_CON_BYTE 0b10100000                                      /* This byte should contain the control byte from the
                                                                            datasheet with control code, AN0, AN1, AN2 pins, READ or WRITE bit */
                                                                            /* bit<0> = 0 for write and 1 for read
                                                                            bits<1:3> = the values of the AN0:2 pins
                                                                            bits<4:7> = control code of "1010" for EEPROM24AA128 */
                                                                            /* !!! Dont forget that these must be put as the most significant bits first,
                                                                            A0, A1, A2 become A2, A1, A0, making:
                                                                            10100000 = control 4 bits/A2, A1, A0,/Read or write */
                                                                            /* HEX = 0xA0 */
                                                                            
#define SLAVE1_READ_CON_BYTE 0b10100001                                     /* This byte should contain the control byte from the
                                                                            datasheet with control code, AN0, AN1, AN2 pins, READ or WRITE bit */
                                                                            /* bit<0> = 0 for write and 1 for read
                                                                            bits<1:3> = the values of the AN0:2 pins
                                                                            bits<4:7> = control code of "1010" for EEPROM24AA128 */
                                                                            /* !!! Dont forget that these must be put as the most significant bits first,
                                                                            A0, A1, A2 become A2, A1, A0, making:
                                                                            10100001 = control 4 bits/A2, A1, A0,/Read or write */
                                                                            /* HEX = A1 */
                                                                            
#define SLAVE1_WRITE_ADDRESS_2_BYTES 0x00A2
#define SLAVE1_READ_ADDRESS_2_BYTES 0x00A3
//====================================
 
void main()
{
   unsigned int8 i;
   unsigned int8 data;
   
   while(1)
   {
      printf ("Starting i2c module....\r\n");
                            /* to write data */
      i2c_start();
      i2c_write(SLAVE1_WRT_CON_BYTE);           /* 1. Following the start condition, the next byte is control code (1010)\chip select (A2, A1, A0), R/W bit (logic low). 
                                                   This indicates to the addressed slave that the address high byte will follow after it has generated an Acknowledge bit */
      delay_ms (10);                            /* delay 10ms for the acknowledge bit to be sent, because we dont record it with the microcontroller */
      
      i2c_write (0x80);                     /* 2. The next byte is the high order byte of the word address and will be written into the Address Pointer of 24AA128 */
 
      i2c_write (0x00);                     /* 3. The next byte is the least significant address byte */     
      
      i2c_write (0x01);                     /* 4. After receiving another acknowledge signal from the 24AA128, the master will transmit tha data word to be written into 
                                                   the  addressed memory location */
                            /* After a byte write command, the internal address counter will point
                            /* to the address location following the one that was just written, because its automatically EEPROM hardware incremented */
      i2c_write (0x02);
      i2c_stop ();                  /* 5. After another acknowledge bit, the master sends the stop condition */
      delay_ms (10);                        /* The information is kept in a buffer, until the "stop condition", after which
                            /* it is recorded in the EEPROM, so we need a delay, after the "stop condition" */
 
      /* to write data */
      i2c_start();
      i2c_write(SLAVE1_WRT_CON_BYTE);           /* 1. Following the start condition, the next byte is control code (1010)\chip select (A2, A1, A0), R/W bit (logic low). This indicates to the addressed slave that the address high byte will follow after it has generated an Acknowledge bit */
      delay_ms (10);                    /* delay 10ms for the acknowledge bit to be sent, because we dont record it with the microcontroller */
      
      i2c_write (0x80);                 /* 2. The next byte is the high order byte of the word address and will be written into the Address Pointer of 24AA128 */
 
      i2c_write (0x00);                 /* 3. The next byte is the least significant address byte */     
      
      i2c_write (0x02);                 /* 4. After receiving another acknowledge signal from the 24AA128, the master will transmit tha data word to be written into the addressed memory location */
                            /* After a byte write command, the internal address counter will point
                            /* to the address location following the one that was just written, because its automatically EEPROM hardware incremented */
      i2c_write (0x02);
      i2c_stop ();                  /* 5. After another acknowledge bit, the master sends the stop condition */
      delay_ms (10);                        /* The information is kept in a buffer, until the "stop condition", after which
         
                            /* To read data from a random address */
      i2c_start();
      i2c_write(SLAVE1_WRT_CON_BYTE);           /* 1. Following the start condition, the next byte is control code (1010)\chip select (A2, A1, A0), R/W bit (logic low). 
                                                   This indicates to the addressed slave that the address high byte will follow after it has generated an "acknowledge bit".
                                                   We set "the internal address pointer" as a part of a "write" operation, in
                                                   order to be able to use "sequential read" */
     
      i2c_write (0x80);                 /* 2. The next byte is the high order byte of the word address and will be written into the Address Pointer of 24AA128 */
 
      i2c_write (0x00);                 /* 3. The next byte is the least significant address byte */
      
      i2c_start ();                     /* 4. We send a start condition, instead of a "stop condition", after the "internal address pointer" has been set in
                                  a "write" operation */
      
      i2c_write (SLAVE1_READ_CON_BYTE);         /* 5. For sequentual read, we send a "read" control byte, after the "internal address pointer" has
                                                   been set in a "write" operation and after that a "start condition" has been generated */
                               
      for (i=0; i<2 ;i++)
      {  
     data = i2c_read(0);
      
         output_c (data);
         delay_ms(1000);
      }
     i2c_stop ();                   /* 6. The data for the EEPROM is kept in a buffer, until the stop condition is issues,
                               and written after that, so we need a delay after the stop condition */
                            /* We make the delay 1 sec, in order to test the result of the program */
delay_ms (1000);
  }
}
