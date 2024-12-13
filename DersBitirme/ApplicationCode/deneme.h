#include <16F887.h>
#device ADC=10

#FUSES WDT                   	//Watch Dog Timer

#use delay(crystal=4000000)
#use i2c(Master,Fast,sda=PIN_C0,scl=PIN_C1)

