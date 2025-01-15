#include "uart.h"
#include <stdio.h>

struct _FILE{int dummy;};
FILE __stdout;
// int fputc(int ch, FILE* f) {
// 	uart_send_char(ch);
// 	return ch;
// }

void uart_init(void) {
    // Enable clocks for GPIOA and USART1
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // Enable GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Enable USART1 clock

    // Configure PA9 as USART1_TX (Alternate function push-pull)
    GPIOA->CRH &= ~GPIO_CRH_CNF9;
    GPIOA->CRH |= GPIO_CRH_CNF9_1; // Alternate function push-pull
    GPIOA->CRH |= GPIO_CRH_MODE9; // Output mode, max speed 50 MHz

    // Configure PA10 as USART1_RX (Input floating)
    GPIOA->CRH &= ~GPIO_CRH_MODE10;
    GPIOA->CRH &= ~GPIO_CRH_CNF10;
    GPIOA->CRH |= GPIO_CRH_CNF10_0; // Input floating

    // Configure USART1
    USART1->BRR = 0x1D4C; // Baud rate 9600 at 72MHz (assuming 8MHz crystal and PLL multiplier 9)
    USART1->CR1 |= USART_CR1_TE; // Enable transmitter
    USART1->CR1 |= USART_CR1_RE; // Enable receiver
    USART1->CR1 |= USART_CR1_UE; // Enable USART
}

void uart_send_char(char c) {
		USART1->DR = c;
    // Wait until transmit data register is empty
    while (!(USART1->SR & USART_SR_TXE));
    // Send a char using USART1
    USART1->SR &=~ ( 1 << 6);
}

void uart_send_string(const char *str) {
    while (*str) {
        uart_send_char(*str++);
    }
}
