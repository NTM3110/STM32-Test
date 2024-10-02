#include <stdint.h>
#include <stdio.h>
#include "stm32f407xx.h"
#include "uart.h"
#include "adc.h"
#include "systick.h"
#include "tim.h"

// uint32_t sensor_value;
// char snum[5];
#define SR_UIF    (1 << 0)
int main(void)
{
    usart2_tx_init();
    // adc1_channel3_init();
    // start_conversion();
    tim2_1hz_init();

    while (1)
    {
        // sensor_value = adc_read();
        // printf("%lu",sensor_value);
        // itoa(sensor_value, snum, 10);

        usart2_write_string("Hello");
        // usart2_write_string(snum);
        usart2_write('\n');
        while(!(TIM2->SR & SR_UIF)){}
        TIM2->SR &= ~(SR_UIF);
        // systickDelayMs(1000);
    }                                 
}