
// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1

#include <stdint.h>
//Register definitions for ClockEnable
#define 		SYSCTLRCGCUARTR			(*((volatile unsigned long*)0x400FE618))
#define 		SYSCTLRCGCGPIOR			(*((volatile unsigned long*)0x400FE608))

//Register definitions for UART0 module
#define 	UART0CTLR						(*((volatile unsigned long*)0x4000C030))
#define  	UART0IBRDR					(*((volatile unsigned long*)0x4000C024))
#define  	UART0FBRDR					(*((volatile unsigned long*)0x4000C028))
#define 	UART0LCRHR					(*((volatile unsigned long*)0x4000C02C))
#define 	UART0CCR						(*((volatile unsigned long*)0x4000CFC8))
#define 	UART0FRR						(*((volatile unsigned long*)0x4000C018))
#define 	UART0DRR						(*((volatile unsigned long*)0x4000C000))

//Register definitions for GPIOPortA
#define 	GPIOPORTAAFSELR			(*((volatile unsigned long*)0x40004420))
#define 	GPIOPORTAPCTLR			(*((volatile unsigned long*)0x4000452C))
#define 	GPIOPORTADENR				(*((volatile unsigned long*)0x4000451C))
#define 	GPIOPORTADIRR				(*((volatile unsigned long*)0x40004400))
#define 	GPIOPORTAAMSELR			(*((volatile unsigned long*)0x40004528))


#define UART_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART_FR_RXFE            0x00000210 // UART Receive FIFO Empty
#define UART_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART_CTL_UARTEN         0x00000001  // UART Enable
#define CR											13
#define BS											8
#define LF											10


void UART_Init(void);
char UART_InChar(void);
void UART_OutChar(char data);
void UART_OutString(char *pt);
void UART_InString(char *bufPt, uint16_t max);
void OutCRLF(void);
