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
#include <time.h>
#include "../include/spi.h"
#include "../include/gpio.h"
#include "../include/uart.h"
#include "../include/sd_card.h"
#include "../include/imem.h"
#define MAX_APP_SIZE               0x200

void SystemInit(void){};

int main (void){
  
	GPIO_Init();
  UART_Init();
  OutCRLF();

  UART_OutString("Uart Initialized");
  OutCRLF();

  SPI_Init(); 
  sdcard_init();
  SD_Read_Block(0x00000000);  
while(1);

}
