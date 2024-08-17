/****************************************************************** 
 * Filename :    spi.c                                    
 * Date     :    28-12-2021                                
 * Author   :    Muhammad Tahir                            
 *                                                         
 * Description:  Flash memory driver for writing and 
 *               reading its contents.                                 
 ******************************************************************/ 
 
#include <inttypes.h>
#include "../include/spi.h"
#include "../include/uart.h"


// SPI intialization and GPIO alternate function configuration
void SPI_Init(void)
{		
  RCGC_GPIO_R |=  CLOCK_GPIOD;  // Enable the clock for port D 
	RCGC_SPI_R |= CLOCK_SPI1;     // Enable the clock for SPI 1
	SPI_DIS();
  GPIO_PORTD_DEN_R |= 0x0F;     // DEN for port D pins 0-3
	GPIO_PORTD_DIR_R |= 0x0B;
  
  // Configure Port D pins 0-3 for SPI 1 
  GPIO_PORTD_AFSEL_R |= 0x0000000D;
  GPIO_PORTD_PCTL_R |= 0x00002202;
	  
  // Disable SPI and perform control configuration
  SPI_CONTROL1_R &= ~(SPI_ENABLE);

  // Freescale frame format, mode 3, 8 bit data & 1 MHz SPI clock
  SPI_CONTROL0_R |= 0x00C7;   
  SPI_CLK_PRESCALE_R = 0x0028;    // Prescale system clock by 200
  SPI_CLK_CONFIG_R = 0x00;        // Clock source is system clock

  SPI_CONTROL1_R |= SPI_ENABLE + SPI_EOT;   // Enable SPI master module
}

// This function transmits one data element and reads the 
// received byte  
void SPI_Transfer(unsigned char Data)
{    
	SPI_DATA_R = (Data & 255);

}

uint64_t SPI_Receive_Data_Compare(uint64_t EXP_OUT)
{
	uint64_t value = 0;
  
  while(value != EXP_OUT)
  {
    SPI_Transfer(0xFF); // KEEP MOSI HIGH
    value = SPI_DATA_R & 0xFF;
  }
  return value;
}

uint32_t SPI_SEND_CMD(uint8_t cmd, uint32_t arg, uint32_t receive) {
    // Send the command byte with the start bit (0x40)
    int i;
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

    if (cmd == 0 || cmd == 8) {      // CMD0 or CMD8
      SPI_Transfer(crc);
      while(SPI_STATUS_R & SPI_BUSY_FLAG);
    }
    else{
    SPI_Transfer(0xFF);
    while(SPI_STATUS_R & SPI_BUSY_FLAG);
    }
    if (receive)
    {
      SPI_Transfer(0xFF);
      while(SPI_STATUS_R & SPI_BUSY_FLAG);
    for (i = 0; i<4; i++)
      {
        SPI_Transfer(0xFF);
        while(SPI_STATUS_R & SPI_BUSY_FLAG);
        value =value << 8;
        value |=(SPI_DATA_R & 0xFF);
    }
    return value;
    }
return 0;
}

uint64_t SPI_Receive_Data(uint16_t num_of_bytes)
{
	uint64_t value = 0xFF;
	uint64_t value_out = 0;
  value = SPI_DATA_R & 0xFF;
  while(value == 0xFF)
  {
    SPI_Transfer(0xFF); // KEEP MOSI HIGH
    while(SPI_STATUS_R & SPI_BUSY_FLAG);
    value = SPI_DATA_R & 0xFF;
  }
  while(num_of_bytes)
  {
    value_out |= value;
    SPI_Transfer(0xFF); // KEEP MOSI HIGH
    while(SPI_STATUS_R & SPI_BUSY_FLAG);
    value = SPI_DATA_R & 0xFF;
    num_of_bytes--;
    if (num_of_bytes){value_out = value_out<<8;}
  }
  return value_out;
}


uint8_t SPI_Receive_Data_Single_Shot(void)
{
	uint8_t value=0;
  int i;
  SPI_Transfer(0xFF);
  while(SPI_STATUS_R & SPI_BUSY_FLAG);	

  value = SPI_DATA_R & 0xFF;

  return value;
}


void SD_Read_Block(uint32_t block_address) {
    uint8_t response;
    uint16_t i;
    uint8_t buffer[200];
    // Convert block address to byte address if card is in byte addressing mode
    // Most modern SD cards use block addressing, so no conversion might be necessary.

    // Send CMD17 (Read Single Block)
    SPI_SEND_CMD(17, block_address, 0);

    // Wait for the R1 response (should be 0x00 if successful)
    response = SPI_Receive_Data_Single_Shot();
    if (response != 0x00) {
        UART_OutString("CMD17 failed");
        OutCRLF();
        return;
    }

    // Wait for data token (0xFE)
    while ((response = SPI_Receive_Data_Single_Shot()) != 0xFE);

    // Read the 512-byte data block
    for (i = 0; i < 512; i++) {
        buffer[i] = SPI_Receive_Data_Single_Shot();
    }

    // Skip the 2-byte CRC (optional)
    SPI_Receive_Data_Single_Shot();
    SPI_Receive_Data_Single_Shot();

    // Now buffer[] contains the data from the block
    UART_OutString("Block Read Successfully");
    OutCRLF();
}
