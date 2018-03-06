/* 16 x 4 LCD interfacing      */
/* Coding by Winston           */
/* wintu.vogga@gmail.com       */

#include "hw_memmap.h"
#include "hw_types.h"
#include "gpio.h"
#include "uart.h"
#include "sysctl.h"


#define ENABLE GPIO_PIN_7
#define RS GPIO_PIN_6
#define LCD_PORT_BASE GPIO_PORTB_BASE
#define LCD_DATA0 GPIO_PIN_5
#define LCD_DATA1 GPIO_PIN_4
#define LCD_DATA2 GPIO_PIN_1
#define LCD_DATA3 GPIO_PIN_0

unsigned char LCD_Command[]={0x28,0x0C,0x06,0x01,0x80};

/*delay routine */
void LCD_Delay()
 {
  unsigned long i,j;
  for(i=0;i<10;i++)
  {
     for(j=0;j<2000;j++);
  }
 }
/* write data to LCD , make RS=0 for command make RS=1 for data*/
void LCD_Data_Write(unsigned char Data)
{
// unsigned char temp = 0;
 unsigned char i,j,shift;

//GPIOPinWrite(GPIO_PORTB_BASE, RS ,RS);	    //set RS for Data
 for (j=0;j<2;j++)
 {
    GPIOPinWrite(LCD_PORT_BASE, ENABLE ,ENABLE);	 //Set Enable
	for(i=0;i<4;i++)
	 {
		if(j) 
	  		shift = 0;
		else
	  		shift = 4;
			    
 		switch(i)
 		{
 	 	case 0:GPIOPinWrite(LCD_PORT_BASE, LCD_DATA0 , ((Data>>shift)&0x01==1)?LCD_DATA0:0);
        	    break;
   	 	case 1:GPIOPinWrite(LCD_PORT_BASE, LCD_DATA1 , ((Data>>shift+1)&0x01==1)?LCD_DATA1:0);
        	    break;
     	case 2:GPIOPinWrite(LCD_PORT_BASE, LCD_DATA2 , ((Data>>shift+2)&0x01==1)?LCD_DATA2:0);
        	    break;
     	case 3:GPIOPinWrite(LCD_PORT_BASE, LCD_DATA3 , ((Data>>shift+3)&0x01==1)?LCD_DATA3:0);
        	    break;
	 	}													 
 	 }
	 GPIOPinWrite(LCD_PORT_BASE, ENABLE ,0);   //clear Enable
  }
}

/*sets the position in row colomn*/
void LCD_Set_Cursor(unsigned char Row,unsigned char Col)
{
 GPIOPinWrite(LCD_PORT_BASE, RS ,0);		// claear RS for commmand 
 if(Row == 0) 
	 LCD_Data_Write(0x80 + Col);
 if(Row == 1)			 
     LCD_Data_Write(0xC0 + Col);
 LCD_Delay();
}

/*put a char on LCD*/
void LCD_PutChar(unsigned char Data )
{
  GPIOPinWrite(LCD_PORT_BASE, RS ,RS); 
  LCD_Data_Write(Data);
}

/* Clear the LCD */
void LCD_Clear()
{
 GPIOPinWrite(LCD_PORT_BASE, RS ,0); 
 LCD_Data_Write(0x0C);
 LCD_Data_Write(0x80);
}
/*Put a string on LCD*/
void LCD_Put_String(unsigned char *str)
{
 unsigned char i;
 GPIOPinWrite(LCD_PORT_BASE, RS ,RS);
 for(i=0;i<strlen(str);i++)
   {
    LCD_Data_Write(str[i]);
	LCD_Delay();
   }
}



/* initialize the LCD*/
void LCD_Init()
 {
  unsigned char i;
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  GPIOPinTypeGPIOOutput(LCD_PORT_BASE, LCD_DATA0 | LCD_DATA1 | LCD_DATA2 | LCD_DATA3 | ENABLE | RS  );
  GPIOPinWrite(LCD_PORT_BASE, RS ,0);
  for(i=0;i<5;i++)
   {
	LCD_Data_Write(LCD_Command[i]);
	LCD_Delay();
   } 
 }

