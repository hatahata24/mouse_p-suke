/************************************************************
		WMMC Micromouse 標準プログラム 2018
		2007年~2012年	H8/3694f版が運用される
		2013/03/01		LPC向けに移植 by 長谷川 健人
		2013/09/20		標準化のため色々弄る by 高橋 有礼
		2017/11/12		STM32向けに移植
 ************************************************************/
/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#define MAIN_C_
#include "global.h"


void sysclk_init(void){						// set SystemClock to 64MHz
	RCC->CFGR |= RCC_CFGR_PLLMUL16;			// PLL source & multiplication x 16
	RCC->CR |= RCC_CR_PLLON;				// set PLL on
	while( !(RCC->CR & RCC_CR_PLLRDY) );	// wait until PLL is ready

	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;		// APB1 clock is 32MHz (up to 36MHz)
	FLASH->ACR |= FLASH_ACR_LATENCY_1;
	RCC->CFGR |= RCC_CFGR_SW_PLL;			// switch sysclk to PLL
	while( (RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL );	// wait until the setting is applied

	SystemCoreClockUpdate();	// Updates the variable SystemCoreClock and must be called whenever the core clock is changed during program execution.
}


void ms_wait(uint32_t ms){		// SystemClock is prescaled by 8 in SysTick timer
	SysTick->LOAD = 8000 - 1;	// reset value for count-down timer
	SysTick->VAL = 0;			// initial value
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	// count start

	uint32_t i;
	for(i = 0; i < ms; i++){
		while( !(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) );
	}
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	// count stop
}


void gpio_init(void){
	__HAL_RCC_GPIOA_CLK_ENABLE();	// Peripheral clock enable
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

	/*
	 * SW_INC, SW_DEC, SW_RET : Input, Pullup
	 */
	pin_dir(PIN_SW_INC, IN);
	pin_dir(PIN_SW_DEC, IN);
	pin_dir(PIN_SW_RET, IN);

	/*
	 * LED0~2 : Output
	 */
	pin_dir(PIN_LED1, OUT);
	pin_dir(PIN_LED2, OUT);
	pin_dir(PIN_LED3, OUT);

}


void led_write(uint8_t led1, uint8_t led2, uint8_t led3){
	pin_write(PIN_LED1, led1);
	pin_write(PIN_LED2, led2);
	pin_write(PIN_LED3, led3);
}




UART_HandleTypeDef uart_handler;
HAL_StatusTypeDef uart_init(void){

	/*USART1 GPIO Configuration
		PA9     ------> USART1_TX
		PA10    ------> USART1_RX
	*/
	GPIO_InitTypeDef gpio_init_struct;
	gpio_init_struct.Pin = GPIO_PIN_9|GPIO_PIN_10;
	gpio_init_struct.Mode = GPIO_MODE_AF_PP;
	gpio_init_struct.Pull = GPIO_PULLUP;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_init_struct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &gpio_init_struct);

	__HAL_RCC_USART1_CLK_ENABLE();	// Peripheral clock enable

	uart_handler.Instance = USART1;
	uart_handler.Init.BaudRate = 38400;
	uart_handler.Init.WordLength = UART_WORDLENGTH_8B;
	uart_handler.Init.StopBits = UART_STOPBITS_1;
	uart_handler.Init.Parity = UART_PARITY_NONE;
	uart_handler.Init.Mode = UART_MODE_TX_RX;
	uart_handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart_handler.Init.OverSampling = UART_OVERSAMPLING_16;
	uart_handler.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	uart_handler.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	return HAL_UART_Init(&uart_handler);
}



int main(void){

	ms_wait(12);	// 12*8 = 96 ms

	sysclk_init();

	gpio_init();
	uart_init();

	ms_wait(100);


	sensor_init();
	drive_init();
	search_init();

	int mode = 0;

	while(1){

		if( is_sw_pushed(PIN_SW_INC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_INC) );
			mode++;
			if(mode > 7){
				mode = 0;
			}
		}
		if( is_sw_pushed(PIN_SW_DEC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_DEC) );
			mode--;
			if(mode < 0){
				mode = 7;
			}
		}

		led_write(mode & 0b001, mode & 0b010, mode & 0b100);

		if( is_sw_pushed(PIN_SW_RET) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_RET) );
			switch(mode){

				case 0:
					//----基準値を取る----
					get_base();
					break;

				case 1:
					//----一次探索走行----
					drive_enable_motor();

					MF.FLAG.SCND = 0;
					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					rotate_R90();
					drive_wait();
					set_position(0);
					drive_wait();
					rotate_L90();
					drive_wait();
					set_position(0);
					drive_wait();

					searchA();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchA();

					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					drive_disable_motor();
					break;

				case 2:
					//----一次探索連続走行----
					drive_enable_motor();

					MF.FLAG.SCND = 0;
					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					rotate_R90();
					drive_wait();
					set_position(0);
					drive_wait();
					rotate_L90();
					drive_wait();
					set_position(0);
					drive_wait();

					searchB();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchB();

					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					drive_disable_motor();
					break;

				case 3:
					//----二次探索走行----
					drive_enable_motor();

					MF.FLAG.SCND = 1;
					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					rotate_R90();
					drive_wait();
					set_position(0);
					drive_wait();
					rotate_L90();
					drive_wait();
					set_position(0);
					drive_wait();

					searchB();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchB();

					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					drive_disable_motor();
					break;

				case 4:
					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					break;
			}
		}

		ms_wait(100);
	}
}
