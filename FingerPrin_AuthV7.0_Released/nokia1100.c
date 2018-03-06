/* nokia 1100 fbus 	protocol implementation
   coding by: Winston Rodrigues
   wintu.vogga@gmail.com */
#include "hw_memmap.h"
#include "hw_types.h"
#include "gpio.h"
#include "uart.h"
#include "sysctl.h"

// {0x08, 0x91, 0x69, 0x74, 0x07, 0x11, 0x44, 0x11, 0xF0, 0x00, 0x00, 0x00};
#define UP_KEY GPIO_PIN_4
#define DN_KEY GPIO_PIN_7
#define LF_KEY GPIO_PIN_0
#define RT_KEY GPIO_PIN_1
#define OK_KEY GPIO_PIN_0
	
unsigned char GSM_buffer[200];
unsigned char GSM_Buffer_COunt = 0;		
unsigned char Recivewd_Byte;
unsigned char count = 0;
unsigned char GSM_Buffer[20];
unsigned char command_AT[] = "AT\r";
unsigned char command_CMGF[] = "AT+CMGF=1\r";
unsigned char command_CMGS[] = "AT+CMGS=";
unsigned char ph_number[] = "+91";
unsigned char SMS_mesg[] = "Your One Time Password is ****";
unsigned char CTRLZ = 0x1A;
unsigned char ENTER_KEY = 0x0D;
unsigned char QUOTE = 0x22;
extern int OTP_GSM;
extern unsigned char temp_str[];
extern unsigned char Key;
extern unsigned char GSM_Modem_Talking = 0;
extern unsigned char password[];


void Nokia_UART_int()
{
while(UARTCharsAvail(UART0_BASE))
   {
     Recivewd_Byte = UARTCharGet(UART0_BASE);
	 GSM_Buffer[GSM_Buffer_COunt] = Recivewd_Byte;
	 GSM_Buffer_COunt++;
	 if(GSM_Buffer_COunt>19) GSM_Buffer_COunt =0;
   }
}

void SMS_Delay()
{
 unsigned long j;
   for(j=0;j<2000000;j++);
}

void Nokia_Port_init()
{
 SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
 GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
 UARTConfigSetExpClk(UART0_BASE, 8000000, 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
 //IntMasterEnable();
 //IntEnable(21);
 //IntRegister(21,Nokia_UART_int);
 //UARTIntRegister(UART0_BASE,Nokia_UART_int);
 //HWREG(UART0_BASE+0x2C) &= ~0x10;
 //UARTIntEnable(UART0_BASE,UART_INT_RX);

} 


void Nokia_1100_init()
{						 
 Nokia_Port_init();
}


unsigned char Nokia_1100_Send_SMS(unsigned char *number) 
{
 unsigned char i;
 for(i=0;i<10;i++)
 {
  UARTCharPut(UART0_BASE,command_CMGF[i]); 
 }
 SMS_Delay();
 for(i=0;i<8;i++)
 {
  UARTCharPut(UART0_BASE,command_CMGS[i]);
 }
 UARTCharPut(UART0_BASE,QUOTE);
 for(i=0;i<10;i++)
  {
   	ph_number[i+3] = number[i];
  }
 for(i=0;i<13;i++)
 {
  UARTCharPut(UART0_BASE,ph_number[i]);
 }
 UARTCharPut(UART0_BASE,QUOTE);
 UARTCharPut(UART0_BASE,ENTER_KEY);
 SMS_Delay();

 SMS_mesg[26] = password[0];
 SMS_mesg[27] = password[1];
 SMS_mesg[28] = password[2];
 SMS_mesg[29] = password[3];

 for(i=0;i<30;i++)
 {
  UARTCharPut(UART0_BASE,SMS_mesg[i]);
 }
 UARTCharPut(UART0_BASE,CTRLZ);
}


void Init_Keypad()
{
  GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, UP_KEY | DN_KEY );
  GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, LF_KEY );
  GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, RT_KEY | OK_KEY );
  GPIOPadConfigSet(GPIO_PORTC_BASE, UP_KEY | DN_KEY, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  GPIOPadConfigSet(GPIO_PORTD_BASE, LF_KEY , GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  GPIOPadConfigSet(GPIO_PORTE_BASE, RT_KEY | OK_KEY, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

//up = 1
//dn = 2
//lf = 3
//rt = 4
//ok = 5
void Read_Keypad()
{
   int count2;
   unsigned char but;
   if(GPIOPinRead(GPIO_PORTC_BASE,  UP_KEY ) == 0 )
	  {
	    for(count2 = 0 ;count2 < 120000;count2++);
		if(GPIOPinRead(GPIO_PORTC_BASE,  UP_KEY ) == 0 )
		{  but = 1;  }
	  }
   else if(GPIOPinRead(GPIO_PORTC_BASE,  DN_KEY ) == 0 )
	  {
	    for(count2 = 0 ;count2 < 120000;count2++);
		if(GPIOPinRead(GPIO_PORTC_BASE,  DN_KEY ) == 0 )
		{ but = 2; } 
	  }
   else if(GPIOPinRead(GPIO_PORTD_BASE,  LF_KEY ) == 0 )
	  {
	    for(count2 = 0 ;count2 < 120000;count2++);
		if(GPIOPinRead(GPIO_PORTD_BASE,  LF_KEY ) == 0 )
		{  but = 3;  }
	  }
   else if(GPIOPinRead(GPIO_PORTE_BASE,  RT_KEY ) == 0 )
	  {
	    for(count2 = 0 ;count2 < 120000;count2++);
		if(GPIOPinRead(GPIO_PORTE_BASE,  RT_KEY ) == 0 )
		{ but = 4; }
	  }
   else if(GPIOPinRead(GPIO_PORTE_BASE,  OK_KEY ) == 0 )
	  {
	    for(count2 = 0 ;count2 < 120000;count2++);
		if(GPIOPinRead(GPIO_PORTE_BASE,  OK_KEY ) == 0 )
		{ but = 5; } 
	  }
	else but = 0;
	Key = but;
}

void Process_GSM_Modem()
{
 
}
