#include <stdint.h>
#include <stdio.h>
#include "stm32f407xx.h"
#include "uart.h"
#include "adc.h"
#include "systick.h"

uint32_t sensor_value;
char snum[5];
int main(void)
{

    usart2_tx_init();
    adc1_channel3_init();
    start_conversion();
    
    while (1)
    {
        sensor_value = adc_read();
    //    printf("%lu",sensor_value);
        itoa(sensor_value, snum, 10);
        usart2_write_string("Sensor: ");
        usart2_write_string(snum);
        usart2_write('\n');
        systickDelayMs(2000);
    }                                 
}