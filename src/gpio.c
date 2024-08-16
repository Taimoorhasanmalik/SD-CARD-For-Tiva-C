/****************************************************************** 
 * Filename :    gpio.c                                    
 * Date     :    28-12-2021                                
 * Author   :    Muhammad Tahir                            
 *                                                         
 * Description:  Flash memory driver for writing and 
 *               reading its contents.                                 
 ******************************************************************/ 
 
#include <inttypes.h>
#include "../include/gpio.h"


// SPI intialization and GPIO alternate function configuration
void GPIO_Init(void)
{		
  RCGC_GPIO_R |=  CLOCK_GPIOF;  // Enable the clock for port F 

	// Configure port F pins 1 and 3 as digital output
	GPIO_PORTF_DEN_R |= GPIO_PORTF_PIN_3_1;    // Digital enable for PF1 and PF3  
  GPIO_PORTF_DIR_R |= GPIO_PORTF_PIN_3_1;    // PF1 and PF3 as output  
	

}



