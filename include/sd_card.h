#include <inttypes.h>
#include <stdio.h>

#define BLOCK_SIZE 512
void sdcard_init(void);
void SD_Read_Block(uint32_t block_address);
uint32_t SPI_SEND_CMD(uint8_t cmd, uint32_t arg, uint32_t receive); 
void SD_Write_Block(uint32_t block_address, uint32_t* data);
void sdcard_format(void);
uint32_t SPI_SEND_ACMD(uint8_t cmd, uint32_t arg);
void delay_ms(int ms);