#ifndef ADC_H
#define ADC_H

#include <stdint.h>

void start_conversion(void);
void adc1_channel3_init(void);
uint32_t adc_read(void);

#endif
