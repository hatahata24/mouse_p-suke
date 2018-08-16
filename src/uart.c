/*
 * uart.c
 *
 *  Created on: 2017/11/01
 *      Author: Blue
 */

#include "global.h"


void uart_init(uint32_t baudrate) {
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->CFGR3 |= RCC_CFGR3_USART1SW_SYSCLK; // system clock == usart1 clock

	// In this project, word length is set to 8bit.
	USART1->CR1 &= ~USART_CR1_OVER8; // not OVER8
	// Baud rate = f_ck / USARTDIV (16x over sampling)
	USART1->BRR |= (64000000UL / baudrate);

	/*USART1 GPIO Configuration
		PA9     ------> USART1_TX
		PA10    ------> USART1_RX
	*/
	pin_set_alternate_function(PA9, 7);			//PA9 : USART1_TXはAF7に該当
	pin_set_alternate_function(PA10, 7);		//PA10 : USART1_RXはAF7に該当

	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE; // transmitter & receiver enable
	USART1->CR1 |= USART_CR1_UE; // enable USART1

	setbuf(stdout, NULL);	// printf 函数を使うために必要
}


void uart_send_char(char c) {
	// Do not fail to set TE bit ==> Write data to USART_TDR ==> Wait until TDR_TC is set.
	while( !(USART1->ISR & USART_ISR_TXE) );
	USART1->TDR = c;
	while( !(USART1->ISR & USART_ISR_TC) );
}


char uart_get_char(void) {
	// Do not fail to set CR1_RE bit ==> Wait until RXNE bit is set. ==> Read from RDR
	while( !(USART1->ISR & USART_ISR_RXNE) );
	char c = USART1->RDR;
	return c;
}


int __io_putchar(int ch) {
	if(ch == '\n'){
		uart_send_char('\r');
	}
	uart_send_char(ch);
	return 0;
}
