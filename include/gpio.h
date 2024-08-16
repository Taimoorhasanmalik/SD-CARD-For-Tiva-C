#include <inttypes.h>

// Clock gating control related register definitions
#define RCGC_GPIO_R        *((volatile unsigned long *)0x400FE608)
#define CLOCK_GPIOF        0x00000020 //Port F clock gating control

#define GPIO_PORTF_DATA_R  *((volatile unsigned long *)0x40025028) 
#define GPIO_PORTF_DIR_R   *((volatile unsigned long *)0x40025400)
#define GPIO_PORTF_DEN_R   *((volatile unsigned long *)0x4002551C)

#define SYS_CLK_FREQUENCY  16000000  // Default clock frequency
#define DELAY_VALUE				 SYS_CLK_FREQUENCY/80 

#define GPIO_PORTF_PIN_3_1 0x0A      // For green and red LEDs
#define GPIO_LED_GREEN     0x08      // For green LED
#define GPIO_LED_RED       0x02      // For red LED


// Function prototypes for SPI interface
void GPIO_Init(void);

