/*
 * port.c
 *
 *  Created on: 2017/10/22
 *      Author: Blue
 */


#include "global.h"

// GPIO_TypeDef *gpio のところで警告が出るので一時的に無効化
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

//+++++++++++++++++++++++++++++++++++++++++++++++
//_get_gpio_name
// 指定されたピンのGPIOグループ名を取得する
// 引数1：pin …… ピン番号(PXn)
// 戻り値：GPIOグループ名
//+++++++++++++++++++++++++++++++++++++++++++++++
GPIO_TypeDef * _get_gpio_name(uint32_t pin){
	GPIO_TypeDef *gpio;
	switch(pin / 16){
		case 0:
			#ifdef GPIOA
			gpio = GPIOA;
			#endif
			break;
		case 1:
			#ifdef GPIOB
			gpio = GPIOB;
			#endif
			break;
		case 2:
			#ifdef GPIOC
			gpio = GPIOC;
			#endif
			break;
		case 3:
			#ifdef GPIOD
			gpio = GPIOD;
			#endif
			break;
		case 4:
			#ifdef GPIOE
			gpio = GPIOE;
			#endif
			break;
		case 5:
			#ifdef GPIOF
			gpio = GPIOF;
			#endif
			break;
	}
	return gpio;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//pin_write
// 指定されたピンをHighまたはLowにする
// 引数1：pin …… ピン番号(PXn)
// 引数2：dat …… HighかLowか(0:Low,それ以外:High)
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void pin_write(uint32_t pin, uint8_t val){
	GPIO_TypeDef *gpio = _get_gpio_name(pin);

	if(val){
		gpio->ODR |= 1 << (pin % 16);
	}else{
		gpio->ODR &= ~(1 << (pin % 16));
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//pin_dir
// 指定されたピンの入出力を設定する
// 引数1：pin …… ピン番号(PXn)
// 引数2：dir …… 入力か出力か(0:入力,それ以外:出力)
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void pin_dir(uint32_t pin, uint8_t dir){
	GPIO_TypeDef *gpio = _get_gpio_name(pin);

	if(dir){	// Output
		gpio->MODER &= ~(1 << (pin % 16 * 2 + 1));
		gpio->MODER |= 1 << (pin % 16 * 2);
		gpio->OTYPER &= ~(1 << pin % 16);
		gpio->OSPEEDR &= ~(0b11 << (pin % 16 * 2));
		gpio->PUPDR &= ~(0b11 << (pin % 16 * 2));
	}else{		// Input, Pullup
		gpio->MODER &= ~(0b11 << (pin % 16 * 2));
		gpio->PUPDR &= ~(1 << (pin % 16 * 2 + 1));
		gpio->PUPDR |= 1 << (pin % 16 * 2);
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//pin_read
// 指定されたピンの入力値を取得する
// 引数1：pin …… ピン番号(PXn)
// 戻り値：0:Low,それ以外:High
//+++++++++++++++++++++++++++++++++++++++++++++++
uint32_t pin_read(uint32_t pin){
	GPIO_TypeDef *gpio = _get_gpio_name(pin);

	return gpio->IDR & 1 << (pin % 16);
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//pin_set_alternate_function
// 指定されたピンのAlternate Functionを設定する
// 引数1：pin …… ピン番号(PXn)
// 引数2：af …… Alternate Function 番号
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void pin_set_alternate_function(uint32_t pin, uint8_t alternate_function){
	GPIO_TypeDef *gpio = _get_gpio_name(pin);

	gpio->MODER |= 1 << (pin % 16 * 2 + 1);
	gpio->MODER &= ~(1 << (pin % 16 * 2));

	gpio->PUPDR &= ~(0b11 << (pin % 16 * 2));

	gpio->AFR[pin % 16 / 8] &= ~(0b1111 << (pin % 8) * 4);
	gpio->AFR[pin % 16 / 8] |= alternate_function << (pin % 8) * 4;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//pin_set_analog_mode
// 指定されたピンをAnalog Modeに設定する
// 引数1：pin …… ピン番号(PXn)
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void pin_set_analog_mode(uint32_t pin){
	GPIO_TypeDef *gpio = _get_gpio_name(pin);

	gpio->MODER |= 0b11 << (pin % 16 * 2);
	gpio->PUPDR &= ~(0b11 << (pin % 16 * 2));
}

// 警告を再有効化
#pragma GCC diagnostic warning "-Wmaybe-uninitialized"
