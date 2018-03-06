 /* API to access stepper motor
  by Winston Rodrigues
   wintu.vogga@gmail.com */

#include "hw_memmap.h"
#include "hw_types.h"
#include "hw_gpio.h"
#include "gpio.h"
#include "uart.h"
#include "sysctl.h"
#include "string.h"



void Stepper_delay()
{
  unsigned long i ;
  for(i=0;i<50000;i++);
}

void Stepper_Set_Pins(unsigned char Drive)
{
  unsigned long i;
  /*for(i=0;i<4;i++)
	 {
 		switch(i)
 		{
 	 	case 0:GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7 , ((Drive>>3)&0x01==1)?GPIO_PIN_7:0);
        	    break;
   	 	case 1:GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6 , ((Drive>>2)&0x01==1)?GPIO_PIN_6:0);
        	    break;
     	case 2:GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5 , ((Drive>>1)&0x01==1)?GPIO_PIN_5:0);
        	    break;
     	case 3:GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4 , ((Drive)&0x01==1)?GPIO_PIN_4:0);
        	    break;
	 	}													 
 	 }	*/
 // GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4 , 0)h;
  
}

void Door_Open()
{
  unsigned long i;
  GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);
  GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_5   , GPIO_PIN_6);
  for(i=0;i<6000000;i++);
  GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_5   , 0);
}

void Door_Close()
{  
   unsigned long i;
   GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);
  GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_5   , GPIO_PIN_5);
  for(i=0;i<6000000;i++);
  GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_5   , 0);

}




