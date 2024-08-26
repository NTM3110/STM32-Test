#include "uart.h"
#include <string.h>

#define GPIOAEN                     (1U << 0)
#define GPIODEN 					(1U << 3)

#define USART2EN                    (1U << 17)
#define CR1_TE                      (1U << 3)
#define CR1_RE                      (1U << 2)
#define CR1_UE                      (1U << 13)

#define SR_TXE                      (1U << 7)
#define SR_RXNE                     (1U << 5)

#define SYS_FREQ                    16000000
#define APB1_CLK                    SYS_FREQ
#define UART_BAUDRATE               115200

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate);

void usart2_rxtx_init(void){
    /********** Configure UART GPIO pin ************/
    //1. Enable clock access to the pin
    RCC->AHB1ENR |= GPIOAEN;

    //2. Set PA2 (UART_TX) mode to alternative function mode
    GPIOA->MODER &= ~(1U << 4);
    GPIOA->MODER |=  (1U << 5);
    //3. Set PA2 alternate function type to UART_TX
    GPIOA->AFR[0]   |= (0b0111 << 8);
    
    //4. Set PA3 (UART_RX) mode to alternative function mode 
    GPIOA->MODER &= ~(1U << 6);
    GPIOA->MODER |= (1U << 7);

    //5. Set PA3 alternate function type to UART_RX
    GPIOA->AFR[0] |= (0b0111 << 12);
    
    /********* Configure UART module *************/
    // 1. Enable clock access to UART2  
    RCC->APB1ENR |= USART2EN;
    // 2. Configure baudrate 
    uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

    // 3. Configure the transfer and receive direction
    USART2->CR1 |= CR1_TE; 
    USART2->CR1 |= CR1_RE;

    // 4. Enable UART module
    USART2->CR1 |= CR1_UE;
}

void usart2_tx_init(void){
    /********** Configure UART GPIO pin ************/
    //1. Enable clock access to the pin
    RCC->AHB1ENR |= GPIOAEN;
    //2. Set PA2 (UART_TX) mode to alternative function mode
    GPIOA->MODER &= ~(1U << 4);
    GPIOA->MODER |=  (1U << 5);
    //3. Set pin alternate function type to UART_TX
    GPIOA->AFR[0]   |= (0b0111 << 8);
    /********* Configure UART module *************/

    // 1. Enable clock access to UART2  
    RCC->APB1ENR |= USART2EN;
    // 2. Configure baudrate 
    uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

    // 3. Configure the transfer direction
    USART2->CR1 |= CR1_TE; 
    // 4. Enable UART module
    USART2->CR1 |= CR1_UE;
}

void usart2_write_string(char* str){
    int DataIdx;
    int len = strlen(str);
    for (DataIdx = 0; DataIdx < len; DataIdx++)
    {
        usart2_write(*str++);
    }
}
char usart2_read(void){
    while (!(USART2->SR & SR_RXNE)){}

    return USART2->DR;
}
void usart2_write(char ch){
    while (!(USART2->SR & SR_TXE)){}
    
    USART2-> DR = (ch & 0xFF);
}
static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate){
    USARTx->BRR = compute_uart_bd(PeriphClk, BaudRate);
}

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate){
    return ((PeriphClk + (BaudRate/2U))/BaudRate);
}