#include <inttypes.h>

// Clock gating control related register definitions
#define RCGC_SPI_R           *((volatile unsigned long *)0x400FE61C)
#define RCGC_GPIO_R          *((volatile unsigned long *)0x400FE608)
#define CLOCK_GPIOD          0x00000008   //Port D clock control
#define CLOCK_SPI1           0x00000002   // SPI 1 clock control

// SPI base address and register definitions 
#define SPI_BASE             0x40009000

#define SPI_CONTROL0_R       *(volatile long *)(SPI_BASE + 0x000)
#define SPI_CONTROL1_R       *(volatile long *)(SPI_BASE + 0x004)
#define SPI_DATA_R           *(volatile long *)(SPI_BASE + 0x008)
#define SPI_STATUS_R         *(volatile long *)(SPI_BASE + 0x00C)
#define SPI_CLK_PRESCALE_R   *(volatile long *)(SPI_BASE + 0x010)
#define SPI_CLK_CONFIG_R     *(volatile long *)(SPI_BASE + 0xFC8)

// SPI interrupt configuration register definitions
#define SPI_INT_MASK_R       *(volatile long *)(SPI_BASE + 0x014)
#define SPI_RIS_R            *(volatile long *)(SPI_BASE + 0x018)
#define SPI_MIS_R            *(volatile long *)(SPI_BASE + 0x01C)
#define SPI_INT_CLEAR_R      *(volatile long *)(SPI_BASE + 0x020)

// GPIO Port D alternate function configuration
#define GPIO_PORTD_AFSEL_R   *((volatile unsigned long *)0x40007420)
#define GPIO_PORTD_PCTL_R    *((volatile unsigned long *)0x4000752C)
#define GPIO_PORTD_DEN_R     *((volatile unsigned long *)0x4000751C)
#define GPIO_PORTD_DATA_R    *((volatile unsigned long *)0x40007008)
#define GPIO_PORTD_DIR_R     *((volatile unsigned long *)0x40007400)


// SPI bit field definitions
#define SPI_ENABLE                   0x02
#define SPI_EOT                      0x10
#define SPI_BUSY_FLAG                0x10
#define SPI_TX_FIFO_EMPTY_FLAG       0x01
#define SPI_TX_FIFO_NOT_FULL_FLAG    0x02
#define SPI_RX_FIFO_NOT_EMPTY_FLAG   0x04

#define SPI_DONE                     0x08


#define ARRAY_SIZE(x)               (sizeof(x) / sizeof((x)[0]))
#define SPI_EN()                     GPIO_PORTD_DATA_R &= (~0x02)
#define SPI_DIS()                    GPIO_PORTD_DATA_R |= 0x02

// Function prototypes for SPI interface
void SPI_Init(void);
uint64_t SPI_Receive_Data_Compare(uint64_t EXP_OUT);
void SPI_Transfer(uint8_t data);
uint32_t SPI_SEND_CMD(uint8_t cmd, uint32_t arg, uint32_t receive); 
uint64_t SPI_Receive_Data(uint16_t num_of_bytes);

uint64_t SPI_Receive_Data_Single_Shot(void);
