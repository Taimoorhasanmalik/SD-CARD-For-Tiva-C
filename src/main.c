/****************************************************************** 
* Filename :    main.c                                    
* Date     :    28-12-2021                                
* Author   :    Muhammad Tahir                            
*                                                         
* Description:  Flash memory driver for writing and 
*               reading its contents.                                 
******************************************************************/ 

#include <inttypes.h>
#include <stdio.h>
#include "../include/spi.h"
#include "../include/gpio.h"
#include "../include/uart.h"

#define MAX_APP_SIZE               0x200

void SystemInit(void){};

int main (void){
  char ch;
  char string[20];
  uint32_t n; 
  uint64_t response;
  int i =0;
	GPIO_Init();
  UART_Init();
  OutCRLF();
  UART_OutString("Uart Initialized");
  OutCRLF();
 // Initialize the SPI module
  SPI_Init(); 

  SPI_DIS(); //CS is set High

  for (i = 0; i < 10; i++) {
    SPI_Transfer(0xFF);
	  while(SPI_STATUS_R & SPI_BUSY_FLAG);	
  }

  SPI_EN(); //CS is set low
	SPI_SEND_CMD(0,0x00000000);
  while(SPI_STATUS_R & SPI_BUSY_FLAG);
  response = SPI_Receive_Data_Compare(0x01);
	OutCRLF();
  sprintf(string,"%lld",response);
	UART_OutString(string);
	OutCRLF();
  response = 0;
  UART_OutString("SD CARD is in SPI MODE");
	OutCRLF();
  GPIO_PORTF_DATA_R = GPIO_LED_GREEN;

  SPI_SEND_CMD(8,0x000001AA);

  response = SPI_Receive_Data(5);
  // response=SPI_Receive_Data_Compare(0x01);
  UART_OutString("CMD 8 Executed");
	OutCRLF();
  sprintf(string,"%lld",response);
	UART_OutString(string);
	OutCRLF();

do {
    SPI_SEND_CMD(55, 0x00000000);          // CMD55
    SPI_SEND_CMD(41, 0x40000000);          // CMD55
    response = SPI_Receive_Data(8);  // CMD41
} while (response != 0x00);
  response = SPI_Receive_Data(8);  
  // SPI_SEND_CMD(0x0000000077);
  response = SPI_Receive_Data_Compare(0x00);
  // sprintf(string,"%lld",response);
	// UART_OutString(string);
  response = SPI_Receive_Data(4);
	OutCRLF();
  UART_OutString("CMD 17 Executed");

	OutCRLF();
  sprintf(string,"%lld",response);
	UART_OutString(string);

while(1);

}
