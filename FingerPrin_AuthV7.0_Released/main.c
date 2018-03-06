 /* Advanced Bio Metric Authentication system
 /* Advanced Bio Metric Authentication system
 * 	platorm: LM3S608 ARM-Cortex-M3 micro controller
 * 	coding by:
 *  Winston Rodrigues
 *  wintu.vogga@gmail.com
 */

#include "hw_memmap.h"
#include "hw_types.h"
#include "gpio.h"
#include "uart.h"
#include "sysctl.h"
#include "ff.h"

#define UP 1
#define DN 2
#define LF 3
#define RT 4
#define OK 5



unsigned char R305_Talking;
unsigned char R305_buffer[30];
unsigned char Inst_Code = 1;
unsigned char R305_Working 	= 0;
unsigned char Access_Granted = 0;
int OTP_GSM;
unsigned char temp_str[] = {0,0,0,0}; 
unsigned char Pass_code_entered= 0;
unsigned char Key = 0;	 
signed char current_digit;
signed char current_cursor;
unsigned char password[];
unsigned char Admin_logged_in = 0;
unsigned char looped =0;
unsigned char Enroll_Loop = 0; 
unsigned char Phone_Number_Temp[] = "__________"; 
unsigned char Phone_Number_To_Send[] ="0000000000";
unsigned char Phone_Numb_Entered = 0;
unsigned char Current_Finger_Print_Location = 0;
unsigned char ID_for_delete[] = "___";
unsigned char ID_Del = 0;
//unsigned char Pointer;
unsigned char Finger_location[];
FATFS FatFs;
FIL Fil;	
UINT Pointer;


void Timer_Init()
{
//  TimerConfigure(TIMER0_BASE, (TIMER_CFG_16_BIT_PAIR | TIMER_CFG_A_ONE_SHOT | TIMER_CFG_B_CAP_COUNT));
}

