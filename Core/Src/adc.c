#include "adc.h"
#include "stm32f407xx.h"

#define GPIOAEN                     (1U << 0)
#define ADC1_EN                     (1U << 8)
#define ADC_CH3                     0b0011
#define ADC_SQ_LEN_1                0b0000
#define ADC_ADON                    (1U << 0)
#define CR2_SWSTART                 (1U << 30)
#define CR2_CONT                    (1U << 1)
#define SR_EOC                      (1U << 1)

/*ADC configured with 3 channels 
ch2, ch3, ch4
first = ch5 SQ1 = 5
second= ch2 SQ2 = 2
third= ch3 SQ3 = 3
*/

void adc1_channel3_init(void){
    /***************Configure ADC GPIO pin ***************/
    //1. Enable clock access to the pin
    RCC->AHB1ENR |= GPIOAEN;

    //2. Set PA3 (ADC1_IN3) MODE TO ALTERVATIVE function mode
    GPIOA->MODER |= (1U<<6);
    GPIOA->MODER |= (1U<<7);
    

    //3. Enable clock for ADC1 RCC
    RCC->APB2ENR |= ADC1_EN;

    //4. Conversion sequence start
    ADC1->SQR3 = ADC_CH3;

    //5. Set sequence length
    ADC1->SQR1 = ADC_SQ_LEN_1;

    //Enable ADC module
    ADC1->CR2 |= ADC_ADON;
}

void start_conversion(void){

    //Enable Start continuous conversion
    ADC1->CR2 |= CR2_CONT;

    //Start ADC conversion
    ADC1->CR2 |= CR2_SWSTART;
}

uint32_t adc_read(void){
    /*Wait for conversion to be complete*/
    while(!(ADC1->SR & SR_EOC)){}

    /* Read converted result*/
    return (ADC1->DR);
}

