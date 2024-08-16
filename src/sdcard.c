/****************************************************************** 
 * Filename :    gpio.c                                    
 * Date     :    28-12-2021                                
 * Author   :    Muhammad Tahir                            
 *                                                         
 * Description:  Flash memory driver for writing and 
 *               reading its contents.                                 
 ******************************************************************/ 
 
#include <inttypes.h>

void sd_card_init(void){
        GPIOPinWrite(SD_CS_PORT, SD_CS_PIN, SD_CS_PIN);

    // Step 6: Send 80 clock cycles with CS high to set the SD card into SPI mode
    for (int i = 0; i < 10; i++) {
        SSIDataPut(SSI1_BASE, 0xFF);
        while (SSIBusy(SSI1_BASE));
    }
}