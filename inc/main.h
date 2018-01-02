/*
 * main.h
 *
 *  Created on: 2017/10/22
 *      Author: Blue
 */

#ifndef MAIN_H_
#define MAIN_H_


#define is_sw_pushed(A) !pin_read(A)

#define min(A,B) ((A)>(B))?(B):(A)
#define max(A,B) ((A)>(B))?(A):(B)

void sysclk_init(void);
void ms_wait(uint32_t);

void gpio_init(void);
void led_write(uint8_t, uint8_t, uint8_t);

extern UART_HandleTypeDef uart_handler;
HAL_StatusTypeDef uart_init(void);

#endif /* MAIN_H_ */
