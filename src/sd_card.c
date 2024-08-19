
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>


#include "../include/spi.h"
#include "../include/gpio.h"
#include "../include/uart.h"
#include "../include/sd_card.h"

void sdcard_init(void){  
  uint8_t response_byte;
  char string[20];
  uint64_t response;
  int i =0;

  
  SPI_DIS(); //CS is set High

  for (i = 0; i < 10; i++) {
    SPI_Transfer(0xFF);
	  while(SPI_STATUS_R & SPI_BUSY_FLAG);	
  }

  SPI_EN(); //CS is set low
//CMD0 START
	SPI_SEND_CMD(0,0x00000000,0);
  response = SPI_Receive_Data_Compare(0x01);
	OutCRLF();
  sprintf(string,"%lld",response);
	UART_OutString(string);
	OutCRLF();
//CMD0 END
  
  response = 0;
  UART_OutString("SD CARD is in SPI MODE");
	OutCRLF();
  GPIO_PORTF_DATA_R = GPIO_LED_GREEN;

//CMD8 START

  SPI_SEND_CMD(8,0x000001AA,0);
  response=SPI_Receive_Data_Compare(0xAA);
  UART_OutString("CMD 8 Executed");
	OutCRLF();
  sprintf(string,"%lld",response);
	UART_OutString(string);
	OutCRLF();
//CMD8 END


//CMD55-CMD41 START

  do {
      response_byte= SPI_SEND_ACMD(41,0X40000000);
    } while (response_byte != 0x00);
  sprintf(string,"%d",response_byte);
  UART_OutString(string);
	OutCRLF();
//CMD55-CMD41 END

  UART_OutString("SD CARD READY!!!");

//CMD58 START

  SPI_Transfer(0xFF);  // Send a dummy byte before CMD58
  while (SPI_STATUS_R & SPI_BUSY_FLAG);
  if(SPI_SEND_CMD(58, 0x00000000, 0))
  {
    OutCRLF();
  	UART_OutString("FAILED CMD58");
    OutCRLF();
    sprintf(string,"%d",response_byte);
    UART_OutString(string);
  }  // CMD58 to read OCR
  else{
  if (response_byte & 0x8 != 0xC0){
  	UART_OutString("FAILED CMD58");
    OutCRLF();
    sprintf(string,"%d",response_byte);
    UART_OutString(string);
  }
    OutCRLF();
  	UART_OutString("Completed Initialization");
    OutCRLF();

  }
}
uint32_t SPI_SEND_ACMD(uint8_t cmd, uint32_t arg) {
    SPI_Receive_Data_Single_Shot();
    SPI_SEND_CMD(55, 0x00000000, 0);
    SPI_Receive_Data_Single_Shot();
    return SPI_SEND_CMD(cmd, arg,0);

}


uint32_t SPI_SEND_CMD(uint8_t cmd, uint32_t arg, uint32_t receive) {
    // Send the command byte with the start bit (0x40)
    uint8_t i;
  	uint32_t value = 0xFF;
    uint8_t crc = 0xFF; // Default CRC value (not checked for most commands)

    
    // Reverse the bytes of the argument manually
    uint32_t reversed_arg = ((arg >> 24) & 0xFF) |
                            ((arg >> 8) & 0xFF00) |
                            ((arg << 8) & 0xFF0000) |
                            ((arg << 24) & 0xFF000000);

    // Add CRC byte (required for CMD0 and CMD8, usually not needed for others)
    if (cmd == 0) {      // CMD0
        crc = 0x95;
    } else if (cmd == 8) { // CMD8
        crc = 0x87;
    }
    else{
      crc= 0xFF;
    }

    delay_ms(300);
    SPI_Transfer(cmd | 0x40);
    while(SPI_STATUS_R & SPI_BUSY_FLAG);
    // Send the 4-byte reversed argument (MSB first)
    SPI_Transfer(reversed_arg & 0xFF);
    while(SPI_STATUS_R & SPI_BUSY_FLAG);
    SPI_Transfer(reversed_arg>>8 & 0xFF);
    while(SPI_STATUS_R & SPI_BUSY_FLAG);
    SPI_Transfer(reversed_arg>>16 & 0xFF);
    while(SPI_STATUS_R & SPI_BUSY_FLAG);
    SPI_Transfer(reversed_arg>>24 & 0xFF);
    while(SPI_STATUS_R & SPI_BUSY_FLAG);

    SPI_Transfer(crc);
    while(SPI_STATUS_R & SPI_BUSY_FLAG);
    for ( i = 0; ((value = (SPI_DATA_R)) & 0X80) && i != 0XFF; i++);
return value;
}


