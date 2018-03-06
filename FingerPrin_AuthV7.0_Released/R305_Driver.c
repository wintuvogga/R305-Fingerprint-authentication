/* fingerprint module R305 integration API 
 by Winston Rodrigues 
  wintu.vogga@gmail.com */

#include "hw_memmap.h"
#include "hw_types.h"
#include "gpio.h"
#include "uart.h"
#include "sysctl.h"
#include "string.h"

/*Frames to be sent to R305 in order to do fingerprint function */
/*R305 replies with the answers */
/* So be ready to accept it from UART */
unsigned char Get_Image_R305[] = {0xEF,0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x01, 0x00, 0x05};
unsigned char Get_Template_Number[] = {0xEF, 1, 0xFF, 0xFF, 0xFF, 0xFF, 1, 0, 0x03, 0x1D, 0, 0x21};
unsigned char ConvertImageToChar[] = {0xEF, 1, 0xFF, 0xFF, 0xFF, 0xFF, 1, 0, 4, 2, 1, 0, 0x08};
unsigned char ConvertImageToChar2[] = {0xEF, 1, 0xFF, 0xFF, 0xFF, 0xFF, 1, 0, 4, 2, 2, 0, 0x09};
unsigned char Gen_Template[] = {0xEF, 1, 0xFF, 0xFF, 0xFF, 0xFF, 1, 0, 3, 5, 0, 9};
unsigned char Store_template[] = {0xEF, 1, 0xFF, 0xFF, 0xFF, 0xFF, 1, 0, 6, 6, 1, 0, 0, 0, 0x0E};
unsigned char Delete_Template[] = {0xEF, 1, 0xFF, 0xFF, 0xFF, 0xFF, 1, 0, 7, 0x0C, 0, 0, 0, 1, 0, 0x15};
unsigned char Delete_all_Templates[] = {0xEF, 1, 0xFF, 0xFF, 0xFF, 0xFF, 1, 0, 3, 0x0D, 0, 0x11};
unsigned char Read_template_from_Lib[] = {0xEF, 1, 0xFF, 0xFF, 0xFF, 0xFF, 1, 0, 6, 7, 2, 0, 0, 0, 0x10};
unsigned char Verify_Template[] = {0xEF, 1, 0xFF, 0xFF, 0xFF, 0xFF, 1, 0, 3, 3, 0, 0x07};
unsigned char Gen_Rand_Numb[] = {0xEF,1,0xFF,0xFF,0xFF,0xFF,1,0,3,0x14,0,0x18};
unsigned char Search_library[] = {0xEF,1,0xFF,0xFF,0xFF,0xFF,1,0,8,4,1,0,0,0,50,0,64};


unsigned char i;
unsigned char Recived_Byte;
extern unsigned char R305_buffer[30];
unsigned char R305_buffer_Count= 0 ;
extern unsigned char R305_Talking;
unsigned char Length;
unsigned char header_recieved = 0;	  
extern int OTP_GSM;
extern unsigned char temp_str[];
extern unsigned char password[];
unsigned int random_seed = 6;

void Recieve_ISR_R305()
 {
  if(UARTCharsAvail(UART1_BASE))
   {
     Recived_Byte = UARTCharGet(UART1_BASE);
	 //UARTCharPut(UART0_BASE,Recived_Byte);
	 if(Recived_Byte==0xEF)
	  {
       R305_buffer_Count = 0;
	   header_recieved = 1;
	  }
	 if(header_recieved=1)
	 {
	   R305_buffer[R305_buffer_Count]=Recived_Byte;
	   if(R305_buffer_Count == 8) 
	    {
	      Length = R305_buffer[8] ; 
	    }
	   if(R305_buffer_Count == (Length + 8))
	   {
	     R305_Talking = 1;
	   }
	   else
	   {
	     R305_Talking = 0;
	   }	   
	   R305_buffer_Count++;
	}
   }
 }

/*
void Init_R305()
{
 SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
 GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
 UARTConfigSetExpClk(UART0_BASE, 8000000, 57600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
 IntMasterEnable();
 IntEnable(21);
 IntRegister(21,Recieve_ISR_R305);
 UARTIntRegister(UART0_BASE,Recieve_ISR_R305);
 HWREG(UART0_BASE+0x2C) &= ~0x10;
 UARTIntEnable(UART0_BASE,UART_INT_RX);
} */


