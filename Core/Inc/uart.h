#ifndef UART_H
#define UART_H 

#include "stm32f407xx.h"

void usart2_tx_init(void);
void usart2_write(char ch);
void usart2_write_string(char* str);
char usart2_read(void);

#endif