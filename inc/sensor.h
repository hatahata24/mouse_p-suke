/*
 * sensor.h
 *
 *  Created on: 2017/10/24
 *      Author: Blue
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#define VREFINT_CAL *((uint16_t*)0x1FFFF7BA)


enum {
	ADC1_CH1 = 1,
	ADC1_CH2 = 2,
	ADC1_CH3 = 3,
	ADC1_CH4 = 4,
	ADC1_CH11 = 11,
	ADC1_CH12 = 12,
	ADC1_VREFINT = 18,
	ADC2_CH1 = 32 + 1,
	ADC2_CH2 = 32 + 2,
	ADC2_CH3 = 32 + 3,
	ADC2_CH4 = 32 + 4
};



/*============================================================
		各種定数・変数宣言
============================================================*/
//----LEDのON/OFF出力----
#define IR_ON	1
#define IR_OFF	0

//====変数====
#ifdef MAIN_C_												//main.cからこのファイルが呼ばれている場合
	/*グローバル変数の定義*/
	//----その他----
	uint8_t tp;												//タスクポインタ
	uint32_t ad_r, ad_fr, ad_fl, ad_l;						//A-D値格納
	uint16_t base_l, base_r;								//基準値を格納
	int16_t dif_l, dif_r;									//AD値と基準との差
#else														//main.c以外からこのファイルが呼ばれている場合
	extern uint8_t tp;
	extern uint32_t ad_r, ad_fr, ad_fl, ad_l;
	extern uint16_t base_l,  base_r;
	extern int16_t dif_l, dif_r;
#endif

/*============================================================
		関数プロトタイプ宣言
============================================================*/

void sensor_init(void);
int get_adc_value(int);
int get_battery_voltage(void);


uint8_t get_base();					//センサ基準値を取得
void get_wall_info();				//壁情報を読む

#endif /* SENSOR_H_ */