void SD_Read_Block(uint32_t block_address) {
    uint8_t response;
    uint16_t i;
    uint8_t buffer[200];
    uint8_t filename[11];
    uint8_t attributes;
    uint16_t file_size;
    // Convert block address to byte address if card is in byte addressing mode
    // Most modern SD cards use block addressing, so no conversion might be necessary.

    // Send CMD17 (Read Single Block)
    if (SPI_SEND_CMD(17, block_address, 0)){
    // Read the 512-byte data block
    UART_OutString("Error Reading Block");
    OutCRLF();  
    }
    else{
    for (i = 0; i < 512; i++) {
        buffer[i] = SPI_Receive_Data_Single_Shot();
    }
    }
    // Skip the 2-byte CRC (optional)
    SPI_Receive_Data_Single_Shot();
    SPI_Receive_Data_Single_Shot();

    // Now buffer[] contains the data from the block
    UART_OutString("Block Read Successfully");
    OutCRLF();
    for (i = 0; i < 512; i++) { // Assume 16 directory entries per block
            // Extract file name, attributes, and size from directory entry
            UART_OutString(buffer);
            OutCRLF();
        }
}

void SD_Read_Multiple_Blocks(uint32_t block_address)
{
    uint8_t response;
    char string[20];
		int i;
	
    SPI_SEND_CMD(18, block_address, 0);
    response = SPI_Receive_Data_Compare(0x00);
    response = SPI_Receive_Data_Compare(0xFE);
    for (i = 0; i < 512; i++) {
        response = SPI_Receive_Data_Single_Shot();
        sprintf(string,"%d",response);
        UART_OutString(string);
	    OutCRLF();
    }  

    for (i = 0; i < 512; i++) {
        response = SPI_Receive_Data_Single_Shot();
        sprintf(string,"%d",response);
        UART_OutString(string);
	    OutCRLF();
    }
    SPI_SEND_CMD(12, 0x00000000, 0);
    response = SPI_Receive_Data_Compare(0x00);
    response = SPI_Receive_Data_Compare(0xFF);
    sprintf(string,"%d",response);
    UART_OutString("ENDED TRANSMISSION");
	OutCRLF();

}


void SD_Write_Block(uint32_t block_address, uint32_t* data)
{
    int i;
    char string[20];
    uint8_t response;
    uint8_t arr[256];
    // Convert 32-bit data to byte array
    for (i = 0; i < data_size/512; i++) {
      for(j=0; j< 4;j++)
        arr[j] = (imem[i] >> (24 - (j * 8))) & 0xFF;
    }

    // Send CMD24 to write single block
    if(SPI_SEND_CMD(24, block_address, 0))
    {
      UART_OutString("CMD24 Failed");
      OutCRLF();
    }
    
    SPI_Transfer(0xFE);
    while(SPI_STATUS_R & SPI_BUSY_FLAG);
    // Write data to SD card
    for (i = 0; i < 4*(data_size/512); i++) {
      SPI_Transfer(arr[i]);
      while(SPI_STATUS_R & SPI_BUSY_FLAG);
    }
    spi_transfer(0xFF);  // Send dummy CRC (not used but must be sent)
    spi_transfer(0xFF);
    response = spi_transfer(0xFF);
    if ((response & 0x1F) != 0x05) {
        // Handle error
    }

    // Wait for write operation to complete
    while (response == 0x00 || response == 0xFF || response == 0x80) {
        response = SPI_Receive_Data_Single_Shot();
    }

    // Check response
    if (response == 0x05 || response == 0x00) {
        UART_OutString("Write successful!");
    } else {
        sprintf(string,"%d",response);
        UART_OutString(string);
        
        UART_OutString("Write failed!");
    }
}


void sdcard_format(void)
{
    char string[20];
    uint8_t response= 0xFF;

    SPI_SEND_CMD(58, 0x00000000, 0); // Set start address
    while (response == 0xFF) {
        response = SPI_Receive_Data_Single_Shot();
    }
    sprintf(string,"%d",response);
    OutCRLF();
    UART_OutString("CMD 58");

    OutCRLF();
    UART_OutString(string);
    OutCRLF();
    SPI_SEND_CMD(32, 0x00000000, 0); // Set start address
    while (response == 0xFF) {
        response = SPI_Receive_Data_Single_Shot();
    }
    sprintf(string,"%d",response);
    UART_OutString("CMD 32");
    OutCRLF();
    UART_OutString(string);
    OutCRLF();
    SPI_SEND_CMD(33, 0x00000000, 0); // Set end address
    while (response == 0xFF) {
        response = SPI_Receive_Data_Single_Shot();
    }
    sprintf(string,"%d",response);
    UART_OutString("CMD 33");
    OutCRLF();
    UART_OutString(string);
    OutCRLF();
    SPI_SEND_CMD(38, 0x00000000, 0); // Trigger format
    while (response == 0xFF) {
        response = SPI_Receive_Data_Single_Shot();
    }
    sprintf(string,"%d",response);
    UART_OutString("CMD 38");
    OutCRLF();
    UART_OutString(string);
    OutCRLF();

}
void delay_ms(int ms) {
    int i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 1000; j++) {
            // Do nothing, just waste some time
        }
    }
}

