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
  uint32_t n,ocr=0; 
  uint8_t response_byte;
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
	SPI_SEND_CMD(0,0x00000000,0);
  response = SPI_Receive_Data_Compare(0x01);
	OutCRLF();
  sprintf(string,"%lld",response);
	UART_OutString(string);
	OutCRLF();
  response = 0;
  UART_OutString("SD CARD is in SPI MODE");
	OutCRLF();
  GPIO_PORTF_DATA_R = GPIO_LED_GREEN;

  SPI_SEND_CMD(8,0x000001AA,0);
  response=SPI_Receive_Data_Compare(0x01);
  response=SPI_Receive_Data_Compare(0xAA);
  UART_OutString("CMD 8 Executed");
	OutCRLF();
  sprintf(string,"%lld",response);
	UART_OutString(string);
	OutCRLF();

  do {
      SPI_SEND_CMD(55, 0x00000000,0);          // CMD55
	    response_byte = SPI_Receive_Data_Single_Shot();
      SPI_SEND_CMD(41, 0x40000000,0);          // CMD41
	    response_byte = SPI_Receive_Data_Single_Shot();
      while(SPI_STATUS_R & SPI_BUSY_FLAG);	
    } while (response_byte != 0x00);
  SPI_Transfer(0xFF);
  sprintf(string,"%d",response_byte);
  UART_OutString(string);
	OutCRLF();
  UART_OutString("SD CARD READY!!!");
SPI_Transfer(0xFF);  // Send a dummy byte before CMD58
while (SPI_STATUS_R & SPI_BUSY_FLAG);
SPI_SEND_CMD(58, 0x00000000, 0);  // CMD58 to read OCR
while (SPI_STATUS_R & SPI_BUSY_FLAG);
response_byte = SPI_Receive_Data_Single_Shot();
while(response_byte == 0x00 || response_byte == 0xFF){response_byte = SPI_Receive_Data_Single_Shot();} // OCR CHECK
response = SPI_Receive_Data(4);
	OutCRLF();
  sprintf(string,"%lld",response);
	UART_OutString(string);

SPI_SEND_CMD(17, 0x00000000, 0);
response = SPI_Receive_Data_Compare(0x00);
response = SPI_Receive_Data_Compare(0xFE);
for (i = 0; i < 512; i++) {
    response = SPI_Receive_Data_Single_Shot();
    sprintf(string,"%lld",response);

OutCRLF();
UART_OutString(string);
}
OutCRLF();
UART_OutString(string);
UART_OutString("CMD17");


while(1);

}
