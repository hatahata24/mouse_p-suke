/*
 * uart.h
 *
 *  Created on: 2017/11/01
 *      Author: Blue
 */

#ifndef UART_H_
#define UART_H_

#include <stdio.h>

void uart_init(uint32_t);
void uart_send_char(char);
char uart_get_char(void);

int __io_putchar(int);


#endif /* UART_H_ */
