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

void SPI_SEND_CMD(uint8_t cmd, uint32_t arg) {
    // Send the command byte with the start bit (0x40)
    int i;
    uint8_t crc = 0x01; // Default CRC value (not checked for most commands)

    
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
    while (reversed_arg){
    SPI_Transfer((reversed_arg) & 0xFF);
    while(SPI_STATUS_R & SPI_BUSY_FLAG);
    reversed_arg= reversed_arg>>8;
    }
    SPI_Transfer(crc);
    while(SPI_STATUS_R & SPI_BUSY_FLAG);

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


