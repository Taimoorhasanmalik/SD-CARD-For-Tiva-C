//------------UART_Init------------
// Initialize the UART for 115,200 baud rate (assuming 50 MHz UART clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none

#include "../include/uart.h"
void UART_Init(void){
  SYSCTLRCGCUARTR |= 0x00000001;            		// activate UART0
  SYSCTLRCGCGPIOR |= 0x00000001;            		// activate port A
  UART0CCR |= 0;
 
  UART0CTLR &= ~UART_CTL_UARTEN;      		// disable UART
  UART0IBRDR = 0x000000D0 ;                    		// IBRD = int(16,000,000 / (16 * 115,200)) = int(8.6805)
  UART0FBRDR = 0x00000015;                     	// FBRD = int(0.6805 * 64 + 0.5) = 44
  UART0CTLR |= 0x20;																				// 8 bit word length (no parity bits, one stop bit, FIFOs)
  UART0LCRHR = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
  UART0CTLR |= UART_CTL_UARTEN; 	// enable UART
	
  GPIOPORTAAFSELR |= 0x0000003;           			// enable alt funct on PA1-0
  GPIOPORTADENR |= 0x0000003;             			// enable digital I/O on PA1-0
																					// configure PA1-0 as UART
  GPIOPORTAPCTLR = (GPIOPORTAPCTLR & 0xFFFFFF00)+0x00000011;
  GPIOPORTAAMSELR &= ~0x0000003;          // disable analog functionality on PA

}


//------------UART_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
char UART_InChar(void){
  while((UART0FRR & UART_FR_RXFE) != 0);
  return((char)(UART0DRR & 0xFF));
}

//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART_OutChar(char data){
  while((UART0FRR & UART_FR_TXFF) != 0);
  UART0DRR = data;
}


//------------UART_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART_OutString(char *pt){

    while(*pt){
    UART_OutChar(*pt);
    pt++;
    }
}


//------------UART_InString------------
// Accepts ASCII characters from the serial port
//    and adds them to a string until <enter> is typed
//    or until max length of the string is reached.
// It echoes each character as it is inputted.
// If a backspace is inputted, the string is modified
//    and the backspace is echoed
// terminates the string with a null character
// uses busy-waiting synchronization on RDRF
// Input: pointer to empty buffer, size of buffer
// Output: Null terminated string
// -- Modified by Agustinus Darmawan + Mingjie Qiu --
void UART_InString(char *bufPt, uint16_t max) {
int length=0;
char character;
  character = UART_InChar();
  while(character != CR){
    if(character == BS){
      if(length){
        bufPt--;
        length--;
        UART_OutChar(BS);
      }
    }
    else if(length < max){
      *bufPt = character;
      bufPt++;
      length++;
      UART_OutChar(character);
    }
  }
  *bufPt = 0;
}
 // END OF UART.c



//---------------------OutCRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
void OutCRLF(void){
  UART_OutChar(CR);
  UART_OutChar(LF);
}