int main(void)
{
    unsigned long count;
	
	/* Clock setting */
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

	/*Periferals Enabling */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    
	
	/* Init LCD*/
	LCD_Init();
	LCD_Set_Cursor(0,0);
	LCD_Put_String("   *WELCOME*    ");
	LCD_Set_Cursor(1,0);
	LCD_Put_String("Plz Wait...     ");
	for(count=0;count<1000000;count++);
	

	/* Nokia 1100 GSM modem init*/
	Nokia_1100_init();

	/* R305 Fingerprint Module Init */
	Init_R305();
	//Nokia_1100_Send_SMS("Hello");
    
	Init_Keypad();
	f_mount(&FatFs,"",0);   //sd card init 
	// create a new file if it exists dont create
	if(f_open(&Fil, "CONTACTS.TXT", FA_WRITE | FA_OPEN_ALWAYS)==0)
	 {
	  LCD_Set_Cursor(0,0);
	  LCD_Put_String("Scan Your Finger");
	  //f_write(&Fil,
	  f_close(&Fil);
	 }
	 else
	  {
	   LCD_Set_Cursor(0,0);
	   LCD_Put_String("SD Card Missing!");
	   LCD_Set_Cursor(1,0);
	   LCD_Put_String("Re-Insert SDcard");
       for(count=0;count<1000000;count++);
	   LCD_Set_Cursor(0,0);
	   LCD_Put_String("Scan Your Finger");
	   LCD_Set_Cursor(1,0);
	   LCD_Put_String("                ");
	   f_close(&Fil);
	  }

	Timer_Init();
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_0 );
	//GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1 );

	//GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	Get_Current_Template_Number();
	Inst_Code = 123;
	//GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	//Door_Close();
	//Init_Keypad();
	while(1)
    {
		if(R305_Talking)
		{
		 switch (R305_buffer[9])
		  {
		    case 0: switch(Inst_Code)
			        {
					  case 1:Convert_To_Char();
					         Inst_Code = 2;
							 R305_Working = 1;
							 break;
					  case 2:Search_Finger();
					         Inst_Code = 4;
							 R305_Working =1;
					 		 break;
					  case 4:if(R305_buffer[11] == 0)
							  {
							   Admin_logged_in = 1;
							  }	
							  else
							   {
							   /* see who logged in
							    send sms to them .. and remainig OTP shit. Do it.
								 read phone number from SD card... */
							     LCD_Set_Cursor(0,0);
						         LCD_Put_String("Match Found!!   ");
								 LCD_Set_Cursor(1,0);
								 LCD_Put_String("                ");
								 LCD_Set_Cursor(1,0);
						         LCD_Put_String("ID=");
								 LCD_PutChar((R305_buffer[11]/100)+48);
							     for(count=0;count<6000;count++);
							     LCD_PutChar(((R305_buffer[11]/10)%10)+48);
							     for(count=0;count<6000;count++);
							     LCD_PutChar((R305_buffer[11]%10)+48);
								 for(count=0;count<2000000;count++);
								 Decode_Random_number();
								 // read the phone number from SD card corresponding to the Finger ID
								 if(f_open(&Fil, "CONTACTS.TXT", FA_WRITE |FA_READ | FA_OPEN_ALWAYS) == 0)

								  {
								  f_lseek(&Fil,(R305_buffer[11]-1)*10);
							      f_read(&Fil,Phone_Number_To_Send,10,&Pointer);
                                  f_close(&Fil);
								  LCD_Set_Cursor(0,0);
							      //LCD_PutChar(R305_buffer[11]+48);
								  LCD_Put_String("Sending SMS.....");
                                  LCD_Set_Cursor(1,0);
								  LCD_Put_String("To:");
								  LCD_Put_String(Phone_Number_To_Send);
								  //LCD_PutChar(Pointer+48);
							      Nokia_1100_Send_SMS(Phone_Number_To_Send);
								  }
								 else
								  {
								  LCD_Set_Cursor(0,0);
							      //LCD_PutChar(R305_buffer[11]+48);
								  LCD_Put_String("SD Card Error !!");
                                  LCD_Set_Cursor(1,0);
								  LCD_Put_String("Use secret Pass ");
								  for(count=0;count<1000000;count++);								   
								  }
								 LCD_Set_Cursor(0,0);
							     LCD_Put_String("Plz Enter OTP   ");
					             //wait for user to enter the cod  
							     LCD_Set_Cursor(1,0);
								 LCD_Put_String("                ");
							     GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6 ,0);
							     LCD_Data_Write(0x0F);
							     temp_str[0] = 0x30;
							     temp_str[1] = 0x30;
							     temp_str[2] = 0x30;
							     temp_str[3] = 0x30;
								 current_digit = current_cursor =0;
								 while(Pass_code_entered==0)
							     {
								  Read_Keypad();
								  if(Key!=0)
								   {
								    switch(Key)
								    {
								      case 1:current_digit++;
									        break;
								      case 2:current_digit--;
									        break;
								      case 3:current_cursor--;
									        break;
									  case 5:current_cursor++;
									        break;
								      case 4:if(temp_str[0] == password[0] && temp_str[1] == password[1] && temp_str[2] == password[2] && temp_str[3] == password[3] )
									         {
											  Pass_code_entered =1;
											  Access_Granted = 1;
											  break;
									  		 }
											 //secret password ... if otp doesnt this will work
											 else if(temp_str[0] == 0x37 && temp_str[1] == 0x37 && temp_str[2] == 0x33 && temp_str[3] == 0x32)
											  {
											  Pass_code_entered =1;
											  Access_Granted = 1;
											  break;
											  }
											 else
											  {
											   Pass_code_entered =1;
											  Access_Granted = 0;
											  LCD_Set_Cursor(0,0);
		                                      LCD_Put_String("Access Deined!!   ");
											  LCD_Set_Cursor(1,0);
											  LCD_Put_String("                  ");
		                                      for(count=0;count<2000000;count++);
				                              LCD_Set_Cursor(0,0);
	                                          LCD_Put_String("Scan Your Finger");
				                              R305_Working = 0;
				                              Inst_Code = 1;
											  break;
											  }
											 
										/*	else
											 {
											  LCD_Set_Cursor(0,0);
		                                      LCD_Put_String("Access Deined!!   ");
		                                      for(count=0;count<2000000;count++);
				                              LCD_Set_Cursor(0,0);
	                                          LCD_Put_String("Scan Your Finger");
											  break;
											 } */
								 	default:break;
								  }
								  
								  if(current_digit>9  ) current_digit  =0;
								  if(current_cursor>3 ) current_cursor = 0;
								  if(current_digit<0  ) current_digit  =9;
								  if(current_cursor<0 ) current_cursor = 3;
								  LCD_Set_Cursor(1,0);
								  //LCD_PutChar(current_digit+48);
								  if(Pass_code_entered !=1)
								  {
								   temp_str[current_cursor] = current_digit+48;
								   LCD_Put_String(temp_str);
								   //Key = 0;
								  }
								}
							    //Pass_code_entered = 0;
								Key = 0;
							   }
							   Pass_code_entered = 0;                
                              GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6 ,0);
							  LCD_Data_Write(0x0C);
							  LCD_Delay();                                          
					         if(Access_Granted == 1)
							 {
							  LCD_Set_Cursor(0,0);
		           		      LCD_Put_String("Access Granted!!   ");
						      LCD_Set_Cursor(1,0);
						      LCD_Put_String("Plz Close door! ");
		          		      //or(count=0;count<2000000;count++);
						      //LCD_Clear();
							  GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
						      Door_Open();
						      //LCD_Set_Cursor(0,0);
	            		      //LCD_Put_String("Scan Your Finger");
						      R305_Working =1;	
						      //Inst_Code = 1;
						      //Access_Granted = 1;
						      break;
							 }
								 Admin_logged_in = 0;
							    }
							   Inst_Code = 1;
							  R305_Working = 0 ;	 
							 break;
					  case 3:Match_Buffers();
					         Inst_Code = 56;
							 R305_Working =1;
					         break;
					  case 56:
					         R305_Get_Random_Number();
						     Inst_Code = 100;
							  R305_Working =1;
					  case 123:Current_Finger_Print_Location = 	R305_buffer[11];
					           Inst_Code = 1;
							   if(Current_Finger_Print_Location == 0)
							    {
								  R305_Talking = 0;
								  Enroll_Loop = 0;
								  LCD_Set_Cursor(0,0);
						          LCD_Put_String("Scan Your Finger");
								  LCD_Set_Cursor(1,0);
						          LCD_Put_String("For Admin Enroll");
								  while(Enroll_Loop == 0)
				                   {
						             if(R305_Talking)
							           {
							            //UARTCharPut(UART0_BASE,R305_buffer[9]+48);
							              switch (R305_buffer[9])
		                                   {
		                                     case 0: switch(Inst_Code)
			                                  {
					                           case 1:Convert_To_Char();
					                                  Inst_Code = 2;
							                          //LCD_Put_String("Char");
							                          R305_Working = 0;
													  LCD_Set_Cursor(0,0);
						                              LCD_Put_String("Scan Fingr Again");
													  LCD_Set_Cursor(1,0);
						                              LCD_Put_String("                ");
													  for(count=0;count<2000000;count++);
							                          break;
											   case 2:Convert_To_Char2();
											          Inst_Code = 3; 
													  R305_Working = 1;
											          break;
											   case 3:Generate_Template();
											          Inst_Code = 4;
													  R305_Working = 1;
													  break;
											   case 4:Store_Finger();
											          Inst_Code = 5;
													  R305_Working = 0; 
											   case 5:LCD_Set_Cursor(0,0);
											          LCD_Put_String("Admin Enrolled!!");
													  LCD_Set_Cursor(1,0);
											          LCD_Put_String("Plz Reset.      ");
													  R305_Working = 1;
											   }
											  break;
									        }
								            R305_Talking = 0;
							               }
							              if(R305_Working == 0)
		                                  {
		                                   Get_Image_From_R305();
		                                  }
							            for(count=0;count<50000;count++);
						                }
							        	}
							   LCD_Set_Cursor(1,0);
							   LCD_Put_String("Totl Users:");
							  LCD_PutChar((Current_Finger_Print_Location/100)+48);
							   for(count=0;count<1000;count++);
							   LCD_PutChar(((Current_Finger_Print_Location/10)%10)+48);
							   for(count=0;count<1000;count++);
							   LCD_PutChar((Current_Finger_Print_Location%10)+48);
							   break;
					  case 111:LCD_Set_Cursor(0,0);
					           LCD_Put_String("All Deleted !!!!");
							   LCD_Set_Cursor(1,0);
					           LCD_Put_String("Plz Reset.      ");
							   for(count=0;count<2000000;count++);

							   break;
					} 
					break;
		   case 8: //LCD_Clear();
		           LCD_Set_Cursor(0,0);
		           LCD_Put_String("Access Deined!!   ");
		           for(count=0;count<2000000;count++);
				   LCD_Set_Cursor(0,0);
	               LCD_Put_String("Scan Your Finger");
				   R305_Working = 0;
				   Inst_Code = 1;
				   break;

		  case 9:LCD_Set_Cursor(0,0);
		           LCD_Put_String("Match Not Found!");
				   LCD_Set_Cursor(1,0);
	               LCD_Put_String("Plz Try Again...");
				   for(count=0;count<2000000;count++);
				   LCD_Set_Cursor(0,0);
	               LCD_Put_String("Scan Your Finger");
				   LCD_Set_Cursor(1,0);
	               LCD_Put_String("                ");
				   R305_Working = 0;
				   Inst_Code = 1;
				   break; 
		  case 2: R305_Working = 0;
		          break;


		  }
		//  for(count=0;count<2000000;count++);
		  //LCD_Set_Cursor(0,0);
		  //LCD_PutChar(0x30+R305_buffer[9]);
		  //UARTCharPut(UART0_BASE,0x30+R305_buffer[9]);
		  R305_Talking = 0;

		}
		/* Runs if admin loggs in. admins finger is always stored at address location 1 */
		if(Admin_logged_in == 1)
		 {
          LCD_Set_Cursor(0,0);
	      LCD_Put_String("Welcome Admin ! ");
		  LCD_Set_Cursor(1,0);
		  LCD_Put_String("Enroll    Delete");
		  GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
          Door_Open();
	      Init_Keypad();
		  while(looped == 0)
		   {
		     Read_Keypad();
			 if(Key!=0)
			 {
			   switch(Key)
				{
                   case 3:LCD_Set_Cursor(0,0);
						  LCD_Put_String("Scan Finger Plz ");
						  LCD_Set_Cursor(1,0);
						  LCD_Put_String("                ");
						  R305_Working = 0;
				          while(Enroll_Loop == 0)
				           {
						     if(R305_Talking)
							  {
							     UARTCharPut(UART0_BASE,R305_buffer[9]+48);
							     switch (R305_buffer[9])
		                           {
		                             case 0: switch(Inst_Code)
			                                  {
					                           case 1:Convert_To_Char();
					                                  Inst_Code = 2;
							                          //LCD_Put_String("Char");
							                          R305_Working = 0;
													  LCD_Set_Cursor(0,0);
						                              LCD_Put_String("Scan Fingr Again");
													  for(count=0;count<2000000;count++);
							                          break;
											   case 2:Convert_To_Char2();
											          Inst_Code = 3; 
													  R305_Working = 1;
											          break;
											   case 3:Generate_Template();
											          Inst_Code = 4;
													  R305_Working = 1;
													  break;
											   case 4:Store_Finger_wID(Current_Finger_Print_Location);
											          Current_Finger_Print_Location++;
											          Inst_Code = 5;
													  R305_Working = 0; 
											   case 5:LCD_Set_Cursor(0,0);
											          LCD_Put_String("Enter Phone No. ");
													  R305_Working = 1;
													  current_digit = current_cursor = 0;
													  while(Phone_Numb_Entered==0)
							                           {
                         							     Read_Keypad();
														 if(Key!=0)
								                         {
								                         switch(Key)
								                           {
								                            case 1:current_digit++;
									                               break;
								                            case 2:current_digit--;
									                               break;
								                            case 3:current_cursor--;
									                               break;
									                        case 5:current_cursor++;
									                               break;
								                            case 4:Phone_Numb_Entered = 1;
															       looped = 1;
																   Enroll_Loop =1;
																    break;
											                default:break;
								                          }
								                       if(current_digit>9  ) current_digit  =0;
								                       if(current_cursor>9 ) current_cursor = 0;
								                       if(current_digit<0  ) current_digit  =9;
								                       if(current_cursor<0 ) current_cursor = 9;
								                       LCD_Set_Cursor(1,0);
								                       if(Phone_Numb_Entered !=1)
								                         {
								                          Phone_Number_Temp[current_cursor] = current_digit+48;
								                          LCD_Put_String(Phone_Number_Temp);
								                         }												    
								                        }
								                      Key = 0;
							                        }
												  Finger_location[0]=(Current_Finger_Print_Location/100)+48;
												  Finger_location[1]=((Current_Finger_Print_Location/10)%10)+48;
												  Finger_location[2]=(Current_Finger_Print_Location%10)+48;
												  //Write the phone number from sd card corresponding to the Finger ID
												  f_open(&Fil, "CONTACTS.TXT", FA_WRITE | FA_OPEN_ALWAYS);
												  f_lseek(&Fil ,f_size(&Fil));
												  f_write(&Fil,Phone_Number_Temp,10,&Pointer);
												  f_close(&Fil);
							  					  Inst_Code = 5;			  
												  LCD_Set_Cursor(0,0);
						                          LCD_Put_String("Store Success!!  ");
												  LCD_Set_Cursor(1,0);
						                          LCD_Put_String("                 ");
												  for(count=0;count<200000;count++);
												  LCD_Set_Cursor(0,0);
			                                      LCD_Put_String("Scan Your Finger");
			                                      LCD_Set_Cursor(1,0);
			                                      LCD_Put_String("                "); 
												  break;
											   }
									}
								 R305_Talking = 0;
							  }
							 if(R305_Working == 0)
		                      {
		                       Get_Image_From_R305();
		                      }
							 for(count=0;count<50000;count++);
						   }   
						  Enroll_Loop =1;
						  break;
				   case 5:LCD_Set_Cursor(0,0);
			              LCD_Put_String("Enter ID..      ");
			              LCD_Set_Cursor(1,0);
			              LCD_Put_String("                ");
						  while(Phone_Numb_Entered==0)
							{
                             Read_Keypad();
						     if(Key!=0)
								{
								 switch(Key)
								  {
								    case 1:current_digit++;
									       break;
								    case 2:current_digit--;
									       break;
								    case 3:current_cursor--;
									       break;
									case 5:current_cursor++;
									       break;
								    case 4:Phone_Numb_Entered = 1;
							               //looped = 1;
					                       //Enroll_Loop =1;
										   ID_Del = ((ID_for_delete[0]&0x0F)*100)+((ID_for_delete[1]&0x0F)*10)+(ID_for_delete[2]&0x0F);
										   Delete_Template_Func(ID_Del);
										   //Inst_Code = 198;
									       break;
									default:break;
								   }
								  if(current_digit>9  ) current_digit  =0;
								  if(current_cursor>2 ) current_cursor = 0;
								  if(current_digit<0  ) current_digit  =9;
								  if(current_cursor<0 ) current_cursor = 2;
								  LCD_Set_Cursor(1,0);
								  if(Phone_Numb_Entered !=1)
								  {
								    ID_for_delete[current_cursor] = current_digit+48;
								    LCD_Put_String(ID_for_delete);
							      }												    
					          }
						 }
				          looped = 1;
						  Phone_Numb_Entered = 0;
				          break;
				default: break;
			   }
			 }
		   }
		  Enroll_Loop = 0;
		  looped = 0;
		  Phone_Numb_Entered = 0;
		  Admin_logged_in = 0;
		  Inst_Code = 1; 
		  R305_Working = 0;		   
		 }
		if(R305_Working == 0)
		  {
		   Get_Image_From_R305();
		  }
		/* to delete all fingerprints secret way 
		  hold left n right button at the same time */

	    if(GPIOPinRead(GPIO_PORTE_BASE,  GPIO_PIN_1 ) == 0)
	     {
		  LCD_Set_Cursor(0,0);
	      //  LCD_Put_String("Delete");
	      for(count = 0 ;count < 1000000;count++);
		  if(GPIOPinRead(GPIO_PORTE_BASE,  GPIO_PIN_1 ) == 0)
		   {
		    Delete_All_Fingerprints();
			LCD_Set_Cursor(0,0);
	        //LCD_Put_String("Delete"); 
		    Inst_Code = 111;  
			f_unlink("CONTACTS.TXT");
		   }
		 }
        /* Delay for a while */
        if(Access_Granted == 1)
		 {
		  if(GPIOPinRead(GPIO_PORTE_BASE,  GPIO_PIN_0 ) == 0 )
		   {
		    for(count = 0 ;count < 10000;count++);
		    if(GPIOPinRead(GPIO_PORTE_BASE,  GPIO_PIN_0 ) == 0 )
			Door_Close();
			GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
			Init_Keypad();
			Access_Granted = 0; 
			R305_Working = 0;
			Inst_Code =1 ;
			LCD_Set_Cursor(0,0);
	        LCD_Put_String("Scan Your Finger");
			LCD_Set_Cursor(1,0);
			LCD_Put_String("                ");
			Pass_code_entered=0;
		   }

		  }
		
		/* Make Pin High */
		//GPIOPinWrite(GPIO_PORTC_BASE,  GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 ); 
		//Get_Image_From_R305(1);
		/* Delay for a while */
		//GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
		for(count=0;count<50000;count++) //delay required  for r305 busy processing
		{
		}
	//	if(UARTCharsAvail(UART0_BASE))
    //	{
    	/* Unsigned Char */
    //		UARTCharPut(UART0_BASE,(unsigned char)(UARTCharGet(UART0_BASE)+1));
   	//	 }
	//Stepper_Set_Pins();	
	}
		
}