void Init_R305()
{
 SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
 SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
 GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);
 UARTConfigSetExpClk(UART1_BASE, 8000000, 57600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
 IntMasterEnable();
 IntEnable(22);
 HWREG(UART1_BASE+0x2C) &= ~0x10;
 IntRegister(22,Recieve_ISR_R305);
 UARTIntRegister(UART1_BASE,Recieve_ISR_R305);
 UARTIntEnable(UART1_BASE,UART_INT_RX);
} 
  

void Get_Image_From_R305(unsigned char Buffer)
{			 
  for(i=0;i<12;i++)
  {
    UARTCharPut(UART1_BASE,Get_Image_R305[i]);
  }
}

void Convert_To_Char(unsigned char buffer)
{
  for(i=0;i<13;i++)
  {
//   ConvertImageToChar[10]    
   UARTCharPut(UART1_BASE,ConvertImageToChar[i]);
  }
}

void Convert_To_Char2(unsigned char buffer)
{
  for(i=0;i<13;i++)
  {
//   ConvertImageToChar[10]    
   UARTCharPut(UART1_BASE,ConvertImageToChar2[i]);
  }
}

void Get_Template_From_R305(unsigned char buf)
{
 for(i=0;i<15;i++)
  {
   UARTCharPut(UART1_BASE,Read_template_from_Lib[i]);
  }
}

void Match_Buffers()
{
  for(i=0;i<12;i++)
   {
    UARTCharPut(UART1_BASE,Verify_Template[i]);
   } 
}

void R305_Get_Random_Number()
{
  for(i=0;i<12;i++)
   {
    UARTCharPut(UART1_BASE,Gen_Rand_Numb[i]);
   }
}

void convert_to_asci()
{
 OTP_GSM = OTP_GSM % 10000;
 password[0]= (OTP_GSM / 1000)+0x30;
 password[1]= ((OTP_GSM%1000)/100)+0x30;
 password[2]= (((OTP_GSM%1000)%100)/10)+0x30;
 password[3]= (OTP_GSM % 10)+0x30;

}

void Decode_Random_number()
{

 OTP_GSM = R305_buffer[12];
 OTP_GSM = (OTP_GSM << 8) | R305_buffer[13]; 
// OTP_GSM = rand();
 convert_to_asci();
}

int rand() 
 {  
     
    random_seed = random_seed * 1103515245 +12345;   
	//FlashProgram(random_seed & 0xFFFFFF, 0x28800, 4);
    return (unsigned int)(random_seed / 65536) % 32768; 
 }

void Search_Finger()
{
 for(i=0;i<17;i++)
 {
   UARTCharPut(UART1_BASE,Search_library[i]);
 }
}  

void Generate_Template()
{
   for(i=0;i<12;i++)
 {
   UARTCharPut(UART1_BASE,Gen_Template[i]);
 }
}


void Store_Finger()
{  
 Store_template[14] = 0x0E;
 Store_template[12] = 0;
   for(i=0;i<15;i++)
 {
   UARTCharPut(UART1_BASE,Store_template[i]);
 }
}

void Store_Finger_wID(unsigned char ID)
{
   Store_template[14] = 0x0E;
   Store_template[12] = ID;
   Store_template[14] = 0x0E + ID; 
   for(i=0;i<15;i++)
 {
   UARTCharPut(UART1_BASE,Store_template[i]);
 }
}

void Get_Current_Template_Number()
{
 for(i=0;i<15;i++)
 {
   UARTCharPut(UART1_BASE,Get_Template_Number[i]);
 } 
}

void Delete_All_Fingerprints()
 {
  for(i=0;i<12;i++)
 {
   UARTCharPut(UART1_BASE,Delete_all_Templates[i]);
 }
 }

void Delete_Template_Func(unsigned char ID)
{
 Delete_Template[11] = ID;
 Delete_Template[15] = 0x15+ID;
 for(i=0;i<16;i++)
 {
   UARTCharPut(UART1_BASE,Delete_Template[i]);
 }

}